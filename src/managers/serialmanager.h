#pragma once

#include <QFile>
#include <QSerialPort>
#include <QTimer>
#include <QUrl>
#include <QVariantList>

#include "core/trafficmodel.h"

// 串口助手后端：端口枚举（含 USB 描述/VID/PID）、参数配置、打开关闭、
// 断线自动重连（0.5→1→2→4→5s 退避）、收发终端、定时发送、文件分块发送、
// DTR/RTS/Break 控制与引脚状态监视、字节统计。
class SerialManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrafficModel *traffic READ traffic CONSTANT)
    Q_PROPERTY(QVariantList portInfos READ portInfos NOTIFY portInfosChanged)
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(QString baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(int dataBits READ dataBits WRITE setDataBits NOTIFY dataBitsChanged)
    Q_PROPERTY(QString parity READ parity WRITE setParity NOTIFY parityChanged)
    Q_PROPERTY(double stopBits READ stopBits WRITE setStopBits NOTIFY stopBitsChanged)
    Q_PROPERTY(QString flowControl READ flowControl WRITE setFlowControl NOTIFY flowControlChanged)
    Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
    Q_PROPERTY(bool connecting READ isConnecting NOTIFY openChanged)
    Q_PROPERTY(bool configLocked READ configLocked NOTIFY openChanged)
    Q_PROPERTY(bool reconnecting READ reconnecting NOTIFY reconnectingChanged)
    Q_PROPERTY(QString configSummary READ configSummary NOTIFY configChanged)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect NOTIFY autoReconnectChanged)
    Q_PROPERTY(bool hexSend READ hexSend WRITE setHexSend NOTIFY hexSendChanged)
    Q_PROPERTY(bool escapeEnabled READ escapeEnabled WRITE setEscapeEnabled NOTIFY escapeEnabledChanged)
    Q_PROPERTY(QString lineEnding READ lineEnding WRITE setLineEnding NOTIFY lineEndingChanged)
    Q_PROPERTY(bool dtr READ dtr WRITE setDtr NOTIFY pinsChanged)
    Q_PROPERTY(bool rts READ rts WRITE setRts NOTIFY pinsChanged)
    Q_PROPERTY(bool pinCts READ pinCts NOTIFY pinsChanged)
    Q_PROPERTY(bool pinDsr READ pinDsr NOTIFY pinsChanged)
    Q_PROPERTY(bool pinDcd READ pinDcd NOTIFY pinsChanged)
    Q_PROPERTY(bool pinRi READ pinRi NOTIFY pinsChanged)
    Q_PROPERTY(qint64 rxBytes READ rxBytes NOTIFY statsChanged)
    Q_PROPERTY(qint64 txBytes READ txBytes NOTIFY statsChanged)
    Q_PROPERTY(qint64 rxRate READ rxRate NOTIFY statsChanged)
    Q_PROPERTY(qint64 peakRate READ peakRate NOTIFY statsChanged)
    Q_PROPERTY(bool timerActive READ timerActive NOTIFY timerChanged)
    Q_PROPERTY(int timerRemaining READ timerRemaining NOTIFY timerChanged)
    Q_PROPERTY(bool fileActive READ fileActive NOTIFY fileChanged)
    Q_PROPERTY(int fileProgress READ fileProgress NOTIFY fileChanged)
    Q_PROPERTY(QString fileRate READ fileRate NOTIFY fileChanged)

public:
    explicit SerialManager(QObject *parent = nullptr);

    TrafficModel *traffic() const { return m_traffic; }
    QVariantList portInfos() const { return m_portInfos; }
    QString portName() const { return m_portName; }
    void setPortName(const QString &name);
    QString baudRate() const { return m_baudRate; }
    void setBaudRate(const QString &rate);
    int dataBits() const { return m_dataBits; }
    void setDataBits(int bits);
    QString parity() const { return m_parity; }
    void setParity(const QString &parity);
    double stopBits() const { return m_stopBits; }
    void setStopBits(double bits);
    QString flowControl() const { return m_flowControl; }
    void setFlowControl(const QString &flow);
    bool isOpen() const;
    bool isConnecting() const { return m_reconnectTimer.isActive() && !isOpen(); }
    bool configLocked() const { return isOpen(); }
    bool reconnecting() const { return isConnecting(); }
    QString configSummary() const;
    bool autoReconnect() const { return m_autoReconnect; }
    void setAutoReconnect(bool enabled);
    bool hexSend() const { return m_hexSend; }
    void setHexSend(bool enabled);
    bool escapeEnabled() const { return m_escapeEnabled; }
    void setEscapeEnabled(bool enabled);
    QString lineEnding() const { return m_lineEnding; }
    void setLineEnding(const QString &ending);
    bool dtr() const;
    void setDtr(bool on);
    bool rts() const;
    void setRts(bool on);
    bool pinCts() const { return m_pinCts; }
    bool pinDsr() const { return m_pinDsr; }
    bool pinDcd() const { return m_pinDcd; }
    bool pinRi() const { return m_pinRi; }
    qint64 rxBytes() const { return m_rxBytes; }
    qint64 txBytes() const { return m_txBytes; }
    qint64 rxRate() const { return m_rxRate; }
    qint64 peakRate() const { return m_peakRate; }
    bool timerActive() const { return m_timerSend.isActive(); }
    int timerRemaining() const { return m_timerRemaining; }
    bool fileActive() const { return m_fileActive; }
    int fileProgress() const { return m_fileProgress; }
    QString fileRate() const { return m_fileRate; }

    Q_INVOKABLE void refreshPorts();
    // QML 中 open 是连接状态属性，动作使用独立名称避免属性遮蔽方法。
    Q_INVOKABLE void openConnection();
    Q_INVOKABLE void close();
    Q_INVOKABLE QString send(const QString &text);
    Q_INVOKABLE void startTimerSend(const QString &text, int intervalMs, int repeatCount);
    Q_INVOKABLE void stopTimerSend();
    Q_INVOKABLE void sendFile(const QUrl &path, int chunkSize, int intervalMs);
    Q_INVOKABLE void cancelFileSend();
    Q_INVOKABLE void sendBreak(int durationMs = 100);

signals:
    void portInfosChanged();
    void portNameChanged();
    void baudRateChanged();
    void dataBitsChanged();
    void parityChanged();
    void stopBitsChanged();
    void flowControlChanged();
    void openChanged();
    void reconnectingChanged();
    void configChanged();
    void autoReconnectChanged();
    void hexSendChanged();
    void escapeEnabledChanged();
    void lineEndingChanged();
    void pinsChanged();
    void statsChanged();
    void timerChanged();
    void fileChanged();
    void toastRequested(const QString &text, bool ok);

private:
    void applyConfig();
    void handlePortError(QSerialPort::SerialPortError error);
    void scheduleReconnect();
    void tryReconnect();
    void pollPorts();
    void pollPins();
    void onReadyRead();
    void onTimerSend();
    void onFileChunk();
    void finishFileSend(const QString &reason, bool ok);
    void loadSettings();
    void saveSettings() const;

    TrafficModel *m_traffic = nullptr;
    QSerialPort *m_port = nullptr;
    QVariantList m_portInfos;
    QString m_portName;
    QString m_baudRate = QStringLiteral("115200");
    int m_dataBits = 8;
    QString m_parity = QStringLiteral("none");
    double m_stopBits = 1;
    QString m_flowControl = QStringLiteral("none");
    bool m_autoReconnect = true;
    bool m_hexSend = false;
    bool m_escapeEnabled = true;
    QString m_lineEnding = QStringLiteral("none");
    bool m_userClose = false;
    int m_reconnectAttempt = 0;
    bool m_pinCts = false;
    bool m_pinDsr = false;
    bool m_pinDcd = false;
    bool m_pinRi = false;
    qint64 m_rxBytes = 0;
    qint64 m_txBytes = 0;
    qint64 m_rxWindow = 0;
    qint64 m_rxRate = 0;
    qint64 m_peakRate = 0;

    QTimer m_portPollTimer;
    QTimer m_pinPollTimer;
    QTimer m_rateTimer;
    QTimer m_reconnectTimer;
    QTimer m_timerSend;
    QString m_timerText;
    int m_timerRemaining = 0;

    QTimer m_fileTimer;
    QFile *m_file = nullptr;
    QString m_fileName;
    qint64 m_fileSize = 0;
    qint64 m_fileSent = 0;
    qint64 m_fileStarted = 0;
    int m_fileProgress = 0;
    bool m_fileActive = false;
    QString m_fileRate;
};
