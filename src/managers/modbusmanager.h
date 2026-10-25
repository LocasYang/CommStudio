#pragma once

#include <QElapsedTimer>
#include <QModbusClient>
#include <QModbusServer>
#include <QSerialPort>
#include <QTcpSocket>
#include <QTimer>
#include <QUrl>
#include <QVector>
#include <QVariantList>
#include <QSet>
#include <QStringList>

#include "core/trafficmodel.h"
#include "models/modbusdevicemodel.h"
#include "models/modbuspointmodel.h"

class QModbusReply;

// Modbus 助手后端（双引擎，设计手册 3.1）：
//  - 标准引擎：QModbusRtuSerialClient / QModbusTcpClient 主站，
//    QModbusRtuSerialServer / QModbusTcpServer 从站模拟；
//  - Raw 引擎：ModbusCodec + QSerialPort/QTcpSocket，支持任意原始 ADU 收发与
//    响应解析（CRC 校验 / 异常码 / MBAP）。
//  - 轮询调度：单一定时器统一调度多任务，严格串行（同一时刻只有一个未完成请求）。
class ModbusManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrafficModel *traffic READ traffic CONSTANT)
    Q_PROPERTY(ModbusDeviceModel *devices READ devices CONSTANT)
    Q_PROPERTY(ModbusPointModel *points READ points CONSTANT)
    Q_PROPERTY(int selectedDeviceIndex READ selectedDeviceIndex NOTIFY selectedDeviceChanged)
    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(QString baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(QString parity READ parity WRITE setParity NOTIFY parityChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(int unitId READ unitId WRITE setUnitId NOTIFY unitIdChanged)
    Q_PROPERTY(int slaveAddress READ slaveAddress WRITE setSlaveAddress NOTIFY slaveAddressChanged)
    Q_PROPERTY(int timeoutMs READ timeoutMs WRITE setTimeoutMs NOTIFY timeoutMsChanged)
    Q_PROPERTY(int retries READ retries WRITE setRetries NOTIFY retriesChanged)
    Q_PROPERTY(QVariantList portInfos READ portInfos NOTIFY portInfosChanged)
    Q_PROPERTY(QVariantList tasks READ tasks NOTIFY tasksChanged)
    Q_PROPERTY(bool polling READ polling NOTIFY pollingChanged)
    Q_PROPERTY(bool batchWriting READ batchWriting NOTIFY batchWritingChanged)
    Q_PROPERTY(QVariantList registerMap READ registerMap NOTIFY registerMapChanged)
    Q_PROPERTY(bool simulatorRunning READ simulatorRunning NOTIFY simulatorChanged)
    Q_PROPERTY(QVariantMap simulatorAreas READ simulatorAreas NOTIFY simulatorChanged)
    Q_PROPERTY(QVariantList simulatorRegisters READ simulatorRegisters NOTIFY simulatorChanged)
    Q_PROPERTY(QVariantMap rawResult READ rawResult NOTIFY rawResultChanged)
    Q_PROPERTY(bool rawBusy READ rawBusy NOTIFY rawResultChanged)
    Q_PROPERTY(qint64 txCount READ txCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 rxCount READ rxCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 errorCount READ errorCount NOTIFY statsChanged)
    Q_PROPERTY(int lastResponseMs READ lastResponseMs NOTIFY statsChanged)
    Q_PROPERTY(QVariantMap statistics READ statistics NOTIFY statsChanged)
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanChanged)
    Q_PROPERTY(QVariantList scanResults READ scanResults NOTIFY scanChanged)
    Q_PROPERTY(bool rawLoopActive READ rawLoopActive NOTIFY rawLoopChanged)
    Q_PROPERTY(int rawLoopSent READ rawLoopSent NOTIFY rawLoopChanged)
    Q_PROPERTY(QVariantList rawTemplates READ rawTemplates NOTIFY rawTemplatesChanged)
    Q_PROPERTY(QVariantMap channelOptions READ channelOptions NOTIFY channelOptionsChanged)
    Q_PROPERTY(QVariantMap visibleColumns READ visibleColumns NOTIFY visibleColumnsChanged)
    Q_PROPERTY(int connectionState READ connectionState NOTIFY openChanged)

public:
    explicit ModbusManager(QObject *parent = nullptr, const QString &settingsPrefix = QStringLiteral("modbus"));

    TrafficModel *traffic() const { return m_traffic; }
    ModbusDeviceModel *devices() const { return m_devices; }
    ModbusPointModel *points() const { return m_points; }
    Q_INVOKABLE void readPoint(int row);
    Q_INVOKABLE void readAllPoints();
    Q_INVOKABLE void createPointTasks(int strategy = 0, int maxGap = 0, int maxRegisters = 125);
    Q_INVOKABLE bool updateDevice(int index, const QString &name, int unitId);
    Q_INVOKABLE bool setDeviceEnabled(int index, bool enabled);
    int selectedDeviceIndex() const { return m_selectedDeviceIndex; }
    QString mode() const { return m_mode; }
    void setMode(const QString &mode);
    bool isOpen() const;
    QString portName() const { return m_portName; }
    void setPortName(const QString &name);
    QString baudRate() const { return m_baudRate; }
    void setBaudRate(const QString &rate);
    QString parity() const { return m_parity; }
    void setParity(const QString &parity);
    QString host() const { return m_host; }
    void setHost(const QString &host);
    int port() const { return m_port; }
    void setPort(int port);
    int unitId() const { return m_unitId; }
    void setUnitId(int id);
    int slaveAddress() const { return m_slaveAddress; }
    void setSlaveAddress(int address);
    int timeoutMs() const { return m_timeoutMs; }
    void setTimeoutMs(int ms);
    int retries() const { return m_retries; }
    void setRetries(int retries);
    QVariantList portInfos() const;
    QVariantList tasks() const { return m_tasks; }
    bool polling() const { return m_polling; }
    bool batchWriting() const { return !m_batchPlan.isEmpty(); }
    QVariantList registerMap() const { return m_registerMap; }
    bool simulatorRunning() const { return m_asciiSlaveOpen || (m_simulator && m_simulator->state() == QModbusDevice::ConnectedState); }
    QVariantMap simulatorAreas() const { return m_simAreas; }
    QVariantList simulatorRegisters() const;
    QVariantMap rawResult() const { return m_rawResult; }
    bool rawBusy() const { return m_rawBusy; }
    qint64 txCount() const { return m_txCount; }
    qint64 rxCount() const { return m_rxCount; }
    qint64 errorCount() const { return m_errorCount; }
    int lastResponseMs() const { return m_lastResponseMs; }
    QVariantMap statistics() const;
    Q_INVOKABLE void resetStatistics();
    bool scanning() const { return m_scanning; }
    QVariantList scanResults() const { return m_scanResults; }
    Q_INVOKABLE void startScan(int first, int last, int fc, int address, int timeoutMs);
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE void addDiscoveredDevices();
    bool rawLoopActive() const { return m_rawLoopActive; }
    int rawLoopSent() const { return m_rawLoopSent; }
    QVariantList rawTemplates() const { return m_rawTemplates; }
    Q_INVOKABLE void startRawLoop(const QString &request, bool tcp, bool crc, int count, int intervalMs, int jitterMs = 0);
    Q_INVOKABLE void stopRawLoop();
    Q_INVOKABLE void saveRawTemplate(const QString &name, const QString &request, bool tcp, bool crc);
    Q_INVOKABLE void removeRawTemplate(int index);
    QVariantMap channelOptions() const { return m_channelOptions; }
    QVariantMap visibleColumns() const { return m_visibleColumns; }
    Q_INVOKABLE void setVisibleColumns(const QVariantMap &columns);
    Q_INVOKABLE void setChannelOption(const QString &key, const QVariant &value);
    int connectionState() const;

    Q_INVOKABLE void refreshPorts();
    Q_INVOKABLE void selectDevice(int index);
    Q_INVOKABLE int addDevice(const QString &name, int unitId);
    Q_INVOKABLE void removeDevice(int index);
    bool replaceDevicePoints(const QString &deviceId, const QVariantList &points);
    Q_INVOKABLE bool deleteSelectedPoints();
    Q_INVOKABLE void openConnection();
    Q_INVOKABLE void close();
    // 主站手动请求：fc 为 1/2/3/4 读；写用 writeValues。
    Q_INVOKABLE void manualRead(int slave, int functionCode, int address, int count);
    Q_INVOKABLE void manualWrite(int slave, int functionCode, int address, const QString &valuesText);
    Q_INVOKABLE void writePoint(int row, const QString &valuesText);
    Q_INVOKABLE bool writeBatchOnce();
    Q_INVOKABLE bool canStartBatchWriting(const QStringList &deviceIds);
    Q_INVOKABLE bool startBatchWriting(const QStringList &deviceIds);
    Q_INVOKABLE void stopBatchWriting();
    Q_INVOKABLE void addTask(const QVariantMap &task);
    Q_INVOKABLE void updateTask(int index, const QVariantMap &task);
    Q_INVOKABLE void removeTask(int index);
    Q_INVOKABLE QVariantMap taskAt(int index) const;
    Q_INVOKABLE void startPolling();
    Q_INVOKABLE void startPollingForDevices(const QStringList &deviceIds);
    Q_INVOKABLE void stopPolling();
    Q_INVOKABLE void readTaskOnce(int index);
    Q_INVOKABLE bool importTasksFromCsv(const QUrl &path);
    // 写预览：显示工程量 → 原始值 → 寄存器编码结果（设计手册 3.7）。
    Q_INVOKABLE QVariantMap previewWrite(double value, double scale, double offsetValue,
                                         const QString &type, const QString &wordOrder) const;

    // Raw Test Center：requestText 为 "从站地址 + PDU"（RTU）或 "UnitId + PDU"（TCP）。
    Q_INVOKABLE void sendRawFrame(const QString &requestText, bool isTcp, bool autoCrc);
    Q_INVOKABLE void closeRawLink();
    // 从站模拟器：area = coils / discrete / holding / input。
    Q_INVOKABLE void configureSimulatorArea(const QString &area, int count, const QString &source,
                                            double baseValue, double amplitude);
    Q_INVOKABLE void writeSimulatorRegister(const QString &area, int address, int value);

signals:
    void modeChanged();
    void selectedDeviceChanged();
    void openChanged();
    void portNameChanged();
    void baudRateChanged();
    void parityChanged();
    void hostChanged();
    void portChanged();
    void unitIdChanged();
    void slaveAddressChanged();
    void timeoutMsChanged();
    void retriesChanged();
    void portInfosChanged();
    void tasksChanged();
    void pollingChanged();
    void batchWritingChanged();
    void registerMapChanged();
    void simulatorChanged();
    void rawResultChanged();
    void statsChanged();
    void scanChanged();
    void rawLoopChanged();
    void rawTemplatesChanged();
    void channelOptionsChanged();
    void visibleColumnsChanged();
    void toastRequested(const QString &text, bool ok);

private:
    enum class LinkRole { Master, Simulator };
    QString m_settingsPrefix;

    bool ensureLink(LinkRole role);
    void closeLink(LinkRole role);
    QModbusDataUnit::RegisterType registerTypeForFc(int fc) const;
    void sendRequest(QModbusDataUnit::RegisterType type, int fc, int slave, quint16 address,
                     const QVector<quint16> &values, const QString &label, int taskId,
                     const QString &deviceId = QString(), const QByteArray &rawPdu = QByteArray(),
                     const QVariantMap &requestOptions = {});
    void dispatchNext();
    void scanNext();
    void rawLoopTick();
    void onReplyFinished();
    void sendAsciiRequest(QModbusDataUnit::RegisterType type, int fc, int slave, quint16 address,
                          const QVector<quint16> &values, const QString &label, int taskId,
                          const QString &deviceId, const QVariantMap &requestOptions = {});
    void pollTick();
    void updateTaskStatus(int taskId, const QString &status, bool ok);
    void storeReadValues(int taskId, const QVariantMap &task, const QVector<quint16> &values);
    void startSimulatorTick() {}
    void regenerateSimulatorValues();
    quint16 simulatorValueFor(const QString &source, double base, double amplitude,
                              qint64 tick, int index) const;
    void rawIdleFlush();
    void openAsciiLink();
    void openAsciiSlaveLink();
    void handleAsciiSlaveFrame();
    void saveWorkspace();
    TrafficModel *m_traffic = nullptr;
    ModbusDeviceModel *m_devices = nullptr;
    ModbusPointModel *m_points = nullptr;
    QString m_currentDeviceId;
    struct PendingRequest {
        QModbusDataUnit::RegisterType type;
        int fc;
        int slave;
        quint16 address;
        QVector<quint16> values;
        QString label;
        int taskId;
        QString deviceId;
        int priority;
        QByteArray rawPdu;
        QVariantMap options;
    };
    bool prepareBatch(const QStringList &deviceIds, QList<PendingRequest> &prepared, QString &error) const;
    bool batchWriteTick();
    void removeBatchDevice(const QString &deviceId);
    void applyStartupWrites(const QStringList &deviceIds = {});
    QList<PendingRequest> m_batchPlan;
    QVector<qint64> m_batchDue;
    QElapsedTimer m_batchClock;
    bool m_preferBatch = true;
    QList<PendingRequest> m_requests;
    bool m_currentRaw = false;
    bool m_scanning = false;
    int m_scanNext = 1, m_scanLast = 247, m_scanFc = 3, m_scanAddress = 0;
    int m_scanTimeout = 100;
    QModbusReply *m_scanReply = nullptr;
    QVariantList m_scanResults;
    bool m_rawLoopActive = false;
    int m_rawLoopSent = 0, m_rawLoopCount = 0, m_rawLoopInterval = 1000, m_rawLoopJitter = 0;
    QString m_rawLoopRequest;
    bool m_rawLoopTcp = false, m_rawLoopCrc = true;
    QTimer m_rawLoopTimer;
    QVariantList m_rawTemplates;
    QVariantMap m_channelOptions{{"dataBits", 8}, {"stopBits", 1}, {"flowControl", 0},
        {"interFrameUs", -1}, {"turnaroundMs", 100}, {"addressOffset", 0},
        {"crcByteOrder", QStringLiteral("little")}, {"coilBitOrder", QStringLiteral("lsb")},
        {"writeCacheMs", 0}, {"tcpConcurrency", 1}, {"broadcastAddress", 0},
        {"autoReconnect", false}, {"reconnectMs", 1000}, {"minimumPollMs", 10},
        {"autoPoll", false}, {"keepAlive", true}, {"mergeGap", 0}, {"maxRead", 125}};
    QVariantMap m_visibleColumns;
    QTimer m_reconnectTimer;
    bool m_userDisconnect = true;
    bool m_countedConnection = false;
    int m_selectedDeviceIndex = 0;

    QString m_mode = QStringLiteral("rtuMaster");
    QModbusClient *m_masterClient = nullptr;
    QModbusServer *m_simulator = nullptr;
    QModbusReply *m_currentReply = nullptr;

    QString m_portName;
    QString m_baudRate = QStringLiteral("9600");
    QString m_parity = QStringLiteral("none");
    QString m_host = QStringLiteral("127.0.0.1");
    int m_port = 502;
    int m_unitId = 1;
    int m_slaveAddress = 1;
    int m_timeoutMs = 1000;
    int m_retries = 1;

    QVariantList m_tasks;
    QVector<qint64> m_nextDue;
    QSet<QString> m_activePollingDevices;
    bool m_polling = false;
    QTimer m_pollTimer;
    int m_currentTaskId = -1;
    QString m_currentLabel;
    int m_currentAddress = 0;
    int m_currentFc = 0;
    int m_currentSlave = 0;

    QVariantList m_registerMap;

    // 从站模拟配置：area → {count, source, base, amplitude}
    QVariantMap m_simAreas;
    QTimer m_simTick;
    qint64 m_simCounter = 0;

    // Raw Test Center
    QSerialPort *m_rawSerial = nullptr;
    QTcpSocket *m_rawTcp = nullptr;
    QByteArray m_rawBuffer;
    QTimer m_rawIdleTimer;
    bool m_rawIsTcp = false;
    bool m_rawIsAscii = false;
    bool m_rawBusy = false;
    QVariantMap m_rawResult;
    QElapsedTimer m_rawElapsed;
    bool m_asciiOpen = false;
    bool m_asciiSlaveOpen = false;
    QByteArray m_asciiSlaveBuffer;

    qint64 m_txCount = 0;
    qint64 m_rxCount = 0;
    qint64 m_errorCount = 0;
    int m_lastResponseMs = 0;
    QElapsedTimer m_responseTimer;
    qint64 m_txBytes = 0, m_rxBytes = 0, m_timeouts = 0, m_exceptions = 0;
    qint64 m_crcErrors = 0, m_frameErrors = 0;
    qint64 m_rttSum = 0;
    qint64 m_successCount = 0;
    int m_rttMin = -1, m_rttMax = 0;
};
