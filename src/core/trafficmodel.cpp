#include "trafficmodel.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QUrl>

namespace {
constexpr int FlushIntervalMs = 15;
constexpr qint64 RxMergeWindowMs = 15;
constexpr int MaxPayloadDisplay = 512;
} // namespace

TrafficModel::TrafficModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_flushTimer.setInterval(FlushIntervalMs);
    m_flushTimer.setSingleShot(true);
    connect(&m_flushTimer, &QTimer::timeout, this, &TrafficModel::flushPending);
}

int TrafficModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_rows.size();
}

QVariant TrafficModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size())
        return {};
    const Row &row = m_rows.at(index.row());
    switch (role) {
    case TimeRole:
        return formatTime(row.timestampMs);
    case DirRole:
        return row.direction;
    case DataHexRole: {
        const QByteArray shown = row.payload.left(MaxPayloadDisplay);
        return row.plain ? QString() : toHexText(shown);
    }
    case DataTextRole: {
        const QByteArray shown = row.payload.left(MaxPayloadDisplay);
        return row.plain ? QString::fromUtf8(shown) : toDisplayText(shown);
    }
    case DataAsciiRole:
        return toAsciiText(row.payload.left(MaxPayloadDisplay));
    case OmittedBytesRole:
        return qMax<qsizetype>(0, row.payload.size() - MaxPayloadDisplay);
    case PeerRole:
        return row.peer;
    case SizeRole:
        return row.payload.size();
    case PlainRole:
        return row.plain;
    default:
        return {};
    }
}

QHash<int, QByteArray> TrafficModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    roles[TimeRole] = "time";
    roles[DirRole] = "dir";
    roles[DataHexRole] = "dataHex";
    roles[DataTextRole] = "dataText";
    roles[PeerRole] = "peer";
    roles[SizeRole] = "size";
    roles[PlainRole] = "plain";
    roles[DataAsciiRole] = "dataAscii";
    roles[OmittedBytesRole] = "omittedBytes";
    return roles;
}

void TrafficModel::setPaused(bool paused)
{
    if (m_paused == paused)
        return;
    m_paused = paused;
    emit pausedChanged();
    if (m_paused) {
        m_flushTimer.stop();
    } else {
        flushPending();
    }
}

void TrafficModel::setMaxRows(int rows)
{
    rows = qBound(10, rows, 200000);
    if (m_maxRows == rows)
        return;
    m_maxRows = rows;
    emit maxRowsChanged();
}

void TrafficModel::setMaxBytes(qint64 bytes)
{
    bytes = qBound<qint64>(1024, bytes, 1024LL * 1024 * 1024);
    if (m_maxBytes == bytes)
        return;
    m_maxBytes = bytes;
    emit maxBytesChanged();
}

void TrafficModel::clear()
{
    m_flushTimer.stop();
    beginResetModel();
    m_rows.clear();
    m_pending.clear();
    m_pendingBytes = 0;
    m_rowBytes = 0;
    endResetModel();
    emit countChanged();
}

void TrafficModel::append(int direction, const QByteArray &payload, const QString &peer)
{
    if (payload.isEmpty() && peer.isEmpty())
        return;
    Row row;
    row.timestampMs = QDateTime::currentMSecsSinceEpoch();
    row.direction = direction;
    row.plain = false;
    row.payload = payload;
    row.peer = peer;
    // 同一端口在 15 ms 内连续到达的 RX 片段通常属于同一帧。合并发生在
    // 已有的模型待刷新队列中，界面只等待一个刷新窗口，不再叠加串口层定时器。
    if (direction == Rx && !m_pending.isEmpty()) {
        Row &last = m_pending.last();
        if (last.direction == Rx && !last.plain && last.peer == peer
            && row.timestampMs - last.timestampMs <= RxMergeWindowMs) {
            last.payload += payload;
            m_pendingBytes += payload.size();
            while (m_pending.size() > m_maxRows || m_pendingBytes > m_maxBytes) {
                m_pendingBytes -= m_pending.constFirst().payload.size();
                m_pending.removeFirst();
            }
            if (!m_paused && !m_flushTimer.isActive())
                m_flushTimer.start();
            return;
        }
    }
    m_pending.append(row);
    m_pendingBytes += payload.size();
    while (m_pending.size() > m_maxRows || m_pendingBytes > m_maxBytes) {
        m_pendingBytes -= m_pending.constFirst().payload.size();
        m_pending.removeFirst();
    }
    if (!m_paused && !m_flushTimer.isActive())
        m_flushTimer.start();
}

void TrafficModel::appendInfo(const QString &text, bool isError)
{
    Row row;
    row.timestampMs = QDateTime::currentMSecsSinceEpoch();
    row.direction = isError ? Error : Info;
    row.plain = true;
    row.payload = text.toUtf8();
    m_pending.append(row);
    m_pendingBytes += row.payload.size();
    while (m_pending.size() > m_maxRows || m_pendingBytes > m_maxBytes) {
        m_pendingBytes -= m_pending.constFirst().payload.size();
        m_pending.removeFirst();
    }
    if (!m_paused && !m_flushTimer.isActive())
        m_flushTimer.start();
}

QString TrafficModel::rowText(int row) const
{
    if (row < 0 || row >= m_rows.size())
        return QString();
    const Row &r = m_rows.at(row);
    const QString time = formatTime(r.timestampMs);
    QString dirText;
    switch (r.direction) {
    case Rx: dirText = QStringLiteral("RX"); break;
    case Tx: dirText = QStringLiteral("TX"); break;
    case Error: dirText = QStringLiteral("ERR"); break;
    default: dirText = QStringLiteral("INFO"); break;
    }
    QString body;
    if (r.plain) {
        body = QString::fromUtf8(r.payload);
    } else {
        body = toHexText(r.payload.left(MaxPayloadDisplay));
        if (r.payload.size() > MaxPayloadDisplay)
            body += QStringLiteral(" …(+%1 字节)").arg(r.payload.size() - MaxPayloadDisplay);
    }
    QString line = QStringLiteral("%1  %2  %3  [%4]").arg(time, dirText, body, r.peer);
    return line;
}

QString TrafficModel::payloadText(int row) const
{
    if (row < 0 || row >= m_rows.size())
        return {};
    const Row &item = m_rows.at(row);
    return item.plain ? QString::fromUtf8(item.payload) : toDisplayText(item.payload);
}

QString TrafficModel::payloadHex(int row) const
{
    if (row < 0 || row >= m_rows.size())
        return {};
    return toHexText(m_rows.at(row).payload);
}

QString TrafficModel::payloadAscii(int row) const
{
    if (row < 0 || row >= m_rows.size())
        return {};
    return toAsciiText(m_rows.at(row).payload);
}

bool TrafficModel::exportToFile(const QUrl &path) const
{
    const QString local = path.toLocalFile().isEmpty() ? path.toString() : path.toLocalFile();
    if (local.isEmpty())
        return false;
    QFile file(local);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    for (const Row &row : m_rows) {
        stream << formatTime(row.timestampMs) << '\t';
        switch (row.direction) {
        case Rx: stream << "RX"; break;
        case Tx: stream << "TX"; break;
        case Error: stream << "ERR"; break;
        default: stream << "INFO"; break;
        }
        stream << '\t' << row.payload.size() << '\t';
        if (row.plain) {
            stream << QString::fromUtf8(row.payload);
        } else {
            stream << toHexText(row.payload);
        }
        if (!row.peer.isEmpty())
            stream << '\t' << row.peer;
        stream << '\n';
    }
    return true;
}

void TrafficModel::flushPending()
{
    if (m_paused)
        return;
    if (m_pending.isEmpty())
        return;
    const QVector<Row> incoming = m_pending;
    m_pending.clear();
    m_pendingBytes = 0;

    // 先裁掉旧行腾出空间（保证索引有效），再追加新行；若单批数据本身
    // 超过 maxRows，则只保留该批的尾部。
    if (m_rows.size() + incoming.size() > m_maxRows && !m_rows.isEmpty()) {
        const int removeCount = qMin(m_rows.size(),
                                     m_rows.size() + incoming.size() - m_maxRows);
        beginRemoveRows(QModelIndex(), 0, removeCount - 1);
        for (int i = 0; i < removeCount; ++i)
            m_rowBytes -= m_rows.at(i).payload.size();
        m_rows.remove(0, removeCount);
        endRemoveRows();
    }
    QVector<Row> rowsToInsert = incoming;
    if (m_rows.size() + rowsToInsert.size() > m_maxRows)
        rowsToInsert = rowsToInsert.mid(rowsToInsert.size() - (m_maxRows - m_rows.size()));
    if (!rowsToInsert.isEmpty()) {
        beginInsertRows(QModelIndex(), m_rows.size(),
                        m_rows.size() + rowsToInsert.size() - 1);
        for (const Row &row : rowsToInsert)
            m_rows.append(row);
        for (const Row &row : rowsToInsert)
            m_rowBytes += row.payload.size();
        endInsertRows();
    }
    while (m_rows.size() > 1 && m_rowBytes > m_maxBytes) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_rowBytes -= m_rows.constFirst().payload.size();
        m_rows.removeFirst();
        endRemoveRows();
    }
    emit countChanged();
}

QString TrafficModel::formatTime(qint64 timestampMs)
{
    const QDateTime moment = QDateTime::fromMSecsSinceEpoch(timestampMs);
    return moment.toString(QStringLiteral("HH:mm:ss.zzz"));
}

QString TrafficModel::toHexText(const QByteArray &payload)
{
    static const char hexDigits[] = "0123456789ABCDEF";
    QString text;
    text.reserve(payload.size() * 3);
    for (int i = 0; i < payload.size(); ++i) {
        if (i > 0)
            text += QLatin1Char(' ');
        const quint8 byte = static_cast<quint8>(payload.at(i));
        text += QLatin1Char(hexDigits[byte >> 4]);
        text += QLatin1Char(hexDigits[byte & 0x0F]);
    }
    return text;
}

QString TrafficModel::toAsciiText(const QByteArray &payload)
{
    QString text;
    text.reserve(payload.size());
    for (char byte : payload) {
        const char c = byte;
        if (c == '\r')
            text += QStringLiteral("\\r");
        else if (c == '\n')
            text += QStringLiteral("\\n");
        else if (c == '\t')
            text += QStringLiteral("\\t");
        else if (c == '\0')
            text += QStringLiteral("\\0");
        else if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) >= 0x7F)
            text += QStringLiteral("\\x") + QString::number(static_cast<unsigned char>(c), 16).rightJustified(2, QLatin1Char('0')).toUpper();
        else
            text += QLatin1Char(c);
    }
    return text;
}

QString TrafficModel::toDisplayText(const QByteArray &payload)
{
    // TEXT is a readable UTF-8 view. HEX+ASCII remains byte-oriented so a
    // carriage return is never mistaken for a missing/garbled device response.
    const QString decoded = QString::fromUtf8(payload);
    QString result;
    result.reserve(decoded.size());
    for (qsizetype i = 0; i < decoded.size(); ++i) {
        const QChar c = decoded.at(i);
        if (c == QLatin1Char('\r') || c == QLatin1Char('\n')) {
            result += QLatin1Char('\n');
            if (i + 1 < decoded.size() && decoded.at(i + 1) != c
                && (decoded.at(i + 1) == QLatin1Char('\r') || decoded.at(i + 1) == QLatin1Char('\n')))
                ++i;
        } else if (c == QLatin1Char('\0'))
            result += QStringLiteral("\\0");
        else if ((c.unicode() < 0x20 && c != QLatin1Char('\n') && c != QLatin1Char('\t'))
                 || c.unicode() == 0x7f)
            result += QStringLiteral("\\x") + QString::number(c.unicode(), 16).rightJustified(2, QLatin1Char('0')).toUpper();
        else
            result += c;
    }
    return result;
}
