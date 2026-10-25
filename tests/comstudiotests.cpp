#include <QJsonDocument>
#include <QObject>
#include <QTest>

#include "core/checksumengine.h"
#include "core/hexcodec.h"
#include "core/modbuscodec.h"
#include "core/trafficmodel.h"
#include "models/modbusdevicemodel.h"
#include "models/modbuspointmodel.h"
#include <QAbstractItemModelTester>
#include <QTemporaryDir>
#include <QModbusTcpServer>
#include <QTcpServer>
#include <QSettings>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include "core/mthingspointtemplate.h"
#include "managers/modbusmanager.h"
#include "managers/modbusworkspace.h"

class RecordingModbusServer : public QModbusTcpServer
{
public:
    QList<QModbusRequest> requests;
protected:
    QModbusResponse processRequest(const QModbusPdu &request) override
    {
        requests.append(QModbusRequest(request.functionCode(), request.data()));
        return QModbusTcpServer::processRequest(request);
    }
};

static bool startBatchTestServer(RecordingModbusServer &server, quint16 &port)
{
    QTcpServer reservation;
    if (!reservation.listen(QHostAddress::LocalHost, 0)) return false;
    port = reservation.serverPort(); reservation.close();
    server.setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    server.setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    server.setServerAddress(1);
    QModbusDataUnitMap map;
    map[QModbusDataUnit::HoldingRegisters] = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 512);
    map[QModbusDataUnit::Coils] = QModbusDataUnit(QModbusDataUnit::Coils, 0, 512);
    return server.setMap(map) && server.connectDevice();
}

class CommStudioCoreTest : public QObject
{
    Q_OBJECT

private slots:
    void deviceAndPointPersistence();
    void pointDecodeAndTemplate();
    void pointToolbarWorkflows();
    void pointCellEditing();
    void pointWriteEncoding();
    void pointBatchWriteParticipation();
    void batchWriteOnceAgainstTcpSimulator();
    void batchPacking_data();
    void batchPacking();
    void cyclicBatchWriteLifecycle();
    void cyclicWriteChannelIsolation();
    void startupBatchWrite();
    void pointDeletionCleansPlans();
    void mthingsDeviceTemplate();
    void pointPollingUsesDeviceAdvancedOptions();
    void queuedRequestsAgainstTcpSimulator();
    void channelIsolation();
    void crossChannelPointCopy();
    void taskValidation();
    void deviceRemovalCascades();
    void crc16ModbusVectors();
    void crcOtherVectors();
    void hexCodecParsing();
    void escapeSequences();
    void lineEndings();
    void trafficTextAndRawBytes();
    void fragmentedAtReply();
    void trafficPreviewAndFullPayload();
    void pausedTrafficFlushesOnResume();
    void modbusRequestFrames();
    void modbusResponseParsing();
    void wordOrder32();
    void trafficModelRingBuffer();

    static QByteArray hex(const char *text) { return QByteArray::fromHex(QByteArray(text)); }
};

void CommStudioCoreTest::channelIsolation()
{
    QTemporaryDir directory;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    int firstIndex = -1;
    int secondIndex = -1;
    int savedChannelCount = 0;
    {
        ModbusWorkspace workspace;
        firstIndex = workspace.currentIndex();
        auto *first = workspace.current();
        QCOMPARE(workspace.channelBackend(firstIndex), static_cast<QObject *>(first));
        QVERIFY(workspace.channelBackend(-1) == nullptr);
        QVERIFY(workspace.channelBackend(workspace.channels().size()) == nullptr);
        first->setHost("first-device");
        first->setVisibleColumns({{QStringLiteral("value"), false}, {QStringLiteral("name"), true}});
        QCOMPARE(first->visibleColumns().value(QStringLiteral("value")).toBool(), false);
        QVERIFY(first->points()->savePoint(-1, {{"name", "First point"}, {"address", 0}}));
        workspace.add("Second channel");
        secondIndex = workspace.currentIndex();
        QVERIFY(workspace.current() != first);
        QCOMPARE(workspace.current()->points()->rowCount(), 0);
        QCOMPARE(workspace.channels().constLast().toMap().value("transport").toString(), QString("serial"));
        workspace.addChannel("TCP channel", "tcpMaster");
        QCOMPARE(workspace.current()->mode(), QString("tcpMaster"));
        QCOMPARE(workspace.channels().constLast().toMap().value("transport").toString(), QString("network"));
        workspace.remove(workspace.currentIndex());
        workspace.current()->setHost("second-device");
        workspace.select(firstIndex);
        QCOMPARE(workspace.current()->host(), QString("first-device"));
        QCOMPARE(workspace.current()->points()->rowCount(), 1);
        savedChannelCount = workspace.channels().size();
    }
    ModbusWorkspace restored;
    QCOMPARE(restored.channels().size(), savedChannelCount);
    restored.select(firstIndex);
    QCOMPARE(restored.current()->host(), QString("first-device"));
    QCOMPARE(restored.current()->visibleColumns().value(QStringLiteral("value")).toBool(), false);
    restored.select(secondIndex);
    QCOMPARE(restored.current()->host(), QString("second-device"));
    QCOMPARE(restored.current()->points()->rowCount(), 0);
}

void CommStudioCoreTest::crossChannelPointCopy()
{
    QTemporaryDir settingsDir;
    QVERIFY(settingsDir.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    ModbusWorkspace workspace;
    const int sourceChannel = workspace.currentIndex();
    auto *source = workspace.current();
    QVERIFY(source->points()->savePoint(-1, {{"name", "Source voltage"}, {"fc", 3}, {"address", 5}}));
    workspace.addChannel(QStringLiteral("Copy target"), QStringLiteral("tcpMaster"));
    const int targetChannel = workspace.currentIndex();
    auto *target = workspace.current();
    QVERIFY(target->points()->savePoint(-1, {{"name", "Old target"}, {"fc", 3}, {"address", 0}}));
    target->addTask({{"name", "Generated"}, {"fc", 3}, {"address", 0}, {"count", 1},
                     {"pointId", target->points()->pointAt(0).value("id")}});
    target->addTask({{"name", "Independent task"}, {"fc", 3}, {"address", 10}, {"count", 1}});
    QVERIFY(!workspace.copyDevicePoints(sourceChannel, 0, sourceChannel, 0));
    QVERIFY(workspace.copyDevicePoints(sourceChannel, 0, targetChannel, 0));
    QCOMPARE(source->points()->pointAt(0).value("name").toString(), QStringLiteral("Source voltage"));
    QCOMPARE(target->points()->rowCount(), 1);
    QCOMPARE(target->points()->pointAt(0).value("name").toString(), QStringLiteral("Source voltage"));
    QCOMPARE(target->points()->pointAt(0).value("address").toInt(), 5);
    QVERIFY(target->points()->pointAt(0).value("id") != source->points()->pointAt(0).value("id"));
    QCOMPARE(target->tasks().size(), 1);
    QCOMPARE(target->taskAt(0).value("name").toString(), QStringLiteral("Independent task"));
}

void CommStudioCoreTest::pointBatchWriteParticipation()
{
    ModbusPointModel model;
    model.setDeviceId("device-a");
    QVERIFY(model.savePoint(-1, {{"name", "Setpoint"}, {"address", 0}, {"poll", false}}));
    QVERIFY(model.savePoint(-1, {{"name", "Read only"}, {"address", 1}, {"fc", 4}, {"batchWrite", true}}));
    QVERIFY(!model.pointAt(0).value("batchWrite").toBool());
    QVERIFY(!model.pointAt(1).value("batchWrite").toBool());
    QVERIFY(!model.togglePointBatchWrite(1));
    QVERIFY(!model.togglePointBatchWrite(-1));
    QVERIFY(model.togglePointBatchWrite(0));
    QVERIFY(model.pointAt(0).value("batchWrite").toBool());
    QVERIFY(!model.pointAt(0).value("poll").toBool());
    QCOMPARE(model.selectedCount(), 0);
    model.selectPoint(1, false, false);
    QVERIFY(model.pointAt(0).value("batchWrite").toBool());
    ModbusPointModel restored;
    QVERIFY(restored.restore(model.definitions()));
    restored.setDeviceId("device-a");
    QVERIFY(restored.pointAt(0).value("batchWrite").toBool());
    QCOMPARE(restored.selectedCount(), 0);
    QTemporaryDir directory;
    for (const auto &extension : {"json", "csv", "dtmthings"}) {
        const auto file = QUrl::fromLocalFile(directory.filePath(QString("batch.") + extension));
        QVERIFY(model.exportFile(file));
        ModbusPointModel imported;
        imported.setDeviceId("imported");
        QVERIFY(imported.importFile(file));
        QVERIFY(imported.pointAt(0).value("batchWrite").toBool());
        QVERIFY(!imported.pointAt(1).value("batchWrite").toBool());
    }
    QVERIFY(model.togglePointBatchWrite(0));
    QVERIFY(!model.pointAt(0).value("batchWrite").toBool());
}

void CommStudioCoreTest::batchWriteOnceAgainstTcpSimulator()
{
    QTemporaryDir directory;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    QTcpServer reservation;
    QVERIFY(reservation.listen(QHostAddress::LocalHost, 0));
    const quint16 port = reservation.serverPort();
    reservation.close();
    QModbusTcpServer server;
    server.setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    server.setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    server.setServerAddress(1);
    QModbusDataUnitMap map;
    map[QModbusDataUnit::HoldingRegisters] = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 20);
    map[QModbusDataUnit::Coils] = QModbusDataUnit(QModbusDataUnit::Coils, 0, 20);
    QVERIFY(server.setMap(map));
    QVERIFY(server.connectDevice());
    ModbusManager manager(nullptr, "batch-write-test");
    auto *points = manager.points();
    QVERIFY(points->savePoint(-1, {{"name", "Holding"}, {"address", 0}, {"batchWrite", true}, {"writeValue", "42"}, {"poll", false}}));
    QVERIFY(points->savePoint(-1, {{"name", "Coil"}, {"address", 1}, {"fc", 1}, {"type", "bit"}, {"batchWrite", true}, {"writeValue", "1"}}));
    QVERIFY(points->savePoint(-1, {{"name", "Not participating"}, {"address", 2}, {"writeValue", "99"}}));
    QVERIFY(points->savePoint(-1, {{"name", "Read only"}, {"address", 3}, {"fc", 4}, {"batchWrite", true}, {"writeValue", "99"}}));
    const int second = manager.addDevice("other", 2);
    QVERIFY(points->savePoint(-1, {{"name", "Other device"}, {"address", 0}, {"batchWrite", true}, {"writeValue", "99"}}));
    manager.selectDevice(0);
    QVERIFY(!manager.writeBatchOnce()); // Offline: no queue is created.
    manager.setMode("tcpMaster"); manager.setHost("127.0.0.1"); manager.setPort(port);
    manager.openConnection();
    QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    QVERIFY(points->setWriteValue(1, "invalid"));
    QVERIFY(!manager.writeBatchOnce()); // Whole-batch preflight, first point must not leak out.
    QCOMPARE(manager.txCount(), qint64(0));
    QVERIFY(points->setWriteValue(1, "1"));
    QVERIFY(manager.setDeviceEnabled(0, false));
    QVERIFY(!manager.writeBatchOnce());
    QVERIFY(manager.setDeviceEnabled(0, true));
    auto options = manager.devices()->advancedOptions(0);
    options["packageMode"] = "line"; // Keep the cancellation assertion independent of address-sort order.
    options["addressOffset"] = -1;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    QVERIFY(!manager.writeBatchOnce());
    QCOMPARE(manager.txCount(), qint64(0));
    options["addressOffset"] = 0;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    points->setAreaMask(0); // Visibility and current row selection do not affect participation.
    QCOMPARE(points->selectedCount(), 0);
    QVERIFY(manager.writeBatchOnce());
    QVERIFY(!manager.writeBatchOnce()); // Prevent overlapping batches.
    manager.selectDevice(second); // Queued requests retain the original device context.
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(2), 3000);
    QCOMPARE(manager.errorCount(), qint64(0));
    quint16 value = 0;
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 0, &value)); QCOMPARE(value, quint16(42));
    QVERIFY(server.data(QModbusDataUnit::Coils, 1, &value)); QVERIFY(value != 0);
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 1, &value)); QCOMPARE(value, quint16(0));
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 2, &value)); QCOMPARE(value, quint16(0));
    manager.selectDevice(0);
    manager.writePoint(1, "0");
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(3), 2000);
    QVERIFY(server.data(QModbusDataUnit::Coils, 1, &value)); QCOMPARE(value, quint16(0));
    QVERIFY(points->setWriteValue(1, "1"));
    QVERIFY(manager.writeBatchOnce());
    QVERIFY(manager.setDeviceEnabled(0, false)); // Drop unsent requests; an in-flight write cannot be undone.
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(4), 2000);
    QTest::qWait(50);
    QCOMPARE(manager.txCount(), qint64(4));
    QVERIFY(server.data(QModbusDataUnit::Coils, 1, &value)); QCOMPARE(value, quint16(0));
    QVERIFY(manager.setDeviceEnabled(0, true));
    QVERIFY(points->togglePointBatchWrite(0));
    QVERIFY(points->togglePointBatchWrite(1));
    QVERIFY(!manager.writeBatchOnce());
    QCOMPARE(manager.txCount(), qint64(4));
    manager.close();
    server.disconnectDevice();
}

void CommStudioCoreTest::batchPacking_data()
{
    QTest::addColumn<QString>("mode");
    QTest::addColumn<int>("frames");
    QTest::newRow("by-line") << QString("line") << 4;
    QTest::newRow("by-address-no-holes") << QString("address") << 2;
    QTest::newRow("by-list-order") << QString("list") << 3;
}

void CommStudioCoreTest::batchPacking()
{
    QFETCH(QString, mode); QFETCH(int, frames);
    QTemporaryDir directory;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    RecordingModbusServer server; quint16 port = 0;
    QVERIFY(startBatchTestServer(server, port));
    QVERIFY(server.setData(QModbusDataUnit::HoldingRegisters, 3, 777));
    ModbusManager manager(nullptr, "batch-packing");
    auto *points = manager.points();
    for (int address : {2, 0, 1, 4})
        QVERIFY(points->savePoint(-1, {{"name", QString::number(address)}, {"address", address},
            {"writeValue", QString::number((address + 1) * 11)}, {"batchWrite", true}, {"poll", false}}));
    auto options = manager.devices()->advancedOptions(0); options["packageMode"] = mode;
    options["singleRegisterWriteCode"] = 16;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    manager.setMode("tcpMaster"); manager.setHost("127.0.0.1"); manager.setPort(port); manager.openConnection();
    QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    QVERIFY(manager.writeBatchOnce());
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(frames), 3000);
    QCOMPARE(server.requests.size(), frames);
    for (const auto &request : server.requests) QCOMPARE(int(request.functionCode()), 16);
    const auto first = server.requests.first().data();
    QCOMPARE(quint8(first[1]), quint8(mode == "address" ? 0 : 2));
    QCOMPARE(quint8(first[3]), quint8(mode == "address" ? 3 : 1));
    quint16 value = 0;
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 3, &value)); QCOMPARE(value, quint16(777));
    for (int address : {0, 1, 2, 4}) {
        QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, address, &value));
        QCOMPARE(value, quint16((address + 1) * 11));
    }
    points->toggleSelectAll(); QVERIFY(manager.deleteSelectedPoints());
    options["singleCoilWriteCode"] = 15;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    QVERIFY(points->savePoint(-1, {{"name", "Coil"}, {"fc", 1}, {"type", "bit"}, {"address", 1},
        {"batchWrite", true}, {"writeValue", "1"}}));
    QVERIFY(manager.writeBatchOnce());
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(frames + 1), 2000);
    QCOMPARE(int(server.requests.last().functionCode()), 15); // Actual received PDU, not reconstructed UI trace.
    QVERIFY(server.data(QModbusDataUnit::Coils, 1, &value)); QVERIFY(value != 0);
    manager.close();
}

void CommStudioCoreTest::cyclicBatchWriteLifecycle()
{
    QTemporaryDir directory;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    RecordingModbusServer server; quint16 port = 0;
    QVERIFY(startBatchTestServer(server, port));
    ModbusManager manager(nullptr, "cycle-lifecycle");
    const auto id = manager.devices()->deviceId(0);
    auto *points = manager.points();
    QVERIFY(points->savePoint(-1, {{"name", "Setpoint"}, {"address", 0}, {"batchWrite", true}, {"writeValue", "10"}, {"periodMs", 30}}));
    const int other = manager.addDevice("Other", 2);
    const auto otherId = manager.devices()->deviceId(other);
    QVERIFY(points->savePoint(-1, {{"name", "Invalid command"}, {"address", 0}, {"batchWrite", true}, {"writeValue", "bad"}}));
    manager.selectDevice(0);
    QVERIFY(!manager.startBatchWriting({id}));
    manager.setMode("tcpMaster"); manager.setHost("127.0.0.1"); manager.setPort(port); manager.openConnection();
    QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    QVERIFY(!manager.startBatchWriting({id, otherId}));
    QVERIFY(!manager.startBatchWriting({}));
    QVERIFY(!manager.batchWriting()); QCOMPARE(manager.txCount(), qint64(0));
    QVERIFY(manager.startBatchWriting({id, id})); // Repeated IDs are deduplicated.
    QTRY_VERIFY_WITH_TIMEOUT(manager.rxCount() >= 3, 2000);
    manager.selectDevice(other); // Current UI context is not a write target.
    QTest::qWait(80);
    QCOMPARE(manager.errorCount(), qint64(0));
    manager.selectDevice(0);
    QVERIFY(points->setWriteValue(0, "77"));
    auto options = manager.devices()->advancedOptions(0); options["addressOffset"] = 2;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    QTest::qWait(120);
    quint16 value = 0;
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 0, &value)); QCOMPARE(value, quint16(10));
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 2, &value)); QCOMPARE(value, quint16(0));
    manager.stopBatchWriting();
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), manager.txCount(), 1000);
    const auto stopped = manager.txCount(); QTest::qWait(120); QCOMPARE(manager.txCount(), stopped);
    QVERIFY(manager.startBatchWriting({id}));
    QTRY_VERIFY_WITH_TIMEOUT(manager.rxCount() > stopped + 1, 2000);
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 2, &value)); QCOMPARE(value, quint16(77));
    manager.startScan(1, 1, 3, 0, 50); QVERIFY(!manager.scanning());
    manager.startRawLoop("01 03 00 00 00 01", true, true, 2, 10); QVERIFY(!manager.rawLoopActive());
    QVERIFY(manager.setDeviceEnabled(0, false)); QVERIFY(!manager.batchWriting());
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), manager.txCount(), 1000);
    QVERIFY(manager.setDeviceEnabled(0, true)); QVERIFY(!manager.batchWriting()); // Default startup is off.
    options["addressOffset"] = 0; QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    manager.startPollingForDevices({id}); QVERIFY(manager.polling());
    QVERIFY(manager.startBatchWriting({id}));
    server.requests.clear(); QTest::qWait(240);
    bool read = false, write = false;
    for (const auto &request : server.requests) { read |= request.functionCode() == 3; write |= request.functionCode() == 6; }
    QVERIFY(read); QVERIFY(write); // Neither scheduler starves the other.
    manager.stopBatchWriting(); QVERIFY(manager.polling());
    QVERIFY(manager.startBatchWriting({id}));
    manager.stopPolling(); QVERIFY(manager.batchWriting());
    manager.close(); QVERIFY(!manager.batchWriting()); QVERIFY(!manager.polling());
    const auto closed = manager.txCount(); QTest::qWait(100); QCOMPARE(manager.txCount(), closed);
    manager.openConnection(); QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    QTest::qWait(120); QVERIFY(!manager.batchWriting()); QCOMPARE(manager.txCount(), closed);
    QVERIFY(manager.startBatchWriting({id}));
    server.disconnectDevice(); // Unexpected peer loss must also stop cycling.
    QTRY_VERIFY_WITH_TIMEOUT(!manager.isOpen() && !manager.batchWriting(), 2000);
    QVERIFY(server.connectDevice());
    manager.openConnection(); QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    QVERIFY(!manager.batchWriting());
    QVERIFY(manager.startBatchWriting({id}));
    manager.removeDevice(0); QVERIFY(!manager.batchWriting());
    manager.close();
}

void CommStudioCoreTest::cyclicWriteChannelIsolation()
{
    QTemporaryDir directory;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    RecordingModbusServer firstServer, secondServer; quint16 firstPort = 0, secondPort = 0;
    QVERIFY(startBatchTestServer(firstServer, firstPort));
    QVERIFY(startBatchTestServer(secondServer, secondPort));
    ModbusManager first(nullptr, "cycle-first"), second(nullptr, "cycle-second");
    for (auto *manager : {&first, &second}) {
        QVERIFY(manager->points()->savePoint(-1, {{"name", "Channel target"}, {"address", 0},
            {"batchWrite", true}, {"writeValue", manager == &first ? "11" : "22"}, {"periodMs", 30}}));
        manager->setMode("tcpMaster"); manager->setHost("127.0.0.1");
        manager->setPort(manager == &first ? firstPort : secondPort); manager->openConnection();
        QTRY_VERIFY_WITH_TIMEOUT(manager->isOpen(), 2000);
        QVERIFY(manager->startBatchWriting({manager->devices()->deviceId(0)}));
    }
    QTRY_VERIFY_WITH_TIMEOUT(first.rxCount() >= 2 && second.rxCount() >= 2, 2000);
    quint16 value = 0;
    QVERIFY(firstServer.data(QModbusDataUnit::HoldingRegisters, 0, &value)); QCOMPARE(value, quint16(11));
    QVERIFY(secondServer.data(QModbusDataUnit::HoldingRegisters, 0, &value)); QCOMPARE(value, quint16(22));
    first.stopBatchWriting();
    QTRY_COMPARE_WITH_TIMEOUT(first.rxCount(), first.txCount(), 1000);
    const auto firstStopped = first.txCount(), secondBefore = second.rxCount();
    QTRY_VERIFY_WITH_TIMEOUT(second.rxCount() > secondBefore + 1, 2000);
    QCOMPARE(first.txCount(), firstStopped); QVERIFY(second.batchWriting());
    first.close(); second.close();
}

void CommStudioCoreTest::startupBatchWrite()
{
    QTemporaryDir directory;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    RecordingModbusServer server; quint16 port = 0;
    QVERIFY(startBatchTestServer(server, port));
    ModbusManager manager(nullptr, "startup-writes");
    QVERIFY(manager.points()->savePoint(-1, {{"name", "Startup"}, {"address", 0}, {"batchWrite", true}, {"writeValue", "32"}, {"periodMs", 30}}));
    auto options = manager.devices()->advancedOptions(0);
    QCOMPARE(options.value("writeAfterStartup").toString(), QString("off"));
    options["writeAfterStartup"] = "once"; QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    ModbusDeviceModel restored; QVERIFY(restored.restore(manager.devices()->definitions()));
    QCOMPARE(restored.advancedOptions(0).value("writeAfterStartup").toString(), QString("once"));
    manager.setMode("tcpMaster"); manager.setHost("127.0.0.1"); manager.setPort(port); manager.openConnection();
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(1), 2000);
    QTest::qWait(150); QCOMPARE(manager.txCount(), qint64(1)); QVERIFY(!manager.batchWriting());
    QVERIFY(manager.setDeviceEnabled(0, true)); QTest::qWait(80); QCOMPARE(manager.txCount(), qint64(1));
    QVERIFY(manager.setDeviceEnabled(0, false)); QVERIFY(manager.setDeviceEnabled(0, true));
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(2), 2000);
    manager.close(); options["writeAfterStartup"] = "cycle"; options["pollAfterStartup"] = true;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    manager.openConnection(); QTRY_VERIFY_WITH_TIMEOUT(manager.batchWriting() && manager.polling(), 2000);
    QTRY_VERIFY_WITH_TIMEOUT(manager.rxCount() >= 6, 2000);
    manager.close(); QVERIFY(!manager.batchWriting()); QVERIFY(!manager.polling());
    options["writeAfterStartup"] = "off"; options["pollAfterStartup"] = false;
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    manager.openConnection(); QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    const auto off = manager.txCount(); QTest::qWait(120); QCOMPARE(manager.txCount(), off);
    manager.close();
}

void CommStudioCoreTest::pointWriteEncoding()
{
    QVector<quint16> words; QString error;
    for (const QString &order : {QString("ABCD"), QString("BADC"), QString("CDAB"), QString("DCBA")}) {
        const QVariantMap point{{"name", "Setpoint"}, {"deviceId", "device"}, {"address", 0},
                                {"fc", 3}, {"type", "float32"}, {"count", 2},
                                {"wordOrder", order}, {"scale", 2}, {"offset", 10}};
        QVERIFY2(ModbusCodec::encodePointValue(point, "110.5", words, error), qPrintable(error));
        ModbusPointModel model; model.setDeviceId("device");
        QVERIFY(model.savePoint(-1, point));
        model.receive("device", 3, 0, words);
        QCOMPARE(model.pointAt(0).value("value").toDouble(), 110.5);
    }
    QVERIFY(ModbusCodec::encodePointValue({{"type", "int16"}}, "-123", words, error));
    QCOMPARE(words, QVector<quint16>({quint16(-123)}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "uint16"}, {"scale", 0.1}}, "12.3", words, error));
    QCOMPARE(words, QVector<quint16>({123}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "uint64"}, {"count", 4}}, "18446744073709551615", words, error));
    QCOMPARE(words, QVector<quint16>({65535, 65535, 65535, 65535}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "int64"}, {"count", 4}}, "-9223372036854775808", words, error));
    QCOMPARE(words, QVector<quint16>({0x8000, 0, 0, 0}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "bcd"}}, "1234", words, error));
    QCOMPARE(words, QVector<quint16>({0x1234}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "float64"}, {"count", 4}}, "50", words, error));
    QCOMPARE(words, QVector<quint16>({0x4049, 0, 0, 0}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "string"}, {"count", 2}}, "AB", words, error));
    QCOMPARE(words, QVector<quint16>({0x4142, 0}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "bytes"}}, "12 34", words, error));
    QCOMPARE(words, QVector<quint16>({0x1234}));
    QVERIFY(ModbusCodec::encodePointValue({{"type", "bit"}, {"fc", 1}}, "1", words, error));
    QCOMPARE(words, QVector<quint16>({1}));
    for (const QString &invalid : {QString("65536"), QString("-1"), QString("12.5"), QString("NaN"), QString("1 2")}) {
        QVERIFY(!ModbusCodec::encodePointValue({}, invalid, words, error));
        QVERIFY(words.isEmpty()); QVERIFY(!error.isEmpty());
    }
    QVERIFY(!ModbusCodec::encodePointValue({{"type", "bit"}, {"fc", 1}}, "2", words, error));
    QVERIFY(!ModbusCodec::encodePointValue({{"type", "int16"}}, "32768", words, error));
    QVERIFY(!ModbusCodec::encodePointValue({{"scale", 0}}, "1", words, error));
    QVERIFY(!ModbusCodec::encodePointValue({{"bitCount", 1}}, "1", words, error));
    QVERIFY(!ModbusCodec::encodePointValue({{"fc", 4}}, "1", words, error));
    QVERIFY(!ModbusCodec::encodePointValue({{"type", "float32"}, {"count", 2}}, "1e100", words, error));
}

void CommStudioCoreTest::pointDeletionCleansPlans()
{
    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    {
        ModbusManager manager(nullptr, "point-delete");
        QVERIFY(manager.points()->addPoints(2, 3, 0));
        manager.createPointTasks(0, 0, 125);
        QCOMPARE(manager.tasks().size(), 1);
        QCOMPARE(manager.taskAt(0).value("count").toInt(), 2);
        manager.addTask({{"name", "Independent"}, {"address", 10}});
        manager.addDevice("Other", 2);
        QVERIFY(manager.points()->addPoints(1, 3, 5));
        manager.createPointTasks(0, 0, 125);
        const QVariantMap otherTask = manager.taskAt(2);
        manager.selectDevice(0);
        // Delete the second member of a merged plan, not its leading pointId.
        manager.points()->selectPoint(1, false, false);
        QVERIFY(manager.deleteSelectedPoints());
        QCOMPARE(manager.points()->rowCount(), 1);
        QCOMPARE(manager.points()->pointAt(0).value("address").toInt(), 0);
        QCOMPARE(manager.tasks().size(), 2);
        QCOMPARE(manager.taskAt(0).value("name").toString(), QString("Independent"));
        QCOMPARE(manager.taskAt(1), otherTask);
        QVERIFY(!manager.deleteSelectedPoints());
    }
    ModbusManager restored(nullptr, "point-delete");
    QCOMPARE(restored.points()->definitions().size(), 2);
    QCOMPARE(restored.tasks().size(), 2);
}

void CommStudioCoreTest::pointCellEditing()
{
    ModbusPointModel model;
    model.setDeviceId("cells");
    QVERIFY(model.addPoints(4, 3, 0));
    model.selectPoint(0, false, false);
    model.selectPoint(2, true, false);
    QVERIFY(model.updateSelectedField("unit", "V"));
    QCOMPARE(model.pointAt(0).value("unit").toString(), QString("V"));
    QVERIFY(model.pointAt(1).value("unit").toString().isEmpty());
    QVERIFY(model.updateSelectedField("scale", "0.01"));
    QVERIFY(model.updateSelectedField("range", "-10,250"));
    QCOMPARE(model.pointAt(2).value("minValue").toDouble(), -10.0);
    const auto snapshot = model.definitions();
    QVERIFY(!model.updateSelectedField("address", "abc"));
    QVERIFY(!model.updateSelectedField("decimals", "99"));
    QVERIFY(!model.updateSelectedField("range", "250,10"));
    QCOMPARE(model.definitions(), snapshot);

    // Excel text fills existing rows, without a CSV header or new point IDs.
    const auto firstId = model.pointAt(0).value("id").toString();
    const auto thirdId = model.pointAt(2).value("id").toString();
    QVERIFY(model.setVisiblePointIds({firstId, thirdId, model.pointAt(3).value("id").toString()}));
    QVERIFY(model.pasteCells(0, {"name"}, "Voltage\r\nCurrent\r\n"));
    QCOMPARE(model.pointAt(0).value("name").toString(), QString("Voltage"));
    QCOMPARE(model.pointAt(2).value("name").toString(), QString("Current"));
    QCOMPARE(model.pointAt(1).value("name"), snapshot[1].toMap().value("name"));
    QCOMPARE(model.pointAt(0).value("id").toString(), firstId);
    QCOMPARE(model.rowCount(), 4);
    QVERIFY(model.pasteCells(0, {"address", "count"}, "100\t2\n110\t3"));
    QCOMPARE(model.pointAt(2).value("address").toInt(), 110);
    QCOMPARE(model.pointAt(2).value("count").toInt(), 3);
    const auto beforeInvalidPaste = model.definitions();
    QVERIFY(!model.pasteCells(0, {"address", "count"}, "200\t2\ninvalid\t3"));
    QVERIFY(!model.pasteCells(0, {"name", "value"}, "Changed\t1"));
    QVERIFY(!model.pasteCells(3, {"name"}, "Too\nmany"));
    QCOMPARE(model.definitions(), beforeInvalidPaste);
    QVERIFY(model.pasteCells(0, {"name"}, "\"A\tB\"\n\"Quoted \"\"name\"\"\"\n"));
    QCOMPARE(model.pointAt(0).value("name").toString(), QString("A\tB"));
    QCOMPARE(model.pointAt(2).value("name").toString(), QString("Quoted \"name\""));
}

void CommStudioCoreTest::mthingsDeviceTemplate()
{
    ModbusPointModel model;
    model.setDeviceId("local-device");
    const auto fixture = QUrl::fromLocalFile(QFINDTESTDATA("data/modbus-device.dtmthings"));
    QVERIFY(model.importFile(fixture));
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.pointAt(0).value("name").toString(), QStringLiteral("电压"));
    QCOMPARE(model.pointAt(0).value("fc").toInt(), 4);
    QCOMPARE(model.pointAt(0).value("scale").toDouble(), 0.1);
    QCOMPARE(model.pointAt(0).value("minValue").toDouble(), -10.0);
    QCOMPARE(model.pointAt(0).value("group").toString(), QStringLiteral("测量"));
    QCOMPARE(model.pointAt(1).value("type").toString(), QString("float32"));
    QCOMPARE(model.pointAt(1).value("wordOrder").toString(), QString("DCBA"));
    QVERIFY(!model.pointAt(1).value("poll").toBool());
    QCOMPARE(model.pointAt(2).value("enumMap").toMap().value("1").toString(), QStringLiteral("运行"));
    QVERIFY(model.pointAt(0).value("deviceId").toString() == "local-device");

    QTemporaryDir directory;
    const QVariantMap metadata{{"manufacturer", "Test factory"}, {"deviceName", "Meter"}, {"model", "Demo"}, {"version", "1.0"}};
    const auto native = QUrl::fromLocalFile(directory.filePath("meter.dtmthings"));
    QVERIFY(model.exportFile(native, metadata));
    ModbusPointModel restored;
    restored.setDeviceId("target");
    QVERIFY(restored.importFile(native));
    QCOMPARE(restored.rowCount(), 3);
    for (int i = 0; i < 3; ++i) {
        auto expected = model.pointAt(i), actual = restored.pointAt(i);
        for (const auto &key : {"id", "deviceId", "selected", "sourceRow"}) { expected.remove(key); actual.remove(key); }
        QCOMPARE(actual, expected);
    }
    const auto json = QUrl::fromLocalFile(directory.filePath("meter.json"));
    QVERIFY(model.exportFile(json, metadata));
    QFile file(json.toLocalFile()); QVERIFY(file.open(QIODevice::ReadOnly));
    const auto object = QJsonDocument::fromJson(file.readAll()).object();
    QCOMPARE(object.value("profile").toObject().value("manufacturer").toString(), QString("Test factory"));
    QVERIFY(!object.value("points").toArray().first().toObject().contains("deviceId"));
    QVERIFY(!object.value("points").toArray().first().toObject().contains("id"));

    QVariantList invalid; QString error;
    QVERIFY(!MThingsPointTemplate::decode("<MThings><DEVICE><DATA_LIST><DATA DLT645DI=\"1\" /></DATA_LIST></DEVICE></MThings>", invalid, error));
    QVERIFY(!error.isEmpty());
    QVERIFY(!MThingsPointTemplate::decode("<MThings><DEVICE_LIST /></MThings>", invalid, error));
    QVERIFY(!MThingsPointTemplate::decode("<MThings><DEVICE /></MThings><bad", invalid, error));
}

void CommStudioCoreTest::taskValidation()
{
    QTemporaryDir settingsDir;
    QVERIFY(settingsDir.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    ModbusManager manager(nullptr, QStringLiteral("task-validation"));
    manager.addTask({{"name", "bad-fc"}, {"fc", 9}, {"address", 0}, {"count", 1}});
    QCOMPARE(manager.tasks().size(), 0);
    manager.addTask({{"name", "bad-range"}, {"fc", 3}, {"slave", 1}, {"address", 65535}, {"count", 2}});
    QCOMPARE(manager.tasks().size(), 0);
    manager.addTask({{"name", "fast"}, {"fc", 3}, {"slave", 2}, {"address", 10}, {"count", 2},
                     {"periodMs", 20}, {"priority", 2}, {"timeoutMs", 250}, {"retries", 1}});
    QCOMPARE(manager.tasks().size(), 1);
    const auto task = manager.taskAt(0);
    QCOMPARE(task.value("priority").toInt(), 2);
    QCOMPARE(task.value("timeoutMs").toInt(), 250);
    QCOMPARE(task.value("retries").toInt(), 1);
    QVERIFY(manager.updateDevice(0, QStringLiteral("Renamed device"), 7));
    QCOMPARE(manager.unitId(), 7);
    QCOMPARE(manager.taskAt(0).value("slave").toInt(), 7);
    QCOMPARE(manager.devices()->definitions().first().toMap().value("name").toString(),
             QStringLiteral("Renamed device"));
    QVERIFY(!manager.updateDevice(0, QString(), 8));
    QCOMPARE(manager.unitId(), 7);
    QCOMPARE(manager.addDevice(QStringLiteral("Duplicate address"), 7), -1);
    QCOMPARE(manager.devices()->rowCount(), 1);
    const int second = manager.addDevice(QStringLiteral("Another address"), 8);
    QVERIFY(second >= 0);
    QVERIFY(!manager.updateDevice(second, QStringLiteral("Conflict"), 7));
    QCOMPARE(manager.devices()->unitId(second), 8);
}

void CommStudioCoreTest::deviceRemovalCascades()
{
    QTemporaryDir settingsDir;
    QVERIFY(settingsDir.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    ModbusManager manager(nullptr, QStringLiteral("device-removal-cascade"));
    QCOMPARE(manager.devices()->rowCount(), 1);
    QVERIFY(manager.points()->savePoint(-1, {{"name", "Voltage"}, {"fc", 3}, {"address", 0}}));
    manager.addTask({{"name", "poll-voltage"}, {"fc", 3}, {"slave", 1}, {"address", 0}, {"count", 1}});
    QCOMPARE(manager.points()->definitions().size(), 1);
    QCOMPARE(manager.tasks().size(), 1);

    manager.removeDevice(0);

    QCOMPARE(manager.devices()->rowCount(), 0);
    QCOMPARE(manager.points()->definitions().size(), 0);
    QCOMPARE(manager.tasks().size(), 0);
    QCOMPARE(manager.selectedDeviceIndex(), -1);

    ModbusManager restored(nullptr, QStringLiteral("device-removal-cascade"));
    QCOMPARE(restored.devices()->rowCount(), 0);
    QCOMPARE(restored.points()->definitions().size(), 0);
    QCOMPARE(restored.tasks().size(), 0);
    QCOMPARE(restored.selectedDeviceIndex(), -1);
}

void CommStudioCoreTest::queuedRequestsAgainstTcpSimulator()
{
    QTemporaryDir settingsDir;
    QVERIFY(settingsDir.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    QTcpServer reservation;
    QVERIFY(reservation.listen(QHostAddress::LocalHost, 0));
    const quint16 port = reservation.serverPort();
    reservation.close();
    QModbusTcpServer server;
    server.setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    server.setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    server.setServerAddress(1);
    QModbusDataUnitMap map;
    map[QModbusDataUnit::HoldingRegisters] = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 20);
    QVERIFY(server.setMap(map));
    QVERIFY(server.connectDevice());
    ModbusManager manager;
    manager.setMode("tcpMaster"); manager.setHost("127.0.0.1"); manager.setPort(port);
    manager.openConnection();
    QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    QVERIFY(manager.points()->savePoint(-1, {{"name", "Counter"}, {"address", 0}}));
    manager.readPoint(0);
    manager.manualRead(1, 3, 0, 1);
    manager.manualWrite(1, 6, 0, "123");
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), qint64(3), 3000);
    QCOMPARE(manager.txCount(), qint64(3));
    QCOMPARE(manager.errorCount(), qint64(0));
    QCOMPARE(manager.points()->pointAt(0).value("value").toDouble(), 123.0);
    QCOMPARE(manager.statistics().value("success").toDouble(), 100.0);
    QVERIFY(manager.statistics().value("txBytes").toInt() > 0);
    manager.startScan(1, 2, 3, 0, 50);
    QTRY_VERIFY_WITH_TIMEOUT(!manager.scanning(), 2000);
    QCOMPARE(manager.scanResults().size(), 2);
    QVERIFY(manager.scanResults().at(0).toMap().value("online").toBool());
    manager.startRawLoop("01 03 00 00 00 01", true, true, 3, 10);
    QTRY_VERIFY_WITH_TIMEOUT(!manager.rawLoopActive(), 3000);
    QCOMPARE(manager.rawLoopSent(), 3);
    QVERIFY(manager.rawResult().value("ok").toBool());
    QCOMPARE(manager.rxCount(), qint64(6));
    manager.saveRawTemplate("Read counter", "01 03 00 00 00 01", true, true);
    QCOMPARE(manager.rawTemplates().size(), 1);
    QVERIFY(manager.points()->savePoint(-1, {{"name", "Float setpoint"}, {"address", 2},
                                            {"type", "float32"}, {"wordOrder", "CDAB"},
                                            {"scale", 2}, {"offset", 10}}));
    const qint64 beforeTypedWrite = manager.rxCount();
    manager.writePoint(1, "110.5");
    QTRY_COMPARE_WITH_TIMEOUT(manager.rxCount(), beforeTypedWrite + 1, 2000);
    quint16 low = 0, high = 0;
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 2, &low));
    QVERIFY(server.data(QModbusDataUnit::HoldingRegisters, 3, &high));
    QCOMPARE(low, quint16(0)); QCOMPARE(high, quint16(0x4249));
    const qint64 beforeInvalidWrite = manager.txCount();
    manager.writePoint(1, "not-a-number");
    QCOMPARE(manager.txCount(), beforeInvalidWrite);
    QCOMPARE(manager.points()->pointAt(1).value("writeValue").toString(), QString("110.5"));
    QVERIFY(manager.setDeviceEnabled(0, false));
    manager.writePoint(1, "22");
    QCOMPARE(manager.txCount(), beforeInvalidWrite);
    QVERIFY(manager.setDeviceEnabled(0, true));
    manager.manualRead(1, 3, 0, 1);
    manager.manualRead(1, 3, 0, 1);
    manager.close();
    QVERIFY(!manager.isOpen());
    QTest::qWait(100);
    server.disconnectDevice();
}

void CommStudioCoreTest::pointDecodeAndTemplate()
{
    ModbusPointModel model;
    model.setDeviceId("device");
    QVERIFY(model.savePoint(-1, {{"name", "Float"}, {"address", 0}, {"type", "float32"}, {"minValue", 0}, {"maxValue", 100}}));
    model.receive("device", 3, 0, {0x4248, 0});
    QCOMPARE(model.pointAt(0).value("value").toDouble(), 50.0);
    QCOMPARE(model.pointAt(0).value("status").toString(), QString("OK"));
    QCOMPARE(model.trendSeries().first().toMap().value("avg").toDouble(), 50.0);
    model.receive("device", 3, 0, {0x43C8, 0});
    QCOMPARE(model.pointAt(0).value("status").toString(), QString("OUT OF RANGE"));
    QVERIFY(model.savePoint(-1, {{"name", "Double"}, {"address", 2}, {"type", "float64"}}));
    model.receive("device", 3, 2, {0x4049, 0, 0, 0});
    QCOMPARE(model.pointAt(1).value("value").toDouble(), 50.0);
    QVERIFY(model.savePoint(-1, {{"name", "Counter"}, {"address", 6}, {"type", "uint64"}}));
    model.receive("device", 3, 6, {65535, 65535, 65535, 65535});
    QCOMPARE(model.pointAt(2).value("value").toString(), QString("18446744073709551615"));
    QVERIFY(model.savePoint(-1, {{"name", "BCD"}, {"address", 10}, {"type", "bcd"}}));
    model.receive("device", 3, 10, {0x1234});
    QCOMPARE(model.pointAt(3).value("value").toDouble(), 1234.0);
    model.receive("device", 3, 10, {0xabcd});
    QCOMPARE(model.pointAt(3).value("status").toString(), QString("DECODE ERROR"));
    QCOMPARE(model.pollingPlans(0).size(), 1);
    QCOMPARE(model.pollingPlans(0).first().toMap().value("count").toInt(), 11);
    QCOMPARE(model.pollingPlans(1).size(), 4);
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QUrl file = QUrl::fromLocalFile(directory.filePath("points.json"));
    QVERIFY(model.exportFile(file));
    ModbusPointModel imported;
    imported.setDeviceId("another-device");
    QVERIFY(imported.importFile(file));
    QCOMPARE(imported.rowCount(), 4);
    QVERIFY(imported.pointAt(0).value("id") != model.pointAt(0).value("id"));
    QVERIFY(!imported.pointAt(0).contains("value"));
    const QUrl csv = QUrl::fromLocalFile(directory.filePath("points.csv"));
    QVERIFY(model.exportFile(csv));
    QVERIFY(model.exportHistory(QUrl::fromLocalFile(directory.filePath("history.csv"))));
    QVERIFY(imported.importFile(csv));
    QCOMPARE(imported.rowCount(), 8);
    imported.setSearch("Double");
    QCOMPARE(imported.filtered()->rowCount(), 2);
    imported.setArea(1);
    QCOMPARE(imported.filtered()->rowCount(), 0);
    imported.setArea(0); imported.setSearch("");
    imported.selectPoint(0, false, false);
    imported.selectPoint(1, true, false);
    QVERIFY(imported.updateSelected({{"periodMs", 250}, {"group", "Fast"}}));
    QCOMPARE(imported.pointAt(0).value("periodMs").toInt(), 250);
    QCOMPARE(imported.pointAt(2).value("periodMs").toInt(), 1000);
    QVERIFY(imported.setWriteValue(0, "123"));
    QCOMPARE(imported.pointAt(0).value("writeValue").toString(), QString("123"));
    imported.copySelectedToDevice("third-device");
    imported.setDeviceId("third-device");
    QCOMPARE(imported.rowCount(), 2);
    QCOMPARE(ModbusCodec::decodeRegisters({0x4248, 0}, 0, "float32", "ABCD", 1, 0).value("value").toDouble(), 50.0);
}

void CommStudioCoreTest::pointToolbarWorkflows()
{
    ModbusPointModel model;
    model.setDeviceId(QStringLiteral("source"));
    QCOMPARE(model.suggestedAddress(3), 0);
    QVERIFY(model.addPoints(3, 3, 20));
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.pointAt(0).value("fc").toInt(), 3);
    QCOMPARE(model.pointAt(2).value("address").toInt(), 22);
    QCOMPARE(model.suggestedAddress(3), 23);
    QCOMPARE(model.findNext(QStringLiteral("保持寄存器")), 0);
    QCOMPARE(model.findNext(QStringLiteral("保持寄存器"), 0), 1);
    QCOMPARE(model.findNext(QStringLiteral("保持寄存器"), 2), 0);
    QCOMPARE(model.findNext(QStringLiteral("holding")), -1); // 手册规定名称搜索区分大小写。

    const auto parsed = model.parsePdu(20, QStringLiteral("03 06 00 64 00 C8 01 2C"));
    QVERIFY(parsed.value("ok").toBool());
    const auto values = parsed.value("results").toList();
    QCOMPARE(values.size(), 3);
    QCOMPARE(values.at(0).toMap().value("value").toDouble(), 100.0);
    QCOMPARE(values.at(2).toMap().value("value").toDouble(), 300.0);

    model.setDeviceId(QStringLiteral("target"));
    QVERIFY(model.savePoint(-1, {{"name", "Old target point"}, {"fc", 1}, {"address", 1}, {"type", "bit"}}));
    QCOMPARE(model.rowCount(), 1);
    model.setDeviceId(QStringLiteral("source"));
    QVERIFY(model.copyCurrentDeviceToDevice(QStringLiteral("target")));
    model.setDeviceId(QStringLiteral("target"));
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.pointAt(0).value("address").toInt(), 20);

    model.setAreaMask(1 << 2);
    QCOMPARE(model.filtered()->rowCount(), 3);
    model.setAreaMask(1 << 0);
    QCOMPARE(model.filtered()->rowCount(), 0);
    model.setAreaMask(0x0F);
    model.setRowFilterMode(2);
    QCOMPARE(model.filtered()->rowCount(), 3);

    ModbusPointModel sortModel;
    sortModel.setDeviceId(QStringLiteral("sort-device"));
    QVERIFY(sortModel.savePoint(-1, {{"name", "wide"}, {"fc", 3}, {"address", 8},
                                    {"bitOffset", 4}, {"bitCount", 4}}));
    QVERIFY(sortModel.savePoint(-1, {{"name", "low"}, {"fc", 3}, {"address", 8},
                                    {"bitOffset", 1}, {"bitCount", 2}}));
    QVERIFY(sortModel.savePoint(-1, {{"name", "narrow"}, {"fc", 3}, {"address", 8},
                                    {"bitOffset", 4}, {"bitCount", 1}}));
    sortModel.setDeviceId(QStringLiteral("other-sort-device"));
    QVERIFY(sortModel.savePoint(-1, {{"name", "other-high"}, {"fc", 3}, {"address", 9}}));
    QVERIFY(sortModel.savePoint(-1, {{"name", "other-low"}, {"fc", 3}, {"address", 1}}));
    const auto otherBeforeSort = sortModel.definitions().mid(3);
    sortModel.setDeviceId(QStringLiteral("sort-device"));
    sortModel.sortByAddress();
    QCOMPARE(sortModel.definitions().mid(3), otherBeforeSort);
    QCOMPARE(sortModel.pointAt(0).value("name").toString(), QStringLiteral("low"));
    QCOMPARE(sortModel.pointAt(1).value("name").toString(), QStringLiteral("narrow"));
    QCOMPARE(sortModel.pointAt(2).value("name").toString(), QStringLiteral("wide"));
    const QString firstVisibleId = sortModel.pointAt(0).value("id").toString();
    const QString lastVisibleId = sortModel.pointAt(2).value("id").toString();
    QVERIFY(sortModel.setVisiblePointIds({firstVisibleId, lastVisibleId}));
    QCOMPARE(sortModel.rowCount(), 3);
    QCOMPARE(sortModel.filtered()->rowCount(), 2);
    QVERIFY(!sortModel.pointAt(1).value("rowVisible").toBool());
    ModbusPointModel restoredVisibility;
    QVERIFY(restoredVisibility.restore(sortModel.definitions()));
    restoredVisibility.setDeviceId(QStringLiteral("sort-device"));
    QCOMPARE(restoredVisibility.filtered()->rowCount(), 2);
    QVERIFY(sortModel.setVisiblePointIds({firstVisibleId, sortModel.pointAt(1).value("id").toString(), lastVisibleId}));
    QVERIFY(sortModel.togglePointPolling(1));
    QVERIFY(!sortModel.pointAt(1).value("poll").toBool());
    QCOMPARE(sortModel.pollingPlans(1).size(), 2);
    QVERIFY(sortModel.togglePointPolling(1));
    QCOMPARE(sortModel.pollingPlans(1).size(), 3);
}

void CommStudioCoreTest::pointPollingUsesDeviceAdvancedOptions()
{
    QTemporaryDir settingsDir;
    QVERIFY(settingsDir.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    QTcpServer reservation;
    QVERIFY(reservation.listen(QHostAddress::LocalHost, 0));
    const quint16 port = reservation.serverPort();
    reservation.close();
    QModbusTcpServer server;
    server.setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    server.setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    server.setServerAddress(1);
    QModbusDataUnitMap map;
    map[QModbusDataUnit::HoldingRegisters] = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 20);
    QVERIFY(server.setMap(map));
    QVERIFY(server.connectDevice());

    ModbusManager manager(nullptr, QStringLiteral("point-poll-options"));
    const QString firstId = manager.devices()->deviceId(0);
    QVERIFY(manager.points()->savePoint(-1, {{"name", "first"}, {"fc", 3}, {"address", 0}, {"periodMs", 0}}));
    QVERIFY(manager.points()->savePoint(-1, {{"name", "second"}, {"fc", 3}, {"address", 3}, {"periodMs", 0}}));
    QVariantMap options = manager.devices()->advancedOptions(0);
    options.insert("packageMode", "address");
    options.insert("maxRegisterGap", 2);
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));

    const int secondIndex = manager.addDevice(QStringLiteral("Another device"), 2);
    const QString secondId = manager.devices()->deviceId(secondIndex);
    QVERIFY(manager.points()->savePoint(-1, {{"name", "coil-a"}, {"fc", 1}, {"address", 1}, {"type", "bit"}}));
    QVERIFY(manager.points()->savePoint(-1, {{"name", "coil-b"}, {"fc", 1}, {"address", 3}, {"type", "bit"}}));
    QCOMPARE(manager.selectedDeviceIndex(), secondIndex);

    manager.setMode("tcpMaster");
    manager.setHost("127.0.0.1");
    manager.setPort(port);
    manager.openConnection();
    QTRY_VERIFY_WITH_TIMEOUT(manager.isOpen(), 2000);
    manager.startPolling();
    QVERIFY(manager.polling());
    QCOMPARE(manager.tasks().size(), 3);
    QCOMPARE(manager.taskAt(0).value("deviceId").toString(), firstId);
    QCOMPARE(manager.taskAt(0).value("count").toInt(), 4);
    QCOMPARE(manager.taskAt(0).value("periodMs").toInt(), 0);
    QCOMPARE(manager.taskAt(1).value("deviceId").toString(), secondId);
    QCOMPARE(manager.taskAt(1).value("slave").toInt(), 2);
    QCOMPARE(manager.selectedDeviceIndex(), secondIndex);
    QVERIFY(manager.setDeviceEnabled(0, false));
    QVERIFY(manager.polling()); // The other device keeps polling.
    QVERIFY(manager.setDeviceEnabled(secondIndex, false));
    QVERIFY(!manager.polling()); // No enabled device remains in the active plan.
    QVERIFY(manager.setDeviceEnabled(secondIndex, true));

    QVERIFY(!manager.devices()->isDeviceEnabled(firstId));
    const qint64 sentBeforeDisabledRead = manager.txCount();
    manager.manualRead(1, 3, 0, 1);
    QCOMPARE(manager.txCount(), sentBeforeDisabledRead);
    manager.startPollingForDevices({firstId});
    QVERIFY(!manager.polling());
    QVERIFY(manager.setDeviceEnabled(0, true));

    manager.startPollingForDevices({secondId});
    QVERIFY(manager.polling());
    QCOMPARE(manager.tasks().size(), 3);
    QCOMPARE(manager.taskAt(0).value("deviceId").toString(), firstId);
    QCOMPARE(manager.taskAt(1).value("deviceId").toString(), secondId);
    QCOMPARE(manager.taskAt(2).value("deviceId").toString(), secondId);
    manager.stopPolling();

    options.insert("packageMode", "line");
    QVERIFY(manager.devices()->setAdvancedOptions(0, options));
    manager.startPolling();
    QCOMPARE(manager.tasks().size(), 4);
    QCOMPARE(manager.taskAt(0).value("count").toInt(), 1);
    manager.stopPolling();
    manager.close();
    server.disconnectDevice();
}

void CommStudioCoreTest::deviceAndPointPersistence()
{
    ModbusDeviceModel devices;
    QAbstractItemModelTester deviceTester(&devices, QAbstractItemModelTester::FailureReportingMode::QtTest);
    const QString first = devices.deviceId(0);
    const int second = devices.addDevice("Meter", 2);
    QVERIFY(devices.deviceId(second) != first);
    QVariantMap advanced = devices.advancedOptions(0);
    QCOMPARE(advanced.value("maxRegisterRead").toInt(), 125);
    advanced.insert("addressOffset", 12);
    advanced.insert("packageMode", "list");
    QVERIFY(devices.setAdvancedOptions(0, advanced));
    QVERIFY(devices.copyAdvancedOptions(0, second));
    QCOMPARE(devices.advancedOptions(second).value("addressOffset").toInt(), 12);
    QVERIFY(devices.setDeviceEnabled(second, false));
    QVERIFY(!devices.isDeviceEnabled(devices.deviceId(second)));
    ModbusDeviceModel restored;
    QVERIFY(restored.restore(devices.definitions()));
    QCOMPARE(restored.deviceId(0), first);
    QCOMPARE(restored.advancedOptions(0).value("packageMode").toString(), QStringLiteral("list"));
    QVERIFY(!restored.isDeviceEnabled(restored.deviceId(second)));
    QVERIFY(restored.setDeviceEnabled(second, true));
    QVERIFY(restored.isDeviceEnabled(restored.deviceId(second)));
    auto legacy = devices.definitions();
    auto legacySecond = legacy.at(second).toMap();
    legacySecond.remove("enabled");
    legacy[second] = legacySecond;
    QVERIFY(restored.restore(legacy));
    QVERIFY(restored.isDeviceEnabled(restored.deviceId(second)));
    auto invalid = devices.definitions(); invalid.append(invalid.first());
    QVERIFY(!restored.restore(invalid));
    QCOMPARE(restored.rowCount(), 2);

    ModbusPointModel points;
    QAbstractItemModelTester pointTester(&points, QAbstractItemModelTester::FailureReportingMode::QtTest);
    points.setDeviceId(first);
    QVERIFY(points.savePoint(-1, {{"name", "Voltage"}, {"fc", 3}, {"address", 10}, {"scale", 0.1}, {"offset", 2}}));
    points.receive(devices.deviceId(second), 3, 10, {100});
    QVERIFY(!points.pointAt(0).contains("value"));
    points.receive(first, 4, 10, {100});
    QVERIFY(!points.pointAt(0).contains("value"));
    points.receive(first, 3, 10, {100});
    QCOMPARE(points.pointAt(0).value("value").toDouble(), 12.0);
    QVERIFY(!points.definitions().first().toMap().contains("value"));
    ModbusPointModel loaded;
    QVERIFY(loaded.restore(points.definitions()));
    loaded.setDeviceId(first);
    QCOMPARE(loaded.rowCount(), 1);
    QVERIFY(!loaded.pointAt(0).contains("value"));
    loaded.setDeviceId(devices.deviceId(second));
    QCOMPARE(loaded.rowCount(), 0);
    points.copyPoint(0);
    QCOMPARE(points.rowCount(), 2);
    QVERIFY(points.pointAt(0).value("id") != points.pointAt(1).value("id"));
    QCOMPARE(points.selectedCount(), 0);
    points.selectPoint(0, false, false);
    QCOMPARE(points.selectedCount(), 1);
    QCOMPARE(points.selectedQuantityLimit(), 123);
    QVERIFY(points.updateSelected({{"count", 5}}));
    QCOMPARE(points.pointAt(0).value("count").toInt(), 5);
    QVERIFY(!points.updateSelected({{"count", 124}}));
    QCOMPARE(points.pointAt(0).value("count").toInt(), 5);
    points.toggleSelectAll();
    QCOMPARE(points.selectedCount(), 2);
    points.toggleSelectAll();
    QCOMPARE(points.selectedCount(), 0);
    QVERIFY(!points.savePoint(-1, {{"name", "Overflow"}, {"address", 65535}, {"type", "float32"}}));
    points.removePoint(0);
    QCOMPARE(points.rowCount(), 1);
}

void CommStudioCoreTest::crc16ModbusVectors()
{
    // 经典校验向量：01 03 00 00 00 02 → C4 0B（低字节在前）。
    QCOMPARE(ChecksumEngine::crc16Modbus(hex("010300000002")), quint16(0x0BC4));
    // 01 04 02 FF FF 响应体 CRC（B8 80 低字节在前追加）。
    const QByteArray frame = hex("010402FFFF");
    const quint16 crc = ChecksumEngine::crc16Modbus(frame);
    QCOMPARE(crc, quint16(0x80B8));
}

void CommStudioCoreTest::crcOtherVectors()
{
    // CRC-16/CCITT-FALSE("123456789") = 0x29B1。
    QCOMPARE(ChecksumEngine::crc16Ccitt(QByteArray("123456789")), quint16(0x29B1));
    // CRC-32("123456789") = 0xCBF43926。
    QCOMPARE(ChecksumEngine::crc32(QByteArray("123456789")), quint32(0xCBF43926));
    QCOMPARE(ChecksumEngine::sum8(hex("0102")), quint8(0x03));
    QCOMPARE(ChecksumEngine::xor8(hex("0103")), quint8(0x02));
}

void CommStudioCoreTest::hexCodecParsing()
{
    QByteArray out;
    QString error;
    QVERIFY(HexCodec::parseHex("AA 55 0x1B,2c", out, &error));
    QCOMPARE(out, hex("AA551B2C"));
    QVERIFY(!HexCodec::parseHex("AA 5", out, &error));
    QVERIFY(!HexCodec::parseHex("AG", out, &error));
}

void CommStudioCoreTest::escapeSequences()
{
    QCOMPARE(HexCodec::encodeText(QStringLiteral("A\\r\\n\\x41"), true),
             hex("410D0A41"));
    QCOMPARE(HexCodec::encodeText(QStringLiteral("\\x00\\t\\\\"), true),
             hex("00095C"));
    // 未启用转义时按 UTF-8 原样输出。
    QCOMPARE(HexCodec::encodeText(QStringLiteral("\\r"), false), QByteArrayLiteral("\\r"));
}

void CommStudioCoreTest::lineEndings()
{
    QString error;
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false, QStringLiteral("none"), &error), hex("4154"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false, QStringLiteral("cr"), &error), hex("41540D"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false, QStringLiteral("crlf"), &error), hex("41540D0A"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT\\r\\n"), false, true, QStringLiteral("none"), &error), hex("41540D0A"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("01"), true, false,
                                    QStringLiteral("crlf"), &error),
             hex("010D0A"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false,
                                    QStringLiteral("lf"), &error),
             QByteArrayLiteral("AT\n"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false,
                                    QStringLiteral("lfcr"), &error),
             QByteArrayLiteral("AT\n\r"));
}

void CommStudioCoreTest::trafficTextAndRawBytes()
{
    TrafficModel model;
    QAbstractItemModelTester modelTester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
    const QByteArray reply = QByteArrayLiteral("AT\r\n\r\nERROR\r\nAT\r\n\r\nOK\r\n");
    model.append(TrafficModel::Rx, reply, QStringLiteral("AT fixture"));
    model.flushPending();
    QCOMPARE(model.data(model.index(0, 0), TrafficModel::DataTextRole).toString(), QStringLiteral("AT\n\nERROR\nAT\n\nOK\n"));
    QCOMPARE(model.data(model.index(0, 0), TrafficModel::DataAsciiRole).toString(), QStringLiteral("AT\\r\\n\\r\\nERROR\\r\\nAT\\r\\n\\r\\nOK\\r\\n"));
    QCOMPARE(model.data(model.index(0, 0), TrafficModel::SizeRole).toInt(), reply.size());
    QCOMPARE(model.payloadHex(0), HexCodec::toHex(reply));
    QCOMPARE(model.payloadAscii(0), QStringLiteral("AT\\r\\n\\r\\nERROR\\r\\nAT\\r\\n\\r\\nOK\\r\\n"));
    const QByteArray chinese = QStringLiteral("状态：正常\r\n").toUtf8();
    model.append(TrafficModel::Rx, chinese);
    model.flushPending();
    QCOMPARE(model.payloadText(1), QStringLiteral("状态：正常\n"));
    model.append(TrafficModel::Rx, QByteArrayLiteral("A\rB\nC\n\rD\tE\0F"));
    model.flushPending();
    QCOMPARE(model.payloadText(2), QStringLiteral("A\nB\nC\nD\tE\\0F"));
    QTemporaryDir directory;
    const QString path = directory.filePath("traffic.log");
    QVERIFY(model.exportToFile(QUrl::fromLocalFile(path)));
    QFile output(path);
    QVERIFY(output.open(QIODevice::ReadOnly));
    QVERIFY(output.readAll().contains(HexCodec::toHex(reply).toUtf8()));
}

void CommStudioCoreTest::fragmentedAtReply()
{
    TrafficModel model;
    // readyRead is a byte stream, not a reply boundary: an echo and a later
    // result must both survive UI flushes, including while display is paused.
    model.append(TrafficModel::Tx, QByteArrayLiteral("AT\r\n"), QStringLiteral("AT fixture"));
    model.flushPending();
    model.append(TrafficModel::Rx, QByteArrayLiteral("AT\r"), QStringLiteral("AT fixture"));
    model.flushPending();
    QVERIFY(!model.payloadText(1).contains(QStringLiteral("OK")));
    model.setPaused(true);
    model.append(TrafficModel::Rx, QByteArrayLiteral("\n\r\nO"), QStringLiteral("AT fixture"));
    model.append(TrafficModel::Rx, QByteArrayLiteral("K\r\n"), QStringLiteral("AT fixture"));
    model.flushPending();
    QCOMPARE(model.count(), 2);
    model.setPaused(false);
    QByteArray received;
    QString displayed;
    for (int i = 1; i < model.count(); ++i) {
        received += QByteArray::fromHex(model.payloadHex(i).toLatin1());
        displayed += model.payloadText(i);
    }
    QCOMPARE(received, QByteArrayLiteral("AT\r\n\r\nOK\r\n"));
    QVERIFY(displayed.contains(QStringLiteral("OK\n")));
}

void CommStudioCoreTest::trafficPreviewAndFullPayload()
{
    TrafficModel model;
    const QByteArray reply = QByteArray(600, 'x') + QByteArrayLiteral("\r\nOK\r\n");
    model.append(TrafficModel::Rx, reply);
    model.flushPending();
    QVERIFY(model.data(model.index(0, 0), TrafficModel::OmittedBytesRole).toInt() > 0);
    QVERIFY(model.payloadText(0).endsWith(QStringLiteral("\nOK\n")));
    QCOMPARE(QByteArray::fromHex(model.payloadHex(0).toLatin1()), reply);
    QVERIFY(model.payloadText(-1).isEmpty());
    QVERIFY(model.payloadHex(1).isEmpty());
}

void CommStudioCoreTest::pausedTrafficFlushesOnResume()
{
    TrafficModel model;
    model.setMaxRows(10);
    model.setPaused(true);
    model.append(TrafficModel::Rx, QByteArrayLiteral("rx"));
    model.flushPending();
    QCOMPARE(model.count(), 0);
    model.setPaused(false);
    QCOMPARE(model.count(), 1);
}

void CommStudioCoreTest::modbusRequestFrames()
{
    // 读保持寄存器 0x0000 x2：01 03 00 00 00 02 C4 0B。
    const QByteArray pdu = ModbusCodec::buildPdu(3, 0, QVector<quint16>{ 2 });
    QCOMPARE(pdu, hex("0300000002"));
    const QByteArray adu = ModbusCodec::buildRtuAdu(1, pdu);
    QCOMPARE(adu, hex("010300000002C40B"));
    QCOMPARE(ModbusCodec::buildAsciiAdu(1, pdu), QByteArrayLiteral(":010300000002FA\r\n"));
}

void CommStudioCoreTest::modbusResponseParsing()
{
    // 正常响应：01 03 04 41 20 00 00 ...（数据 0x4120 0x0000）。
    const QByteArray response = hex("01030441200000EFC5");
    const ModbusCodec::ParsedResponse parsed = ModbusCodec::parseResponse(response, false);
    QVERIFY(parsed.ok);
    QVERIFY(parsed.crcOk);
    QVERIFY(!parsed.exception);
    QCOMPARE(parsed.slave, quint8(1));
    QCOMPARE(parsed.functionCode, quint8(3));
    QCOMPARE(parsed.words.size(), 2);
    QCOMPARE(parsed.words.at(0), quint16(0x4120));
    QCOMPARE(parsed.words.at(1), quint16(0x0000));

    // 异常响应：01 83 02 C0 F1 → 非法数据地址。
    const ModbusCodec::ParsedResponse exc = ModbusCodec::parseResponse(hex("018302C0F1"), false);
    QVERIFY(exc.ok);
    QVERIFY(exc.exception);
    QCOMPARE(exc.exceptionCode, quint8(0x02));
    QVERIFY(ModbusCodec::exceptionText(0x02).contains(QStringLiteral("非法数据地址")));

    // CRC 错误检测。
    const ModbusCodec::ParsedResponse badCrc = ModbusCodec::parseResponse(hex("010300000002C40C"), false);
    QVERIFY(badCrc.ok);
    QVERIFY(!badCrc.crcOk);

    // TCP 响应（MBAP）解析。
    const QByteArray tcpAdu = hex("00010000000601030441200000");
    const ModbusCodec::ParsedResponse tcp = ModbusCodec::parseResponse(tcpAdu, true);
    QVERIFY(tcp.ok);
    QCOMPARE(tcp.transactionId, quint16(1));
    QCOMPARE(tcp.words.at(0), quint16(0x4120));

    const QByteArray ascii = ModbusCodec::buildAsciiAdu(1, hex("030441200000"));
    const auto parsedAscii = ModbusCodec::parseAsciiResponse(ascii);
    QVERIFY(parsedAscii.ok);
    QVERIFY(parsedAscii.crcOk);
    QCOMPARE(parsedAscii.words.size(), 2);
    QVERIFY(!ModbusCodec::parseAsciiResponse(ascii.left(ascii.size() - 4) + "00\r\n").ok);
}

void CommStudioCoreTest::wordOrder32()
{
    // Float32 12.34 = 0x414570A4：ABCD → 41 45 70 A4；CDAB → 70 A4 41 45。
    const QByteArray bytes = ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("ABCD"));
    QCOMPARE(bytes, hex("414570A4"));
    QCOMPARE(ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("CDAB")), hex("70A44145"));
    QCOMPARE(ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("BADC")), hex("4541A470"));
    QCOMPARE(ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("DCBA")), hex("A4704541"));

    // 解码往返：float 12.34。
    const float value = ModbusCodec::bytesToFloat32(hex("414570A4"), QStringLiteral("ABCD"));
    QVERIFY(qAbs(value - 12.34f) < 0.001f);
}

void CommStudioCoreTest::trafficModelRingBuffer()
{
    TrafficModel model;
    model.setMaxRows(10);
    for (int i = 0; i < 25; ++i)
        // 连续 RX 片段会按短时间窗口合帧；用 TX 行验证环形缓冲的逐行裁剪。
        model.append(TrafficModel::Tx, QByteArrayLiteral("\x01\x02"), QStringLiteral("TEST"));
    model.flushPending();
    QCOMPARE(model.count(), 10);
    QCOMPARE(model.data(model.index(0, 0), TrafficModel::SizeRole), QVariant(2));

    model.appendInfo(QStringLiteral("信息行"));
    model.flushPending();
    QCOMPARE(model.count(), 10);

    model.setPaused(true);
    model.append(TrafficModel::Tx, QByteArrayLiteral("xx"));
    model.flushPending();
    QCOMPARE(model.count(), 10);

    model.clear();
    model.flushPending();
    QCOMPARE(model.count(), 0);

}

QTEST_MAIN(CommStudioCoreTest)
#include "comstudiotests.moc"
