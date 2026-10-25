#include "modbusmanager.h"

#include "core/checksumengine.h"
#include "core/hexcodec.h"
#include "core/modbuscodec.h"
#include "managers/studio.h"

#include <QDateTime>
#include <QFile>
#include <QModbusDataUnit>
#include <QModbusRtuSerialClient>
#include <QModbusRtuSerialServer>
#include <QModbusTcpClient>
#include <QModbusTcpServer>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QSettings>
#include <QUuid>

#include <limits>
#include <algorithm>

namespace {

Studio *g_studio = nullptr;

int parityValue(const QString &parity)
{
    if (parity == QLatin1String("none")) return QSerialPort::NoParity;
    if (parity == QLatin1String("odd")) return QSerialPort::OddParity;
    if (parity == QLatin1String("even")) return QSerialPort::EvenParity;
    return QSerialPort::EvenParity;
}

QModbusDataUnit::RegisterType areaType(const QString &area)
{
    if (area == QLatin1String("coils")) return QModbusDataUnit::Coils;
    if (area == QLatin1String("discrete")) return QModbusDataUnit::DiscreteInputs;
    if (area == QLatin1String("input")) return QModbusDataUnit::InputRegisters;
    return QModbusDataUnit::HoldingRegisters;
}

QString areaName(QModbusDataUnit::RegisterType type)
{
    switch (type) {
    case QModbusDataUnit::Coils: return QStringLiteral("线圈");
    case QModbusDataUnit::DiscreteInputs: return QStringLiteral("离散输入");
    case QModbusDataUnit::InputRegisters: return QStringLiteral("输入寄存器");
    case QModbusDataUnit::HoldingRegisters: return QStringLiteral("保持寄存器");
    default: return QStringLiteral("无效区");
    }
}

QVector<quint16> parseValues(const QString &text, bool *ok, QString *error)
{
    QVector<quint16> values;
    const QStringList parts = text.split(QRegularExpression(QStringLiteral("[,;\\s]+")),
                                         Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        bool valid = false;
        qlonglong value = 0;
        const QString trimmed = part.trimmed();
        if (trimmed.startsWith(QLatin1String("0x"), Qt::CaseInsensitive)) {
            value = trimmed.mid(2).toLongLong(&valid, 16);
        } else {
            value = trimmed.toLongLong(&valid, 10);
        }
        if (!valid || value < 0 || value > 0xFFFF) {
            if (error)
                *error = QStringLiteral("非法寄存器值: %1").arg(part);
            if (ok)
                *ok = false;
            return {};
        }
        values.append(static_cast<quint16>(value));
    }
    if (ok)
        *ok = true;
    return values;
}

bool normalizeTask(QVariantMap &task)
{
    const int fc = task.value(QStringLiteral("fc"), 3).toInt();
    const int slave = task.value(QStringLiteral("slave"), 1).toInt();
    const int address = task.value(QStringLiteral("address"), 0).toInt();
    const int count = task.value(QStringLiteral("count"), 1).toInt();
    const int period = task.value(QStringLiteral("periodMs"), 1000).toInt();
    const int timeout = task.value(QStringLiteral("timeoutMs"), -1).toInt();
    const int retries = task.value(QStringLiteral("retries"), -1).toInt();
    if (fc < 1 || fc > 4 || slave < 1 || slave > 247 || address < 0 || address > 65535
        || count < 1 || count > (fc <= 2 ? 2000 : 125) || address + count > 65536
        || period < 0 || period > 3600000 || timeout < -1 || timeout > 60000
        || retries < -1 || retries > 10)
        return false;
    task.insert(QStringLiteral("fc"), fc);
    task.insert(QStringLiteral("slave"), slave);
    task.insert(QStringLiteral("address"), address);
    task.insert(QStringLiteral("count"), count);
    task.insert(QStringLiteral("periodMs"), period);
    task.insert(QStringLiteral("timeoutMs"), timeout);
    task.insert(QStringLiteral("retries"), retries);
    task.insert(QStringLiteral("priority"), qBound(2, task.value(QStringLiteral("priority"), 3).toInt(), 4));
    task.insert(QStringLiteral("enabled"), task.value(QStringLiteral("enabled"), true).toBool());
    return true;
}

} // namespace

ModbusManager::ModbusManager(QObject *parent, const QString &settingsPrefix)
    : QObject(parent)
    , m_settingsPrefix(settingsPrefix)
    , m_traffic(new TrafficModel(this))
    , m_devices(new ModbusDeviceModel(this))
    , m_points(new ModbusPointModel(this))
{
    QSettings settings;
    if (settings.contains(m_settingsPrefix + "/devices"))
        m_devices->restore(settings.value(m_settingsPrefix + "/devices").toList());
    const QVariantList storedPoints = settings.value(m_settingsPrefix + "/points").toList();
    // A fresh channel already owns an empty point model. Avoid emitting a
    // reset from the model constructor path when no persisted definitions
    // exist; this also keeps proxy views stable during workspace creation.
    if (!storedPoints.isEmpty())
        m_points->restore(storedPoints);
    m_rawTemplates = settings.value(m_settingsPrefix + "/rawTemplates").toList();
    m_simAreas = settings.value(m_settingsPrefix + "/simulatorAreas").toMap();
    const auto options = settings.value(m_settingsPrefix + "/channelOptions").toMap();
    for (auto it = options.begin(); it != options.end(); ++it) setChannelOption(it.key(), it.value());
    setVisibleColumns(settings.value(m_settingsPrefix + "/visibleColumns").toMap());
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this] {
        if (!m_userDisconnect && m_masterClient && m_masterClient->state() == QModbusDevice::UnconnectedState) {
            if (!m_masterClient->connectDevice()) m_reconnectTimer.start(m_channelOptions.value("reconnectMs").toInt());
        }
    });
    connect(this, &ModbusManager::channelOptionsChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::visibleColumnsChanged, this, &ModbusManager::saveWorkspace);
    m_rawLoopTimer.setSingleShot(true);
    connect(&m_rawLoopTimer, &QTimer::timeout, this, &ModbusManager::rawLoopTick);
    connect(m_points, &ModbusPointModel::definitionsChanged, this, &ModbusManager::saveWorkspace);
    connect(m_points, &ModbusPointModel::validationError, this, [this](const QString &message) { emit toastRequested(message, false); });
    if (m_devices->rowCount() > 0)
        selectDevice(qBound(0, settings.value(m_settingsPrefix + "/selectedDevice", 0).toInt(), m_devices->rowCount() - 1));
    else
        selectDevice(-1);
    const QVariantList storedTasks = settings.value(m_settingsPrefix + "/tasks").toList();
    for (const auto &entry : storedTasks) {
        QVariantMap task = entry.toMap();
        if (normalizeTask(task))
            m_tasks.append(task);
    }
    m_nextDue.fill(0, m_tasks.size());
    const auto connection = settings.value(m_settingsPrefix + "/connection").toMap();
    m_mode = connection.value("mode", m_mode).toString();
    m_portName = connection.value("portName", m_portName).toString();
    m_baudRate = connection.value("baudRate", m_baudRate).toString();
    m_parity = connection.value("parity", m_parity).toString();
    m_host = connection.value("host", m_host).toString();
    m_port = qBound(1, connection.value("port", m_port).toInt(), 65535);
    m_timeoutMs = qBound(10, connection.value("timeoutMs", m_timeoutMs).toInt(), 60000);
    m_retries = qBound(0, connection.value("retries", m_retries).toInt(), 10);
    connect(this, &ModbusManager::modeChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::portNameChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::baudRateChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::parityChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::hostChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::portChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::timeoutMsChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::retriesChanged, this, &ModbusManager::saveWorkspace);
    connect(m_devices, &ModbusDeviceModel::definitionsChanged, this, &ModbusManager::saveWorkspace);
    connect(this, &ModbusManager::selectedDeviceChanged, this, &ModbusManager::saveWorkspace);
    if (qobject_cast<Studio *>(parent))
        g_studio = qobject_cast<Studio *>(parent);

    m_pollTimer.setInterval(25);
    connect(&m_pollTimer, &QTimer::timeout, this, &ModbusManager::pollTick);

    m_simTick.setInterval(1000);
    connect(&m_simTick, &QTimer::timeout, this, &ModbusManager::regenerateSimulatorValues);

    m_rawIdleTimer.setSingleShot(true);
    m_rawIdleTimer.setInterval(60);
    connect(&m_rawIdleTimer, &QTimer::timeout, this, &ModbusManager::rawIdleFlush);

    m_traffic->appendInfo(QStringLiteral("Modbus 助手就绪 · RTU/TCP 主站 · 从站模拟 · Raw Test Center"));
}

void ModbusManager::setMode(const QString &mode)
{
    if (m_mode == mode)
        return;
    close();
    m_mode = mode;
    emit modeChanged();
}

bool ModbusManager::isOpen() const
{
    if (m_mode == QLatin1String("asciiMaster"))
        return m_asciiOpen;
    return m_masterClient && m_masterClient->state() == QModbusDevice::ConnectedState;
}

void ModbusManager::setPortName(const QString &name)
{
    if (m_portName == name) return;
    m_portName = name;
    emit portNameChanged();
}

void ModbusManager::setBaudRate(const QString &rate)
{
    if (m_baudRate == rate) return;
    m_baudRate = rate;
    emit baudRateChanged();
}

void ModbusManager::setParity(const QString &parity)
{
    if (m_parity == parity) return;
    m_parity = parity;
    emit parityChanged();
}

void ModbusManager::setHost(const QString &host)
{
    if (m_host == host) return;
    m_host = host;
    emit hostChanged();
}

void ModbusManager::setPort(int port)
{
    if (m_port == port) return;
    m_port = port;
    emit portChanged();
}

void ModbusManager::setUnitId(int id)
{
    id = qBound(1, id, 247);
    if (m_unitId == id) return;
    if (m_currentReply || m_polling) {
        emit toastRequested(QStringLiteral("请先停止轮询并等待请求完成 / Stop polling first"), false);
        return;
    }
    m_unitId = id;
    if (m_devices)
        m_devices->setUnitId(m_selectedDeviceIndex, id);
    for (int i = 0; i < m_tasks.size(); ++i) {
        auto task = m_tasks[i].toMap();
        if (task.value("deviceId").toString() == m_devices->deviceId(m_selectedDeviceIndex)) {
            task["slave"] = id;
            m_tasks[i] = task;
        }
    }
    saveWorkspace();
    emit tasksChanged();
    emit unitIdChanged();
}

void ModbusManager::saveWorkspace()
{
    QSettings settings;
    settings.setValue(m_settingsPrefix + "/devices", m_devices->definitions());
    settings.setValue(m_settingsPrefix + "/points", m_points->definitions());
    settings.setValue(m_settingsPrefix + "/channelOptions", m_channelOptions);
    settings.setValue(m_settingsPrefix + "/visibleColumns", m_visibleColumns);
    settings.setValue(m_settingsPrefix + "/simulatorAreas", m_simAreas);
    settings.setValue(m_settingsPrefix + "/selectedDevice", m_selectedDeviceIndex);
    QVariantList definitions;
    for (const auto &entry : m_tasks) {
        auto task = entry.toMap();
        task.remove("status");
        task.remove("ok");
        definitions.append(task);
    }
    settings.setValue(m_settingsPrefix + "/tasks", definitions);
    settings.setValue(m_settingsPrefix + "/connection", QVariantMap{{"mode", m_mode}, {"portName", m_portName},
        {"baudRate", m_baudRate}, {"parity", m_parity}, {"host", m_host}, {"port", m_port}, {"timeoutMs", m_timeoutMs}, {"retries", m_retries}});
}

void ModbusManager::setVisibleColumns(const QVariantMap &columns)
{
    static const QSet<QString> allowed{
        QStringLiteral("number"), QStringLiteral("name"), QStringLiteral("value"),
        QStringLiteral("command"), QStringLiteral("write"), QStringLiteral("unit"),
        QStringLiteral("range"), QStringLiteral("area"), QStringLiteral("address"),
        QStringLiteral("count"), QStringLiteral("bitOffset"), QStringLiteral("bitCount"),
        QStringLiteral("scale"), QStringLiteral("offset"), QStringLiteral("decimals"),
        QStringLiteral("transferType"), QStringLiteral("displayType"),
        QStringLiteral("byteOrder"), QStringLiteral("wordOrder"),
        QStringLiteral("polling"), QStringLiteral("period"),
        QStringLiteral("timeout"), QStringLiteral("retries")};
    QVariantMap normalized;
    for (auto it = columns.cbegin(); it != columns.cend(); ++it) {
        if (allowed.contains(it.key()) && !it.value().toBool())
            normalized.insert(it.key(), false);
    }
    if (normalized == m_visibleColumns)
        return;
    m_visibleColumns = normalized;
    emit visibleColumnsChanged();
}

void ModbusManager::selectDevice(int index)
{
    if (!m_devices || index < -1 || index >= m_devices->rowCount()
        || (index < 0 && m_devices->rowCount() > 0))
        return;
    const bool selectionChanged = m_selectedDeviceIndex != index;
    m_selectedDeviceIndex = index;
    m_devices->setSelectedIndex(index);
    m_points->setDeviceId(index >= 0 ? m_devices->deviceId(index) : QString());
    if (index < 0) {
        if (selectionChanged)
            emit selectedDeviceChanged();
        return;
    }
    const QModelIndex modelIndex = m_devices->index(index, 0);
    const int id = m_devices->data(modelIndex, ModbusDeviceModel::UnitIdRole).toInt();
    if (m_unitId != id) {
        m_unitId = id;
        emit unitIdChanged();
    }
    if (selectionChanged)
        emit selectedDeviceChanged();
}

int ModbusManager::addDevice(const QString &name, int unitId)
{
    if (!m_devices || unitId < 1 || unitId > 247)
        return -1;
    for (int index = 0; index < m_devices->rowCount(); ++index) {
        if (m_devices->unitId(index) == unitId) {
            emit toastRequested(QStringLiteral("当前通道已有相同设备地址 / Duplicate device address"), false);
            return -1;
        }
    }
    const int index = m_devices->addDevice(name, unitId);
    selectDevice(index);
    return index;
}

bool ModbusManager::updateDevice(int index, const QString &name, int unitId)
{
    if (!m_devices || index < 0 || index >= m_devices->rowCount()
        || name.trimmed().isEmpty() || unitId < 1 || unitId > 247)
        return false;
    for (int other = 0; other < m_devices->rowCount(); ++other) {
        if (other != index && m_devices->unitId(other) == unitId) {
            emit toastRequested(QStringLiteral("当前通道已有相同设备地址 / Duplicate device address"), false);
            return false;
        }
    }
    const bool addressChanged = m_devices->unitId(index) != unitId;
    if (addressChanged && (m_currentReply || !m_requests.isEmpty() || m_polling || m_rawBusy)) {
        emit toastRequested(QStringLiteral("请先停止通信后修改设备地址 / Stop active requests first"), false);
        return false;
    }
    const QString deviceId = m_devices->deviceId(index);
    if (!m_devices->renameDevice(index, name) || !m_devices->setUnitId(index, unitId))
        return false;
    if (addressChanged) {
        for (int taskIndex = 0; taskIndex < m_tasks.size(); ++taskIndex) {
            QVariantMap task = m_tasks.at(taskIndex).toMap();
            if (task.value(QStringLiteral("deviceId")).toString() != deviceId)
                continue;
            task.insert(QStringLiteral("slave"), unitId);
            m_tasks[taskIndex] = task;
        }
        if (index == m_selectedDeviceIndex) {
            m_unitId = unitId;
            emit unitIdChanged();
        }
        emit tasksChanged();
    }
    saveWorkspace();
    return true;
}

bool ModbusManager::setDeviceEnabled(int index, bool enabled)
{
    if (!m_devices || index < 0 || index >= m_devices->rowCount())
        return false;
    const QString deviceId = m_devices->deviceId(index);
    const bool wasEnabled = m_devices->isDeviceEnabled(deviceId);
    if (!m_devices->setDeviceEnabled(index, enabled))
        return false;
    if (!enabled) {
        removeBatchDevice(deviceId);
        for (int request = m_requests.size() - 1; request >= 0; --request) {
            if (m_requests.at(request).deviceId == deviceId)
                m_requests.removeAt(request);
        }
        if (m_polling) {
            bool remaining = false;
            for (const auto &entry : std::as_const(m_tasks)) {
                const QVariantMap task = entry.toMap();
                const QString id = task.value(QStringLiteral("deviceId")).toString();
                if (task.value(QStringLiteral("enabled"), true).toBool()
                    && (m_activePollingDevices.isEmpty() || m_activePollingDevices.contains(id))
                    && m_devices->isDeviceEnabled(id)) {
                    remaining = true;
                    break;
                }
            }
            if (!remaining)
                stopPolling();
        }
    }
    if (enabled && !wasEnabled && isOpen())
        applyStartupWrites({deviceId});
    saveWorkspace();
    return true;
}

void ModbusManager::removeDevice(int index)
{
    if (!m_devices || index < 0 || index >= m_devices->rowCount())
        return;
    const QString deviceId = m_devices->deviceId(index);
    stopPolling();
    removeBatchDevice(deviceId);
    m_requests.removeIf([&deviceId](const PendingRequest &request) { return request.deviceId == deviceId; });
    m_points->removeDevice(deviceId);
    bool tasksRemoved = false;
    for (int taskIndex = m_tasks.size() - 1; taskIndex >= 0; --taskIndex) {
        if (m_tasks[taskIndex].toMap().value("deviceId").toString() != deviceId) continue;
        if (m_currentTaskId == taskIndex)
            m_currentTaskId = -1;
        else if (m_currentTaskId > taskIndex)
            --m_currentTaskId;
        m_tasks.removeAt(taskIndex);
        if (taskIndex < m_nextDue.size())
            m_nextDue.remove(taskIndex);
        tasksRemoved = true;
    }
    if (tasksRemoved)
        emit tasksChanged();
    if (!m_devices->removeDevice(index))
        return;
    int nextIndex = m_selectedDeviceIndex;
    if (index < nextIndex)
        --nextIndex;
    nextIndex = qMin(nextIndex, m_devices->rowCount() - 1);
    selectDevice(nextIndex);
    saveWorkspace();
}

bool ModbusManager::replaceDevicePoints(const QString &deviceId, const QVariantList &points)
{
    if (m_currentReply || m_rawBusy || !m_requests.isEmpty()) {
        emit toastRequested(QStringLiteral("目标通道正在通信，请停止轮询并等待请求完成后复制"), false);
        return false;
    }
    if (!m_points->replaceDevicePoints(deviceId, points)) return false;
    removeBatchDevice(deviceId);
    // Generated tasks carry old point IDs; retain independently configured tasks.
    stopPolling();
    for (int index = m_tasks.size() - 1; index >= 0; --index) {
        const auto task = m_tasks[index].toMap();
        if (task.value("deviceId").toString() == deviceId && task.contains("pointId")) {
            m_tasks.removeAt(index);
            if (index < m_nextDue.size()) m_nextDue.removeAt(index);
        }
    }
    m_currentTaskId = -1;
    saveWorkspace();
    emit tasksChanged();
    return true;
}

bool ModbusManager::deleteSelectedPoints()
{
    if (m_points->selectedCount() == 0) return false;
    // Never reindex tasks while a reply or queued request still refers to them.
    stopPolling();
    removeBatchDevice(m_points->deviceId());
    if (m_currentReply || m_rawBusy || !m_requests.isEmpty()) {
        emit toastRequested(QStringLiteral("已停止轮询，请等待当前通信完成后再删除数据点"), false);
        return false;
    }
    const QString deviceId = m_points->deviceId();
    m_points->deleteSelected();
    // A merged task may include deleted points other than its leading pointId.
    // Discard this device's generated plans; startPolling rebuilds the survivors.
    for (int index = m_tasks.size() - 1; index >= 0; --index) {
        const auto task = m_tasks[index].toMap();
        if (task.value("deviceId").toString() == deviceId && task.contains("pointId")) {
            m_tasks.removeAt(index);
            if (index < m_nextDue.size()) m_nextDue.removeAt(index);
        }
    }
    m_currentTaskId = -1;
    saveWorkspace();
    emit tasksChanged();
    return true;
}

void ModbusManager::setSlaveAddress(int address)
{
    if (m_slaveAddress == address) return;
    m_slaveAddress = address;
    emit slaveAddressChanged();
}

void ModbusManager::setTimeoutMs(int ms)
{
    if (m_timeoutMs == ms) return;
    m_timeoutMs = ms;
    emit timeoutMsChanged();
}

void ModbusManager::setRetries(int retries)
{
    if (m_retries == retries) return;
    m_retries = retries;
    emit retriesChanged();
}

QVariantList ModbusManager::portInfos() const
{
    QVariantList list;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        QVariantMap map;
        map.insert(QStringLiteral("name"), info.portName());
        map.insert(QStringLiteral("label"),
                   QStringLiteral("%1 · %2").arg(info.portName(),
                                                 info.description().isEmpty()
                                                     ? QStringLiteral("未知设备")
                                                     : info.description()));
            list.append(map);
    }
    return list;
}

void ModbusManager::refreshPorts()
{
    // 端口列表为惰性枚举：QML 侧需要刷新时重新读取并发送变更信号。
    emit portInfosChanged();
}

void ModbusManager::openAsciiLink()
{
    if (m_portName.isEmpty()) {
        emit toastRequested(QStringLiteral("请先选择串口端口 / Select a serial port first"), false);
        return;
    }
    if (!m_rawSerial) {
        m_rawSerial = new QSerialPort(this);
        connect(m_rawSerial, &QSerialPort::readyRead, this, [this] {
            const QByteArray data = m_rawSerial->readAll();
            if (m_asciiSlaveOpen) {
                m_asciiSlaveBuffer += data;
                if (m_asciiSlaveBuffer.contains("\r\n"))
                    QTimer::singleShot(0, this, &ModbusManager::handleAsciiSlaveFrame);
                return;
            }
            m_rawBuffer += data;
            m_rawIdleTimer.start();
            if (m_rawIsAscii && m_rawBuffer.contains("\r\n"))
                QTimer::singleShot(0, this, &ModbusManager::rawIdleFlush);
        });
    }
    if (m_rawSerial->isOpen()) {
        m_asciiOpen = true;
        emit openChanged();
        return;
    }
    m_rawSerial->setPortName(m_portName);
    m_rawSerial->setBaudRate(m_baudRate.toInt());
    m_rawSerial->setDataBits(QSerialPort::DataBits(m_channelOptions.value("dataBits", 7).toInt()));
    m_rawSerial->setParity(QSerialPort::Parity(parityValue(m_parity)));
    m_rawSerial->setStopBits(QSerialPort::StopBits(m_channelOptions.value("stopBits", 1).toInt()));
    m_rawSerial->setFlowControl(QSerialPort::FlowControl(m_channelOptions.value("flowControl").toInt()));
    if (!m_rawSerial->open(QIODevice::ReadWrite)) {
        const QString message = QStringLiteral("Modbus ASCII 串口打开失败: %1").arg(m_rawSerial->errorString());
        m_traffic->appendInfo(message, true);
        emit toastRequested(message, false);
        return;
    }
    m_asciiOpen = true;
    m_userDisconnect = false;
    QTimer::singleShot(0, this, [this] { applyStartupWrites(); });
    m_traffic->appendInfo(QStringLiteral("Modbus ASCII 主站已连接 (%1 @ %2)").arg(m_portName, m_baudRate));
    emit openChanged();
}

void ModbusManager::openAsciiSlaveLink()
{
    if (m_portName.isEmpty()) {
        emit toastRequested(QStringLiteral("请先选择串口端口 / Select a serial port first"), false);
        return;
    }
    if (!m_rawSerial) {
        m_rawSerial = new QSerialPort(this);
        connect(m_rawSerial, &QSerialPort::readyRead, this, [this] {
            const QByteArray data = m_rawSerial->readAll();
            if (!m_asciiSlaveOpen) return;
            m_asciiSlaveBuffer += data;
            if (m_asciiSlaveBuffer.contains("\r\n"))
                QTimer::singleShot(0, this, &ModbusManager::handleAsciiSlaveFrame);
        });
    }
    if (m_rawSerial->isOpen()) {
        m_asciiSlaveOpen = true;
        m_asciiSlaveBuffer.clear();
        m_simTick.start();
        emit simulatorChanged();
        emit openChanged();
        return;
    }
    m_rawSerial->setPortName(m_portName);
    m_rawSerial->setBaudRate(m_baudRate.toInt());
    m_rawSerial->setDataBits(QSerialPort::DataBits(m_channelOptions.value("dataBits", 8).toInt()));
    m_rawSerial->setParity(QSerialPort::Parity(parityValue(m_parity)));
    m_rawSerial->setStopBits(QSerialPort::StopBits(m_channelOptions.value("stopBits", 1).toInt()));
    m_rawSerial->setFlowControl(QSerialPort::FlowControl(m_channelOptions.value("flowControl").toInt()));
    if (!m_rawSerial->open(QIODevice::ReadWrite)) {
        const QString message = QStringLiteral("Modbus ASCII 从站串口打开失败: %1").arg(m_rawSerial->errorString());
        m_traffic->appendInfo(message, true);
        emit toastRequested(message, false);
        return;
    }
    m_asciiSlaveOpen = true;
    m_asciiSlaveBuffer.clear();
    m_userDisconnect = false;
    m_simTick.start();
    m_traffic->appendInfo(QStringLiteral("Modbus ASCII 从站模拟已启动 (%1 @ %2)").arg(m_portName, m_baudRate));
    emit simulatorChanged();
    emit openChanged();
}

void ModbusManager::handleAsciiSlaveFrame()
{
    if (!m_asciiSlaveOpen || !m_rawSerial || !m_rawSerial->isOpen()) return;
    const int end = m_asciiSlaveBuffer.indexOf("\r\n");
    if (end < 0) return;
    const QByteArray frame = m_asciiSlaveBuffer.left(end + 2);
    m_asciiSlaveBuffer.remove(0, end + 2);
    const QByteArray text = frame.trimmed();
    QByteArray binary = text.startsWith(':') ? QByteArray::fromHex(text.mid(1)) : QByteArray();
    m_traffic->append(TrafficModel::Rx, frame, QStringLiteral("ASCII SLAVE REQUEST"));
    auto sendException = [this, binary](quint8 code) {
        if (binary.size() < 2) return;
        const QByteArray response = ModbusCodec::buildAsciiAdu(quint8(binary.at(0)), QByteArray(1, char(quint8(binary.at(1)) | 0x80)) + QByteArray(1, char(code)));
        m_rawSerial->write(response);
        m_traffic->append(TrafficModel::Tx, response, QStringLiteral("ASCII SLAVE EXCEPTION"));
        ++m_txCount; m_txBytes += response.size();
        emit statsChanged();
    };
    if (binary.size() < 8) { sendException(3); return; }
    quint8 checksum = 0; for (char byte : binary) checksum = quint8(checksum + quint8(byte));
    if (checksum != 0) { m_traffic->appendInfo(QStringLiteral("ASCII 从站收到 LRC 错误帧"), true); return; }
    const quint8 slave = quint8(binary.at(0));
    const quint8 fc = quint8(binary.at(1));
    if (slave != quint8(m_slaveAddress) && slave != 0) return;
    const quint16 address = (quint16(quint8(binary.at(2))) << 8) | quint8(binary.at(3));
    const quint16 quantity = (quint16(quint8(binary.at(4))) << 8) | quint8(binary.at(5));
    const auto typeFor = [fc]() {
        return fc == ModbusCodec::FcReadCoils || fc == ModbusCodec::FcWriteSingleCoil || fc == ModbusCodec::FcWriteMultipleCoils
            ? QModbusDataUnit::Coils : fc == ModbusCodec::FcReadDiscreteInputs ? QModbusDataUnit::DiscreteInputs
            : fc == ModbusCodec::FcReadInputRegisters ? QModbusDataUnit::InputRegisters : QModbusDataUnit::HoldingRegisters;
    };
    const auto type = typeFor();
    QByteArray pdu;
    if (fc >= 1 && fc <= 4) {
        const int limit = fc <= 2 ? 2000 : 125;
        if (quantity < 1 || quantity > limit || int(address) + quantity > 1024) { sendException(2); return; }
        if (fc <= 2) {
            const int bytes = (quantity + 7) / 8;
            pdu.append(char(fc)); pdu.append(char(bytes)); pdu.append(QByteArray(bytes, '\0'));
            for (int i = 0; i < quantity; ++i) { quint16 value = 0; m_simulator->data(type, address + i, &value); if (value) pdu[2 + i / 8] = char(quint8(pdu.at(2 + i / 8)) | (1 << (i % 8))); }
        } else {
            pdu.append(char(fc)); pdu.append(char(quantity * 2));
            for (int i = 0; i < quantity; ++i) { quint16 value = 0; m_simulator->data(type, address + i, &value); pdu.append(char(value >> 8)); pdu.append(char(value)); }
        }
    } else if (fc == 5 || fc == 6) {
        if (quantity > 1023 || binary.size() < 8) { sendException(3); return; }
        const quint16 value = (quint16(quint8(binary.at(4))) << 8) | quint8(binary.at(5));
        if (int(address) >= 1024) { sendException(2); return; }
        m_simulator->setData(type, address, value == 0xFF00 ? 1 : value);
        pdu = binary.mid(1, 5);
    } else if (fc == 15 || fc == 16) {
        const int byteCount = binary.size() > 6 ? quint8(binary.at(6)) : 0;
        if (quantity < 1 || int(address) + quantity > 1024 || binary.size() < 7 + byteCount || byteCount != (fc == 15 ? (quantity + 7) / 8 : quantity * 2)) { sendException(3); return; }
        if (fc == 15) for (int i = 0; i < quantity; ++i) m_simulator->setData(type, address + i, (quint8(binary.at(7 + i / 8)) >> (i % 8)) & 1);
        else for (int i = 0; i < quantity; ++i) m_simulator->setData(type, address + i, (quint16(quint8(binary.at(7 + i * 2))) << 8) | quint8(binary.at(8 + i * 2)));
        pdu = QByteArray(1, char(fc)); pdu.append(char(address >> 8)); pdu.append(char(address)); pdu.append(char(quantity >> 8)); pdu.append(char(quantity));
    } else { sendException(1); return; }
    const QByteArray response = ModbusCodec::buildAsciiAdu(slave, pdu);
    m_rawSerial->write(response);
    m_traffic->append(TrafficModel::Tx, response, QStringLiteral("ASCII SLAVE RESPONSE"));
    ++m_rxCount; m_rxBytes += frame.size(); ++m_txCount; m_txBytes += response.size();
    emit simulatorChanged(); emit statsChanged();
    if (!m_asciiSlaveBuffer.isEmpty() && m_asciiSlaveBuffer.contains("\r\n")) QTimer::singleShot(0, this, &ModbusManager::handleAsciiSlaveFrame);
}

bool ModbusManager::ensureLink(LinkRole role)
{
    if (role == LinkRole::Master) {
        if (m_masterClient)
            return true;
        if (m_mode == QLatin1String("tcpMaster"))
            m_masterClient = new QModbusTcpClient(this);
        else
            m_masterClient = new QModbusRtuSerialClient(this);
        connect(m_masterClient, &QModbusClient::stateChanged, this, [this](QModbusDevice::State state) {
            if (state == QModbusDevice::ConnectedState) {
                m_reconnectTimer.stop();
                if (auto *socket = qobject_cast<QTcpSocket *>(m_masterClient->device())) socket->setSocketOption(QAbstractSocket::KeepAliveOption, m_channelOptions.value("keepAlive").toBool());
                if (auto *serial = qobject_cast<QSerialPort *>(m_masterClient->device())) serial->setFlowControl(QSerialPort::FlowControl(m_channelOptions.value("flowControl").toInt()));
                QStringList startupDevices;
                for (const auto &entry : m_devices->definitions()) {
                    const QString id = entry.toMap().value(QStringLiteral("id")).toString();
                    if (m_devices->isDeviceEnabled(id)
                        && m_devices->advancedOptionsForId(id).value(QStringLiteral("pollAfterStartup")).toBool())
                        startupDevices.append(id);
                }
                if (m_channelOptions.value("autoPoll").toBool())
                    QTimer::singleShot(0, this, &ModbusManager::startPolling);
                else if (!startupDevices.isEmpty())
                    QTimer::singleShot(0, this, [this, startupDevices] { startPollingForDevices(startupDevices); });
                QTimer::singleShot(0, this, [this] { applyStartupWrites(); });
                m_traffic->appendInfo(QStringLiteral("Modbus 主站已连接 (%1)")
                                          .arg(m_mode == QLatin1String("tcpMaster")
                                                   ? QStringLiteral("%1:%2").arg(m_host).arg(m_port)
                                                   : QStringLiteral("%1 @ %2").arg(m_portName, m_baudRate)));
                if (g_studio) {
                    g_studio->log(QStringLiteral("success"), QStringLiteral("Modbus 主站已连接"));
                    if (!m_countedConnection) g_studio->addTraffic(0, 0, 1);
                }
                m_countedConnection = true;
            } else if (state == QModbusDevice::UnconnectedState) {
                stopBatchWriting();
                m_requests.clear();
                m_traffic->appendInfo(QStringLiteral("Modbus 主站连接已断开"));
                if (g_studio && m_countedConnection)
                    g_studio->addTraffic(0, 0, -1);
                m_countedConnection = false;
                if (!m_userDisconnect && m_channelOptions.value("autoReconnect").toBool()) m_reconnectTimer.start(m_channelOptions.value("reconnectMs").toInt());
            }
            emit openChanged();
        });
        connect(m_masterClient, &QModbusClient::errorOccurred, this, [this](QModbusDevice::Error) {
            if (m_masterClient->state() != QModbusDevice::ConnectedState
                && m_masterClient->error() != QModbusDevice::NoError) {
                m_traffic->appendInfo(QStringLiteral("主站错误: %1").arg(m_masterClient->errorString()), true);
            }
        });
        return true;
    }

    if (m_simulator)
        return true;
    if (m_mode == QLatin1String("tcpSlave"))
        m_simulator = new QModbusTcpServer(this);
    else
        m_simulator = new QModbusRtuSerialServer(this);

    QModbusDataUnitMap simMap;
    simMap[QModbusDataUnit::Coils] = QModbusDataUnit(QModbusDataUnit::Coils, 0, 1024);
    simMap[QModbusDataUnit::DiscreteInputs] = QModbusDataUnit(QModbusDataUnit::DiscreteInputs, 0, 1024);
    simMap[QModbusDataUnit::InputRegisters] = QModbusDataUnit(QModbusDataUnit::InputRegisters, 0, 1024);
    simMap[QModbusDataUnit::HoldingRegisters] = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 1024);
    if (!m_simulator->setMap(simMap)) {
        m_traffic->appendInfo(QStringLiteral("从站寄存器区初始化失败"), true);
        delete m_simulator;
        m_simulator = nullptr;
        return false;
    }
    connect(m_simulator, &QModbusServer::stateChanged, this, [this](QModbusDevice::State state) {
        emit simulatorChanged();
        emit openChanged();
        if (state == QModbusDevice::ConnectedState) {
            m_traffic->appendInfo(QStringLiteral("从站模拟器已启动 (%1)")
                                      .arg(m_mode == QLatin1String("tcpSlave")
                                               ? QStringLiteral("TCP %1:%2").arg(m_host).arg(m_port)
                                               : QStringLiteral("RTU %1 @ %2").arg(m_portName, m_baudRate)));
            if (g_studio) {
                g_studio->log(QStringLiteral("success"), QStringLiteral("Modbus 从站模拟器已启动"));
                g_studio->addTraffic(0, 0, 1);
            }
            m_simTick.start();
        } else if (state == QModbusDevice::UnconnectedState) {
            m_simTick.stop();
            if (g_studio)
                g_studio->addTraffic(0, 0, -1);
        }
    });
    connect(m_simulator, &QModbusServer::dataWritten, this,
            [this](QModbusDataUnit::RegisterType type, int address, int size) {
                m_traffic->appendInfo(QStringLiteral("主站写入 %1 @ %2 × %3")
                                          .arg(areaName(type)).arg(address).arg(size));
                emit simulatorChanged();
            });
    return true;
}

void ModbusManager::closeLink(LinkRole role)
{
    if (role == LinkRole::Master) {
        m_userDisconnect = true; m_reconnectTimer.stop();
        stopBatchWriting();
        stopPolling();
        m_requests.clear();
        stopScan();
        for (const auto &entry : m_devices->definitions()) m_devices->setOnline(entry.toMap().value("id").toString(), false);
        if (m_currentReply) {
            disconnect(m_currentReply, nullptr, this, nullptr);
            m_currentReply->deleteLater();
            m_currentReply = nullptr;
        }
        if (m_masterClient) {
            m_masterClient->disconnectDevice();
            m_masterClient->deleteLater();
            m_masterClient = nullptr;
            emit openChanged();
        }
        if (m_mode == QLatin1String("asciiMaster") && m_asciiOpen) {
            closeRawLink();
            m_asciiOpen = false;
            emit openChanged();
        }
    } else {
        m_simTick.stop();
        if (m_mode == QLatin1String("asciiSlave")) {
            m_asciiSlaveOpen = false;
            m_asciiSlaveBuffer.clear();
            if (m_rawSerial && m_rawSerial->isOpen()) m_rawSerial->close();
            emit simulatorChanged();
            emit openChanged();
        }
        if (m_simulator) {
            m_simulator->disconnectDevice();
            m_simulator->deleteLater();
            m_simulator = nullptr;
            emit simulatorChanged();
            emit openChanged();
        }
    }
}

void ModbusManager::openConnection()
{
    if (isOpen() || simulatorRunning()) {
        close();
        return;
    }
    const LinkRole role = (m_mode == QLatin1String("rtuMaster")
                           || m_mode == QLatin1String("asciiMaster")
                           || m_mode == QLatin1String("tcpMaster"))
                              ? LinkRole::Master : LinkRole::Simulator;
    if (m_mode == QLatin1String("asciiMaster")) {
        openAsciiLink();
        return;
    }
    if (m_mode == QLatin1String("asciiSlave")) {
        if (ensureLink(LinkRole::Simulator)) openAsciiSlaveLink();
        return;
    }
    if (!ensureLink(role))
        return;
    if (role == LinkRole::Master) {
        closeRawLink();
        m_userDisconnect = false;
        if (auto *rtu = qobject_cast<QModbusRtuSerialClient *>(m_masterClient)) {
            rtu->setInterFrameDelay(m_channelOptions.value("interFrameUs").toInt());
            rtu->setTurnaroundDelay(m_channelOptions.value("turnaroundMs").toInt());
        }
        if (m_mode == QLatin1String("tcpMaster")) {
            m_masterClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_host);
            m_masterClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_port);
        } else {
            if (m_portName.isEmpty()) {
                emit toastRequested(QStringLiteral("请先选择串口端口"), false);
                return;
            }
            m_masterClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_portName);
            m_masterClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                                   m_baudRate.toInt());
            m_masterClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, m_channelOptions.value("dataBits"));
            m_masterClient->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                                   parityValue(m_parity));
            m_masterClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, m_channelOptions.value("stopBits"));
        }
        m_masterClient->setTimeout(m_timeoutMs);
        m_masterClient->setNumberOfRetries(m_retries);
        if (!m_masterClient->connectDevice()) {
            const QString message = QStringLiteral("Modbus 主站连接失败: %1")
                                        .arg(m_masterClient->errorString());
            m_traffic->appendInfo(message, true);
            emit toastRequested(message, false);
        }
    } else {
        m_simulator->setServerAddress(m_slaveAddress);
        if (m_mode == QLatin1String("tcpSlave")) {
            m_simulator->setConnectionParameter(QModbusDevice::NetworkAddressParameter,
                                                QStringLiteral("0.0.0.0"));
            m_simulator->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_port);
        } else {
            if (m_portName.isEmpty()) {
                emit toastRequested(QStringLiteral("请先选择串口端口"), false);
                return;
            }
            m_simulator->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_portName);
            m_simulator->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                                m_baudRate.toInt());
            m_simulator->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, m_channelOptions.value("dataBits"));
            m_simulator->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                                parityValue(m_parity));
            m_simulator->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, m_channelOptions.value("stopBits"));
        }
        if (!m_simulator->connectDevice()) {
            const QString message = QStringLiteral("从站模拟器启动失败: %1")
                                        .arg(m_simulator->errorString());
            m_traffic->appendInfo(message, true);
            emit toastRequested(message, false);
        }
    }
}

void ModbusManager::close()
{
    stopRawLoop();
    closeLink(LinkRole::Master);
    closeLink(LinkRole::Simulator);
    closeRawLink();
}

QModbusDataUnit::RegisterType ModbusManager::registerTypeForFc(int fc) const
{
    switch (fc) {
    case 5:
    case 15:
    case 1: return QModbusDataUnit::Coils;
    case 2: return QModbusDataUnit::DiscreteInputs;
    case 4: return QModbusDataUnit::InputRegisters;
    default: return QModbusDataUnit::HoldingRegisters;
    }
}

void ModbusManager::sendAsciiRequest(QModbusDataUnit::RegisterType type, int fc, int slave,
                                     quint16 address, const QVector<quint16> &values,
                                     const QString &label, int taskId, const QString &deviceId, const QVariantMap &requestOptions)
{
    Q_UNUSED(type)
    if (!m_asciiOpen || !m_rawSerial || !m_rawSerial->isOpen()) {
        emit toastRequested(QStringLiteral("Modbus ASCII 主站未连接"), false);
        return;
    }
    const bool isWrite = fc == 5 || fc == 6 || fc == 15 || fc == 16;
    const QVariantMap deviceOptions = !requestOptions.isEmpty() ? requestOptions : deviceId.isEmpty()
        ? m_devices->advancedOptions(m_selectedDeviceIndex)
        : m_devices->advancedOptionsForId(deviceId);
    const int addressOffset = deviceOptions.value(QStringLiteral("addressOffset"), 0).toInt();
    const int wireAddress = int(address) + addressOffset;
    const int quantity = isWrite ? values.size() : int(values.value(0, 1));
    if (wireAddress < 0 || wireAddress > 65535 || quantity < 1 || wireAddress + quantity > 65536) {
        emit toastRequested(QStringLiteral("地址偏移后超出 Modbus 范围 / Address offset exceeds Modbus range"), false);
        return;
    }
    const QVariantMap task = !requestOptions.isEmpty() ? requestOptions
        : taskId >= 0 && taskId < m_tasks.size() ? m_tasks.at(taskId).toMap() : QVariantMap();
    const int timeout = task.value("timeoutMs", -1).toInt() < 0 ? m_timeoutMs : qBound(10, task.value("timeoutMs").toInt(), 60000);
    const int retries = task.value("retries", -1).toInt() < 0 ? m_retries : qBound(0, task.value("retries").toInt(), 10);
    Q_UNUSED(retries)
    QVector<quint16> pduValues = values;
    if (!isWrite) pduValues = {quint16(quantity)};
    const QByteArray pdu = ModbusCodec::buildPdu(quint8(fc), quint16(wireAddress), pduValues);
    const QByteArray adu = ModbusCodec::buildAsciiAdu(quint8(slave), pdu);
    m_rawBuffer.clear();
    m_rawIsTcp = false;
    m_rawIsAscii = true;
    m_rawBusy = true;
    m_rawElapsed.restart();
    m_currentTaskId = taskId;
    m_currentDeviceId = deviceId.isEmpty() ? (slave == m_unitId ? m_devices->deviceId(m_selectedDeviceIndex) : QStringLiteral("unmapped")) : deviceId;
    m_currentLabel = label;
    m_currentAddress = address;
    m_currentFc = fc;
    m_currentSlave = slave;
    if (taskId >= 0 && taskId < m_tasks.size()) {
        QVariantMap task = m_tasks.at(taskId).toMap();
        task.insert(QStringLiteral("lastTxAt"), QDateTime::currentDateTime().toString("HH:mm:ss.zzz"));
        m_tasks[taskId] = task;
        emit tasksChanged();
    }
    m_traffic->append(TrafficModel::Tx, adu, QStringLiteral("ASCII %1").arg(label));
    m_rawSerial->write(adu);
    ++m_txCount;
    m_txBytes += adu.size();
    emit statsChanged();
    m_rawIdleTimer.start(qMax(20, timeout));
}

void ModbusManager::sendRequest(QModbusDataUnit::RegisterType type, int fc, int slave,
                                quint16 address, const QVector<quint16> &values,
                                const QString &label, int taskId, const QString &deviceId, const QByteArray &rawPdu,
                                const QVariantMap &requestOptions)
{
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("Modbus 主站未连接"), false);
        return;
    }
    const QString context = !deviceId.isEmpty() ? deviceId : taskId >= 0
        ? m_tasks.at(taskId).toMap().value("deviceId").toString()
        : (slave == m_unitId ? m_devices->deviceId(m_selectedDeviceIndex) : QStringLiteral("unmapped"));
    if (rawPdu.isEmpty()) {
        bool disabled = !context.isEmpty() && context != QLatin1String("unmapped")
                        && !m_devices->isDeviceEnabled(context);
        if (!disabled) {
            for (const auto &entry : m_devices->definitions()) {
                const QVariantMap device = entry.toMap();
                if (device.value(QStringLiteral("unitId")).toInt() == slave
                    && !device.value(QStringLiteral("enabled"), true).toBool()) {
                    disabled = true;
                    break;
                }
            }
        }
        if (disabled) {
            emit toastRequested(QStringLiteral("设备已停用，请先恢复 / Device is disabled"), false);
            return;
        }
    }
    if (m_scanning) { emit toastRequested(QStringLiteral("设备扫描中 / Device scan in progress"), false); return; }
    if (m_mode == QLatin1String("asciiMaster")) {
        if (m_rawBusy) {
            if (m_requests.size() >= 256) { emit toastRequested(QStringLiteral("请求队列已满 / Request queue full"), false); return; }
            const int priority = taskId < 0 ? (fc >= 5 ? 0 : 1) : qBound(2, m_tasks[taskId].toMap().value("priority", 3).toInt(), 4);
            m_requests.append({type, fc, slave, address, values, label, taskId, context, priority, {}, requestOptions});
        } else sendAsciiRequest(type, fc, slave, address, values, label, taskId, context, requestOptions);
        return;
    }
    if (m_currentReply) {
        if (m_requests.size() >= 256) {
            emit toastRequested(QStringLiteral("请求队列已满 / Request queue full"), false);
            return;
        }
        const int priority = taskId < 0 ? (!rawPdu.isEmpty() ? 1 : fc >= 5 ? 0 : 1) : qBound(2, m_tasks[taskId].toMap().value("priority", 3).toInt(), 4);
        m_requests.append({type, fc, slave, address, values, label, taskId, context, priority, rawPdu, requestOptions});
        return;
    }

    // 读功能码：values 携带读取数量；写功能码：values 为写入数据。
    const bool isWrite = fc == 5 || fc == 6 || fc == 15 || fc == 16;
    const QVariantMap deviceOptions = requestOptions.isEmpty() ? m_devices->advancedOptionsForId(context) : requestOptions;
    const int addressOffset = deviceOptions.value(QStringLiteral("addressOffset"), 0).toInt();
    const int wireAddress = int(address) + addressOffset;
    const int quantity = isWrite ? values.size() : int(values.value(0, 1));
    if (rawPdu.isEmpty() && (wireAddress < 0 || wireAddress > 65535 || quantity < 1 || wireAddress + quantity > 65536)) {
        emit toastRequested(QStringLiteral("地址偏移后超出 Modbus 范围 / Address offset exceeds Modbus range"), false);
        return;
    }
    const auto task = !requestOptions.isEmpty() ? requestOptions : taskId >= 0 ? m_tasks.at(taskId).toMap() : QVariantMap();
    const int timeout = task.value("timeoutMs", -1).toInt();
    const int retries = task.value("retries", -1).toInt();
    m_masterClient->setTimeout(timeout < 0 ? m_timeoutMs : qBound(10, timeout, 60000));
    m_masterClient->setNumberOfRetries(retries < 0 ? m_retries : qBound(0, retries, 10));
    QModbusDataUnit unit;
    if (isWrite)
        unit = QModbusDataUnit(type, quint16(wireAddress), values);
    else
        unit = QModbusDataUnit(type, quint16(wireAddress),
                               static_cast<quint16>(qBound(1, int(values.value(0, 1)),
                                   fc <= 2 ? deviceOptions.value(QStringLiteral("maxBitRead"), 2000).toInt()
                                           : deviceOptions.value(QStringLiteral("maxRegisterRead"), 125).toInt())));

    // Qt's convenience API chooses FC05/06 for one value. Preserve an explicit
    // FC0F/10 override even when the payload contains just one coil/register.
    const QByteArray forcedWrite = isWrite && values.size() == 1 && (fc == 15 || fc == 16)
        ? ModbusCodec::buildPdu(quint8(fc), quint16(wireAddress), values) : QByteArray();
    m_currentReply = !rawPdu.isEmpty() ? m_masterClient->sendRawRequest(QModbusRequest(QModbusPdu::FunctionCode(quint8(rawPdu[0])), rawPdu.mid(1)), slave)
                    : !forcedWrite.isEmpty() ? m_masterClient->sendRawRequest(QModbusRequest(QModbusPdu::FunctionCode(fc), forcedWrite.mid(1)), slave)
                    : isWrite ? m_masterClient->sendWriteRequest(unit, slave)
                             : m_masterClient->sendReadRequest(unit, slave);
    if (!m_currentReply) {
        if (!rawPdu.isEmpty()) { m_rawBusy = false; emit rawResultChanged(); }
        // 广播写入（slave == 0）不产生应答属于正常行为。
        if (slave != 0) {
            m_traffic->appendInfo(QStringLiteral("请求发送失败: %1").arg(m_masterClient->errorString()), true);
            ++m_errorCount;
            emit statsChanged();
        }
        QTimer::singleShot(0, this, &ModbusManager::dispatchNext);
        return;
    }
    ++m_txCount;
    m_responseTimer.restart();

    // 流量记录：用自研 Codec 重建请求 ADU 供终端显示。
    QVector<quint16> pduValues = values;
    if (!isWrite)
        pduValues = QVector<quint16>{ static_cast<quint16>(unit.valueCount()) };
    const QByteArray pdu = rawPdu.isEmpty() ? ModbusCodec::buildPdu(static_cast<quint8>(fc), quint16(wireAddress), pduValues) : rawPdu;
    m_txBytes += pdu.size() + (m_mode == "tcpMaster" ? 7 : 3);
    if (m_mode == QLatin1String("tcpMaster")) {
        const QByteArray adu = ModbusCodec::buildTcpAdu(quint16(m_txCount & 0xFFFF),
                                                        static_cast<quint8>(slave), pdu);
        m_traffic->append(TrafficModel::Tx, adu, QStringLiteral("MBAP %1").arg(label));
    } else {
        const QByteArray adu = ModbusCodec::buildRtuAdu(static_cast<quint8>(slave), pdu);
        m_traffic->append(TrafficModel::Tx, adu, QStringLiteral("RTU %1").arg(label));
    }

    connect(m_currentReply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
    m_currentTaskId = taskId;
    m_currentRaw = !rawPdu.isEmpty();
    m_currentDeviceId = context;
    m_currentLabel = label;
    m_currentAddress = address;
    m_currentFc = fc;
    m_currentSlave = slave;
    if (taskId >= 0 && taskId < m_tasks.size()) {
        QVariantMap task = m_tasks.at(taskId).toMap();
        task.insert(QStringLiteral("lastTxAt"), QDateTime::currentDateTime().toString("HH:mm:ss.zzz"));
        m_tasks[taskId] = task;
        emit tasksChanged();
    }
    emit statsChanged();
    if (m_currentReply->isFinished()) QTimer::singleShot(0, this, &ModbusManager::onReplyFinished);
}

void ModbusManager::dispatchNext()
{
    if (m_currentReply || (m_mode == QLatin1String("asciiMaster") && m_rawBusy) || m_requests.isEmpty()) return;
    if (!isOpen()) { m_requests.clear(); m_rawBusy = false; emit rawResultChanged(); return; }
    int selected = 0;
    for (int i = 1; i < m_requests.size(); ++i)
        if (m_requests[i].priority < m_requests[selected].priority) selected = i;
    const auto request = m_requests.takeAt(selected);
    sendRequest(request.type, request.fc, request.slave, request.address, request.values,
                request.label, request.taskId, request.deviceId, request.rawPdu, request.options);
}

void ModbusManager::onReplyFinished()
{
    QModbusReply *reply = m_currentReply;
    if (!reply)
        return;
    m_currentReply = nullptr;
    m_lastResponseMs = int(m_responseTimer.elapsed());
    m_devices->setOnline(m_currentDeviceId, reply->error() == QModbusDevice::NoError || reply->rawResult().isException());
    if (m_currentRaw) {
        const auto response = reply->rawResult();
        const QByteArray pdu = QByteArray(1, char(response.functionCode())) + response.data();
        const auto adu = m_mode == "tcpMaster" ? ModbusCodec::buildTcpAdu(quint16(m_txCount), quint8(m_currentSlave), pdu)
                                               : ModbusCodec::buildRtuAdu(quint8(m_currentSlave), pdu);
        const auto parsed = ModbusCodec::parseResponse(adu, m_mode == "tcpMaster");
        QStringList words; for (quint16 value : parsed.words) words.append(QString::number(value, 16).rightJustified(4, '0').toUpper());
        m_rawResult = {{"ok", reply->error() == QModbusDevice::NoError}, {"crcOk", parsed.crcOk},
            {"slave", m_currentSlave}, {"functionCode", m_currentFc}, {"exception", response.isException()},
            {"exceptionText", response.isException() ? ModbusCodec::exceptionText(quint8(response.exceptionCode())) : QString()},
            {"timeout", reply->error() == QModbusDevice::TimeoutError}, {"elapsedMs", m_lastResponseMs},
            {"responseHex", reply->error() == QModbusDevice::TimeoutError ? QString() : HexCodec::toHex(adu)}, {"words", words}};
        m_rawBusy = false; emit rawResultChanged();
    }

    if (reply->error() == QModbusDevice::NoError) {
        ++m_rxCount;
        m_rttSum += m_lastResponseMs;
        ++m_successCount;
        m_rttMin = m_rttMin < 0 ? m_lastResponseMs : qMin(m_rttMin, m_lastResponseMs);
        m_rttMax = qMax(m_rttMax, m_lastResponseMs);
        const QModbusDataUnit unit = reply->result();
        QVector<quint16> values;
        values.reserve(int(unit.valueCount()));
        for (uint i = 0; i < unit.valueCount(); ++i)
            values.append(unit.value(i));
        if (m_currentFc >= 1 && m_currentFc <= 4)
            m_points->receive(m_currentDeviceId, m_currentFc, m_currentAddress, values);
        // Preserve the received PDU, including packed coils and write echoes.
        QByteArray respPdu(1, char(reply->rawResult().functionCode()));
        respPdu.append(reply->rawResult().data());
        m_rxBytes += respPdu.size() + (m_mode == "tcpMaster" ? 7 : 3);
        if (m_mode == QLatin1String("tcpMaster")) {
            const QByteArray adu = ModbusCodec::buildTcpAdu(quint16(m_txCount & 0xFFFF),
                                                            static_cast<quint8>(m_currentSlave), respPdu);
            m_traffic->append(TrafficModel::Rx, adu, QStringLiteral("%1 ms").arg(m_lastResponseMs));
        } else {
            const QByteArray adu = ModbusCodec::buildRtuAdu(static_cast<quint8>(m_currentSlave), respPdu);
            m_traffic->append(TrafficModel::Rx, adu, QStringLiteral("%1 ms").arg(m_lastResponseMs));
        }
        if (m_currentTaskId >= 0 && m_currentTaskId < m_tasks.size()) {
            QVariantMap task = m_tasks.at(m_currentTaskId).toMap();
            updateTaskStatus(m_currentTaskId, QStringLiteral("OK · %1 ms").arg(m_lastResponseMs), true);
            storeReadValues(m_currentTaskId, task, values);
        } else if (m_currentFc >= 1 && m_currentFc <= 4) {
            // 手动读取也要进入寄存器表。此前只有轮询任务会写入表格，
            // 导致手动读取已收到响应却没有可见的解码结果。
            const QVariantMap manualTask{
                { QStringLiteral("name"), QStringLiteral("MANUAL FC%1 @ %2")
                                              .arg(m_currentFc, 2, 10, QLatin1Char('0'))
                                              .arg(m_currentAddress) },
                { QStringLiteral("slave"), m_currentSlave },
                { QStringLiteral("address"), m_currentAddress },
                { QStringLiteral("type"), QStringLiteral("uint16") },
                { QStringLiteral("wordOrder"), QStringLiteral("ABCD") },
                { QStringLiteral("scale"), 1.0 },
                { QStringLiteral("offset"), 0.0 },
                { QStringLiteral("unit"), QString() }
            };
            storeReadValues(-1, manualTask, values);
        }
    } else {
        ++m_errorCount;
        QString detail = reply->errorString();
        if (reply->error() == QModbusDevice::TimeoutError) ++m_timeouts;
        if (reply->rawResult().isException()) { ++m_exceptions; ++m_rxCount; m_rxBytes += m_mode == "tcpMaster" ? 9 : 5; }
        if (reply->error() == QModbusDevice::ProtocolError
            && reply->rawResult().isException()) {
            detail = ModbusCodec::exceptionText(quint8(reply->rawResult().exceptionCode()));
        }
        m_traffic->appendInfo(QStringLiteral("请求失败 (%1): %2").arg(m_currentLabel, detail), true);
        if (m_currentTaskId >= 0)
            updateTaskStatus(m_currentTaskId, QStringLiteral("失败 · %1").arg(detail), false);
        if (g_studio)
            g_studio->log(QStringLiteral("warning"),
                          QStringLiteral("Modbus %1: %2").arg(m_currentLabel, detail));
    }
    reply->deleteLater();
    emit statsChanged();
    QTimer::singleShot(0, this, &ModbusManager::dispatchNext);
}

void ModbusManager::manualRead(int slave, int functionCode, int address, int count)
{
    if (slave < 1 || slave > 247 || functionCode < 1 || functionCode > 4 || address < 0 || address > 65535
        || count < 1 || count > (functionCode <= 2 ? 2000 : 125) || address + count > 65536) {
        emit toastRequested(QStringLiteral("读取参数超出范围 / Invalid read parameters"), false); return;
    }
    QVector<quint16> values{ quint16(count) };
    sendRequest(registerTypeForFc(functionCode), functionCode, slave, quint16(address),
                values, QStringLiteral("FC%1 读").arg(functionCode, 2, 10, QLatin1Char('0')), -1);
}

void ModbusManager::readPoint(int row)
{
    const auto p = m_points->pointAt(row);
    if (!p.isEmpty()) manualRead(m_unitId, p.value("fc").toInt(), p.value("address").toInt(), p.value("count").toInt());
}

void ModbusManager::readAllPoints()
{
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("请先连接当前通道 / Connect the current channel first"), false);
        return;
    }
    const int count = m_points->pointCount();
    if (count <= 0) {
        emit toastRequested(QStringLiteral("当前设备没有数据点 / No data points for this device"), false);
        return;
    }
    for (int row = 0; row < count; ++row)
        readPoint(row);
    emit toastRequested(QStringLiteral("已提交 %1 个单次读取请求 / %1 read requests queued").arg(count), true);
}

void ModbusManager::createPointTasks(int strategy, int maxGap, int maxRegisters)
{
    if (m_currentReply || m_polling) {
        emit toastRequested(QStringLiteral("请先停止轮询并等待请求完成 / Stop polling first"), false);
        return;
    }
    const auto device = m_devices->deviceId(m_selectedDeviceIndex);
    for (int i = m_tasks.size() - 1; i >= 0; --i) {
        const auto task = m_tasks[i].toMap();
        if (task.value("deviceId") == device && task.contains("pointId")) removeTask(i);
    }
    for (const auto &plan : m_points->pollingPlans(strategy, maxGap, maxRegisters)) {
        auto p = plan.toMap();
        p["slave"] = m_unitId; p["enabled"] = true;
        addTask(p);
    }
    emit toastRequested(QStringLiteral("已生成当前设备轮询任务 / Device polling tasks generated"), true);
}

void ModbusManager::manualWrite(int slave, int functionCode, int address, const QString &valuesText)
{
    bool ok = false;
    QString error;
    const QVector<quint16> values = parseValues(valuesText, &ok, &error);
    if (!ok) {
        emit toastRequested(error, false);
        return;
    }
    if (functionCode == 5 || functionCode == 6 || functionCode == 15 || functionCode == 16) {
        const int limit = functionCode == 5 || functionCode == 6 ? 1 : functionCode == 15 ? 1968 : 123;
        if (slave < 0 || slave > 247 || address < 0 || address > 65535 || values.size() > limit || address + values.size() > 65536
            || ((functionCode == 5 || functionCode == 15) && std::any_of(values.begin(), values.end(), [](quint16 v) { return v > 1; }))) {
            emit toastRequested(QStringLiteral("写入参数超出范围 / Invalid write parameters"), false); return;
        }
        if (values.isEmpty()) {
            emit toastRequested(QStringLiteral("请提供要写入的值"), false);
            return;
        }
        sendRequest(registerTypeForFc(functionCode), functionCode, slave, quint16(address),
                    values, QStringLiteral("FC%1 写").arg(functionCode, 2, 10, QLatin1Char('0')), -1);
    } else {
        emit toastRequested(QStringLiteral("写操作请使用功能码 05/06/0F/10"), false);
    }
}

void ModbusManager::writePoint(int row, const QString &valuesText)
{
    const auto point = m_points->pointAt(row);
    if (point.isEmpty()) {
        emit toastRequested(QStringLiteral("数据点不存在 / Point not found"), false);
        return;
    }
    const int area = point.value(QStringLiteral("fc"), 3).toInt();
    const int count = qMax(1, point.value(QStringLiteral("count"), 1).toInt());
    const QVariantMap deviceOptions = m_devices->advancedOptions(m_selectedDeviceIndex);
    int writeFc = 0;
    if (area == 1) writeFc = count == 1 ? deviceOptions.value(QStringLiteral("singleCoilWriteCode"), 5).toInt() : 15;
    else if (area == 3) writeFc = count == 1 ? deviceOptions.value(QStringLiteral("singleRegisterWriteCode"), 6).toInt() : 16;
    else {
        emit toastRequested(QStringLiteral("当前数据区不可写 / This data area is read-only"), false);
        return;
    }
    const int slave = m_devices->unitId(m_selectedDeviceIndex);
    if (!isOpen() || !m_devices->isDeviceEnabled(m_devices->deviceId(m_selectedDeviceIndex))) {
        emit toastRequested(QStringLiteral("请连接通道并启用当前设备后再写入"), false);
        return;
    }
    QVector<quint16> words;
    QString error;
    if (!ModbusCodec::encodePointValue(point, valuesText, words, error)) {
        emit toastRequested(error, false);
        return;
    }
    m_points->setWriteValue(row, valuesText);
    sendRequest(registerTypeForFc(writeFc), writeFc, slave,
                quint16(point.value(QStringLiteral("address")).toInt()), words,
                QStringLiteral("FC%1 写 %2").arg(writeFc).arg(point.value("name").toString()), -1);
}

bool ModbusManager::writeBatchOnce()
{
    const QString deviceId = m_devices->deviceId(m_selectedDeviceIndex);
    if (!isOpen() || deviceId.isEmpty() || !m_devices->isDeviceEnabled(deviceId)) {
        emit toastRequested(QStringLiteral("请连接通道并启用当前设备后再批量写入 / Connect and enable the current device first"), false);
        return false;
    }
    if (m_scanning || m_rawLoopActive || m_rawBusy || m_currentReply || !m_requests.isEmpty()) {
        emit toastRequested(QStringLiteral("通信忙，请等待当前请求结束后重试 / Communication busy; retry after the current request"), false);
        return false;
    }
    QList<PendingRequest> prepared;
    QString error;
    if (!prepareBatch({deviceId}, prepared, error)) {
        emit toastRequested(error, false);
        return false;
    }
    m_requests.append(prepared);
    dispatchNext();
    return true;
}

void ModbusManager::addTask(const QVariantMap &task)
{
    QVariantMap copy = task;
    if (!normalizeTask(copy)) {
        emit toastRequested(QStringLiteral("轮询任务参数无效 / Invalid polling task"), false);
        return;
    }
    copy.insert("id", QUuid::createUuid().toString(QUuid::WithoutBraces));
    copy.insert("deviceId", m_devices->deviceId(m_selectedDeviceIndex));
    copy.insert(QStringLiteral("status"), QStringLiteral("待开始"));
    m_tasks.append(copy);
    m_nextDue.append(0);
    saveWorkspace();
    emit tasksChanged();
}

void ModbusManager::updateTask(int index, const QVariantMap &task)
{
    if (m_currentReply || !m_requests.isEmpty()) {
        emit toastRequested(QStringLiteral("请求执行中，请稍后编辑 / Request in progress"), false);
        return;
    }
    if (index < 0 || index >= m_tasks.size())
        return;
    QVariantMap copy = task;
    if (!normalizeTask(copy)) {
        emit toastRequested(QStringLiteral("轮询任务参数无效 / Invalid polling task"), false);
        return;
    }
    copy.insert("id", m_tasks.at(index).toMap().value("id"));
    copy.insert("deviceId", m_tasks.at(index).toMap().value("deviceId"));
    copy.insert(QStringLiteral("status"), m_tasks.at(index).toMap().value(QStringLiteral("status")));
    m_tasks[index] = copy;
    saveWorkspace();
    emit tasksChanged();
}

void ModbusManager::removeTask(int index)
{
    if (m_currentReply || !m_requests.isEmpty()) {
        emit toastRequested(QStringLiteral("请求执行中，请稍后删除 / Request in progress"), false);
        return;
    }
    if (index < 0 || index >= m_tasks.size())
        return;
    m_tasks.removeAt(index);
    m_nextDue.remove(index);
    saveWorkspace();
    emit tasksChanged();
}

QVariantMap ModbusManager::taskAt(int index) const
{
    return (index >= 0 && index < m_tasks.size()) ? m_tasks.at(index).toMap() : QVariantMap();
}

void ModbusManager::startPolling()
{
    startPollingForDevices({});
}

void ModbusManager::startPollingForDevices(const QStringList &deviceIds)
{
    if (m_polling)
        return;
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("Modbus 主站未连接"), false);
        return;
    }
    if (m_currentReply || !m_requests.isEmpty() || m_rawBusy) {
        emit toastRequested(QStringLiteral("请求执行中，请稍后启动轮询 / Request in progress"), false);
        return;
    }
    QSet<QString> requestedDevices;
    for (const QString &id : deviceIds)
        requestedDevices.insert(id);

    // Keep every device's point-derived tasks visible and persisted. Selection
    // limits execution only; it must not delete another device's task plan.
    QVariantList plannedTasks;
    for (const auto &entry : std::as_const(m_tasks)) {
        const QVariantMap task = entry.toMap();
        if (!task.contains(QStringLiteral("pointId")))
            plannedTasks.append(task);
    }
    for (const auto &entry : m_devices->definitions()) {
        const QVariantMap device = entry.toMap();
        const QString deviceId = device.value(QStringLiteral("id")).toString();
        const QVariantMap options = m_devices->advancedOptionsForId(deviceId);
        const QString mode = options.value(QStringLiteral("packageMode"), QStringLiteral("address")).toString();
        const int strategy = mode == QLatin1String("line") ? 1 : mode == QLatin1String("list") ? 2 : 0;
        const auto plans = m_points->pollingPlansForDevice(
            deviceId, strategy, options.value(QStringLiteral("maxBitGap"), 0).toInt(),
            options.value(QStringLiteral("maxRegisterGap"), 0).toInt(),
            options.value(QStringLiteral("maxBitRead"), 2000).toInt(),
            options.value(QStringLiteral("maxRegisterRead"), 125).toInt());
        for (const auto &plan : plans) {
            QVariantMap task = plan.toMap();
            task.insert(QStringLiteral("id"), QUuid::createUuid().toString(QUuid::WithoutBraces));
            task.insert(QStringLiteral("deviceId"), deviceId);
            task.insert(QStringLiteral("slave"), device.value(QStringLiteral("unitId")).toInt());
            task.insert(QStringLiteral("enabled"), true);
            task.insert(QStringLiteral("status"), QStringLiteral("待开始"));
            if (normalizeTask(task))
                plannedTasks.append(task);
        }
    }
    bool hasActiveTask = false;
    for (const auto &entry : std::as_const(plannedTasks)) {
        const QVariantMap task = entry.toMap();
        if (task.value(QStringLiteral("enabled"), true).toBool()
            && m_devices->isDeviceEnabled(task.value(QStringLiteral("deviceId")).toString())
            && (requestedDevices.isEmpty()
                || requestedDevices.contains(task.value(QStringLiteral("deviceId")).toString()))) {
            hasActiveTask = true;
            break;
        }
    }
    if (!hasActiveTask) {
        emit toastRequested(QStringLiteral("没有参与轮询的数据点或任务 / No polling points or tasks"), false);
        return;
    }
    m_tasks = plannedTasks;
    m_activePollingDevices = requestedDevices;
    m_nextDue.fill(0, m_tasks.size());
    saveWorkspace();
    emit tasksChanged();
    m_polling = true;
    m_pollTimer.start();
    m_traffic->appendInfo(QStringLiteral("轮询已启动 · %1 个任务").arg(m_tasks.size()));
    if (g_studio)
        g_studio->log(QStringLiteral("info"), QStringLiteral("Modbus 轮询已启动"));
    emit pollingChanged();
}

void ModbusManager::stopPolling()
{
    for (int i = m_requests.size() - 1; i >= 0; --i)
        if (m_requests[i].taskId >= 0) m_requests.removeAt(i);
    if (!m_polling) {
        m_activePollingDevices.clear();
        return;
    }
    m_polling = false;
    m_activePollingDevices.clear();
    if (!batchWriting()) m_pollTimer.stop();
    m_traffic->appendInfo(QStringLiteral("轮询已停止"));
    emit pollingChanged();
}

void ModbusManager::readTaskOnce(int index)
{
    if (index < 0 || index >= m_tasks.size())
        return;
    const QVariantMap task = m_tasks.at(index).toMap();
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("Modbus 主站未连接"), false);
        return;
    }
    QVector<quint16> values{ quint16(task.value(QStringLiteral("count"), 1).toInt()) };
    sendRequest(registerTypeForFc(task.value(QStringLiteral("fc"), 3).toInt()),
                task.value(QStringLiteral("fc"), 3).toInt(),
                task.value(QStringLiteral("slave"), 1).toInt(),
                quint16(task.value(QStringLiteral("address"), 0).toInt()),
                values, task.value(QStringLiteral("name")).toString(), index);
}

void ModbusManager::pollTick()
{
    if (!isOpen() || m_currentReply || m_rawBusy || m_scanning || m_rawLoopActive || !m_requests.isEmpty())
        return;
    if ((m_preferBatch || !m_polling) && batchWriteTick()) {
        m_preferBatch = false;
        return;
    }
    if (!m_polling || m_tasks.isEmpty()) return;
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    int dueIndex = -1;
    qint64 bestDue = std::numeric_limits<qint64>::max();
    for (int i = 0; i < m_tasks.size(); ++i) {
        const QVariantMap task = m_tasks.at(i).toMap();
        if (!m_activePollingDevices.isEmpty()
            && !m_activePollingDevices.contains(task.value(QStringLiteral("deviceId")).toString()))
            continue;
        if (!m_devices->isDeviceEnabled(task.value(QStringLiteral("deviceId")).toString()))
            continue;
        if (!task.value(QStringLiteral("enabled"), true).toBool())
            continue;
        if (m_nextDue.at(i) <= now && m_nextDue.at(i) < bestDue) {
            bestDue = m_nextDue.at(i);
            dueIndex = i;
        }
    }
    if (dueIndex < 0) {
        batchWriteTick();
        return;
    }
    const QVariantMap task = m_tasks.at(dueIndex).toMap();
    m_nextDue[dueIndex] = now + qMax(m_channelOptions.value("minimumPollMs").toInt(), task.value(QStringLiteral("periodMs"), 1000).toInt());
    readTaskOnce(dueIndex);
    m_preferBatch = true;
}

void ModbusManager::updateTaskStatus(int taskId, const QString &status, bool ok)
{
    if (taskId < 0 || taskId >= m_tasks.size())
        return;
    QVariantMap task = m_tasks.at(taskId).toMap();
    task.insert(QStringLiteral("status"), status);
    task.insert(QStringLiteral("ok"), ok);
    const qint64 success = task.value(QStringLiteral("successCount"), 0).toLongLong() + (ok ? 1 : 0);
    const qint64 errors = task.value(QStringLiteral("errorCount"), 0).toLongLong() + (ok ? 0 : 1);
    const qint64 total = success + errors;
    task.insert(QStringLiteral("successCount"), success);
    task.insert(QStringLiteral("errorCount"), errors);
    task.insert(QStringLiteral("successRate"), total ? (100.0 * double(success) / double(total)) : 0.0);
    task.insert(QStringLiteral("lastRttMs"), m_lastResponseMs);
    task.insert(QStringLiteral("lastRxAt"), QDateTime::currentDateTime().toString("HH:mm:ss.zzz"));
    m_tasks[taskId] = task;
    emit tasksChanged();
}

void ModbusManager::storeReadValues(int taskId, const QVariantMap &task,
                                    const QVector<quint16> &values)
{
    const QString type = task.value(QStringLiteral("type"), QStringLiteral("uint16")).toString();
    const QString wordOrder = task.value(QStringLiteral("wordOrder"), QStringLiteral("ABCD")).toString();
    const double scale = task.value(QStringLiteral("scale"), 1).toDouble();
    const double offsetValue = task.value(QStringLiteral("offset"), 0).toDouble();
    const int slave = task.value(QStringLiteral("slave"), 1).toInt();
    const int base = task.value(QStringLiteral("address"), 0).toInt();
    const QString name = task.value(QStringLiteral("name")).toString();
    const int width = (type == QLatin1String("uint32") || type == QLatin1String("int32")
                       || type == QLatin1String("float32")) ? 2 : 1;
    const int rows = qMax(1, int(values.size()) / width);

    // 用本任务的新数据替换寄存器表中该任务名的旧行。
    QVariantList updated;
    for (const QVariant &entry : std::as_const(m_registerMap)) {
        if (entry.toMap().value(QStringLiteral("task")) != name)
            updated.append(entry);
    }
    for (int i = 0; i < rows; ++i) {
        const int offset = i * width;
        if (offset >= values.size())
            break;
        QVariantMap row = ModbusCodec::decodeRegisters(values, offset, type, wordOrder,
                                                       scale, offsetValue);
        if (row.isEmpty())
            continue;
        row.insert(QStringLiteral("task"), name);
        row.insert(QStringLiteral("slave"), slave);
        row.insert(QStringLiteral("protocolAddress"), base + offset);
        row.insert(QStringLiteral("displayAddress"), base + offset + 1);
        row.insert(QStringLiteral("type"), type);
        row.insert(QStringLiteral("unit"), task.value(QStringLiteral("unit")).toString());
        row.insert(QStringLiteral("updatedAt"),
                   QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz")));
        updated.append(row);
    }
    m_registerMap = updated;
    emit registerMapChanged();
}

QVariantMap ModbusManager::previewWrite(double value, double scale, double offsetValue,
                                        const QString &type, const QString &wordOrder) const
{
    QVariantMap preview;
    QVector<quint16> words;
    QString error;
    const int count = type.endsWith("64") ? 4 : type.endsWith("32") ? 2 : 1;
    if (!ModbusCodec::encodePointValue({{"fc", 3}, {"count", count}, {"type", type},
                                       {"wordOrder", wordOrder}, {"scale", scale}, {"offset", offsetValue}},
                                      QString::number(value, 'g', 17), words, error)) {
        preview.insert(QStringLiteral("error"), error);
        return preview;
    }
    preview.insert(QStringLiteral("value"), value);
    preview.insert(QStringLiteral("raw"), (value - offsetValue) / scale);
    QByteArray bytes;
    for (quint16 word : words) { bytes.append(char(word >> 8)); bytes.append(char(word)); }
    preview.insert(QStringLiteral("registers"), HexCodec::toHex(bytes));
    preview.insert(QStringLiteral("byteCount"), bytes.size());
    preview.insert(QStringLiteral("registerCount"), words.size());
    preview.insert(QStringLiteral("wordOrder"), wordOrder);
    return preview;
}

// ---------------- 从站模拟器 ----------------

void ModbusManager::configureSimulatorArea(const QString &area, int count, const QString &source,
                                           double baseValue, double amplitude)
{
    m_simAreas.insert(area, QVariantMap{
        { QStringLiteral("count"), count },
        { QStringLiteral("source"), source },
        { QStringLiteral("base"), baseValue },
        { QStringLiteral("amplitude"), amplitude }
    });
    saveWorkspace();
    emit simulatorChanged();
    if (g_studio)
        g_studio->log(QStringLiteral("info"), QStringLiteral("从站 %1 区配置: %2 个寄存器, 来源 %3")
                          .arg(area).arg(count).arg(source));
}

void ModbusManager::writeSimulatorRegister(const QString &area, int address, int value)
{
    if (!m_simulator)
        return;
    m_simulator->setData(areaType(area), quint16(address), quint16(value));
    emit simulatorChanged();
}

void ModbusManager::regenerateSimulatorValues()
{
    if (!m_simulator || !simulatorRunning())
        return;
    ++m_simCounter;
    for (auto it = m_simAreas.constBegin(); it != m_simAreas.constEnd(); ++it) {
        const QString source = it->toMap().value(QStringLiteral("source")).toString();
        if (source == QLatin1String("static"))
            continue;
        const int count = it->toMap().value(QStringLiteral("count"), 0).toInt();
        const double base = it->toMap().value(QStringLiteral("base"), 0).toDouble();
        const double amplitude = it->toMap().value(QStringLiteral("amplitude"), 0).toDouble();
        for (int i = 0; i < qMin(count, 1024); ++i) {
            const auto area = it->toMap();
            quint16 value = simulatorValueFor(source, base, amplitude, m_simCounter, i);
            if (source == QLatin1String("sequence")) {
                const QStringList sequence = area.value(QStringLiteral("sequence")).toString().split(QRegularExpression(QStringLiteral("[,;\\s]+")), Qt::SkipEmptyParts);
                if (!sequence.isEmpty()) {
                    bool ok = false; const int number = sequence.at(int((m_simCounter + i) % sequence.size())).toInt(&ok, 0);
                    if (ok) value = quint16(qBound(0, number, 65535));
                }
            }
            m_simulator->setData(areaType(it.key()), quint16(i), value);
        }
    }
    emit simulatorChanged();
}

quint16 ModbusManager::simulatorValueFor(const QString &source, double base, double amplitude,
                                         qint64 tick, int index) const
{
    double value = base;
    if (source == QLatin1String("random"))
        value = base + (QRandomGenerator::global()->bounded(int(qMax(1.0, amplitude * 2)))
                        - amplitude);
    else if (source == QLatin1String("counter"))
        value = base + (tick + index) % int(qMax(1.0, amplitude));
    else if (source == QLatin1String("ramp"))
        value = base + amplitude * ((tick % 100) / 100.0);
    else if (source == QLatin1String("sine"))
        value = base + amplitude * qSin((tick + index) * 0.3);
    else if (source == QLatin1String("parabola")) {
        const double x = ((tick + index) % 100) / 50.0 - 1.0;
        value = base + amplitude * x * x;
    } else if (source == QLatin1String("square")) {
        value = base + (((tick + index) % 20) < 10 ? amplitude : -amplitude);
    }
    return quint16(qBound(0.0, value, 65535.0));
}

QVariantList ModbusManager::simulatorRegisters() const
{
    QVariantList list;
    if (!m_simulator)
        return list;
    const int show = 20;
    const struct { const char *key; QModbusDataUnit::RegisterType type; } areas[] = {
        { "coils", QModbusDataUnit::Coils },
        { "discrete", QModbusDataUnit::DiscreteInputs },
        { "input", QModbusDataUnit::InputRegisters },
        { "holding", QModbusDataUnit::HoldingRegisters }
    };
    for (const auto &area : areas) {
        for (int i = 0; i < show; ++i) {
            quint16 value = 0;
            if (!m_simulator->data(area.type, quint16(i), &value))
                continue;
            QVariantMap row;
            row.insert(QStringLiteral("area"), areaName(area.type));
            row.insert(QStringLiteral("areaKey"), QString::fromLatin1(area.key));
            row.insert(QStringLiteral("address"), i);
            row.insert(QStringLiteral("value"), value);
            list.append(row);
        }
    }
    return list;
}

// ---------------- Raw Test Center ----------------

void ModbusManager::sendRawFrame(const QString &requestText, bool isTcp, bool autoCrc)
{
    if (m_rawBusy) {
        emit toastRequested(QStringLiteral("上一个 Raw 请求尚未返回"), false);
        return;
    }
    QByteArray payload;
    QString error;
    if (!HexCodec::parseHex(requestText, payload, &error)) {
        emit toastRequested(error, false);
        return;
    }
    if (payload.isEmpty()) {
        emit toastRequested(QStringLiteral("请输入请求帧"), false);
        return;
    }
    const bool asciiRaw = !isTcp && m_mode == QLatin1String("asciiMaster");
    if (asciiRaw) {
        if (payload.size() < 2 || quint8(payload.at(0)) > 247) {
            emit toastRequested(QStringLiteral("ASCII 请求须为 从站地址 + PDU / ASCII request requires slave + PDU"), false);
            return;
        }
        payload = ModbusCodec::buildAsciiAdu(quint8(payload.at(0)), payload.mid(1));
    }
    if (m_masterClient && isOpen()) {
        if (m_scanning || isTcp != (m_mode == "tcpMaster")) {
            emit toastRequested(QStringLiteral("Raw 模式与当前连接不匹配或正在扫描 / Raw link mismatch or scanning"), false); return;
        }
        if (!isTcp && !autoCrc) {
            if (payload.size() < 4 || ChecksumEngine::crc16Modbus(payload.left(payload.size() - 2))
                != quint16(quint8(payload[payload.size() - 2]) | (quint16(quint8(payload.back())) << 8))) {
                emit toastRequested(QStringLiteral("异常 CRC 注入请先断开标准连接 / Disconnect standard link for invalid CRC injection"), false); return;
            }
            payload.chop(2);
        }
        if (isTcp && payload.size() >= 8 && payload[2] == 0 && payload[3] == 0
            && 6 + (quint16(quint8(payload[4])) << 8) + quint8(payload[5]) == payload.size()) payload = payload.mid(6);
        if (payload.size() < 2 || payload.size() > 254 || quint8(payload[0]) > 247 || m_requests.size() >= 256) {
            emit toastRequested(QStringLiteral("Raw 请求无效或队列已满 / Invalid Raw request or full queue"), false); return;
        }
        m_rawBusy = true; emit rawResultChanged();
        sendRequest(QModbusDataUnit::Invalid, quint8(payload[1]), quint8(payload[0]), 0, {}, "RAW", -1, QString(), payload.mid(1));
        return;
    }
    m_rawIsTcp = isTcp;
    m_rawIsAscii = asciiRaw;
    if (!isTcp) {
        if (asciiRaw) {
            // ASCII 模式已经在上面完成 LRC 封装。
        } else if (autoCrc) {
            const quint16 crc = ChecksumEngine::crc16Modbus(payload);
            payload.append(char(crc & 0xFF));
            payload.append(char(crc >> 8));
        } else if (payload.size() < 4) {
            emit toastRequested(QStringLiteral("RTU 帧（含 CRC）至少 4 字节"), false);
            return;
        }
    }

    // 打开 Raw 传输链路（跟随当前模式：RTU 用串口，TCP 用 Socket）。
    int sentBytes = payload.size();
    if (isTcp) {
        if (!m_rawTcp) {
            m_rawTcp = new QTcpSocket(this);
            connect(m_rawTcp, &QTcpSocket::readyRead, this, [this] {
                m_rawBuffer += m_rawTcp->readAll();
                m_rawIdleTimer.start();
            });
        }
        if (m_rawTcp->state() != QAbstractSocket::ConnectedState) {
            m_rawBuffer.clear();
            m_rawTcp->connectToHost(m_host, quint16(m_port));
            if (!m_rawTcp->waitForConnected(3000)) {
                emit toastRequested(QStringLiteral("Raw TCP 连接失败: %1").arg(m_rawTcp->errorString()), false);
                return;
            }
        }
        // 用户输入 unit + PDU 时自动补 MBAP。
        QByteArray adu;
        if (payload.size() >= 7 && payload.at(2) == 0 && payload.at(3) == 0) {
            adu = payload; // 已是完整 MBAP
        } else {
            adu = ModbusCodec::buildTcpAdu(1, static_cast<quint8>(payload.at(0)), payload.mid(1));
        }
        m_rawBusy = true;
        m_rawElapsed.restart();
        m_rawBuffer.clear();
        sentBytes = adu.size();
        m_traffic->append(TrafficModel::Tx, adu, QStringLiteral("RAW TCP"));
        m_rawTcp->write(adu);
    } else {
        if (!m_rawSerial) {
            m_rawSerial = new QSerialPort(this);
            connect(m_rawSerial, &QSerialPort::readyRead, this, [this] {
                const QByteArray data = m_rawSerial->readAll();
                if (m_asciiSlaveOpen) {
                    m_asciiSlaveBuffer += data;
                    if (m_asciiSlaveBuffer.contains("\r\n"))
                        QTimer::singleShot(0, this, &ModbusManager::handleAsciiSlaveFrame);
                    return;
                }
                m_rawBuffer += data;
                m_rawIdleTimer.start();
                if (m_rawIsAscii && m_rawBuffer.contains("\r\n"))
                    QTimer::singleShot(0, this, &ModbusManager::rawIdleFlush);
            });
        }
        if (!m_rawSerial->isOpen()) {
            if (m_portName.isEmpty()) {
                emit toastRequested(QStringLiteral("请先选择串口端口"), false);
                return;
            }
            m_rawSerial->setPortName(m_portName);
            m_rawSerial->setBaudRate(m_baudRate.toInt());
            m_rawSerial->setParity(QSerialPort::Parity(parityValue(m_parity)));
            m_rawSerial->setDataBits(QSerialPort::Data8);
            m_rawSerial->setStopBits(QSerialPort::OneStop);
            if (!m_rawSerial->open(QIODevice::ReadWrite)) {
                emit toastRequested(QStringLiteral("Raw 串口打开失败: %1").arg(m_rawSerial->errorString()), false);
                return;
            }
        }
        m_rawBusy = true;
        m_rawIsAscii = false;
        m_rawBuffer.clear();
        m_rawElapsed.restart();
        m_traffic->append(TrafficModel::Tx, payload, QStringLiteral("RAW RTU"));
        m_rawSerial->write(payload);
    }
    m_rawIdleTimer.start();
    ++m_txCount;
    m_txBytes += sentBytes;
    emit rawResultChanged();
    emit statsChanged();
}

void ModbusManager::rawIdleFlush()
{
    if (!m_rawBusy)
        return;
    if (m_rawIsAscii) {
        if (!m_rawBuffer.contains("\r\n")) {
            if (m_rawElapsed.elapsed() <= m_timeoutMs) { m_rawIdleTimer.start(20); return; }
            m_rawBusy = false; ++m_timeouts; ++m_errorCount;
            m_traffic->appendInfo(QStringLiteral("Modbus ASCII 请求超时"), true);
            emit rawResultChanged(); emit statsChanged(); QTimer::singleShot(0, this, &ModbusManager::dispatchNext); return;
        }
        const auto parsed = ModbusCodec::parseAsciiResponse(m_rawBuffer);
        m_traffic->append(TrafficModel::Rx, m_rawBuffer, QStringLiteral("%1 ms").arg(m_rawElapsed.elapsed()));
        ++m_rxCount; m_rxBytes += m_rawBuffer.size(); m_lastResponseMs = int(m_rawElapsed.elapsed());
        if (parsed.ok) {
            ++m_successCount; m_rttSum += m_lastResponseMs;
            m_rttMin = m_rttMin < 0 ? m_lastResponseMs : qMin(m_rttMin, m_lastResponseMs);
            m_rttMax = qMax(m_rttMax, m_lastResponseMs);
            if (m_currentFc >= 1 && m_currentFc <= 4) m_points->receive(m_currentDeviceId, m_currentFc, m_currentAddress, parsed.words);
            if (m_currentTaskId >= 0 && m_currentTaskId < m_tasks.size()) {
                const auto task = m_tasks.at(m_currentTaskId).toMap();
                updateTaskStatus(m_currentTaskId, QStringLiteral("OK · %1 ms").arg(m_lastResponseMs), true);
                storeReadValues(m_currentTaskId, task, parsed.words);
            }
        } else {
            ++m_errorCount;
            if (!parsed.crcOk) ++m_crcErrors; else ++m_frameErrors;
            m_traffic->appendInfo(QStringLiteral("ASCII 响应无效: %1").arg(parsed.error), true);
        }
        m_rawBusy = false; m_rawBuffer.clear(); m_rawIsAscii = false;
        emit rawResultChanged(); emit statsChanged(); QTimer::singleShot(0, this, &ModbusManager::dispatchNext); return;
    }
    if (!m_rawBuffer.isEmpty()) {
        const ModbusCodec::ParsedResponse parsed = ModbusCodec::parseResponse(m_rawBuffer, m_rawIsTcp);
        m_traffic->append(TrafficModel::Rx, m_rawBuffer,
                          QStringLiteral("%1 ms").arg(m_rawElapsed.elapsed()));
        ++m_rxCount;
        QVariantMap result;
        m_rxBytes += m_rawBuffer.size();
        m_lastResponseMs = int(m_rawElapsed.elapsed());
        if (parsed.ok) {
            ++m_successCount; m_rttSum += m_lastResponseMs;
            m_rttMin = m_rttMin < 0 ? m_lastResponseMs : qMin(m_rttMin, m_lastResponseMs);
            m_rttMax = qMax(m_rttMax, m_lastResponseMs);
        } else {
            ++m_errorCount;
            if (!parsed.crcOk) ++m_crcErrors;
            else if (parsed.exception) ++m_exceptions;
            else ++m_frameErrors;
        }
        result.insert(QStringLiteral("ok"), parsed.ok);
        result.insert(QStringLiteral("crcOk"), parsed.crcOk);
        result.insert(QStringLiteral("exception"), parsed.exception);
        result.insert(QStringLiteral("slave"), parsed.slave);
        result.insert(QStringLiteral("functionCode"), parsed.functionCode);
        result.insert(QStringLiteral("exceptionCode"), parsed.exceptionCode);
        result.insert(QStringLiteral("exceptionText"),
                      parsed.exception ? ModbusCodec::exceptionText(parsed.exceptionCode) : QString());
        QStringList words;
        for (quint16 word : parsed.words)
            words << QStringLiteral("%1").arg(word, 4, 16, QLatin1Char('0')).toUpper();
        result.insert(QStringLiteral("words"), words);
        result.insert(QStringLiteral("transactionId"), parsed.transactionId);
        result.insert(QStringLiteral("elapsedMs"), int(m_rawElapsed.elapsed()));
        result.insert(QStringLiteral("responseHex"), HexCodec::toHex(m_rawBuffer));
        m_rawResult = result;
        m_rawBusy = false;
        emit rawResultChanged();
        emit statsChanged();
        return;
    }
    // 未收到数据且超过超时 → 视为超时。
    if (m_rawElapsed.elapsed() > 2000) {
        m_rawBusy = false;
        QVariantMap result;
        result.insert(QStringLiteral("ok"), false);
        result.insert(QStringLiteral("timeout"), true);
        m_rawResult = result;
        m_traffic->appendInfo(QStringLiteral("Raw 请求超时（2 s 无响应）"), true);
        ++m_timeouts;
        ++m_errorCount;
        emit rawResultChanged();
        emit statsChanged();
        return;
    }
    m_rawIdleTimer.start();
}

void ModbusManager::closeRawLink()
{
    m_rawIdleTimer.stop();
    if (m_rawSerial && m_rawSerial->isOpen())
        m_rawSerial->close();
    if (m_rawTcp && m_rawTcp->state() == QAbstractSocket::ConnectedState)
        m_rawTcp->abort();
    m_rawBusy = false;
    m_rawIsAscii = false;
}

bool ModbusManager::importTasksFromCsv(const QUrl &path)
{
    const QString local = path.toLocalFile();
    QFile file(local);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit toastRequested(QStringLiteral("无法打开 CSV 文件"), false);
        return false;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    int lineNo = 0;
    int imported = 0;
    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        ++lineNo;
        if (line.isEmpty() || lineNo == 1)
            continue;
        const QStringList columns = line.split(QLatin1Char(','));
        if (columns.size() < 4)
            continue;
        QVariantMap task;
        task.insert(QStringLiteral("name"), columns.at(0).trimmed());
        task.insert(QStringLiteral("enabled"), true);
        task.insert(QStringLiteral("slave"), columns.value(1, QStringLiteral("1")).toInt());
        task.insert(QStringLiteral("fc"), columns.value(2, QStringLiteral("3")).toInt());
        task.insert(QStringLiteral("address"), columns.at(3).trimmed().toInt());
        task.insert(QStringLiteral("count"), columns.value(4, QStringLiteral("1")).toInt());
        task.insert(QStringLiteral("periodMs"), columns.value(5, QStringLiteral("1000")).toInt());
        task.insert(QStringLiteral("type"), columns.value(6, QStringLiteral("uint16")));
        task.insert(QStringLiteral("wordOrder"), columns.value(7, QStringLiteral("ABCD")));
        task.insert(QStringLiteral("scale"), columns.value(8, QStringLiteral("1")).toDouble());
        task.insert(QStringLiteral("offset"), columns.value(9, QStringLiteral("0")).toDouble());
        task.insert(QStringLiteral("unit"), columns.value(10, QString()));
        task.insert(QStringLiteral("status"), QStringLiteral("待开始"));
        m_tasks.append(task);
        m_nextDue.append(0);
        ++imported;
    }
    emit tasksChanged();
    m_traffic->appendInfo(QStringLiteral("CSV 导入完成 · %1 个轮询任务").arg(imported));
    return imported > 0;
}
