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

#include "core/trafficmodel.h"

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
    Q_PROPERTY(QVariantList registerMap READ registerMap NOTIFY registerMapChanged)
    Q_PROPERTY(bool simulatorRunning READ simulatorRunning NOTIFY simulatorChanged)
    Q_PROPERTY(QVariantList simulatorRegisters READ simulatorRegisters NOTIFY simulatorChanged)
    Q_PROPERTY(QVariantMap rawResult READ rawResult NOTIFY rawResultChanged)
    Q_PROPERTY(bool rawBusy READ rawBusy NOTIFY rawResultChanged)
    Q_PROPERTY(qint64 txCount READ txCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 rxCount READ rxCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 errorCount READ errorCount NOTIFY statsChanged)
    Q_PROPERTY(int lastResponseMs READ lastResponseMs NOTIFY statsChanged)

public:
    explicit ModbusManager(QObject *parent = nullptr);

    TrafficModel *traffic() const { return m_traffic; }
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
    QVariantList registerMap() const { return m_registerMap; }
    bool simulatorRunning() const { return m_simulator && m_simulator->state() == QModbusDevice::ConnectedState; }
    QVariantList simulatorRegisters() const;
    QVariantMap rawResult() const { return m_rawResult; }
    bool rawBusy() const { return m_rawBusy; }
    qint64 txCount() const { return m_txCount; }
    qint64 rxCount() const { return m_rxCount; }
    qint64 errorCount() const { return m_errorCount; }
    int lastResponseMs() const { return m_lastResponseMs; }

    Q_INVOKABLE void refreshPorts();
    Q_INVOKABLE void openConnection();
    Q_INVOKABLE void close();
    // 主站手动请求：fc 为 1/2/3/4 读；写用 writeValues。
    Q_INVOKABLE void manualRead(int slave, int functionCode, int address, int count);
    Q_INVOKABLE void manualWrite(int slave, int functionCode, int address, const QString &valuesText);
    Q_INVOKABLE void addTask(const QVariantMap &task);
    Q_INVOKABLE void updateTask(int index, const QVariantMap &task);
    Q_INVOKABLE void removeTask(int index);
    Q_INVOKABLE QVariantMap taskAt(int index) const;
    Q_INVOKABLE void startPolling();
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
    void registerMapChanged();
    void simulatorChanged();
    void rawResultChanged();
    void statsChanged();
    void toastRequested(const QString &text, bool ok);

private:
    enum class LinkRole { Master, Simulator };

    bool ensureLink(LinkRole role);
    void closeLink(LinkRole role);
    QModbusDataUnit::RegisterType registerTypeForFc(int fc) const;
    void sendRequest(QModbusDataUnit::RegisterType type, int fc, int slave, quint16 address,
                     const QVector<quint16> &values, const QString &label, int taskId);
    void onReplyFinished();
    void pollTick();
    void updateTaskStatus(int taskId, const QString &status, bool ok);
    void storeReadValues(int taskId, const QVariantMap &task, const QVector<quint16> &values);
    void startSimulatorTick() {}
    void regenerateSimulatorValues();
    quint16 simulatorValueFor(const QString &source, double base, double amplitude,
                              qint64 tick, int index) const;
    void rawIdleFlush();
    TrafficModel *m_traffic = nullptr;

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
    bool m_rawBusy = false;
    QVariantMap m_rawResult;
    QElapsedTimer m_rawElapsed;

    qint64 m_txCount = 0;
    qint64 m_rxCount = 0;
    qint64 m_errorCount = 0;
    int m_lastResponseMs = 0;
    QElapsedTimer m_responseTimer;
};
