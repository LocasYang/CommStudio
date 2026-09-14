#pragma once

#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QUrl>
#include <QVariantList>

#include "core/trafficmodel.h"

class QUdpSocket;

// 网络助手后端：TCP Client / TCP Server（多客户端会话）/ UDP（单播、广播、
// 组播），本地地址绑定，TCP 应用层分帧（原始流 / 行 / 固定长度 / 空闲间隔），
// 自动重连，定时发送，自动回复规则，流量统计。
class NetworkManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrafficModel *traffic READ traffic CONSTANT)
    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
    Q_PROPERTY(QVariantList interfaces READ interfaces CONSTANT)
    Q_PROPERTY(QString bindAddress READ bindAddress WRITE setBindAddress NOTIFY bindAddressChanged)
    Q_PROPERTY(QString remoteHost READ remoteHost WRITE setRemoteHost NOTIFY remoteHostChanged)
    Q_PROPERTY(int remotePort READ remotePort WRITE setRemotePort NOTIFY remotePortChanged)
    Q_PROPERTY(int listenPort READ listenPort WRITE setListenPort NOTIFY listenPortChanged)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect NOTIFY autoReconnectChanged)
    Q_PROPERTY(bool reuseAddress READ reuseAddress WRITE setReuseAddress NOTIFY reuseAddressChanged)
    Q_PROPERTY(bool broadcast READ broadcast WRITE setBroadcast NOTIFY broadcastChanged)
    Q_PROPERTY(QString multicastGroup READ multicastGroup WRITE setMulticastGroup NOTIFY multicastGroupChanged)
    Q_PROPERTY(bool multicastJoined READ multicastJoined NOTIFY openChanged)
    Q_PROPERTY(QVariantList clients READ clients NOTIFY clientsChanged)
    Q_PROPERTY(QString framing READ framing WRITE setFraming NOTIFY framingChanged)
    Q_PROPERTY(bool hexSend READ hexSend WRITE setHexSend NOTIFY hexSendChanged)
    Q_PROPERTY(bool escapeEnabled READ escapeEnabled WRITE setEscapeEnabled NOTIFY escapeEnabledChanged)
    Q_PROPERTY(QString lineEnding READ lineEnding WRITE setLineEnding NOTIFY lineEndingChanged)
    Q_PROPERTY(QVariantList replyRules READ replyRules WRITE setReplyRules NOTIFY replyRulesChanged)
    Q_PROPERTY(qint64 rxBytes READ rxBytes NOTIFY statsChanged)
    Q_PROPERTY(qint64 txBytes READ txBytes NOTIFY statsChanged)
    Q_PROPERTY(qint64 rxRate READ rxRate NOTIFY statsChanged)
    Q_PROPERTY(bool timerActive READ timerActive NOTIFY timerChanged)
    Q_PROPERTY(int timerRemaining READ timerRemaining NOTIFY timerChanged)
    Q_PROPERTY(QString stateText READ stateText NOTIFY openChanged)

public:
    explicit NetworkManager(QObject *parent = nullptr);

    TrafficModel *traffic() const { return m_traffic; }
    QString mode() const { return m_mode; }
    void setMode(const QString &mode);
    bool isOpen() const;
    QVariantList interfaces() const;
    QString bindAddress() const { return m_bindAddress; }
    void setBindAddress(const QString &address);
    QString remoteHost() const { return m_remoteHost; }
    void setRemoteHost(const QString &host);
    int remotePort() const { return m_remotePort; }
    void setRemotePort(int port);
    int listenPort() const { return m_listenPort; }
    void setListenPort(int port);
    bool autoReconnect() const { return m_autoReconnect; }
    void setAutoReconnect(bool enabled);
    bool reuseAddress() const { return m_reuseAddress; }
    void setReuseAddress(bool enabled);
    bool broadcast() const { return m_broadcast; }
    void setBroadcast(bool enabled);
    QString multicastGroup() const { return m_multicastGroup; }
    void setMulticastGroup(const QString &group);
    bool multicastJoined() const { return !m_multicastGroup.isNull() && m_udpJoined; }
    QVariantList clients() const { return m_clients; }
    QString framing() const { return m_framing; }
    void setFraming(const QString &framing);
    bool hexSend() const { return m_hexSend; }
    void setHexSend(bool enabled);
    bool escapeEnabled() const { return m_escapeEnabled; }
    void setEscapeEnabled(bool enabled);
    QString lineEnding() const { return m_lineEnding; }
    void setLineEnding(const QString &ending);
    QVariantList replyRules() const { return m_replyRules; }
    void setReplyRules(const QVariantList &rules);
    qint64 rxBytes() const { return m_rxBytes; }
    qint64 txBytes() const { return m_txBytes; }
    qint64 rxRate() const { return m_rxRate; }
    bool timerActive() const { return m_timerSend.isActive(); }
    int timerRemaining() const { return m_timerRemaining; }
    QString stateText() const;

    Q_INVOKABLE void openConnection();
    Q_INVOKABLE void close();
    Q_INVOKABLE QString send(const QString &text, qint64 targetClientId = -1);
    Q_INVOKABLE void sendTo(const QString &host, int port, const QString &text);
    Q_INVOKABLE void disconnectClient(qint64 clientId);
    Q_INVOKABLE void joinMulticast(const QString &group);
    Q_INVOKABLE void leaveMulticast();
    Q_INVOKABLE void startTimerSend(const QString &text, int intervalMs, int repeatCount);
    Q_INVOKABLE void stopTimerSend();

signals:
    void modeChanged();
    void openChanged();
    void bindAddressChanged();
    void remoteHostChanged();
    void remotePortChanged();
    void listenPortChanged();
    void autoReconnectChanged();
    void reuseAddressChanged();
    void broadcastChanged();
    void multicastGroupChanged();
    void clientsChanged();
    void framingChanged();
    void hexSendChanged();
    void escapeEnabledChanged();
    void lineEndingChanged();
    void replyRulesChanged();
    void statsChanged();
    void timerChanged();
    void toastRequested(const QString &text, bool ok);

private:
    struct ClientSession {
        QTcpSocket *socket = nullptr;
        quintptr id = 0;
        QString peer;
        quint16 peerPort = 0;
        QDateTime connectedAt;
        quint64 rxBytes = 0;
        quint64 txBytes = 0;
    };

    void setupTcpClient();
    void closeAll();
    void onTcpClientConnected();
    void onTcpClientDisconnected();
    void onTcpClientError(QAbstractSocket::SocketError error);
    void onNewConnection();
    void onSocketReadyRead(QTcpSocket *socket);
    void onUdpReadyRead();
    void processFrames(QTcpSocket *socket, const QByteArray &incoming);
    void emitFrame(const QByteArray &frame, const QString &peer);
    void evaluateReplyRules(const QByteArray &data, const QString &peer,
                            const std::function<void(const QByteArray &)> &send);
    void updateClientsModel();
    ClientSession *sessionById(qint64 id);
    void handleUdpDatagram(const QByteArray &data, const QHostAddress &from, quint16 port);
    void deliverIncoming(const QByteArray &data, const QString &peer);

    TrafficModel *m_traffic = nullptr;
    QString m_mode = QStringLiteral("tcpClient");

    QTcpSocket *m_tcpSocket = nullptr;
    QTcpServer *m_tcpServer = nullptr;
    QUdpSocket *m_udpSocket = nullptr;
    QHash<quintptr, ClientSession *> m_sessions;

    QString m_bindAddress = QStringLiteral("Any");
    QString m_remoteHost = QStringLiteral("127.0.0.1");
    int m_remotePort = 6000;
    int m_listenPort = 6000;
    bool m_autoReconnect = true;
    bool m_reuseAddress = true;
    bool m_broadcast = false;
    QString m_multicastGroup;
    bool m_udpJoined = false;
    bool m_userClose = false;
    QTimer m_reconnectTimer;
    QVariantList m_clients;

    // TCP 应用层分帧状态（按 socket 存缓冲）。
    QString m_framing = QStringLiteral("raw");
    int m_fixedFrameSize = 16;
    QHash<QTcpSocket *, QByteArray> m_rxBuffers;
    QHash<QTcpSocket *, QTimer *> m_idleTimers;

    bool m_hexSend = false;
    bool m_escapeEnabled = true;
    QString m_lineEnding = QStringLiteral("none");
    QVariantList m_replyRules;

    qint64 m_rxBytes = 0;
    qint64 m_txBytes = 0;
    qint64 m_rxWindow = 0;
    qint64 m_rxRate = 0;
    QTimer m_rateTimer;

    QTimer m_timerSend;
    QString m_timerText;
    int m_timerRemaining = 0;
};
