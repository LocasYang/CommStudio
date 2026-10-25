#include "modbusdevicemodel.h"

#include <algorithm>
#include <QUuid>
#include <QSet>

namespace {
QVariantMap defaultAdvancedOptions()
{
    return {
        {QStringLiteral("addressOffset"), 0},
        {QStringLiteral("displayAddress"), QStringLiteral("decimal")},
        {QStringLiteral("crcByteOrder"), QStringLiteral("little")},
        {QStringLiteral("coilBitOrder"), QStringLiteral("msb")},
        {QStringLiteral("packageMode"), QStringLiteral("address")},
        {QStringLiteral("maxBitGap"), 0},
        {QStringLiteral("maxRegisterGap"), 5},
        {QStringLiteral("maxBitRead"), 2000},
        {QStringLiteral("maxRegisterRead"), 125},
        {QStringLiteral("singleRegisterWriteCode"), 6},
        {QStringLiteral("singleCoilWriteCode"), 5},
        {QStringLiteral("broadcast"), false},
        {QStringLiteral("pollAfterStartup"), false},
        {QStringLiteral("writeAfterStartup"), QStringLiteral("off")}
    };
}

QString enumValue(const QVariantMap &source, const QString &key,
                  const QStringList &allowed, const QString &fallback)
{
    const QString value = source.value(key, fallback).toString();
    return allowed.contains(value) ? value : fallback;
}

QVariantMap normalizedAdvancedOptions(const QVariantMap &source)
{
    QVariantMap result = defaultAdvancedOptions();
    result[QStringLiteral("addressOffset")] = qBound(-65535, source.value(QStringLiteral("addressOffset"), 0).toInt(), 65535);
    result[QStringLiteral("displayAddress")] = enumValue(source, QStringLiteral("displayAddress"),
                                                          {QStringLiteral("decimal"), QStringLiteral("hex")}, QStringLiteral("decimal"));
    result[QStringLiteral("crcByteOrder")] = enumValue(source, QStringLiteral("crcByteOrder"),
                                                        {QStringLiteral("little"), QStringLiteral("big")}, QStringLiteral("little"));
    result[QStringLiteral("coilBitOrder")] = enumValue(source, QStringLiteral("coilBitOrder"),
                                                        {QStringLiteral("msb"), QStringLiteral("lsb")}, QStringLiteral("msb"));
    result[QStringLiteral("packageMode")] = enumValue(source, QStringLiteral("packageMode"),
                                                       {QStringLiteral("line"), QStringLiteral("address"), QStringLiteral("list")}, QStringLiteral("address"));
    result[QStringLiteral("maxBitGap")] = qBound(0, source.value(QStringLiteral("maxBitGap"), 0).toInt(), 1999);
    result[QStringLiteral("maxRegisterGap")] = qBound(0, source.value(QStringLiteral("maxRegisterGap"), 5).toInt(), 124);
    result[QStringLiteral("maxBitRead")] = qBound(1, source.value(QStringLiteral("maxBitRead"), 2000).toInt(), 2000);
    result[QStringLiteral("maxRegisterRead")] = qBound(1, source.value(QStringLiteral("maxRegisterRead"), 125).toInt(), 125);
    result[QStringLiteral("singleRegisterWriteCode")] = source.value(QStringLiteral("singleRegisterWriteCode"), 6).toInt() == 16 ? 16 : 6;
    result[QStringLiteral("singleCoilWriteCode")] = source.value(QStringLiteral("singleCoilWriteCode"), 5).toInt() == 15 ? 15 : 5;
    result[QStringLiteral("broadcast")] = source.value(QStringLiteral("broadcast"), false).toBool();
    result[QStringLiteral("pollAfterStartup")] = source.value(QStringLiteral("pollAfterStartup"), false).toBool();
    result[QStringLiteral("writeAfterStartup")] = enumValue(source, QStringLiteral("writeAfterStartup"),
        {QStringLiteral("off"), QStringLiteral("once"), QStringLiteral("cycle")}, QStringLiteral("off"));
    return result;
}
}

ModbusDeviceModel::ModbusDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_devices.append({QStringLiteral("Main Controller"), 1, false,
                      QUuid::createUuid().toString(QUuid::WithoutBraces), defaultAdvancedOptions()});
}

int ModbusDeviceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_devices.size();
}

QVariant ModbusDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_devices.size())
        return {};
    const Device &device = m_devices.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
    case LabelRole:
        return QStringLiteral("%1 · ID %2").arg(device.name).arg(device.unitId);
    case NameRole:
        return device.name;
    case UnitIdRole:
        return device.unitId;
    case OnlineRole:
        return device.online;
    case IdRole:
        return device.id;
    case EnabledRole:
        return device.enabled;
    default:
        return {};
    }
}

QHash<int, QByteArray> ModbusDeviceModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {UnitIdRole, "unitId"},
        {OnlineRole, "online"},
        {LabelRole, "label"},
        {IdRole, "deviceId"},
        {EnabledRole, "deviceEnabled"}
    };
}

void ModbusDeviceModel::setSelectedIndex(int index)
{
    if (index < -1 || index >= m_devices.size() || index == m_selectedIndex)
        return;
    m_selectedIndex = index;
}

int ModbusDeviceModel::addDevice(const QString &name, int unitId)
{
    const int safeId = std::clamp(unitId, 1, 247);
    const QString safeName = name.trimmed().isEmpty()
                                 ? QStringLiteral("Device %1").arg(safeId)
                                 : name.trimmed();
    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append({safeName, safeId, false,
                      QUuid::createUuid().toString(QUuid::WithoutBraces), defaultAdvancedOptions()});
    endInsertRows();
    emit definitionsChanged();
    return m_devices.size() - 1;
}

bool ModbusDeviceModel::removeDevice(int index)
{
    if (index < 0 || index >= m_devices.size())
        return false;
    beginRemoveRows(QModelIndex(), index, index);
    m_devices.removeAt(index);
    endRemoveRows();
    emit definitionsChanged();
    if (m_selectedIndex >= m_devices.size())
        m_selectedIndex = m_devices.size() - 1;
    return true;
}

QString ModbusDeviceModel::deviceId(int index) const
{
    return index >= 0 && index < m_devices.size() ? m_devices.at(index).id : QString();
}

bool ModbusDeviceModel::isDeviceEnabled(const QString &deviceId) const
{
    const auto it = std::find_if(m_devices.cbegin(), m_devices.cend(), [&deviceId](const Device &device) {
        return device.id == deviceId;
    });
    return it != m_devices.cend() && it->enabled;
}

bool ModbusDeviceModel::setDeviceEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_devices.size())
        return false;
    Device &device = m_devices[index];
    if (device.enabled == enabled)
        return true;
    device.enabled = enabled;
    if (!enabled)
        device.online = false;
    emit dataChanged(this->index(index), this->index(index), {EnabledRole, OnlineRole});
    emit definitionsChanged();
    return true;
}

int ModbusDeviceModel::unitId(int index) const
{
    return index >= 0 && index < m_devices.size() ? m_devices.at(index).unitId : 1;
}

QVariantList ModbusDeviceModel::definitions() const
{
    QVariantList result;
    for (const auto &device : m_devices)
        result.append(QVariantMap{{"id", device.id}, {"name", device.name}, {"unitId", device.unitId},
                                  {"advanced", device.advanced}, {"enabled", device.enabled}});
    return result;
}

QVariantMap ModbusDeviceModel::advancedOptions(int index) const
{
    return index >= 0 && index < m_devices.size()
        ? normalizedAdvancedOptions(m_devices.at(index).advanced)
        : defaultAdvancedOptions();
}

QVariantMap ModbusDeviceModel::advancedOptionsForId(const QString &deviceId) const
{
    const auto it = std::find_if(m_devices.cbegin(), m_devices.cend(), [&deviceId](const Device &device) {
        return device.id == deviceId;
    });
    return it == m_devices.cend() ? defaultAdvancedOptions() : normalizedAdvancedOptions(it->advanced);
}

bool ModbusDeviceModel::setAdvancedOptions(int index, const QVariantMap &options)
{
    if (index < 0 || index >= m_devices.size())
        return false;
    const QVariantMap normalized = normalizedAdvancedOptions(options);
    if (m_devices[index].advanced == normalized)
        return true;
    m_devices[index].advanced = normalized;
    emit definitionsChanged();
    return true;
}

bool ModbusDeviceModel::copyAdvancedOptions(int sourceIndex, int targetIndex)
{
    if (sourceIndex < 0 || sourceIndex >= m_devices.size()
        || targetIndex < 0 || targetIndex >= m_devices.size()
        || sourceIndex == targetIndex)
        return false;
    return setAdvancedOptions(targetIndex, m_devices.at(sourceIndex).advanced);
}

bool ModbusDeviceModel::restore(const QVariantList &definitions)
{
    if (definitions.size() > 4096)
        return false;
    QVector<Device> restored;
    QSet<QString> ids;
    for (const auto &entry : definitions) {
        const auto item = entry.toMap();
        const auto id = item.value("id").toString();
        const auto name = item.value("name").toString().trimmed();
        bool valid = false;
        const int unit = item.value("unitId").toInt(&valid);
        if (id.isEmpty() || ids.contains(id) || name.isEmpty() || !valid || unit < 1 || unit > 247)
            return false;
        ids.insert(id);
        restored.append({name, unit, false, id, normalizedAdvancedOptions(item.value("advanced").toMap()),
                         item.value("enabled", true).toBool()});
    }
    beginResetModel();
    m_devices = restored;
    m_selectedIndex = m_devices.isEmpty() ? -1 : 0;
    endResetModel();
    return true;
}

bool ModbusDeviceModel::setUnitId(int index, int unitId)
{
    if (index < 0 || index >= m_devices.size())
        return false;
    const int safeId = std::clamp(unitId, 1, 247);
    if (m_devices[index].unitId == safeId)
        return true;
    m_devices[index].unitId = safeId;
    const QModelIndex modelIndex = this->index(index, 0);
    emit dataChanged(modelIndex, modelIndex, {UnitIdRole, LabelRole, Qt::DisplayRole});
    emit definitionsChanged();
    return true;
}

bool ModbusDeviceModel::renameDevice(int index, const QString &name)
{
    if (index < 0 || index >= m_devices.size() || name.trimmed().isEmpty()) return false;
    m_devices[index].name = name.trimmed();
    emit dataChanged(this->index(index), this->index(index), {NameRole, LabelRole, Qt::DisplayRole});
    emit definitionsChanged(); return true;
}

void ModbusDeviceModel::setOnline(const QString &id, bool online)
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (!m_devices[i].enabled)
            continue;
        if (m_devices[i].id != id || m_devices[i].online == online) continue;
        m_devices[i].online = online;
        emit dataChanged(index(i), index(i), {OnlineRole});
    }
}
