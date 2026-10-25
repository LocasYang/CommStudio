#include "serialmanager.h"

#include "core/hexcodec.h"
#include "studio.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

namespace {

Studio *g_studio = nullptr;

QVariantMap portToVariant(const QSerialPortInfo &info)
{
    QVariantMap map;
    const QString description = info.description().isEmpty()
                                    ? QStringLiteral("未知设备") : info.description();
    QString label = QStringLiteral("%1 · %2").arg(info.portName(), description);
    if (info.hasVendorIdentifier() && info.hasProductIdentifier())
        label += QStringLiteral(" (VID:PID %1:%2)")
                     .arg(info.vendorIdentifier(), 4, 16, QLatin1Char('0'))
                     .arg(info.productIdentifier(), 4, 16, QLatin1Char('0')).toUpper();
    map.insert(QStringLiteral("name"), info.portName());
    map.insert(QStringLiteral("label"), label);
    map.insert(QStringLiteral("description"), description);
    map.insert(QStringLiteral("manufacturer"), info.manufacturer());
    map.insert(QStringLiteral("serialNumber"), info.serialNumber());
    map.insert(QStringLiteral("location"), info.systemLocation());
    map.insert(QStringLiteral("vid"), info.hasVendorIdentifier() ? info.vendorIdentifier() : 0);
    map.insert(QStringLiteral("pid"), info.hasProductIdentifier() ? info.productIdentifier() : 0);
    return map;
}

} // namespace

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
    , m_traffic(new TrafficModel(this))
    , m_port(new QSerialPort(this))
{
    if (qobject_cast<Studio *>(parent)) {
        g_studio = qobject_cast<Studio *>(parent);
        connect(g_studio, &Studio::languageChanged, this, &SerialManager::configChanged);
    }

    connect(m_port, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
    connect(m_port, &QSerialPort::errorOccurred, this, &SerialManager::handlePortError);

    m_portPollTimer.setInterval(2000);
    connect(&m_portPollTimer, &QTimer::timeout, this, &SerialManager::pollPorts);
    m_portPollTimer.start();

    m_pinPollTimer.setInterval(500);
    connect(&m_pinPollTimer, &QTimer::timeout, this, &SerialManager::pollPins);

    m_rateTimer.setInterval(1000);
    connect(&m_rateTimer, &QTimer::timeout, this, [this] {
        m_rxRate = m_rxWindow;
        m_rxWindow = 0;
        if (m_rxRate > m_peakRate)
            m_peakRate = m_rxRate;
        emit statsChanged();
    });
    m_rateTimer.start();

    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &SerialManager::tryReconnect);

    connect(&m_timerSend, &QTimer::timeout, this, &SerialManager::onTimerSend);

    m_fileTimer.setSingleShot(true);
    connect(&m_fileTimer, &QTimer::timeout, this, &SerialManager::onFileChunk);

    m_traffic->appendInfo(g_studio
                              ? g_studio->localize(QStringLiteral("Serial assistant ready · refresh ports, choose a device, and open a connection"),
                                                   QStringLiteral("串口助手就绪 · 刷新端口后选择设备并打开连接"))
                              : QStringLiteral("Serial assistant ready"));
    refreshPorts();
    loadSettings();
}

QString SerialManager::configSummary() const
{
    const QString parity = m_parity == QLatin1String("none") ? QStringLiteral("N")
                           : m_parity == QLatin1String("even") ? QStringLiteral("E")
                           : m_parity == QLatin1String("odd") ? QStringLiteral("O")
                           : m_parity.left(1).toUpper();
    const QString flow = m_flowControl == QLatin1String("none")
                         ? (g_studio ? g_studio->localize(QStringLiteral("No flow control"), QStringLiteral("无流控")) : QStringLiteral("No flow control"))
                         : m_flowControl == QLatin1String("rtscts") ? QStringLiteral("RTS/CTS")
                         : QStringLiteral("XON/XOFF");
    return QStringLiteral("%1 · %2 · %3-%4-%5 · %6")
        .arg(m_portName.isEmpty()
                 ? (g_studio ? g_studio->localize(QStringLiteral("No port selected"),
                                                   QStringLiteral("未选择端口"))
                             : QStringLiteral("No port selected"))
                 : m_portName)
        .arg(m_baudRate)
        .arg(m_dataBits).arg(parity).arg(QString::number(m_stopBits))
        .arg(flow);
}

void SerialManager::loadSettings()
{
    QSettings settings;
    m_portName = settings.value(QStringLiteral("serial/portName"), m_portName).toString();
    const QString savedBaud = settings.value(QStringLiteral("serial/baudRate"), m_baudRate).toString();
    bool baudOk = false;
    savedBaud.toInt(&baudOk);
    if (baudOk && savedBaud.toInt() > 0)
        m_baudRate = savedBaud;
    const int savedDataBits = settings.value(QStringLiteral("serial/dataBits"), m_dataBits).toInt();
    if (savedDataBits >= 5 && savedDataBits <= 8)
        m_dataBits = savedDataBits;
    const QString savedParity = settings.value(QStringLiteral("serial/parity"), m_parity).toString();
    if (QStringList{QStringLiteral("none"), QStringLiteral("even"), QStringLiteral("odd"),
                    QStringLiteral("mark"), QStringLiteral("space")}.contains(savedParity))
        m_parity = savedParity;
    const double savedStopBits = settings.value(QStringLiteral("serial/stopBits"), m_stopBits).toDouble();
    if (qFuzzyCompare(savedStopBits, 1.0) || qFuzzyCompare(savedStopBits, 1.5)
        || qFuzzyCompare(savedStopBits, 2.0))
        m_stopBits = savedStopBits;
    const QString savedFlow = settings.value(QStringLiteral("serial/flowControl"), m_flowControl).toString();
    if (QStringList{QStringLiteral("none"), QStringLiteral("rtscts"), QStringLiteral("xonxoff")}.contains(savedFlow))
        m_flowControl = savedFlow;
    m_autoReconnect = settings.value(QStringLiteral("serial/autoReconnect"), m_autoReconnect).toBool();
    m_hexSend = settings.value(QStringLiteral("serial/hexSend"), m_hexSend).toBool();
    m_escapeEnabled = settings.value(QStringLiteral("serial/escapeEnabled"), m_escapeEnabled).toBool();
    const QString savedLineEnding = settings.value(QStringLiteral("serial/lineEnding"), m_lineEnding).toString();
    if (QStringList{QStringLiteral("none"), QStringLiteral("cr"), QStringLiteral("lf"),
                    QStringLiteral("crlf"), QStringLiteral("lfcr")}.contains(savedLineEnding))
        m_lineEnding = savedLineEnding;
    emit portNameChanged(); emit baudRateChanged(); emit dataBitsChanged(); emit parityChanged();
    emit stopBitsChanged(); emit flowControlChanged(); emit autoReconnectChanged();
    emit hexSendChanged(); emit escapeEnabledChanged(); emit lineEndingChanged(); emit configChanged();
}

void SerialManager::saveSettings() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("serial/portName"), m_portName);
    settings.setValue(QStringLiteral("serial/baudRate"), m_baudRate);
    settings.setValue(QStringLiteral("serial/dataBits"), m_dataBits);
    settings.setValue(QStringLiteral("serial/parity"), m_parity);
    settings.setValue(QStringLiteral("serial/stopBits"), m_stopBits);
    settings.setValue(QStringLiteral("serial/flowControl"), m_flowControl);
    settings.setValue(QStringLiteral("serial/autoReconnect"), m_autoReconnect);
    settings.setValue(QStringLiteral("serial/hexSend"), m_hexSend);
    settings.setValue(QStringLiteral("serial/escapeEnabled"), m_escapeEnabled);
    settings.setValue(QStringLiteral("serial/lineEnding"), m_lineEnding);
}

void SerialManager::setPortName(const QString &name)
{
    if (isOpen()) { emit toastRequested(QStringLiteral("串口已连接，请先断开后修改通信参数"), false); return; }
    if (m_portName == name)
        return;
    m_portName = name;
    emit portNameChanged();
    saveSettings(); emit configChanged();
}

void SerialManager::setBaudRate(const QString &rate)
{
    if (isOpen()) { emit toastRequested(QStringLiteral("串口已连接，请先断开后修改通信参数"), false); return; }
    if (m_baudRate == rate)
        return;
    m_baudRate = rate;
    emit baudRateChanged(); saveSettings(); emit configChanged();
}

void SerialManager::setDataBits(int bits)
{
    if (isOpen()) { emit toastRequested(QStringLiteral("串口已连接，请先断开后修改通信参数"), false); return; }
    if (m_dataBits == bits)
        return;
    m_dataBits = bits;
    emit dataBitsChanged(); saveSettings(); emit configChanged();
}

void SerialManager::setParity(const QString &parity)
{
    if (isOpen()) { emit toastRequested(QStringLiteral("串口已连接，请先断开后修改通信参数"), false); return; }
    if (m_parity == parity)
        return;
    m_parity = parity;
    emit parityChanged(); saveSettings(); emit configChanged();
}

void SerialManager::setStopBits(double bits)
{
    if (isOpen()) { emit toastRequested(QStringLiteral("串口已连接，请先断开后修改通信参数"), false); return; }
    if (qFuzzyCompare(m_stopBits, bits))
        return;
    m_stopBits = bits;
    emit stopBitsChanged(); saveSettings(); emit configChanged();
}

void SerialManager::setFlowControl(const QString &flow)
{
    if (isOpen()) { emit toastRequested(QStringLiteral("串口已连接，请先断开后修改通信参数"), false); return; }
    if (m_flowControl == flow)
        return;
    m_flowControl = flow;
    emit flowControlChanged(); saveSettings(); emit configChanged();
}

void SerialManager::setAutoReconnect(bool enabled)
{
    if (m_autoReconnect == enabled)
        return;
    m_autoReconnect = enabled;
    emit autoReconnectChanged(); saveSettings();
}

void SerialManager::setHexSend(bool enabled)
{
    if (m_hexSend == enabled)
        return;
    m_hexSend = enabled;
    emit hexSendChanged(); saveSettings();
}

void SerialManager::setEscapeEnabled(bool enabled)
{
    if (m_escapeEnabled == enabled)
        return;
    m_escapeEnabled = enabled;
    emit escapeEnabledChanged(); saveSettings();
}

void SerialManager::setLineEnding(const QString &ending)
{
    if (m_lineEnding == ending)
        return;
    m_lineEnding = ending;
    emit lineEndingChanged(); saveSettings();
}

bool SerialManager::isOpen() const
{
    return m_port && m_port->isOpen();
}

bool SerialManager::dtr() const
{
    return m_port && m_port->isDataTerminalReady();
}

void SerialManager::setDtr(bool on)
{
    if (!m_port || !m_port->isOpen())
        return;
    m_port->setDataTerminalReady(on);
    emit pinsChanged();
}

bool SerialManager::rts() const
{
    return m_port && m_port->isRequestToSend();
}

void SerialManager::setRts(bool on)
{
    if (!m_port || !m_port->isOpen())
        return;
    m_port->setRequestToSend(on);
    emit pinsChanged();
}

void SerialManager::refreshPorts()
{
    pollPorts();
}

void SerialManager::pollPorts()
{
    QVariantList infos;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports)
        infos.append(portToVariant(info));

    QStringList previousNames;
    for (const QVariant &entry : std::as_const(m_portInfos))
        previousNames << entry.toMap().value(QStringLiteral("name")).toString();
    QStringList newNames;
    for (const QVariant &entry : infos)
        newNames << entry.toMap().value(QStringLiteral("name")).toString();

    m_portInfos = infos;
    emit portInfosChanged();

    if (previousNames != newNames) {
        QStringList appeared;
        for (const QString &name : newNames) {
            if (!previousNames.contains(name))
                appeared << name;
        }
        if (!appeared.isEmpty() && !previousNames.isEmpty()) {
            m_traffic->appendInfo(QStringLiteral("检测到新串口设备: %1").arg(appeared.join(", ")));
            if (g_studio)
                g_studio->log(QStringLiteral("info"),
                              QStringLiteral("串口设备变化: 新增 %1").arg(appeared.join(", ")));
        }
    }
}

void SerialManager::applyConfig()
{
    m_port->setPortName(m_portName);
    m_port->setBaudRate(m_baudRate.toInt());

    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    if (m_dataBits == 5) dataBits = QSerialPort::Data5;
    else if (m_dataBits == 6) dataBits = QSerialPort::Data6;
    else if (m_dataBits == 7) dataBits = QSerialPort::Data7;
    m_port->setDataBits(dataBits);

    QSerialPort::Parity parity = QSerialPort::NoParity;
    if (m_parity == QLatin1String("even")) parity = QSerialPort::EvenParity;
    else if (m_parity == QLatin1String("odd")) parity = QSerialPort::OddParity;
    else if (m_parity == QLatin1String("mark")) parity = QSerialPort::MarkParity;
    else if (m_parity == QLatin1String("space")) parity = QSerialPort::SpaceParity;
    m_port->setParity(parity);

    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    if (m_stopBits > 1.5) stopBits = QSerialPort::TwoStop;
    else if (m_stopBits > 1.0) stopBits = QSerialPort::OneAndHalfStop;
    m_port->setStopBits(stopBits);

    QSerialPort::FlowControl flow = QSerialPort::NoFlowControl;
    if (m_flowControl == QLatin1String("rtscts")) flow = QSerialPort::HardwareControl;
    else if (m_flowControl == QLatin1String("xonxoff")) flow = QSerialPort::SoftwareControl;
    m_port->setFlowControl(flow);
}

void SerialManager::openConnection()
{
    if (isOpen()) {
        close();
        return;
    }
    if (m_portName.isEmpty()) {
        m_traffic->appendInfo(QStringLiteral("请先选择串口端口"), true);
        emit toastRequested(QStringLiteral("请先选择串口端口"), false);
        return;
    }
    bool baudOk = false;
    const int baud = m_baudRate.toInt(&baudOk);
    if (!baudOk || baud <= 0) {
        const QString message = QStringLiteral("无效波特率: %1").arg(m_baudRate);
        m_traffic->appendInfo(message, true);
        emit toastRequested(message, false);
        return;
    }
    m_userClose = false;
    m_reconnectAttempt = 0;
    applyConfig();
    if (!m_port->open(QIODevice::ReadWrite)) {
        const QString message = QStringLiteral("打开 %1 失败: %2")
                                    .arg(m_portName, m_port->errorString());
        m_traffic->appendInfo(message, true);
        if (g_studio)
            g_studio->log(QStringLiteral("error"), message);
        emit toastRequested(message, false);
        return;
    }
    const QString info = QStringLiteral("串口已连接 %1 @ %2  (%3/%4/%5)")
                             .arg(m_portName, m_baudRate, m_parity.toUpper(),
                                  QString::number(m_dataBits), QString::number(m_stopBits));
    m_traffic->appendInfo(info);
    if (g_studio) {
        g_studio->log(QStringLiteral("success"), info);
        g_studio->addTraffic(0, 0, 1);
        g_studio->setStatusMessage(QStringLiteral("串口 %1 已连接").arg(m_portName));
    }
    m_pinPollTimer.start();
    pollPins();
    emit openChanged();
    emit configChanged();
    emit pinsChanged();
}

void SerialManager::close()
{
    if (!isOpen())
        return;
    m_userClose = true;
    m_reconnectTimer.stop();
    m_port->close();
    m_traffic->appendInfo(QStringLiteral("串口已断开"));
    if (g_studio) {
        g_studio->log(QStringLiteral("info"), QStringLiteral("串口 %1 已断开").arg(m_portName));
        g_studio->addTraffic(0, 0, -1);
        g_studio->setStatusMessage(QStringLiteral("串口已断开"));
    }
    m_pinPollTimer.stop();
    emit openChanged();
    emit configChanged();
    emit pinsChanged();
}

void SerialManager::handlePortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
        return;
    // 手动关闭会撤销底层 Win32 串口句柄；驱动在此期间上报的错误不应被
    // 误判为意外断线，也不应再次触发“串口异常”提示。
    if (m_userClose)
        return;
    // 没有处于已打开状态的端口，就不是运行中的链路掉线。启动时恢复的
    // 端口名称、打开失败后的残留回调以及 close() 后的第二个回调都忽略，
    // 避免在没有用户操作时显示“句柄无效”。打开失败由 openConnection()
    // 自己给出明确的失败提示。
    if (!m_port->isOpen())
        return;
    // 资源类错误（拔线 / 被占用 / 设备消失）触发重连。
    const bool fatal = error == QSerialPort::ResourceError
                       || error == QSerialPort::DeviceNotFoundError
                       || error == QSerialPort::PermissionError
                       || error == QSerialPort::OpenError;
    if (!fatal) {
        if (m_port->isOpen())
            m_traffic->appendInfo(QStringLiteral("串口警告: %1").arg(m_port->errorString()), true);
        return;
    }
    const QString message = QStringLiteral("串口异常: %1").arg(m_port->errorString());
    if (m_port->isOpen())
        m_port->close();
    m_pinPollTimer.stop();
    m_traffic->appendInfo(message, true);
    if (g_studio) {
        g_studio->log(QStringLiteral("error"), message);
        g_studio->addTraffic(0, 0, -1);
        g_studio->setStatusMessage(message);
    }
    emit toastRequested(message, false);
    emit openChanged();
    if (m_autoReconnect && !m_userClose && !m_portName.isEmpty())
        scheduleReconnect();
}

void SerialManager::scheduleReconnect()
{
    // 退避: 0.5 s → 1 s → 2 s → 4 s → 最大 5 s。
    static const int delays[] = { 500, 1000, 2000, 4000, 5000 };
    const int delay = delays[qMin(m_reconnectAttempt, 4)];
    ++m_reconnectAttempt;
    m_traffic->appendInfo(QStringLiteral("%1 ms 后尝试自动重连（第 %2 次）")
                              .arg(delay).arg(m_reconnectAttempt));
    m_reconnectTimer.start(delay);
    emit reconnectingChanged();
}

void SerialManager::tryReconnect()
{
    if (isOpen() || m_portName.isEmpty())
        return;
    applyConfig();
    if (m_port->open(QIODevice::ReadWrite)) {
        m_reconnectAttempt = 0;
        m_traffic->appendInfo(QStringLiteral("串口自动重连成功"));
        if (g_studio) {
            g_studio->log(QStringLiteral("success"),
                          QStringLiteral("串口 %1 自动重连成功").arg(m_portName));
            g_studio->addTraffic(0, 0, 1);
        }
        m_pinPollTimer.start();
        emit openChanged();
        emit reconnectingChanged();
        emit pinsChanged();
    } else {
        scheduleReconnect();
    }
}

void SerialManager::pollPins()
{
    if (!isOpen())
        return;
    const auto pins = m_port->pinoutSignals();
    m_pinCts = pins & QSerialPort::ClearToSendSignal;
    m_pinDsr = pins & QSerialPort::DataSetReadySignal;
    m_pinDcd = pins & QSerialPort::DataCarrierDetectSignal;
    m_pinRi = pins & QSerialPort::RingIndicatorSignal;
    emit pinsChanged();
}

void SerialManager::onReadyRead()
{
    const QByteArray data = m_port->readAll();
    if (data.isEmpty())
        return;
    m_rxBytes += data.size();
    m_rxWindow += data.size();
    m_traffic->append(TrafficModel::Rx, data, m_portName);
    if (g_studio)
        g_studio->addTraffic(data.size(), 0, 0);
    emit statsChanged();
}

QString SerialManager::send(const QString &text)
{
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("串口未连接，无法发送"), false);
        return QString();
    }
    QString error;
    const QByteArray payload = HexCodec::buildPayload(text, m_hexSend, m_escapeEnabled,
                                                      m_lineEnding, &error);
    if (payload.isEmpty() && !error.isEmpty()) {
        emit toastRequested(error, false);
        return QString();
    }
    if (payload.isEmpty())
        return QString();
    const qint64 written = m_port->write(payload);
    if (written < 0) {
        m_traffic->appendInfo(QStringLiteral("发送失败: %1").arg(m_port->errorString()), true);
        return QString();
    }
    m_txBytes += written;
    m_traffic->append(TrafficModel::Tx, payload, m_portName);
    if (g_studio)
        g_studio->addTraffic(0, written, 0);
    emit statsChanged();
    return text;
}

void SerialManager::startTimerSend(const QString &text, int intervalMs, int repeatCount)
{
    m_timerText = text;
    m_timerRemaining = repeatCount > 0 ? repeatCount : -1;
    m_timerSend.start(qMax(10, intervalMs));
    emit timerChanged();
}

void SerialManager::stopTimerSend()
{
    if (m_timerSend.isActive()) {
        m_timerSend.stop();
        if (g_studio)
            g_studio->log(QStringLiteral("info"), QStringLiteral("串口定时发送已停止"));
    }
    m_timerRemaining = 0;
    emit timerChanged();
}

void SerialManager::onTimerSend()
{
    if (m_timerRemaining > 0) {
        --m_timerRemaining;
        if (m_timerRemaining == 0) {
            send(m_timerText);
            m_timerSend.stop();
            emit timerChanged();
            return;
        }
    }
    send(m_timerText);
}

void SerialManager::sendFile(const QUrl &path, int chunkSize, int intervalMs)
{
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("串口未连接，无法发送文件"), false);
        return;
    }
    if (m_fileActive) {
        emit toastRequested(QStringLiteral("文件发送进行中"), false);
        return;
    }
    const QString local = path.toLocalFile();
    QFile *file = new QFile(local, this);
    if (!file->open(QIODevice::ReadOnly)) {
        emit toastRequested(QStringLiteral("无法打开文件: %1").arg(local), false);
        delete file;
        return;
    }
    m_file = file;
    m_fileName = QFileInfo(local).fileName();
    m_fileSize = file->size();
    m_fileSent = 0;
    m_fileStarted = QDateTime::currentMSecsSinceEpoch();
    m_fileProgress = 0;
    m_fileActive = true;
    m_traffic->appendInfo(QStringLiteral("开始发送文件 %1 (%2 字节, 每块 %3)")
                              .arg(m_fileName).arg(m_fileSize).arg(chunkSize));
    if (g_studio)
        g_studio->log(QStringLiteral("info"),
                      QStringLiteral("开始发送文件 %1").arg(m_fileName));
    emit fileChanged();
    m_fileTimer.start(qMax(0, intervalMs));
}

void SerialManager::onFileChunk()
{
    if (!m_file || !isOpen()) {
        finishFileSend(QStringLiteral("串口已断开"), false);
        return;
    }
    const int chunkSize = 4096;
    const QByteArray chunk = m_file->read(chunkSize);
    if (chunk.isEmpty()) {
        finishFileSend(QStringLiteral("文件发送完成"), true);
        return;
    }
    const qint64 written = m_port->write(chunk);
    if (written < 0) {
        finishFileSend(QStringLiteral("写入失败: %1").arg(m_port->errorString()), false);
        return;
    }
    m_txBytes += written;
    m_fileSent += written;
    m_traffic->append(TrafficModel::Tx, chunk, QStringLiteral("FILE"));
    if (g_studio)
        g_studio->addTraffic(0, written, 0);
    m_fileProgress = m_fileSize > 0
                         ? static_cast<int>(qMin<qint64>(100, m_fileSent * 100 / m_fileSize))
                         : 100;
    const qint64 elapsed = qMax<qint64>(1, QDateTime::currentMSecsSinceEpoch() - m_fileStarted);
    m_fileRate = QStringLiteral("%1 B/s")
                     .arg(QString::number(m_fileSent * 1000.0 / elapsed, 'f', 0));
    emit statsChanged();
    emit fileChanged();
    m_fileTimer.start(0);
}

void SerialManager::cancelFileSend()
{
    if (m_fileActive)
        finishFileSend(QStringLiteral("文件发送已取消"), false);
}

void SerialManager::finishFileSend(const QString &reason, bool ok)
{
    m_fileTimer.stop();
    if (m_file) {
        m_file->close();
        m_file->deleteLater();
        m_file = nullptr;
    }
    m_fileActive = false;
    m_traffic->appendInfo(reason, !ok);
    if (g_studio)
        g_studio->log(ok ? QStringLiteral("success") : QStringLiteral("warning"),
                      QStringLiteral("串口文件发送: %1 (%2)").arg(reason, m_fileName));
    emit fileChanged();
}

void SerialManager::sendBreak(int durationMs)
{
    if (!isOpen())
        return;
    m_port->setBreakEnabled(true);
    QTimer::singleShot(qMax(1, durationMs), this, [this] {
        if (m_port && m_port->isOpen())
            m_port->setBreakEnabled(false);
    });
    m_traffic->appendInfo(QStringLiteral("已发送 Break 信号 (%1 ms)").arg(durationMs));
}
