#include "trafficmodel.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QUrl>

namespace {
constexpr int FlushIntervalMs = 30;
constexpr int MaxPayloadDisplay = 512;
} // namespace

TrafficModel::TrafficModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_flushTimer.setInterval(FlushIntervalMs);
    connect(&m_flushTimer, &QTimer::timeout, this, &TrafficModel::flushPending);
    m_flushTimer.start();
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
        return row.plain ? QString::fromUtf8(shown) : toAsciiText(shown);
    }
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
    return roles;
}

void TrafficModel::setPaused(bool paused)
{
    if (m_paused == paused)
        return;
    m_paused = paused;
    emit pausedChanged();
    if (!m_paused)
        flushPending();
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
    m_pending.append(row);
    m_pendingBytes += payload.size();
    while (m_pending.size() > m_maxRows || m_pendingBytes > m_maxBytes) {
        m_pendingBytes -= m_pending.constFirst().payload.size();
        m_pending.removeFirst();
    }
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
            text += QStringLiteral("␍");
        else if (c == '\n')
            text += QStringLiteral("␊");
        else if (c == '\t')
            text += QStringLiteral("␉");
        else if (c == '\0')
            text += QStringLiteral("␀");
        else if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) == 0x7F)
            text += QChar(0x00B7);
        else
            text += QLatin1Char(c);
    }
    return text;
}
