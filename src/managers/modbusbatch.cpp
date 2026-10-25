#include "modbusmanager.h"
#include "core/modbuscodec.h"
#include <algorithm>
#include <limits>

// Plans are value snapshots. Editing CMD/packing/timing options affects the next
// start, not an already running cycle (MThings manual pp. 56, 66-67).
bool ModbusManager::prepareBatch(const QStringList &deviceIds, QList<PendingRequest> &prepared, QString &error) const
{
    prepared.clear();
    if (deviceIds.isEmpty()) { error = QStringLiteral("请勾选设备 / Select devices first"); return false; }
    QSet<QString> seen;
    for (const auto &id : deviceIds) {
        if (seen.contains(id)) continue;
        seen.insert(id);
        QVariantMap device;
        for (const auto &entry : m_devices->definitions())
            if (entry.toMap().value("id").toString() == id) { device = entry.toMap(); break; }
        if (device.isEmpty() || !m_devices->isDeviceEnabled(id)) {
            error = QStringLiteral("设备不存在或已停用 / Device missing or disabled"); return false;
        }
        const auto options = m_devices->advancedOptionsForId(id);
        QList<PendingRequest> rows;
        for (const auto &entry : m_points->definitions()) {
            const auto point = entry.toMap();
            if (point.value("deviceId").toString() != id || !point.value("batchWrite").toBool()) continue;
            QVector<quint16> values;
            if (!ModbusCodec::encodePointValue(point, point.value("writeValue").toString(), values, error)) {
                error = point.value("name").toString() + ": " + error; return false;
            }
            const int address = point.value("address").toInt();
            const int wire = address + options.value("addressOffset").toInt();
            if (wire < 0 || wire + values.size() > 65536) {
                error = QStringLiteral("批量写地址偏移越界 / Batch write address offset out of range"); return false;
            }
            auto snapshot = options;
            snapshot["periodMs"] = qMax(qMax(10, m_channelOptions.value("minimumPollMs").toInt()), point.value("periodMs", 1000).toInt());
            const int timeout = point.value("timeoutMs", -1).toInt(), retries = point.value("retries", -1).toInt();
            snapshot["timeoutMs"] = timeout < 0 ? m_timeoutMs : timeout;
            snapshot["retries"] = retries < 0 ? m_retries : retries;
            const bool coil = point.value("fc").toInt() == 1;
            const int fc = values.size() > 1 ? (coil ? 15 : 16)
                : options.value(coil ? "singleCoilWriteCode" : "singleRegisterWriteCode", coil ? 5 : 6).toInt();
            rows.append({registerTypeForFc(fc), fc, device.value("unitId").toInt(), quint16(address), values,
                QStringLiteral("Batch / 批量写 %1").arg(point.value("name").toString()), -1, id, 0, {}, snapshot});
        }
        const QString mode = options.value("packageMode").toString();
        if (mode == "address") std::stable_sort(rows.begin(), rows.end(), [](const auto &a, const auto &b) {
            if (a.type != b.type) return a.type < b.type;
            return a.address < b.address;
        });
        for (const auto &row : rows) {
            if (mode != "line" && !prepared.isEmpty()) {
                auto &last = prepared.last();
                const int limit = row.type == QModbusDataUnit::Coils ? 1968 : 123;
                if (last.deviceId == id && last.type == row.type && last.options == row.options
                    && int(last.address) + last.values.size() == row.address
                    && last.values.size() + row.values.size() <= limit) {
                    last.values.append(row.values);
                    last.fc = row.type == QModbusDataUnit::Coils ? 15 : 16;
                    last.label += ", " + row.label;
                    continue;
                }
            }
            prepared.append(row);
        }
    }
    if (prepared.isEmpty() || prepared.size() > 256) {
        error = prepared.isEmpty() ? QStringLiteral("没有参与批量写的数据点 / No batch-write points")
                                   : QStringLiteral("批量写计划最多 256 个报文 / At most 256 batch frames");
        return false;
    }
    return true;
}

bool ModbusManager::canStartBatchWriting(const QStringList &deviceIds)
{
    QList<PendingRequest> plan; QString error;
    if (!isOpen() || m_scanning || m_rawLoopActive) error = QStringLiteral("请连接通道并停止扫描/Raw循环 / Connect and stop scanning or Raw loop first");
    else if (m_mode == "asciiMaster") error = QStringLiteral("ASCII 循环批量写尚未支持 / ASCII cyclic batch write is not supported yet");
    else prepareBatch(deviceIds, plan, error);
    if (!error.isEmpty()) { emit toastRequested(error, false); return false; }
    return true;
}

bool ModbusManager::startBatchWriting(const QStringList &deviceIds)
{
    if (!canStartBatchWriting(deviceIds)) return false;
    QList<PendingRequest> plan; QString error;
    if (!prepareBatch(deviceIds, plan, error)) return false;
    m_batchPlan = plan;
    m_batchDue.fill(0, plan.size());
    m_batchClock.restart();
    m_preferBatch = true;
    m_pollTimer.start();
    emit batchWritingChanged();
    m_traffic->appendInfo(QStringLiteral("循环批量写已启动 · %1 个报文；参数变更需停止后重启").arg(plan.size()));
    return true;
}

void ModbusManager::stopBatchWriting()
{
    if (!batchWriting()) return;
    m_batchPlan.clear(); m_batchDue.clear();
    if (!m_polling) m_pollTimer.stop();
    emit batchWritingChanged();
    m_traffic->appendInfo(QStringLiteral("循环批量写已停止；已发送请求仍等待结束"));
}

void ModbusManager::removeBatchDevice(const QString &id)
{
    const bool active = batchWriting();
    for (int i = m_batchPlan.size() - 1; i >= 0; --i)
        if (m_batchPlan[i].deviceId == id) { m_batchPlan.removeAt(i); m_batchDue.removeAt(i); }
    if (active && !batchWriting()) {
        if (!m_polling) m_pollTimer.stop();
        emit batchWritingChanged();
    }
}

bool ModbusManager::batchWriteTick()
{
    if (!batchWriting()) return false;
    const auto now = m_batchClock.elapsed();
    int due = -1;
    qint64 oldest = std::numeric_limits<qint64>::max();
    for (int i = 0; i < m_batchPlan.size(); ++i) {
        if (!m_devices->isDeviceEnabled(m_batchPlan[i].deviceId)) continue;
        if (m_batchDue[i] <= now && m_batchDue[i] < oldest) { oldest = m_batchDue[i]; due = i; }
    }
    if (due < 0) return false;
    const auto request = m_batchPlan[due];
    m_batchDue[due] = now + request.options.value("periodMs").toInt();
    sendRequest(request.type, request.fc, request.slave, request.address, request.values,
        request.label, -1, request.deviceId, {}, request.options);
    return true;
}

void ModbusManager::applyStartupWrites(const QStringList &deviceIds)
{
    if (!isOpen() || m_scanning || m_rawLoopActive) return;
    for (const auto &entry : m_devices->definitions()) {
        const auto id = entry.toMap().value("id").toString();
        if ((!deviceIds.isEmpty() && !deviceIds.contains(id)) || !m_devices->isDeviceEnabled(id)) continue;
        const auto mode = m_devices->advancedOptionsForId(id).value("writeAfterStartup", "off").toString();
        if (mode == "off") continue;
        if (m_mode == "asciiMaster") {
            emit toastRequested(QStringLiteral("ASCII 启动批量写尚未支持 / ASCII startup batch write is not supported yet"), false);
            continue;
        }
        QList<PendingRequest> plan; QString error;
        if (!prepareBatch({id}, plan, error)) { emit toastRequested(error, false); continue; }
        if (mode == "once") {
            if (m_requests.size() + plan.size() > 256) { emit toastRequested(QStringLiteral("启动写入队列已满 / Startup write queue full"), false); continue; }
            m_requests.append(plan);
        } else {
            int remaining = 0;
            for (const auto &request : m_batchPlan) if (request.deviceId != id) ++remaining;
            if (remaining + plan.size() > 256) {
                emit toastRequested(QStringLiteral("循环写计划最多 256 个报文 / At most 256 cyclic write frames"), false);
                continue;
            }
            if (!batchWriting()) m_batchClock.restart();
            removeBatchDevice(id);
            m_batchPlan.append(plan);
            for (int i = 0; i < plan.size(); ++i) m_batchDue.append(0);
            m_pollTimer.start();
            emit batchWritingChanged();
        }
    }
    dispatchNext();
}
