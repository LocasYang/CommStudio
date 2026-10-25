#include "modbusworkspace.h"
#include <QSettings>
#include <QUuid>
#include <QSet>
#include <QSerialPortInfo>

ModbusWorkspace::ModbusWorkspace(QObject *parent) : QObject(parent)
{
    QSettings settings;
    QSet<QString> ids;
    for (const auto &entry : settings.value("modbus/channelDefinitions").toList()) {
        const auto channel = entry.toMap(); const auto id = channel.value("id").toString();
        if (id.isEmpty() || ids.contains(id) || m_channels.size() >= 32) continue;
        ids.insert(id); append(id, channel.value("name").toString());
    }
    if (m_channels.isEmpty()) append("default", "Channel 1");
    m_current = qBound(0, settings.value("modbus/currentChannel", 0).toInt(), int(m_channels.size()) - 1);
    syncSerialPorts();
}

void ModbusWorkspace::append(const QString &id, const QString &name)
{
    auto *manager = new ModbusManager(parent(), id == "default" ? "modbus" : "modbus/channel/" + id);
    manager->setParent(this);
    connect(manager, &ModbusManager::openChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::modeChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::portNameChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::baudRateChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::parityChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::hostChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::portChanged, this, &ModbusWorkspace::channelsChanged);
    connect(manager, &ModbusManager::toastRequested, this, &ModbusWorkspace::toastRequested);
    m_channels.append({id, name, manager});
}
QVariantList ModbusWorkspace::channels() const
{
    QVariantList result;
    QSet<QString> availableSerialPorts;
    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : serialPorts)
        availableSerialPorts.insert(port.portName().trimmed().toUpper());
    for (int index = 0; index < m_channels.size(); ++index) {
        const auto &channel = m_channels.at(index);
        const QString mode = channel.manager->mode();
        const bool network = mode.startsWith(QLatin1String("tcp"));
        const bool active = channel.manager->isOpen() || channel.manager->simulatorRunning();
        const bool available = network
                                   || availableSerialPorts.contains(channel.manager->portName().trimmed().toUpper());
        const QString channelName = network
                                        ? (channel.name.trimmed().isEmpty()
                                               ? QStringLiteral("NET%1").arg(index, 3, 10, QLatin1Char('0'))
                                               : channel.name)
                                        : channel.manager->portName();
        const QString endpoint = network
                                     ? QStringLiteral("%1:%2").arg(channel.manager->host()).arg(channel.manager->port())
                                     : (channel.manager->portName().isEmpty()
                                            ? QStringLiteral("--")
                                            : QStringLiteral("%1 @ %2")
                                                  .arg(channel.manager->portName(), channel.manager->baudRate()));
        result.append(QVariantMap{{"index", index},
                                  {"name", channel.name},
                                  {"alias", channel.name},
                                  {"channelName", channelName},
                                  {"id", channel.id},
                                  {"mode", mode},
                                  {"transport", network ? QStringLiteral("network") : QStringLiteral("serial")},
                                  {"endpoint", endpoint},
                                  {"portName", channel.manager->portName()},
                                  {"baudRate", channel.manager->baudRate()},
                                  {"parity", channel.manager->parity()},
                                  {"host", channel.manager->host()},
                                  {"port", channel.manager->port()},
                                  {"connectionState", channel.manager->connectionState()},
                                  {"available", available},
                                  {"open", active}});
    }
    return result;
}
QObject *ModbusWorkspace::channelBackend(int index) const
{
    if (index < 0 || index >= m_channels.size())
        return nullptr;
    return m_channels.at(index).manager;
}
bool ModbusWorkspace::copyDevicePoints(int sourceChannel, int sourceDevice,
                                       int targetChannel, int targetDevice)
{
    if (sourceChannel < 0 || sourceChannel >= m_channels.size()
        || targetChannel < 0 || targetChannel >= m_channels.size())
        return false;
    auto *source = m_channels.at(sourceChannel).manager;
    auto *target = m_channels.at(targetChannel).manager;
    const QString sourceId = source->devices()->deviceId(sourceDevice);
    const QString targetId = target->devices()->deviceId(targetDevice);
    if (sourceId.isEmpty() || targetId.isEmpty()
        || (sourceChannel == targetChannel && sourceId == targetId))
        return false;
    QVariantList sourcePoints;
    for (const auto &entry : source->points()->definitions()) {
        if (entry.toMap().value(QStringLiteral("deviceId")).toString() == sourceId)
            sourcePoints.append(entry);
    }
    return target->replaceDevicePoints(targetId, sourcePoints);
}
void ModbusWorkspace::save()
{
    QSettings settings; settings.setValue("modbus/channelDefinitions", channels()); settings.setValue("modbus/currentChannel", m_current);
}
void ModbusWorkspace::select(int index)
{
    if (index < 0 || index >= m_channels.size() || index == m_current) return;
    m_current = index; save(); emit currentChanged();
}
void ModbusWorkspace::add(const QString &name)
{
    addChannel(name, QStringLiteral("rtuMaster"));
}
void ModbusWorkspace::addChannel(const QString &name, const QString &mode)
{
    if (m_channels.size() >= 32) { emit toastRequested("Maximum 32 channels / 最多 32 个通道", false); return; }
    append(QUuid::createUuid().toString(QUuid::WithoutBraces), name.trimmed().isEmpty() ? QStringLiteral("Channel %1").arg(m_channels.size() + 1) : name.trimmed());
    const QStringList supportedModes{QStringLiteral("rtuMaster"), QStringLiteral("asciiMaster"),
                                     QStringLiteral("tcpMaster"), QStringLiteral("rtuSlave"),
                                     QStringLiteral("asciiSlave"), QStringLiteral("tcpSlave")};
    if (supportedModes.contains(mode))
        m_channels.last().manager->setMode(mode);
    m_current = m_channels.size() - 1; save(); emit channelsChanged(); emit currentChanged();
}
void ModbusWorkspace::remove(int index)
{
    // Keep one channel available as the workspace's fallback context, but allow
    // deleting whichever channel is currently selected (including the default).
    if (index < 0 || index >= m_channels.size() || m_channels.size() <= 1) return;
    m_channels[index].manager->close(); m_channels[index].manager->deleteLater(); m_channels.removeAt(index);
    if (m_current >= index) m_current = qMax(0, m_current - 1);
    save(); emit channelsChanged(); emit currentChanged();
}
void ModbusWorkspace::rename(int index, const QString &name)
{
    if (index < 0 || index >= m_channels.size() || name.trimmed().isEmpty()) return;
    m_channels[index].name = name.trimmed(); save(); emit channelsChanged();
}
void ModbusWorkspace::setAlias(int index, const QString &alias)
{
    if (index < 0 || index >= m_channels.size()) return;
    m_channels[index].name = alias.trimmed(); save(); emit channelsChanged();
}
void ModbusWorkspace::openChannel(int index)
{
    if (index < 0 || index >= m_channels.size())
        return;
    select(index);
    auto *manager = m_channels.at(index).manager;
    if (!manager->isOpen() && !manager->simulatorRunning())
        manager->openConnection();
}
void ModbusWorkspace::closeChannel(int index)
{
    if (index < 0 || index >= m_channels.size())
        return;
    m_channels.at(index).manager->close();
}
void ModbusWorkspace::refreshPorts()
{
    for (const auto &channel : m_channels)
        channel.manager->refreshPorts();
    syncSerialPorts();
    emit channelsChanged();
}
void ModbusWorkspace::closeAll() { for (const auto &channel : m_channels) channel.manager->close(); }

void ModbusWorkspace::syncSerialPorts()
{
    QSet<QString> knownPorts;
    int firstSerialIndex = -1;
    for (int index = 0; index < m_channels.size(); ++index) {
        const auto &channel = m_channels.at(index);
        if (channel.manager->mode().startsWith(QLatin1String("tcp")))
            continue;
        const QString portName = channel.manager->portName().trimmed();
        if (portName.isEmpty())
            continue;
        knownPorts.insert(portName.toUpper());
        if (firstSerialIndex < 0)
            firstSerialIndex = index;
    }

    const auto ports = QSerialPortInfo::availablePorts();
    bool changed = false;
    for (const QSerialPortInfo &port : ports) {
        const QString portName = port.portName().trimmed();
        if (portName.isEmpty() || knownPorts.contains(portName.toUpper()) || m_channels.size() >= 32)
            continue;
        append(QUuid::createUuid().toString(QUuid::WithoutBraces), QString());
        auto &channel = m_channels.last();
        channel.manager->setMode(QStringLiteral("rtuMaster"));
        channel.manager->setPortName(portName);
        knownPorts.insert(portName.toUpper());
        if (firstSerialIndex < 0)
            firstSerialIndex = m_channels.size() - 1;
        changed = true;
    }

    bool selectionChanged = false;
    if (firstSerialIndex >= 0 && m_current >= 0 && m_current < m_channels.size()) {
        const auto &currentChannel = m_channels.at(m_current);
        const bool invalidLegacySerial = !currentChannel.manager->mode().startsWith(QLatin1String("tcp"))
                                         && currentChannel.manager->portName().trimmed().isEmpty();
        if (invalidLegacySerial) {
            m_current = firstSerialIndex;
            selectionChanged = true;
        }
    }

    if (changed || selectionChanged) {
        save();
        emit channelsChanged();
        if (selectionChanged)
            emit currentChanged();
    }
}
