#include "modbusmanager.h"
#include <QModbusReply>
#include <QSettings>
#include <QRandomGenerator>
#include "core/hexcodec.h"

int ModbusManager::connectionState() const
{
    if (m_mode == QLatin1String("asciiMaster")) return m_asciiOpen ? int(QModbusDevice::ConnectedState) : int(QModbusDevice::UnconnectedState);
    if (m_mode == QLatin1String("asciiSlave")) return m_asciiSlaveOpen ? int(QModbusDevice::ConnectedState) : int(QModbusDevice::UnconnectedState);
    const QModbusDevice *device = m_mode.endsWith("Master") ? static_cast<QModbusDevice *>(m_masterClient) : static_cast<QModbusDevice *>(m_simulator);
    if (!device) return 0;
    return device->state() == QModbusDevice::UnconnectedState && device->error() != QModbusDevice::NoError ? 4 : int(device->state());
}

void ModbusManager::setChannelOption(const QString &key, const QVariant &value)
{
    if (!m_channelOptions.contains(key)) return;
    if (key == "autoReconnect" || key == "autoPoll" || key == "keepAlive") m_channelOptions[key] = value.toBool();
    else if (key == "crcByteOrder") {
        const QString order = value.toString().trimmed().toLower();
        if (order != "little" && order != "big") { emit toastRequested(QStringLiteral("CRC 字节序无效 / Invalid CRC byte order"), false); return; }
        m_channelOptions[key] = order;
    } else if (key == "coilBitOrder") {
        const QString order = value.toString().trimmed().toLower();
        if (order != "lsb" && order != "msb") { emit toastRequested(QStringLiteral("Coil 位序无效 / Invalid coil bit order"), false); return; }
        m_channelOptions[key] = order;
    }
    else {
        bool ok = false; const int number = value.toInt(&ok);
        int low = 0, high = 60000;
        if (key == "dataBits") { low = 5; high = 8; }
        else if (key == "stopBits") { low = 1; high = 2; }
        else if (key == "flowControl") high = 2;
        else if (key == "interFrameUs") { low = -1; high = 1000000; }
        else if (key == "addressOffset") { low = -65535; high = 65535; }
        else if (key == "writeCacheMs") { low = 0; high = 60000; }
        else if (key == "tcpConcurrency") { low = 1; high = 16; }
        else if (key == "broadcastAddress") { low = 0; high = 247; }
        else if (key == "minimumPollMs") low = 10;
        else if (key == "reconnectMs") low = 100;
        else if (key == "maxRead") { low = 1; high = 125; }
        else if (key == "mergeGap") high = 124;
        if (!ok || number < low || number > high) { emit toastRequested(QStringLiteral("通道参数超出范围 / Invalid channel option"), false); return; }
        m_channelOptions[key] = number;
    }
    emit channelOptionsChanged();
}

QVariantMap ModbusManager::statistics() const
{
    return {{"txFrames", m_txCount}, {"rxFrames", m_rxCount}, {"txBytes", m_txBytes}, {"rxBytes", m_rxBytes},
            {"errors", m_errorCount}, {"timeout", m_timeouts}, {"exception", m_exceptions},
            {"crcError", m_crcErrors}, {"frameError", m_frameErrors},
            {"success", m_successCount + m_errorCount ? 100.0 * m_successCount / (m_successCount + m_errorCount) : 0.0},
            {"rttCurrent", m_lastResponseMs}, {"rttMin", qMax(0, m_rttMin)}, {"rttMax", m_rttMax},
            {"rttAverage", m_successCount ? double(m_rttSum) / m_successCount : 0.0}};
}

void ModbusManager::resetStatistics()
{
    if (m_currentReply || m_scanning || m_rawBusy) return;
    m_txCount = m_rxCount = m_errorCount = m_txBytes = m_rxBytes = m_timeouts = m_exceptions = m_rttSum = 0;
    m_crcErrors = m_frameErrors = 0;
    m_lastResponseMs = m_rttMax = 0; m_rttMin = -1;
    m_successCount = 0;
    emit statsChanged();
}

void ModbusManager::startScan(int first, int last, int fc, int address, int timeoutMs)
{
    if (!m_masterClient || !isOpen() || m_scanning || batchWriting() || m_currentReply || !m_requests.isEmpty() || m_rawBusy
        || first < 1 || last > 247 || first > last || fc < 1 || fc > 4 || address < 0 || address > 65535) {
        emit toastRequested(QStringLiteral("扫描参数无效或链路忙 / Invalid scan parameters or busy link"), false); return;
    }
    stopPolling();
    m_scanning = true; m_scanResults.clear(); m_scanNext = first; m_scanLast = last;
    m_scanFc = fc; m_scanAddress = address; m_scanTimeout = qBound(20, timeoutMs, 10000);
    emit scanChanged(); scanNext();
}

void ModbusManager::scanNext()
{
    if (!m_scanning || !m_masterClient || !isOpen()) { stopScan(); return; }
    if (m_scanNext > m_scanLast) { stopScan(); return; }
    m_masterClient->setTimeout(m_scanTimeout); m_masterClient->setNumberOfRetries(0);
    const int slave = m_scanNext++;
    QElapsedTimer elapsed; elapsed.start();
    auto *reply = m_masterClient->sendReadRequest(QModbusDataUnit(registerTypeForFc(m_scanFc), m_scanAddress, 1), slave);
    if (!reply) { stopScan(); emit toastRequested(QStringLiteral("无法提交扫描请求 / Scan request failed"), false); return; }
    m_scanReply = reply;
    const auto finish = [this, reply, slave, elapsed] {
        if (m_scanReply != reply) return;
        m_scanReply = nullptr;
        const bool online = reply->error() == QModbusDevice::NoError || reply->rawResult().isException();
        m_scanResults.append(QVariantMap{{"unitId", slave}, {"online", online}, {"rtt", elapsed.elapsed()},
            {"status", online ? (reply->rawResult().isException() ? "EXCEPTION RESPONSE" : "ONLINE") : "NO RESPONSE"}});
        reply->deleteLater(); emit scanChanged(); QTimer::singleShot(0, this, &ModbusManager::scanNext);
    };
    connect(reply, &QModbusReply::finished, this, finish);
    if (reply->isFinished()) QTimer::singleShot(0, this, finish);
}

void ModbusManager::stopScan()
{
    if (m_scanReply) { disconnect(m_scanReply, nullptr, this, nullptr); m_scanReply->deleteLater(); m_scanReply = nullptr; }
    if (!m_scanning) return;
    m_scanning = false;
    if (m_masterClient) { m_masterClient->setTimeout(m_timeoutMs); m_masterClient->setNumberOfRetries(m_retries); }
    emit scanChanged();
}

void ModbusManager::addDiscoveredDevices()
{
    for (const auto &entry : m_scanResults) {
        const auto result = entry.toMap(); if (!result.value("online").toBool()) continue;
        const int slave = result.value("unitId").toInt();
        bool exists = false;
        for (const auto &device : m_devices->definitions()) if (device.toMap().value("unitId").toInt() == slave) exists = true;
        if (!exists) addDevice(QStringLiteral("Device %1").arg(slave), slave);
    }
}

void ModbusManager::startRawLoop(const QString &request, bool tcp, bool crc, int count, int intervalMs, int jitterMs)
{
    QByteArray bytes; QString error;
    if (batchWriting() || m_rawBusy || count < 1 || count > 1000000 || intervalMs < 10 || jitterMs < 0
        || !HexCodec::parseHex(request, bytes, &error) || bytes.size() < 2) {
        emit toastRequested(QStringLiteral("循环参数无效或 Raw 请求忙 / Invalid loop or busy Raw request"), false); return;
    }
    m_rawLoopRequest = request; m_rawLoopTcp = tcp; m_rawLoopCrc = crc;
    m_rawLoopCount = count; m_rawLoopSent = 0; m_rawLoopInterval = qBound(10, intervalMs, 3600000);
    m_rawLoopJitter = qBound(0, jitterMs, m_rawLoopInterval - 1);
    m_rawLoopActive = true; emit rawLoopChanged(); rawLoopTick();
}

void ModbusManager::stopRawLoop()
{
    m_rawLoopTimer.stop();
    if (!m_rawLoopActive) return;
    m_rawLoopActive = false; emit rawLoopChanged();
}

void ModbusManager::rawLoopTick()
{
    if (!m_rawLoopActive) return;
    if (m_rawBusy) { m_rawLoopTimer.start(10); return; }
    if (m_rawLoopSent >= m_rawLoopCount) { stopRawLoop(); return; }
    sendRawFrame(m_rawLoopRequest, m_rawLoopTcp, m_rawLoopCrc);
    if (!m_rawBusy) { stopRawLoop(); return; }
    ++m_rawLoopSent; emit rawLoopChanged();
    const int jitter = m_rawLoopJitter ? int(QRandomGenerator::global()->bounded(quint32(2 * m_rawLoopJitter + 1))) - m_rawLoopJitter : 0;
    m_rawLoopTimer.start(m_rawLoopInterval + jitter);
}

void ModbusManager::saveRawTemplate(const QString &name, const QString &request, bool tcp, bool crc)
{
    QByteArray bytes; QString error;
    if (name.trimmed().isEmpty() || !HexCodec::parseHex(request, bytes, &error) || bytes.size() < 2) {
        emit toastRequested(QStringLiteral("模板名称或请求无效 / Invalid template"), false); return;
    }
    QVariantMap item{{"name", name.trimmed()}, {"request", request}, {"tcp", tcp}, {"crc", crc}};
    int existing = -1;
    for (int i = 0; i < m_rawTemplates.size(); ++i) if (m_rawTemplates[i].toMap().value("name") == name.trimmed()) existing = i;
    if (existing >= 0) m_rawTemplates[existing] = item; else m_rawTemplates.append(item);
    QSettings().setValue(m_settingsPrefix + "/rawTemplates", m_rawTemplates); emit rawTemplatesChanged();
}

void ModbusManager::removeRawTemplate(int index)
{
    if (index < 0 || index >= m_rawTemplates.size()) return;
    m_rawTemplates.removeAt(index); QSettings().setValue(m_settingsPrefix + "/rawTemplates", m_rawTemplates); emit rawTemplatesChanged();
}
