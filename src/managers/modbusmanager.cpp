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

#include <limits>

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

} // namespace

ModbusManager::ModbusManager(QObject *parent)
    : QObject(parent)
    , m_traffic(new TrafficModel(this))
{
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
    if (m_unitId == id) return;
    m_unitId = id;
    emit unitIdChanged();
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
                m_traffic->appendInfo(QStringLiteral("Modbus 主站已连接 (%1)")
                                          .arg(m_mode == QLatin1String("tcpMaster")
                                                   ? QStringLiteral("%1:%2").arg(m_host).arg(m_port)
                                                   : QStringLiteral("%1 @ %2").arg(m_portName, m_baudRate)));
                if (g_studio) {
                    g_studio->log(QStringLiteral("success"), QStringLiteral("Modbus 主站已连接"));
                    g_studio->addTraffic(0, 0, 1);
                }
            } else if (state == QModbusDevice::UnconnectedState) {
                m_traffic->appendInfo(QStringLiteral("Modbus 主站连接已断开"));
                if (g_studio)
                    g_studio->addTraffic(0, 0, -1);
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
        stopPolling();
        if (m_masterClient) {
            m_masterClient->disconnectDevice();
            m_masterClient->deleteLater();
            m_masterClient = nullptr;
            emit openChanged();
        }
    } else {
        m_simTick.stop();
        if (m_simulator) {
            m_simulator->disconnectDevice();
            m_simulator->deleteLater();
            m_simulator = nullptr;
            emit simulatorChanged();
            emit openChanged();
        }
    }
}

void ModbusManager::open()
{
    if (isOpen() || simulatorRunning()) {
        close();
        return;
    }
    const LinkRole role = (m_mode == QLatin1String("rtuMaster")
                           || m_mode == QLatin1String("tcpMaster"))
                              ? LinkRole::Master : LinkRole::Simulator;
    if (!ensureLink(role))
        return;
    if (role == LinkRole::Master) {
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
            m_masterClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
            m_masterClient->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                                   parityValue(m_parity));
            m_masterClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);
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
            m_simulator->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
            m_simulator->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                                parityValue(m_parity));
            m_simulator->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);
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
    closeLink(LinkRole::Master);
    closeLink(LinkRole::Simulator);
    closeRawLink();
}

QModbusDataUnit::RegisterType ModbusManager::registerTypeForFc(int fc) const
{
    switch (fc) {
    case 1: return QModbusDataUnit::Coils;
    case 2: return QModbusDataUnit::DiscreteInputs;
    case 4: return QModbusDataUnit::InputRegisters;
    default: return QModbusDataUnit::HoldingRegisters;
    }
}

void ModbusManager::sendRequest(QModbusDataUnit::RegisterType type, int fc, int slave,
                                quint16 address, const QVector<quint16> &values,
                                const QString &label, int taskId)
{
    if (!m_masterClient || m_masterClient->state() != QModbusDevice::ConnectedState) {
        emit toastRequested(QStringLiteral("Modbus 主站未连接"), false);
        return;
    }
    if (m_currentReply) {
        m_traffic->appendInfo(QStringLiteral("上一个请求尚未完成，已忽略本次请求（串行调度）"), true);
        return;
    }

    // 读功能码：values 携带读取数量；写功能码：values 为写入数据。
    const bool isWrite = fc == 5 || fc == 6 || fc == 15 || fc == 16;
    QModbusDataUnit unit;
    if (isWrite)
        unit = QModbusDataUnit(type, address, values);
    else
        unit = QModbusDataUnit(type, address,
                               static_cast<quint16>(qBound(1, int(values.value(0, 1)), 125)));

    m_currentReply = isWrite ? m_masterClient->sendWriteRequest(unit, slave)
                             : m_masterClient->sendReadRequest(unit, slave);
    if (!m_currentReply) {
        // 广播写入（slave == 0）不产生应答属于正常行为。
        if (slave != 0) {
            m_traffic->appendInfo(QStringLiteral("请求发送失败: %1").arg(m_masterClient->errorString()), true);
            ++m_errorCount;
            emit statsChanged();
        }
        return;
    }
    ++m_txCount;
    m_responseTimer.restart();

    // 流量记录：用自研 Codec 重建请求 ADU 供终端显示。
    QVector<quint16> pduValues = values;
    if (!isWrite)
        pduValues = QVector<quint16>{ static_cast<quint16>(unit.valueCount()) };
    const QByteArray pdu = ModbusCodec::buildPdu(static_cast<quint8>(fc), address, pduValues);
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
    m_currentLabel = label;
    m_currentAddress = address;
    m_currentFc = fc;
    m_currentSlave = slave;
    emit statsChanged();
}

void ModbusManager::onReplyFinished()
{
    QModbusReply *reply = m_currentReply;
    if (!reply)
        return;
    m_currentReply = nullptr;
    m_lastResponseMs = int(m_responseTimer.elapsed());

    if (reply->error() == QModbusDevice::NoError) {
        ++m_rxCount;
        const QModbusDataUnit unit = reply->result();
        QVector<quint16> values;
        values.reserve(int(unit.valueCount()));
        for (uint i = 0; i < unit.valueCount(); ++i)
            values.append(unit.value(i));
        // 响应 PDU: FC + 字节数 + 数据（重建用于流量显示）。
        QByteArray respPdu;
        respPdu.append(char(m_currentFc));
        if (m_currentFc >= 1 && m_currentFc <= 4) {
            respPdu.append(char(values.size() * 2));
            for (quint16 value : values) {
                respPdu.append(char(value >> 8));
                respPdu.append(char(value & 0xFF));
            }
        }
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
        }
    } else {
        ++m_errorCount;
        QString detail = reply->errorString();
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
}

void ModbusManager::manualRead(int slave, int functionCode, int address, int count)
{
    QVector<quint16> values{ quint16(qBound(1, count, 125)) };
    sendRequest(registerTypeForFc(functionCode), functionCode, slave, quint16(address),
                values, QStringLiteral("FC%1 读").arg(functionCode, 2, 10, QLatin1Char('0')), -1);
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

void ModbusManager::addTask(const QVariantMap &task)
{
    QVariantMap copy = task;
    copy.insert(QStringLiteral("status"), QStringLiteral("待开始"));
    m_tasks.append(copy);
    m_nextDue.append(0);
    emit tasksChanged();
}

void ModbusManager::updateTask(int index, const QVariantMap &task)
{
    if (index < 0 || index >= m_tasks.size())
        return;
    QVariantMap copy = task;
    copy.insert(QStringLiteral("status"), m_tasks.at(index).toMap().value(QStringLiteral("status")));
    m_tasks[index] = copy;
    emit tasksChanged();
}

void ModbusManager::removeTask(int index)
{
    if (index < 0 || index >= m_tasks.size())
        return;
    m_tasks.removeAt(index);
    m_nextDue.remove(index);
    emit tasksChanged();
}

QVariantMap ModbusManager::taskAt(int index) const
{
    return (index >= 0 && index < m_tasks.size()) ? m_tasks.at(index).toMap() : QVariantMap();
}

void ModbusManager::startPolling()
{
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("Modbus 主站未连接"), false);
        return;
    }
    for (int i = 0; i < m_tasks.size(); ++i)
        m_nextDue[i] = 0;
    m_polling = true;
    m_pollTimer.start();
    m_traffic->appendInfo(QStringLiteral("轮询已启动 · %1 个任务").arg(m_tasks.size()));
    if (g_studio)
        g_studio->log(QStringLiteral("info"), QStringLiteral("Modbus 轮询已启动"));
    emit pollingChanged();
}

void ModbusManager::stopPolling()
{
    if (!m_polling)
        return;
    m_polling = false;
    m_pollTimer.stop();
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
    if (!m_polling || !isOpen() || m_currentReply || m_tasks.isEmpty())
        return;
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    int dueIndex = -1;
    qint64 bestDue = std::numeric_limits<qint64>::max();
    for (int i = 0; i < m_tasks.size(); ++i) {
        const QVariantMap task = m_tasks.at(i).toMap();
        if (!task.value(QStringLiteral("enabled"), true).toBool())
            continue;
        if (m_nextDue.at(i) <= now && m_nextDue.at(i) < bestDue) {
            bestDue = m_nextDue.at(i);
            dueIndex = i;
        }
    }
    if (dueIndex < 0)
        return;
    const QVariantMap task = m_tasks.at(dueIndex).toMap();
    m_nextDue[dueIndex] = now + qMax(10, task.value(QStringLiteral("periodMs"), 1000).toInt());
    readTaskOnce(dueIndex);
}

void ModbusManager::updateTaskStatus(int taskId, const QString &status, bool ok)
{
    if (taskId < 0 || taskId >= m_tasks.size())
        return;
    QVariantMap task = m_tasks.at(taskId).toMap();
    task.insert(QStringLiteral("status"), status);
    task.insert(QStringLiteral("ok"), ok);
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
    if (qFuzzyIsNull(scale)) {
        preview.insert(QStringLiteral("error"), QStringLiteral("比例系数不能为 0"));
        return preview;
    }
    const double rawDouble = (value - offsetValue) / scale;
    const int raw = int(qRound(rawDouble));
    preview.insert(QStringLiteral("value"), value);
    preview.insert(QStringLiteral("raw"), raw);

    if (type == QLatin1String("uint16") || type == QLatin1String("int16")) {
        const quint16 word = quint16(raw & 0xFFFF);
        preview.insert(QStringLiteral("registers"), ChecksumEngine::wordHex(word));
        preview.insert(QStringLiteral("byteCount"), 1);
    } else {
        const quint16 word0 = quint16((raw >> 16) & 0xFFFF);
        const quint16 word1 = quint16(raw & 0xFFFF);
        const QByteArray bytes = ModbusCodec::wordsToBytes32(word0, word1, wordOrder);
        preview.insert(QStringLiteral("registers"), HexCodec::toHex(bytes));
        preview.insert(QStringLiteral("byteCount"), 2);
    }
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
            const quint16 value = simulatorValueFor(source, base, amplitude, m_simCounter, i);
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
    m_rawIsTcp = isTcp;
    if (!isTcp) {
        if (autoCrc) {
            const quint16 crc = ChecksumEngine::crc16Modbus(payload);
            payload.append(char(crc & 0xFF));
            payload.append(char(crc >> 8));
        } else if (payload.size() < 4) {
            emit toastRequested(QStringLiteral("RTU 帧（含 CRC）至少 4 字节"), false);
            return;
        }
    }

    // 打开 Raw 传输链路（跟随当前模式：RTU 用串口，TCP 用 Socket）。
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
        m_traffic->append(TrafficModel::Tx, adu, QStringLiteral("RAW TCP"));
        m_rawTcp->write(adu);
    } else {
        if (!m_rawSerial) {
            m_rawSerial = new QSerialPort(this);
            connect(m_rawSerial, &QSerialPort::readyRead, this, [this] {
                m_rawBuffer += m_rawSerial->readAll();
                m_rawIdleTimer.start();
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
        m_rawBuffer.clear();
        m_rawElapsed.restart();
        m_traffic->append(TrafficModel::Tx, payload, QStringLiteral("RAW RTU"));
        m_rawSerial->write(payload);
    }
    m_rawIdleTimer.start();
    ++m_txCount;
    emit rawResultChanged();
    emit statsChanged();
}

void ModbusManager::rawIdleFlush()
{
    if (!m_rawBusy)
        return;
    if (!m_rawBuffer.isEmpty()) {
        const ModbusCodec::ParsedResponse parsed = ModbusCodec::parseResponse(m_rawBuffer, m_rawIsTcp);
        m_traffic->append(TrafficModel::Rx, m_rawBuffer,
                          QStringLiteral("%1 ms").arg(m_rawElapsed.elapsed()));
        ++m_rxCount;
        QVariantMap result;
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
        ++m_errorCount;
        emit rawResultChanged();
        emit statsChanged();
        return;
    }
    m_rawIdleTimer.start();
}

void ModbusManager::closeRawLink()
{
    if (m_rawSerial && m_rawSerial->isOpen())
        m_rawSerial->close();
    if (m_rawTcp && m_rawTcp->state() == QAbstractSocket::ConnectedState)
        m_rawTcp->abort();
    m_rawBusy = false;
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
