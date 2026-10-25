#include "modbuspointmodel.h"
#include "core/modbuscodec.h"
#include "core/hexcodec.h"
#include "core/mthingspointtemplate.h"
#include <QDateTime>
#include <QSet>
#include <QUuid>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QGuiApplication>
#include <QClipboard>

namespace {
QByteArray csvEncode(const QVariantList &points)
{
    const QStringList fields{"name", "fc", "address", "count", "type", "displayType", "wordOrder", "scale", "offset", "unit", "minValue", "maxValue", "writeValue", "poll", "batchWrite", "rowVisible", "periodMs", "decimals", "group", "notes", "enumMap", "bitOffset", "bitCount", "timeoutMs", "retries"};
    QString text = fields.join(',') + "\r\n";
    for (const auto &entry : points) {
        const auto point = entry.toMap(); QStringList cells;
        for (const auto &field : fields) {
            QString value = field == "enumMap" ? QString::fromUtf8(QJsonDocument(QJsonObject::fromVariantMap(point.value(field).toMap())).toJson(QJsonDocument::Compact)) : point.value(field).toString();
            value.replace('"', "\"\""); cells.append('"' + value + '"');
        }
        text += cells.join(',') + "\r\n";
    }
    return text.toUtf8();
}
bool decodeRows(QString text, QChar separator, QList<QStringList> &rows)
{
    if (text.startsWith(QChar(0xfeff))) text.remove(0, 1);
    QStringList row; QString cell; bool quoted = false, closed = false;
    for (int i = 0; i < text.size(); ++i) {
        const QChar ch = text[i];
        if (quoted) {
            if (ch == '"') { if (i + 1 < text.size() && text[i + 1] == '"') { cell += '"'; ++i; } else { quoted = false; closed = true; } }
            else cell += ch;
        } else if (ch == '"' && cell.isEmpty() && !closed) quoted = true;
        else if (ch == separator || ch == '\n' || ch == '\r') {
            row.append(cell); cell.clear(); closed = false;
            if (ch != separator) { if (ch == '\r' && i + 1 < text.size() && text[i + 1] == '\n') ++i; rows.append(row); row.clear(); }
        } else { if (closed) return false; cell += ch; }
    }
    if (quoted) return false;
    if (!cell.isEmpty() || !row.isEmpty()) { row.append(cell); rows.append(row); }
    return !rows.isEmpty();
}
bool csvDecode(QString text, QVariantList &points)
{
    QList<QStringList> rows;
    if (!decodeRows(text, text.section('\n', 0, 0).contains('\t') ? '\t' : ',', rows)) return false;
    rows.removeIf([](const QStringList &row) { return row.size() == 1 && row.first().isEmpty(); });
    if (rows.isEmpty()) return false;
    const auto fields = rows.takeFirst(); QSet<QString> names;
    for (const auto &field : fields) { if (field.isEmpty() || names.contains(field)) return false; names.insert(field); }
    if (!names.contains("name") || !names.contains("address")) return false;
    for (const auto &values : rows) {
        if (values.size() != fields.size()) return false;
        QVariantMap point;
        for (int i = 0; i < fields.size(); ++i) {
            if (values[i].isEmpty()) continue;
            if (fields[i] == "enumMap") {
                QJsonParseError error; const auto enums = QJsonDocument::fromJson(values[i].toUtf8(), &error);
                if (error.error != QJsonParseError::NoError || !enums.isObject()) return false;
                point[fields[i]] = enums.object().toVariantMap();
            } else point[fields[i]] = values[i];
        }
        points.append(point);
    }
    return true;
}

// Convert cell text before normalization so invalid numbers cannot silently become zero.
bool setCellText(QVariantMap &point, const QString &field, const QString &text)
{
    if (field == "name" || field == "unit" || field == "writeValue") {
        point[field] = text;
        return field != "name" || !text.trimmed().isEmpty();
    }
    if (field == "range") {
        const QString value = text.trimmed();
        if (value.isEmpty() || value == "--") {
            point.remove("minValue"); point.remove("maxValue");
            return true;
        }
        const auto bounds = value.split(',');
        if (bounds.size() != 2) return false;
        bool lowOk = false, highOk = false;
        const double low = bounds[0].trimmed().toDouble(&lowOk);
        const double high = bounds[1].trimmed().toDouble(&highOk);
        if (!lowOk || !highOk || !std::isfinite(low) || !std::isfinite(high) || low > high) return false;
        point["minValue"] = low; point["maxValue"] = high;
        return true;
    }
    if (field == "scale" || field == "offset") {
        bool ok = false;
        const double value = text.trimmed().toDouble(&ok);
        if (!ok || !std::isfinite(value)) return false;
        point[field] = value;
        return true;
    }
    const QHash<QString, QPair<int, int>> limits{
        {"address", {0, 65535}}, {"count", {1, 125}}, {"bitOffset", {0, 63}},
        {"bitCount", {0, 64}}, {"decimals", {0, 12}}, {"periodMs", {0, 3600000}},
        {"timeoutMs", {-1, 60000}}, {"retries", {-1, 10}}};
    if (!limits.contains(field)) return false;
    bool ok = false;
    const int value = text.trimmed().toInt(&ok);
    const auto limit = limits.value(field);
    if (!ok || value < limit.first || value > limit.second
        || (field == "count" && point.value("fc").toInt() <= 2 && value != 1)) return false;
    point[field] = value;
    return true;
}
class PointFilter final : public QSortFilterProxyModel {
public:
    explicit PointFilter(ModbusPointModel *source) : QSortFilterProxyModel(source), m_source(source) { setSourceModel(source); }
    void refresh() { invalidateFilter(); }
protected:
    bool filterAcceptsRow(int row, const QModelIndex &) const override {
        const auto p = m_source->pointAt(row);
        const QString group = p.value("group").toString().trimmed();
        const QString selectedGroup = m_source->groupFilter();
        const bool groupMatch = selectedGroup.isEmpty()
            || (selectedGroup == QStringLiteral("__default__") && group.isEmpty())
            || group == selectedGroup;
        return groupMatch
            && p.value("rowVisible", true).toBool()
            && ((m_source->areaMask() & (1 << (p.value("fc").toInt() - 1))) != 0)
            && (m_source->rowFilterMode() == 0
                || (m_source->rowFilterMode() == 1 && p.value("poll", true).toBool())
                || (m_source->rowFilterMode() == 2 && (p.value("fc").toInt() == 1 || p.value("fc").toInt() == 3))
                || (m_source->rowFilterMode() == 3 && !p.value("status").toString().isEmpty()
                    && p.value("status").toString() != QStringLiteral("OK")))
            && (m_source->search().isEmpty()
                || p.value("name").toString().contains(m_source->search(), Qt::CaseInsensitive)
                || p.value("address").toString().contains(m_source->search())
                || group.contains(m_source->search(), Qt::CaseInsensitive));
    }
private:
    ModbusPointModel *m_source;
};
}
ModbusPointModel::ModbusPointModel(QObject *parent) : QAbstractTableModel(parent), m_filter(new PointFilter(this)) {}
ModbusPointModel::~ModbusPointModel()
{
    // Detach the proxy before QObject tears down the source model. This avoids
    // a late sourceModelDestroyed callback from a child proxy during workspace
    // channel destruction.
    if (m_filter) {
        m_filter->setSourceModel(nullptr);
        delete m_filter;
        m_filter = nullptr;
    }
}
void ModbusPointModel::setSearch(const QString &search) { if (search == m_search) return; m_search = search; static_cast<PointFilter *>(m_filter)->refresh(); emit filterChanged(); }
void ModbusPointModel::setArea(int area)
{
    if (area < 0 || area > 4) area = 0;
    const int mask = area == 0 ? 0x0F : (1 << (area - 1));
    if (area == m_area && mask == m_areaMask) return;
    m_area = area;
    m_areaMask = mask;
    static_cast<PointFilter *>(m_filter)->refresh();
    emit filterChanged();
}
void ModbusPointModel::setAreaMask(int mask)
{
    mask &= 0x0F;
    if (mask == m_areaMask) return;
    m_areaMask = mask;
    m_area = (mask == 1 ? 1 : mask == 2 ? 2 : mask == 4 ? 3 : mask == 8 ? 4 : 0);
    static_cast<PointFilter *>(m_filter)->refresh();
    emit filterChanged();
}
void ModbusPointModel::setRowFilterMode(int mode)
{
    mode = qBound(0, mode, 3);
    if (mode == m_rowFilterMode) return;
    m_rowFilterMode = mode;
    static_cast<PointFilter *>(m_filter)->refresh();
    emit filterChanged();
}
void ModbusPointModel::setGroupFilter(const QString &group)
{
    if (group == m_groupFilter) return;
    m_groupFilter = group;
    static_cast<PointFilter *>(m_filter)->refresh();
    emit filterChanged();
}
QVariantList ModbusPointModel::groups() const
{
    QVariantList result;
    QSet<QString> seen;
    for (const auto &point : m_points) {
        if (point.value("deviceId").toString() != m_deviceId) continue;
        const QString raw = point.value("group").toString().trimmed();
        const QString key = raw.isEmpty() ? QStringLiteral("__default__") : raw;
        if (seen.contains(key)) continue;
        seen.insert(key);
        int count = 0;
        for (const auto &candidate : m_points) {
            if (candidate.value("deviceId").toString() != m_deviceId) continue;
            const QString candidateGroup = candidate.value("group").toString().trimmed();
            if ((raw.isEmpty() && candidateGroup.isEmpty()) || (!raw.isEmpty() && candidateGroup == raw)) ++count;
        }
        result.append(QVariantMap{{"key", key}, {"name", raw.isEmpty() ? QStringLiteral("默认数据点") : raw}, {"count", count}});
    }
    return result;
}
QList<int> ModbusPointModel::visibleRows() const
{
    QList<int> rows;
    for (int i = 0; i < m_points.size(); ++i)
        if (m_points[i].value("deviceId").toString() == m_deviceId) rows.append(i);
    return rows;
}
int ModbusPointModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : visibleRows().size(); }
int ModbusPointModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 10; }
QHash<int, QByteArray> ModbusPointModel::roleNames() const { return {{Qt::DisplayRole, "display"}, {PointRole, "point"}}; }
QVariantMap ModbusPointModel::pointAt(int row) const
{
    const auto rows = visibleRows();
    if (row < 0 || row >= rows.size()) return {};
    auto point = m_points[rows[row]];
    point["sourceRow"] = row;
    point["selected"] = m_selected.contains(point.value("id").toString());
    const auto sample = m_samples.value(point.value("id").toString());
    for (auto it = sample.begin(); it != sample.end(); ++it) point.insert(it.key(), it.value());
    return point;
}
QVariant ModbusPointModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() >= 10) return {};
    const auto point = pointAt(index.row());
    if (point.isEmpty()) return {};
    if (role == PointRole) return point;
    if (role != Qt::DisplayRole) return {};
    const QStringList keys{"poll", "name", "fc", "address", "value", "writeValue", "type", "unit", "status", "updatedAt"};
    return point.value(keys[index.column()]);
}
void ModbusPointModel::setDeviceId(const QString &id)
{
    if (id == m_deviceId) return;
    beginResetModel(); m_deviceId = id; m_groupFilter.clear(); m_selected.clear(); m_selectionAnchor = -1; endResetModel(); emit deviceIdChanged(); emit groupsChanged(); emit selectionChanged();
}
bool ModbusPointModel::normalize(QVariantMap &p)
{
    bool valid = false;
    const int address = p.value("address").toInt(&valid);
    if (!valid || address < 0 || address > 65535) return false;
    const int fc = p.value("fc", 3).toInt();
    const QString type = p.value("type", "uint16").toString().toLower();
    const QStringList types{"bit", "uint16", "int16", "uint32", "int32", "float32", "uint64", "int64", "float64", "bcd", "bytes", "string"};
    const int minimumCount = type.endsWith("64") ? 4 : type.endsWith("32") ? 2 : 1;
    const int requestedCount = p.value("count", minimumCount).toInt();
    // MThings keeps Quantity independent from the presentation type for registers.
    // Bit areas are always one bit, while FC03/FC04 follow their protocol limits.
    const int count = fc <= 2 ? 1 : requestedCount;
    const int maximumCount = fc == 3 ? 123 : 125;
    const QString order = p.value("wordOrder", "ABCD").toString();
    if (fc < 1 || fc > 4 || !types.contains(type) || count < minimumCount
        || count > maximumCount || address + count > 65536
        || !QStringList{"ABCD", "BADC", "CDAB", "DCBA"}.contains(order)
        || (fc <= 2 && type != "bit")) return false;
    if (p.value("deviceId").toString().isEmpty() || p.value("name").toString().trimmed().isEmpty()) return false;
    p["type"] = type; p["count"] = count; p["fc"] = fc; p["address"] = address; p["wordOrder"] = order;
    p["scale"] = p.value("scale", 1); p["offset"] = p.value("offset", 0);
    // Zero means no point-specific delay; the scheduler applies the channel minimum.
    p["periodMs"] = qBound(0, p.value("periodMs", 1000).toInt(), 3600000);
    p["poll"] = p.value("poll", true);
    // Write participation is independent of selection, polling and row visibility.
    // Legacy/new points opt out; read-only areas cannot participate.
    p["batchWrite"] = (fc == 1 || fc == 3) && p.value("batchWrite", false).toBool();
    p["rowVisible"] = p.value("rowVisible", true).toBool();
    p["displayType"] = p.value("displayType", "DEC");
    if (!QStringList{"DEC", "HEX", "BIN", "FLOAT", "STRING", "ENUM", "TIMESTAMP", "BIT FIELD"}.contains(p.value("displayType").toString())) return false;
    p["decimals"] = qBound(0, p.value("decimals", 3).toInt(), 12);
    const int bitOffset = p.value("bitOffset", 0).toInt();
    const int bitCount = p.value("bitCount", type == "bit" ? 1 : 0).toInt();
    if (bitOffset < 0 || bitCount < 0 || bitCount > 64 || bitOffset + bitCount > qMin(64, count * 16)) return false;
    p["bitOffset"] = bitOffset; p["bitCount"] = bitCount;
    p["timeoutMs"] = p.value("timeoutMs", -1); p["retries"] = p.value("retries", -1);
    if (p.value("timeoutMs").toInt() < -1 || p.value("retries").toInt() < -1 || p.value("retries").toInt() > 10) return false;
    const double scale = p.value("scale").toDouble(&valid);
    if (!valid || !std::isfinite(scale)) return false;
    const double offsetValue = p.value("offset").toDouble(&valid);
    if (!valid || !std::isfinite(offsetValue)) return false;
    for (const auto &key : {"minValue", "maxValue"}) {
        if (!p.contains(key) || p.value(key).toString().trimmed().isEmpty()) {
            p.remove(key);
            continue;
        }
        const double limit = p.value(key).toDouble(&valid);
        if (!valid || !std::isfinite(limit)) return false;
        p[key] = limit;
    }
    if (p.contains("minValue") && p.contains("maxValue")
        && p.value("minValue").toDouble() > p.value("maxValue").toDouble()) return false;
    for (const auto &key : {"value", "status", "updatedAt", "rawHex", "engineering", "sourceRow", "selected"}) p.remove(key);
    return true;
}
bool ModbusPointModel::savePoint(int row, const QVariantMap &point)
{
    auto p = point;
    const auto rows = visibleRows();
    if (row < -1 || row >= rows.size()) return false;
    p["deviceId"] = m_deviceId;
    p["id"] = row < 0 ? QUuid::createUuid().toString(QUuid::WithoutBraces) : m_points[rows[row]].value("id");
    if (!normalize(p)) { emit validationError(QStringLiteral("Invalid point name, area, address or type / 数据点配置无效")); return false; }
    beginResetModel();
    if (row < 0) m_points.append(p); else { m_samples.remove(p.value("id").toString()); m_history.remove(p.value("id").toString()); m_points[rows[row]] = p; }
    endResetModel(); emit definitionsChanged(); emit groupsChanged(); return true;
}
void ModbusPointModel::removePoint(int row)
{
    const auto rows = visibleRows();
    if (row < 0 || row >= rows.size()) return;
    const QString id = m_points[rows[row]].value("id").toString();
    const bool selectionChanged = m_selected.remove(id);
    beginResetModel(); m_samples.remove(id); m_history.remove(id); m_points.removeAt(rows[row]); endResetModel(); emit definitionsChanged(); emit groupsChanged();
    if (selectionChanged) emit ModbusPointModel::selectionChanged();
}
void ModbusPointModel::copyPoint(int row)
{
    auto p = pointAt(row); if (p.isEmpty()) return;
    p["name"] = p.value("name").toString() + " (copy)"; savePoint(-1, p);
}
QVariantList ModbusPointModel::definitions() const { QVariantList result; for (const auto &p : m_points) result.append(p); return result; }
bool ModbusPointModel::restore(const QVariantList &points)
{
    QVector<QVariantMap> restored; QSet<QString> ids;
    for (const auto &entry : points) {
        auto p = entry.toMap(); const auto id = p.value("id").toString();
        if (id.isEmpty() || ids.contains(id) || !normalize(p)) return false;
        ids.insert(id); restored.append(p);
    }
    const bool hadSelection = !m_selected.isEmpty();
    beginResetModel(); m_points = restored; m_samples.clear(); m_history.clear(); m_selected.clear(); m_selectionAnchor = -1; endResetModel(); emit groupsChanged();
    if (hadSelection) emit selectionChanged();
    return true;
}
bool ModbusPointModel::hasDevice(const QString &id) const { for (const auto &p : m_points) if (p.value("deviceId") == id) return true; return false; }

bool ModbusPointModel::removeDevice(const QString &id)
{
    if (id.isEmpty()) return false;
    bool removed = false;
    beginResetModel();
    for (int index = m_points.size() - 1; index >= 0; --index) {
        if (m_points[index].value("deviceId").toString() != id) continue;
        const QString pointId = m_points[index].value("id").toString();
        m_samples.remove(pointId);
        m_history.remove(pointId);
        m_selected.remove(pointId);
        m_points.removeAt(index);
        removed = true;
    }
    endResetModel();
    if (removed) {
        emit definitionsChanged();
        emit groupsChanged();
        emit selectionChanged();
    }
    return removed;
}

int ModbusPointModel::findNext(const QString &name, int afterProxyRow) const
{
    const QString needle = name.trimmed();
    if (needle.isEmpty() || !m_filter)
        return -1;

    const int count = m_filter->rowCount();
    if (count <= 0)
        return -1;

    const int first = afterProxyRow >= 0 && afterProxyRow < count
        ? (afterProxyRow + 1) % count
        : 0;
    for (int offset = 0; offset < count; ++offset) {
        const int row = (first + offset) % count;
        const QVariantMap point = m_filter->data(m_filter->index(row, 0), PointRole).toMap();
        if (point.value(QStringLiteral("name")).toString().contains(needle, Qt::CaseSensitive))
            return row;
    }
    return -1;
}
int ModbusPointModel::suggestedAddress(int functionCode) const
{
    Q_UNUSED(functionCode)
    int address = 0;
    for (const auto &point : m_points) {
        if (point.value(QStringLiteral("deviceId")).toString() != m_deviceId) continue;
        address = qMax(address, point.value(QStringLiteral("address")).toInt()
                                + qMax(1, point.value(QStringLiteral("count"), 1).toInt()));
    }
    return qMin(address, 65535);
}

bool ModbusPointModel::addPoints(int count, int functionCode, int startAddress)
{
    if (m_deviceId.isEmpty() || count < 1 || count > 4096 || functionCode < 1
        || functionCode > 4 || startAddress < 0 || startAddress + count > 65536) {
        emit validationError(QStringLiteral("Invalid data count, block or starting address / 数据条数、区块或起始地址无效"));
        return false;
    }
    QVector<QVariantMap> additions;
    additions.reserve(count);
    const QStringList prefixes{QStringLiteral("线圈"), QStringLiteral("离散输入"),
                               QStringLiteral("保持寄存器"), QStringLiteral("输入寄存器")};
    for (int index = 0; index < count; ++index) {
        QVariantMap point{{QStringLiteral("id"), QUuid::createUuid().toString(QUuid::WithoutBraces)},
                          {QStringLiteral("deviceId"), m_deviceId},
                          {QStringLiteral("name"), QStringLiteral("%1 %2").arg(prefixes.at(functionCode - 1)).arg(startAddress + index)},
                          {QStringLiteral("fc"), functionCode},
                          {QStringLiteral("address"), startAddress + index},
                          {QStringLiteral("count"), 1},
                          {QStringLiteral("type"), functionCode <= 2 ? QStringLiteral("bit") : QStringLiteral("uint16")},
                          {QStringLiteral("wordOrder"), QStringLiteral("ABCD")},
                          {QStringLiteral("scale"), 1.0},
                          {QStringLiteral("offset"), 0.0},
                          {QStringLiteral("periodMs"), 1000},
                          {QStringLiteral("poll"), true}};
        if (!normalize(point)) return false;
        additions.append(point);
    }
    beginResetModel();
    m_points.append(additions);
    endResetModel();
    emit definitionsChanged();
    emit groupsChanged();
    return true;
}

QVariantList ModbusPointModel::pollingPlans(int strategy, int maxGap, int maxRegisters) const
{
    return pollingPlansForDevice(m_deviceId, strategy, maxGap, maxGap, 2000, maxRegisters);
}

QVariantList ModbusPointModel::pollingPlansForDevice(const QString &deviceId, int strategy,
                                                     int maxBitGap, int maxRegisterGap,
                                                     int maxBits, int maxRegisters) const
{
    QVector<QVariantMap> points;
    for (const auto &p : m_points)
        if (p.value("deviceId").toString() == deviceId && p.value("poll").toBool()) points.append(p);
    if (strategy == 0) std::stable_sort(points.begin(), points.end(), [](const auto &a, const auto &b) {
        if (a.value("fc") != b.value("fc")) return a.value("fc").toInt() < b.value("fc").toInt();
        if (a.value("periodMs") != b.value("periodMs")) return a.value("periodMs").toInt() < b.value("periodMs").toInt();
        if (a.value("address") != b.value("address")) return a.value("address").toInt() < b.value("address").toInt();
        if (a.value("bitOffset") != b.value("bitOffset")) return a.value("bitOffset").toInt() < b.value("bitOffset").toInt();
        return a.value("bitCount").toInt() < b.value("bitCount").toInt();
    });
    QVariantList plans;
    for (const auto &p : points) {
        if (strategy != 1 && !plans.isEmpty()) {
            auto previous = plans.last().toMap();
            const int start = previous.value("address").toInt();
            const int end = start + previous.value("count").toInt();
            const int pointStart = p.value("address").toInt();
            const int pointEnd = pointStart + p.value("count").toInt();
            const bool bitArea = p.value("fc").toInt() <= 2;
            const int allowedGap = qBound(0, bitArea ? maxBitGap : maxRegisterGap,
                                          bitArea ? 1999 : 124);
            const int allowedCount = qBound(1, bitArea ? maxBits : maxRegisters,
                                            bitArea ? 2000 : 125);
            if (previous.value("fc") == p.value("fc") && previous.value("periodMs") == p.value("periodMs")
                && previous.value("timeoutMs") == p.value("timeoutMs") && previous.value("retries") == p.value("retries")
                && pointStart >= start && pointStart <= end + allowedGap
                && qMax(end, pointEnd) - start <= allowedCount) {
                previous["count"] = qMax(end, pointEnd) - start;
                previous["type"] = "uint16"; previous["scale"] = 1; previous["offset"] = 0;
                previous["name"] = QStringLiteral("FC%1 @%2 +%3").arg(p.value("fc").toInt()).arg(start).arg(previous.value("count").toInt());
                plans.last() = previous;
                continue;
            }
        }
        auto task = p; task["pointId"] = p.value("id"); plans.append(task);
    }
    return plans;
}

bool ModbusPointModel::exportFile(const QUrl &url, const QVariantMap &metadata) const
{
    if (!url.isLocalFile()) return false;
    QVariantList points;
    for (auto p : m_points) {
        if (p.value("deviceId").toString() != m_deviceId) continue;
        p.remove("id"); p.remove("deviceId");
        points.append(p);
    }
    QByteArray content;
    if (url.toLocalFile().endsWith(".dtmthings", Qt::CaseInsensitive)) {
        content = MThingsPointTemplate::encode(points, metadata);
        QVariantList decoded; QString error;
        // Refuse export if this configuration cannot be read back faithfully by the supported codec.
        if (!MThingsPointTemplate::decode(content, decoded, error)) return false;
        for (int i = 0; i < decoded.size(); ++i)
            if (decoded[i].toMap().value("type") != points[i].toMap().value("type")) return false;
    } else if (url.toLocalFile().endsWith(".csv", Qt::CaseInsensitive)) content = csvEncode(points);
    else content = QJsonDocument(QJsonObject{{"schema", 1}, {"profile", QJsonObject::fromVariantMap(metadata)},
                                            {"points", QJsonArray::fromVariantList(points)}}).toJson();
    QSaveFile file(url.toLocalFile());
    return file.open(QIODevice::WriteOnly) && file.write(content) == content.size() && file.commit();
}

bool ModbusPointModel::importFile(const QUrl &url)
{
    QFile file(url.toLocalFile());
    if (!url.isLocalFile() || !file.open(QIODevice::ReadOnly) || file.size() > 16 * 1024 * 1024) {
        emit validationError("Cannot read point template / 无法读取点表模板"); return false;
    }
    QJsonParseError error;
    const auto bytes = file.readAll();
    if (url.toLocalFile().endsWith(".dtmthings", Qt::CaseInsensitive)) {
        QVariantList points; QString error;
        if (!MThingsPointTemplate::decode(bytes, points, error)) {
            emit validationError(error); return false;
        }
        return appendDefinitions(points);
    }
    QJsonDocument doc;
    if (url.toLocalFile().endsWith(".csv", Qt::CaseInsensitive)) {
        QVariantList points;
        if (!csvDecode(QString::fromUtf8(bytes), points)) { emit validationError("Invalid CSV / CSV 格式无效"); return false; }
        doc = QJsonDocument(QJsonObject{{"schema", 1}, {"points", QJsonArray::fromVariantList(points)}});
    } else doc = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject() || doc.object().value("schema").toInt() != 1 || !doc.object().value("points").isArray()) {
        emit validationError("Invalid point template / 点表模板格式无效"); return false;
    }
    return appendDefinitions(doc.object().value("points").toArray().toVariantList());
}

bool ModbusPointModel::appendDefinitions(const QVariantList &definitions)
{
    QVector<QVariantMap> imported;
    for (const auto &entry : definitions) {
        auto p = entry.toMap();
        p["id"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
        p["deviceId"] = m_deviceId;
        if (!normalize(p)) { emit validationError("Invalid point in template / 模板含无效数据点"); return false; }
        imported.append(p);
    }
    if (imported.isEmpty()) return true;
    beginResetModel(); m_points.append(imported); endResetModel(); emit definitionsChanged(); emit groupsChanged(); return true;
}

bool ModbusPointModel::pasteClipboard()
{
    QVariantList definitions;
    if (!csvDecode(QGuiApplication::clipboard()->text(), definitions)) { emit validationError("Paste CSV/TSV with name,address headers / 请粘贴含 name,address 表头的数据"); return false; }
    return appendDefinitions(definitions);
}

void ModbusPointModel::selectPoint(int row, bool additive, bool range)
{
    const auto rows = visibleRows();
    if (row < 0 || row >= rows.size()) return;
    if (!additive && !range) m_selected.clear();
    if (range && m_selectionAnchor >= 0 && m_selectionAnchor < rows.size()) {
        for (int i = qMin(row, m_selectionAnchor); i <= qMax(row, m_selectionAnchor); ++i)
            m_selected.insert(m_points[rows[i]].value("id").toString());
    } else {
        const auto id = m_points[rows[row]].value("id").toString();
        if (additive && m_selected.contains(id)) m_selected.remove(id); else m_selected.insert(id);
        m_selectionAnchor = row;
    }
    if (rowCount()) emit dataChanged(index(0, 0), index(rowCount() - 1, 9));
    emit selectionChanged();
}

void ModbusPointModel::toggleSelectAll()
{
    const auto rows = visibleRows();
    if (rows.isEmpty()) return;
    bool allSelected = true;
    for (const int row : rows) {
        if (!m_selected.contains(m_points[row].value(QStringLiteral("id")).toString())) {
            allSelected = false;
            break;
        }
    }
    for (const int row : rows) {
        const QString id = m_points[row].value(QStringLiteral("id")).toString();
        if (allSelected) m_selected.remove(id); else m_selected.insert(id);
    }
    m_selectionAnchor = -1;
    if (rowCount()) emit dataChanged(index(0, 0), index(rowCount() - 1, 9));
    emit selectionChanged();
}

int ModbusPointModel::selectedQuantityLimit() const
{
    if (m_selected.isEmpty()) return 0;
    int limit = 125;
    for (const auto &point : m_points) {
        if (!m_selected.contains(point.value(QStringLiteral("id")).toString())) continue;
        const int fc = point.value(QStringLiteral("fc"), 3).toInt();
        if (fc <= 2) return 1;
        limit = qMin(limit, fc == 3 ? 123 : 125);
        limit = qMin(limit, 65536 - point.value(QStringLiteral("address")).toInt());
    }
    return qMax(1, limit);
}

bool ModbusPointModel::updateSelected(const QVariantMap &changes)
{
    if (m_selected.isEmpty()) {
        emit validationError("Select data points first / 请先选择数据点");
        return false;
    }
    auto updated = m_points;
    for (auto &point : updated) {
        if (!m_selected.contains(point.value("id").toString())) continue;
        for (auto it = changes.begin(); it != changes.end(); ++it)
            if (it.key() != "id" && it.key() != "deviceId") point[it.key()] = it.value();
        if (!normalize(point)) { emit validationError("Invalid batch changes / 批量修改包含无效配置"); return false; }
    }
    beginResetModel(); m_points = updated; for (const auto &id : m_selected) { m_samples.remove(id); m_history.remove(id); } endResetModel(); emit definitionsChanged(); return true;
}

bool ModbusPointModel::updateSelectedField(const QString &field, const QString &text)
{
    if (m_selected.isEmpty()) return false;
    auto updated = m_points;
    for (auto &point : updated) {
        if (!m_selected.contains(point.value("id").toString())) continue;
        if (!setCellText(point, field, text) || !normalize(point)) {
            emit validationError(QStringLiteral("批量修改包含无效配置，未修改任何数据 / Invalid batch value"));
            return false;
        }
    }
    beginResetModel();
    m_points = updated;
    for (const auto &id : m_selected) { m_samples.remove(id); m_history.remove(id); }
    endResetModel();
    emit definitionsChanged();
    emit groupsChanged();
    return true;
}

bool ModbusPointModel::pasteCellsFromClipboard(int sourceRow, const QStringList &fields)
{
    return pasteCells(sourceRow, fields, QGuiApplication::clipboard()->text());
}

bool ModbusPointModel::pasteCells(int sourceRow, const QStringList &fields, const QString &text)
{
    const auto rows = visibleRows();
    if (sourceRow < 0 || sourceRow >= rows.size() || fields.isEmpty()) return false;
    const QModelIndex start = m_filter->mapFromSource(index(sourceRow, 0));
    QList<QStringList> cells;
    if (!start.isValid() || !decodeRows(text, '\t', cells)
        || start.row() + cells.size() > m_filter->rowCount()) {
        emit validationError(QStringLiteral("粘贴内容超出可见数据行或格式无效 / Invalid paste range"));
        return false;
    }
    auto updated = m_points;
    QSet<QString> changed;
    for (int row = 0; row < cells.size(); ++row) {
        const int source = m_filter->mapToSource(m_filter->index(start.row() + row, 0)).row();
        auto &point = updated[rows[source]];
        if (cells[row].size() > fields.size()) {
            emit validationError(QStringLiteral("粘贴内容超出可编辑列 / Paste exceeds editable columns"));
            return false;
        }
        for (int column = 0; column < cells[row].size(); ++column) {
            if (!setCellText(point, fields[column], cells[row][column])) {
                emit validationError(QStringLiteral("第 %1 行第 %2 列不可编辑或数值无效；未修改任何数据")
                                     .arg(row + 1).arg(column + 1));
                return false;
            }
        }
        if (!normalize(point)) {
            emit validationError(QStringLiteral("第 %1 行配置无效；未修改任何数据").arg(row + 1));
            return false;
        }
        changed.insert(point.value("id").toString());
    }
    beginResetModel();
    m_points = updated;
    for (const auto &id : changed) { m_samples.remove(id); m_history.remove(id); }
    endResetModel();
    emit definitionsChanged();
    emit groupsChanged();
    return true;
}

bool ModbusPointModel::setWriteValue(int row, const QString &value)
{
    const auto rows = visibleRows();
    if (row < 0 || row >= rows.size()) return false;
    const int source = rows[row];
    m_points[source][QStringLiteral("writeValue")] = value;
    emit dataChanged(index(row, 0), index(row, 9), {Qt::DisplayRole, PointRole});
    emit definitionsChanged();
    return true;
}

void ModbusPointModel::deleteSelected()
{
    beginResetModel();
    for (int i = m_points.size() - 1; i >= 0; --i)
        if (m_selected.contains(m_points[i].value("id").toString())) { m_samples.remove(m_points[i].value("id").toString()); m_history.remove(m_points[i].value("id").toString()); m_points.removeAt(i); }
    m_selected.clear(); m_selectionAnchor = -1; endResetModel(); emit definitionsChanged(); emit groupsChanged(); emit selectionChanged();
}

void ModbusPointModel::copySelectedToDevice(const QString &deviceId)
{
    if (deviceId.isEmpty()) return;
    QVector<QVariantMap> copies;
    for (const auto &point : m_points) {
        if (!m_selected.contains(point.value("id").toString())) continue;
        auto copy = point; copy["id"] = QUuid::createUuid().toString(QUuid::WithoutBraces); copy["deviceId"] = deviceId; copies.append(copy);
    }
    beginResetModel(); m_points.append(copies); endResetModel(); emit definitionsChanged(); emit groupsChanged();
}

bool ModbusPointModel::copyCurrentDeviceToDevice(const QString &deviceId)
{
    if (m_deviceId.isEmpty() || deviceId.isEmpty() || deviceId == m_deviceId) return false;
    QVariantList source;
    for (const auto &point : m_points)
        if (point.value(QStringLiteral("deviceId")).toString() == m_deviceId) source.append(point);
    return replaceDevicePoints(deviceId, source);
}

bool ModbusPointModel::replaceDevicePoints(const QString &deviceId, const QVariantList &sourcePoints)
{
    if (deviceId.isEmpty())
        return false;
    QVector<QVariantMap> replacements;
    for (const auto &entry : sourcePoints) {
        QVariantMap point = entry.toMap();
        point[QStringLiteral("id")] = QUuid::createUuid().toString(QUuid::WithoutBraces);
        point[QStringLiteral("deviceId")] = deviceId;
        if (!normalize(point))
            return false;
        replacements.append(point);
    }
    bool selectionChanged = false;
    beginResetModel();
    for (int index = m_points.size() - 1; index >= 0; --index) {
        if (m_points.at(index).value(QStringLiteral("deviceId")).toString() != deviceId) continue;
        const QString id = m_points.at(index).value(QStringLiteral("id")).toString();
        m_samples.remove(id);
        m_history.remove(id);
        selectionChanged = m_selected.remove(id) || selectionChanged;
        m_points.removeAt(index);
    }
    for (const auto &point : std::as_const(replacements))
        m_points.append(point);
    endResetModel();
    emit definitionsChanged();
    emit groupsChanged();
    if (selectionChanged)
        emit ModbusPointModel::selectionChanged();
    return true;
}

bool ModbusPointModel::togglePointBatchWrite(int row)
{
    const auto rows = visibleRows();
    if (row < 0 || row >= rows.size()) return false;
    auto &point = m_points[rows[row]];
    const int fc = point.value("fc").toInt();
    if (fc != 1 && fc != 3) return false;
    point["batchWrite"] = !point.value("batchWrite", false).toBool();
    emit dataChanged(index(row, 0), index(row, columnCount() - 1), {Qt::DisplayRole, PointRole});
    emit definitionsChanged();
    return true;
}

bool ModbusPointModel::togglePointPolling(int row)
{
    const auto rows = visibleRows();
    if (row < 0 || row >= rows.size())
        return false;
    const int source = rows[row];
    m_points[source][QStringLiteral("poll")] = !m_points[source].value(QStringLiteral("poll"), true).toBool();
    emit dataChanged(index(row, 0), index(row, columnCount() - 1), {Qt::DisplayRole, PointRole});
    emit definitionsChanged();
    return true;
}

bool ModbusPointModel::setVisiblePointIds(const QStringList &visibleIds)
{
    if (m_deviceId.isEmpty())
        return false;
    const QSet<QString> ids(visibleIds.begin(), visibleIds.end());
    bool changed = false;
    for (const auto &point : std::as_const(m_points)) {
        if (point.value(QStringLiteral("deviceId")).toString() == m_deviceId
            && point.value(QStringLiteral("rowVisible"), true).toBool()
                   != ids.contains(point.value(QStringLiteral("id")).toString())) {
            changed = true;
            break;
        }
    }
    if (!changed)
        return true;
    beginResetModel();
    for (auto &point : m_points) {
        if (point.value(QStringLiteral("deviceId")).toString() == m_deviceId)
            point[QStringLiteral("rowVisible")] = ids.contains(point.value(QStringLiteral("id")).toString());
    }
    endResetModel();
    emit definitionsChanged();
    return true;
}

QVariantMap ModbusPointModel::parsePdu(int startAddress, const QString &pduText) const
{
    QByteArray pdu;
    QString error;
    if (!HexCodec::parseHex(pduText, pdu, &error) || pdu.size() < 2)
        return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), error.isEmpty() ? QStringLiteral("PDU 至少需要功能码和数据") : error}};
    const int fc = quint8(pdu.at(0));
    QVariantList results;
    if (fc >= 1 && fc <= 4) {
        const int byteCount = quint8(pdu.at(1));
        if (byteCount != pdu.size() - 2)
            return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), QStringLiteral("PDU 字节数与长度不一致")}};
        QVector<quint16> registers;
        if (fc >= 3) {
            if ((byteCount & 1) != 0)
                return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), QStringLiteral("寄存器响应字节数必须为偶数")}};
            for (int offset = 2; offset + 1 < pdu.size(); offset += 2)
                registers.append((quint16(quint8(pdu.at(offset))) << 8) | quint8(pdu.at(offset + 1)));
        }
        for (const auto &point : m_points) {
            if (point.value(QStringLiteral("deviceId")).toString() != m_deviceId
                || point.value(QStringLiteral("fc")).toInt() != fc) continue;
            const int offset = point.value(QStringLiteral("address")).toInt() - startAddress;
            QVariant value;
            QString raw;
            if (fc <= 2) {
                if (offset < 0 || offset >= byteCount * 8) continue;
                const bool bit = (quint8(pdu.at(2 + offset / 8)) & (1u << (offset % 8))) != 0;
                value = bit;
                raw = bit ? QStringLiteral("1") : QStringLiteral("0");
            } else {
                const auto decoded = ModbusCodec::decodeRegisters(registers, offset,
                    point.value(QStringLiteral("type"), QStringLiteral("uint16")).toString(),
                    point.value(QStringLiteral("wordOrder"), QStringLiteral("ABCD")).toString(),
                    point.value(QStringLiteral("scale"), 1.0).toDouble(),
                    point.value(QStringLiteral("offset"), 0.0).toDouble());
                if (decoded.isEmpty()) continue;
                value = decoded.value(QStringLiteral("value"));
                raw = decoded.value(QStringLiteral("rawHex")).toString();
            }
            results.append(QVariantMap{{QStringLiteral("name"), point.value(QStringLiteral("name"))},
                                       {QStringLiteral("address"), point.value(QStringLiteral("address"))},
                                       {QStringLiteral("value"), value}, {QStringLiteral("raw"), raw}});
        }
    } else if (fc == 5 || fc == 6 || fc == 15 || fc == 16) {
        if (pdu.size() < 5)
            return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), QStringLiteral("写请求 PDU 长度不足")}};
        const int address = (quint16(quint8(pdu.at(1))) << 8) | quint8(pdu.at(2));
        const int value = (quint16(quint8(pdu.at(3))) << 8) | quint8(pdu.at(4));
        results.append(QVariantMap{{QStringLiteral("name"), QStringLiteral("写请求")},
                                   {QStringLiteral("address"), address},
                                   {QStringLiteral("value"), value},
                                   {QStringLiteral("raw"), QStringLiteral("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper()}});
    } else {
        return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), QStringLiteral("暂不支持 FC%1").arg(fc)}};
    }
    return {{QStringLiteral("ok"), true}, {QStringLiteral("functionCode"), fc},
            {QStringLiteral("results"), results}};
}

void ModbusPointModel::movePoint(int from, int to)
{
    const auto rows = visibleRows();
    if (from < 0 || to < 0 || from >= rows.size() || to >= rows.size() || from == to) return;
    beginResetModel(); m_points.move(rows[from], rows[to]); m_selectionAnchor = -1; endResetModel(); emit definitionsChanged();
}

void ModbusPointModel::sortByAddress()
{
    const auto rows = visibleRows();
    if (rows.size() < 2) return;
    QVector<QVariantMap> sorted;
    sorted.reserve(rows.size());
    for (int row : rows) sorted.append(m_points[row]);
    std::stable_sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) {
        if (a.value("fc") != b.value("fc")) return a.value("fc").toInt() < b.value("fc").toInt();
        if (a.value("address") != b.value("address")) return a.value("address").toInt() < b.value("address").toInt();
        if (a.value("bitOffset") != b.value("bitOffset")) return a.value("bitOffset").toInt() < b.value("bitOffset").toInt();
        return a.value("bitCount").toInt() < b.value("bitCount").toInt();
    });
    beginResetModel();
    for (int i = 0; i < rows.size(); ++i) m_points[rows[i]] = sorted[i];
    m_selectionAnchor = -1; endResetModel(); emit definitionsChanged();
}
void ModbusPointModel::receive(const QString &device, int fc, int start, const QVector<quint16> &values)
{
    for (const auto &p : m_points) {
        const int offset = p.value("address").toInt() - start;
        if (p.value("deviceId").toString() != device || p.value("fc").toInt() != fc || offset < 0 || offset + p.value("count").toInt() > values.size()) continue;
        const auto type = p.value("type").toString();
        const auto order = p.value("wordOrder").toString();
        QByteArray bytes;
        QVector<quint16> words = values.mid(offset, p.value("count").toInt());
        if (order == "CDAB" || order == "DCBA") std::reverse(words.begin(), words.end());
        for (quint16 word : words) {
            if (order == "BADC" || order == "DCBA") { bytes.append(char(word)); bytes.append(char(word >> 8)); }
            else { bytes.append(char(word >> 8)); bytes.append(char(word)); }
        }
        quint64 raw = 0;
        for (char byte : bytes.first(qMin<qsizetype>(8, bytes.size()))) raw = (raw << 8) | quint8(byte);
        const int bitCount = p.value("bitCount").toInt();
        if (bitCount > 0) {
            raw >>= p.value("bitOffset").toInt();
            if (bitCount < 64) raw &= (quint64(1) << bitCount) - 1;
        }
        QVariantMap sample{{"rawHex", QString::fromLatin1(bytes.toHex(' ').toUpper())}};
        QVariant value;
        double numeric = 0;
        bool numericType = true;
        bool decodeOk = true;
        if (type == "string") { value = QString::fromUtf8(bytes).remove(QChar(0)); numericType = false; }
        else if (type == "bytes") { value = sample.value("rawHex"); numericType = false; }
        else if (type == "bcd") {
            for (char byte : bytes) {
                const int high = quint8(byte) >> 4, low = quint8(byte) & 15;
                if (high > 9 || low > 9) decodeOk = false;
                numeric = numeric * 100 + high * 10 + low;
            }
        } else if (type == "float64") { std::memcpy(&numeric, &raw, sizeof(numeric)); }
        else if (type == "float32") { const quint32 bits = quint32(raw); float f; std::memcpy(&f, &bits, sizeof(f)); numeric = f; }
        else if (type == "int16") numeric = qint16(raw);
        else if (type == "int32") numeric = qint32(raw);
        else if (type == "int64") { qint64 signedRaw; std::memcpy(&signedRaw, &raw, sizeof(raw)); numeric = double(signedRaw); value = QString::number(signedRaw); }
        else { numeric = double(raw); if (type == "uint64") value = QString::number(raw); }
        const auto display = p.value("displayType").toString();
        if (numericType) {
            const double scale = p.value("scale").toDouble(), bias = p.value("offset").toDouble();
            const double engineering = numeric * scale + bias;
            if (!std::isfinite(engineering)) decodeOk = false;
            if (!value.isValid() || scale != 1 || bias != 0) value = engineering;
            sample["engineering"] = engineering;
            if (display == "HEX") value = "0x" + QString::number(raw, 16).toUpper();
            else if (display == "BIN" || display == "BIT FIELD") value = QString::number(raw, 2).rightJustified(qMin(64, int(bytes.size() * 8)), '0');
            else if (display == "FLOAT") value = QString::number(engineering, 'f', p.value("decimals").toInt());
            else if (display == "ENUM") value = p.value("enumMap").toMap().value(QString::number(raw), value);
            else if (display == "TIMESTAMP") {
                if (numeric < -62135596800.0 || numeric > 253402300799.0) decodeOk = false;
                else value = QDateTime::fromSecsSinceEpoch(qint64(numeric), Qt::UTC).toString(Qt::ISODate);
            }
            else if (display == "STRING") value = value.toString();
        }
        const double engineering = sample.value("engineering").toDouble();
        const bool inRange = !numericType || ((!p.contains("minValue") || engineering >= p.value("minValue").toDouble())
            && (!p.contains("maxValue") || engineering <= p.value("maxValue").toDouble()));
        sample["rangeOk"] = inRange;
        sample["value"] = decodeOk ? value : QVariant(QStringLiteral("Decode error"));
        if (numericType && decodeOk) {
            auto &history = m_history[p.value("id").toString()];
            history.append(QPointF(QDateTime::currentMSecsSinceEpoch(), sample.value("engineering").toDouble()));
            if (history.size() > 10000) history.remove(0, history.size() - 10000);
        }
        sample["status"] = !decodeOk ? "DECODE ERROR" : inRange ? "OK" : "OUT OF RANGE";
        sample["updatedAt"] = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
        m_samples[p.value("id").toString()] = sample;
    }
    if (rowCount()) emit dataChanged(index(0, 0), index(rowCount() - 1, 9));
}

QVariantList ModbusPointModel::trendSeries(int windowMs) const
{
    QVariantList result;
    const qint64 cutoff = QDateTime::currentMSecsSinceEpoch() - qBound(1000, windowMs, 86400000);
    for (const auto &p : m_points) {
        const auto id = p.value("id").toString();
        if (p.value("deviceId").toString() != m_deviceId || (!m_selected.isEmpty() && !m_selected.contains(id))) continue;
        const auto history = m_history.value(id);
        QVariantList samples; double sum = 0, low = 0, high = 0; int count = 0;
        for (const auto &sample : history) {
            if (sample.x() < cutoff) continue;
            if (!count) low = high = sample.y();
            low = qMin(low, sample.y()); high = qMax(high, sample.y()); sum += sample.y(); ++count;
            samples.append(QVariantMap{{"time", sample.x()}, {"value", sample.y()}});
        }
        if (samples.size() > 2000) {
            QVariantList reduced; const int step = (samples.size() + 1998) / 1999;
            for (int i = 0; i < samples.size(); i += step) reduced.append(samples[i]);
            reduced.append(samples.last()); samples = reduced;
        }
        result.append(QVariantMap{{"id", id}, {"name", p.value("name")}, {"unit", p.value("unit")},
            {"samples", samples}, {"min", low}, {"max", high}, {"avg", count ? sum / count : 0}, {"count", count}});
        if (result.size() >= 50) break;
    }
    return result;
}

bool ModbusPointModel::exportHistory(const QUrl &url) const
{
    if (!url.isLocalFile()) return false;
    QSaveFile file(url.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) return false;
    QByteArray output("deviceId,pointId,name,timeMs,value\r\n");
    for (const auto &p : m_points) {
        const auto id = p.value("id").toString();
        if (p.value("deviceId").toString() != m_deviceId || (!m_selected.isEmpty() && !m_selected.contains(id))) continue;
        QString name = p.value("name").toString(); name.replace('"', "\"\"");
        for (const auto &sample : m_history.value(id))
            output += (m_deviceId + ',' + id + ",\"" + name + "\"," + QString::number(qint64(sample.x())) + ',' + QString::number(sample.y(), 'g', 17) + "\r\n").toUtf8();
    }
    return file.write(output) == output.size() && file.commit();
}
