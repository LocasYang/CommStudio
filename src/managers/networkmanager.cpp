#include "networkmanager.h"

#include "core/hexcodec.h"
#include "studio.h"

#include <QDateTime>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QUdpSocket>

namespace {

Studio *g_studio = nullptr;

QString socketErrorText(QAbstractSocket::SocketError error)
{
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError: return QStringLiteral("连接被拒绝");
    case QAbstractSocket::RemoteHostClosedError: return QStringLiteral("对端已关闭连接");
    case QAbstractSocket::HostNotFoundError: return QStringLiteral("主机不可解析");
    case QAbstractSocket::SocketAccessError: return QStringLiteral("访问受限");
    case QAbstractSocket::NetworkError: return QStringLiteral("网络错误");
    case QAbstractSocket::SocketTimeoutError: return QStringLiteral("超时");
    default: return QStringLiteral("套接字错误 %1").arg(int(error));
    }
}

} // namespace

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_traffic(new TrafficModel(this))
{
    if (qobject_cast<Studio *>(parent))
        g_studio = qobject_cast<Studio *>(parent);

    m_rateTimer.setInterval(1000);
    connect(&m_rateTimer, &QTimer::timeout, this, [this] {
        m_rxRate = m_rxWindow;
        m_rxWindow = 0;
        emit statsChanged();
    });
    m_rateTimer.start();

    m_reconnectTimer.setSingleShot(true);
    m_reconnectTimer.setInterval(2000);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this] {
        if (!isOpen() && !m_userClose && m_mode == QLatin1String("tcpClient"))
            open();
    });

    connect(&m_timerSend, &QTimer::timeout, this, [this] {
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
    });

    m_traffic->appendInfo(QStringLiteral("网络助手就绪 · TCP Client / TCP Server / UDP"));
}

void NetworkManager::setMode(const QString &mode)
{
    if (m_mode == mode)
        return;
    if (isOpen())
        close();
    m_mode = mode;
    emit modeChanged();
}

bool NetworkManager::isOpen() const
{
    if (m_mode == QLatin1String("tcpClient"))
        return m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState;
    if (m_mode == QLatin1String("tcpServer"))
        return m_tcpServer && m_tcpServer->isListening();
    return m_udpSocket && m_udpSocket->state() == QAbstractSocket::BoundState;
}

QString NetworkManager::stateText() const
{
    if (m_mode == QLatin1String("tcpClient"))
        return m_tcpSocket ? m_tcpSocket->state() == QAbstractSocket::ConnectedState
                                 ? QStringLiteral("已连接")
                                 : QStringLiteral("未连接")
                           : QStringLiteral("未连接");
    if (m_mode == QLatin1String("tcpServer"))
        return m_tcpServer && m_tcpServer->isListening()
                   ? QStringLiteral("监听中 · %1 个客户端").arg(m_sessions.size())
                   : QStringLiteral("未监听");
    return m_udpSocket && m_udpSocket->state() == QAbstractSocket::BoundState
               ? QStringLiteral("已绑定 :%1").arg(m_udpSocket->localPort())
               : QStringLiteral("未绑定");
}

QVariantList NetworkManager::interfaces() const
{
    QVariantList result;
    QVariantMap any;
    any.insert(QStringLiteral("name"), QStringLiteral("Any"));
    any.insert(QStringLiteral("label"), QStringLiteral("所有网卡 (0.0.0.0)"));
    result.append(any);
    const auto list = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &ni : list) {
        if (!(ni.flags() & QNetworkInterface::CanBroadcast)
            && !(ni.flags() & QNetworkInterface::IsRunning))
            continue;
        for (const QNetworkAddressEntry &entry : ni.addressEntries()) {
            const QHostAddress ip = entry.ip();
            if (ip.protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            QVariantMap map;
            map.insert(QStringLiteral("name"), ni.name());
            map.insert(QStringLiteral("label"),
                       QStringLiteral("%1 · %2").arg(ni.humanReadableName(), ip.toString()));
            map.insert(QStringLiteral("ip"), ip.toString());
            if (!result.isEmpty() && result.first().toMap().value(QStringLiteral("ip")) == ip.toString())
                continue;
            result.append(map);
            break; // 每块网卡只取第一个 IPv4 地址
        }
    }
    return result;
}

void NetworkManager::setBindAddress(const QString &address)
{
    if (m_bindAddress == address)
        return;
    m_bindAddress = address;
    emit bindAddressChanged();
}

void NetworkManager::setRemoteHost(const QString &host)
{
    if (m_remoteHost == host)
        return;
    m_remoteHost = host;
    emit remoteHostChanged();
}

void NetworkManager::setRemotePort(int port)
{
    if (m_remotePort == port)
        return;
    m_remotePort = port;
    emit remotePortChanged();
}

void NetworkManager::setListenPort(int port)
{
    if (m_listenPort == port)
        return;
    m_listenPort = port;
    emit listenPortChanged();
}

void NetworkManager::setAutoReconnect(bool enabled)
{
    if (m_autoReconnect == enabled)
        return;
    m_autoReconnect = enabled;
    emit autoReconnectChanged();
}

void NetworkManager::setReuseAddress(bool enabled)
{
    if (m_reuseAddress == enabled)
        return;
    m_reuseAddress = enabled;
    emit reuseAddressChanged();
}

void NetworkManager::setBroadcast(bool enabled)
{
    if (m_broadcast == enabled)
        return;
    m_broadcast = enabled;
    emit broadcastChanged();
}

void NetworkManager::setMulticastGroup(const QString &group)
{
    if (m_multicastGroup == group)
        return;
    m_multicastGroup = group;
    emit multicastGroupChanged();
}

void NetworkManager::setFraming(const QString &framing)
{
    if (m_framing == framing)
        return;
    m_framing = framing;
    emit framingChanged();
}

void NetworkManager::setHexSend(bool enabled)
{
    if (m_hexSend == enabled)
        return;
    m_hexSend = enabled;
    emit hexSendChanged();
}

void NetworkManager::setEscapeEnabled(bool enabled)
{
    if (m_escapeEnabled == enabled)
        return;
    m_escapeEnabled = enabled;
    emit escapeEnabledChanged();
}

void NetworkManager::setLineEnding(const QString &ending)
{
    if (m_lineEnding == ending)
        return;
    m_lineEnding = ending;
    emit lineEndingChanged();
}

void NetworkManager::setReplyRules(const QVariantList &rules)
{
    m_replyRules = rules;
    emit replyRulesChanged();
}

QHostAddress bindAddressOf(const QString &name)
{
    if (name.isEmpty() || name == QLatin1String("Any"))
        return QHostAddress::AnyIPv4;
    return QHostAddress(name);
}

void NetworkManager::open()
{
    if (isOpen()) {
        close();
        return;
    }
    m_userClose = false;

    if (m_mode == QLatin1String("tcpClient")) {
        setupTcpClient();
        m_traffic->appendInfo(QStringLiteral("正在连接 %1:%2 …").arg(m_remoteHost).arg(m_remotePort));
        m_tcpSocket->connectToHost(m_remoteHost, static_cast<quint16>(m_remotePort));
        emit openChanged();
        return;
    }

    if (m_mode == QLatin1String("tcpServer")) {
        if (!m_tcpServer) {
            m_tcpServer = new QTcpServer(this);
            connect(m_tcpServer, &QTcpServer::newConnection,
                    this, &NetworkManager::onNewConnection);
        }
        const QHostAddress bind = bindAddressOf(m_bindAddress);
        if (!m_tcpServer->listen(bind, static_cast<quint16>(m_listenPort))) {
            const QString message = QStringLiteral("监听失败: %1").arg(m_tcpServer->errorString());
            m_traffic->appendInfo(message, true);
            emit toastRequested(message, false);
            return;
        }
        m_traffic->appendInfo(QStringLiteral("TCP Server 开始监听 %1:%2")
                                  .arg(m_tcpServer->serverAddress().toString())
                                  .arg(m_tcpServer->serverPort()));
        if (g_studio) {
            g_studio->log(QStringLiteral("success"), QStringLiteral("TCP Server 监听 %1:%2")
                              .arg(m_tcpServer->serverAddress().toString())
                              .arg(m_tcpServer->serverPort()));
            g_studio->addTraffic(0, 0, 1);
        }
        emit openChanged();
        return;
    }

    // UDP
    if (!m_udpSocket) {
        m_udpSocket = new QUdpSocket(this);
        connect(m_udpSocket, &QUdpSocket::readyRead,
                this, &NetworkManager::onUdpReadyRead);
    }
    m_udpSocket->close();
    const QHostAddress bind = bindAddressOf(m_bindAddress);
    const QAbstractSocket::BindMode bindMode = m_reuseAddress
        ? QAbstractSocket::BindMode(QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint)
        : QAbstractSocket::BindMode(QAbstractSocket::DefaultForPlatform);
    if (!m_udpSocket->bind(bind, static_cast<quint16>(m_listenPort), bindMode)) {
        const QString message = QStringLiteral("UDP 绑定失败: %1").arg(m_udpSocket->errorString());
        m_traffic->appendInfo(message, true);
        emit toastRequested(message, false);
        return;
    }
    m_traffic->appendInfo(QStringLiteral("UDP 已绑定 %1:%2")
                              .arg(m_udpSocket->localAddress().toString())
                              .arg(m_udpSocket->localPort()));
    if (g_studio) {
        g_studio->log(QStringLiteral("success"), QStringLiteral("UDP 绑定 %1:%2")
                          .arg(m_udpSocket->localAddress().toString())
                          .arg(m_udpSocket->localPort()));
        g_studio->addTraffic(0, 0, 1);
    }
    emit openChanged();
}

void NetworkManager::setupTcpClient()
{
    if (m_tcpSocket)
        return;
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &NetworkManager::onTcpClientConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &NetworkManager::onTcpClientDisconnected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &NetworkManager::onTcpClientError);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, [this] {
        onSocketReadyRead(m_tcpSocket);
    });
}

void NetworkManager::close()
{
    if (!isOpen())
        return;
    m_userClose = true;
    closeAll();
    emit openChanged();
}

void NetworkManager::closeAll()
{
    if (m_tcpSocket) {
        if (m_tcpSocket->state() == QAbstractSocket::ConnectedState)
            m_tcpSocket->disconnectFromHost();
        else
            m_tcpSocket->abort();
    }
    if (m_tcpServer && m_tcpServer->isListening()) {
        m_tcpServer->close();
        if (g_studio)
            g_studio->addTraffic(0, 0, -1);
    }
    for (ClientSession *session : std::as_const(m_sessions)) {
        session->socket->abort();
        session->socket->deleteLater();
        delete session;
    }
    m_sessions.clear();
    if (m_udpSocket && m_udpSocket->state() == QAbstractSocket::BoundState) {
        if (m_udpJoined && !m_multicastGroup.isEmpty())
            leaveMulticast();
        m_udpSocket->close();
        if (g_studio)
            g_studio->addTraffic(0, 0, -1);
    }
    updateClientsModel();
    m_traffic->appendInfo(QStringLiteral("连接已关闭"));
    if (g_studio)
        g_studio->setStatusMessage(QStringLiteral("网络连接已关闭"));
}

void NetworkManager::onTcpClientConnected()
{
    m_traffic->appendInfo(QStringLiteral("TCP 已连接 %1:%2 (本地 %3:%4)")
                              .arg(m_remoteHost).arg(m_remotePort)
                              .arg(m_tcpSocket->localAddress().toString())
                              .arg(m_tcpSocket->localPort()));
    if (g_studio) {
        g_studio->log(QStringLiteral("success"), QStringLiteral("TCP Client 已连接 %1:%2")
                          .arg(m_remoteHost).arg(m_remotePort));
        g_studio->addTraffic(0, 0, 1);
    }
    emit openChanged();
}

void NetworkManager::onTcpClientDisconnected()
{
    m_traffic->appendInfo(QStringLiteral("TCP 连接已断开"));
    if (g_studio) {
        g_studio->addTraffic(0, 0, -1);
        g_studio->setStatusMessage(QStringLiteral("TCP 连接已断开"));
    }
    emit openChanged();
    if (m_autoReconnect && !m_userClose)
        m_reconnectTimer.start();
}

void NetworkManager::onTcpClientError(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::RemoteHostClosedError)
        return; // 由 disconnected 分支处理
    if (!m_tcpSocket)
        return;
    const QString message = QStringLiteral("TCP 客户端: %1").arg(socketErrorText(error));
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        m_traffic->appendInfo(message, true);
        if (g_studio)
            g_studio->log(QStringLiteral("error"), message);
        if (m_autoReconnect && !m_userClose)
            m_reconnectTimer.start();
    }
}

void NetworkManager::onNewConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket *socket = m_tcpServer->nextPendingConnection();
        auto *session = new ClientSession;
        session->socket = socket;
        session->id = reinterpret_cast<quintptr>(socket);
        session->peer = socket->peerAddress().toString();
        session->peerPort = socket->peerPort();
        session->connectedAt = QDateTime::currentDateTime();
        m_sessions.insert(session->id, session);

        connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
            onSocketReadyRead(socket);
        });
        connect(socket, &QTcpSocket::disconnected, this, [this, socket] {
            if (m_sessions.contains(reinterpret_cast<quintptr>(socket))) {
                ClientSession *session = m_sessions.take(reinterpret_cast<quintptr>(socket));
                m_traffic->appendInfo(QStringLiteral("客户端断开 %1:%2")
                                          .arg(session->peer).arg(session->peerPort));
                if (g_studio)
                    g_studio->log(QStringLiteral("info"), QStringLiteral("TCP 客户端 %1:%2 断开")
                                      .arg(session->peer).arg(session->peerPort));
                socket->deleteLater();
                delete session;
                updateClientsModel();
            }
        });
        connect(socket, &QTcpSocket::errorOccurred, this, [this, socket](QAbstractSocket::SocketError) {
            if (socket->error() != QAbstractSocket::RemoteHostClosedError
                && socket->error() != QAbstractSocket::UnknownSocketError) {
                m_traffic->appendInfo(QStringLiteral("客户端 %1 错误: %2")
                                          .arg(socket->peerAddress().toString(),
                                               socketErrorText(socket->error())), true);
            }
        });

        m_traffic->appendInfo(QStringLiteral("客户端接入 %1:%2").arg(session->peer).arg(session->peerPort));
        if (g_studio)
            g_studio->log(QStringLiteral("info"), QStringLiteral("TCP 客户端接入 %1:%2")
                              .arg(session->peer).arg(session->peerPort));
    }
    updateClientsModel();
}

void NetworkManager::updateClientsModel()
{
    QVariantList list;
    for (ClientSession *session : std::as_const(m_sessions)) {
        QVariantMap map;
        map.insert(QStringLiteral("id"), qint64(session->id));
        map.insert(QStringLiteral("peer"), session->peer);
        map.insert(QStringLiteral("peerPort"), session->peerPort);
        map.insert(QStringLiteral("connectedAt"),
                   session->connectedAt.toString(QStringLiteral("HH:mm:ss")));
        map.insert(QStringLiteral("rx"), qint64(session->rxBytes));
        map.insert(QStringLiteral("tx"), qint64(session->txBytes));
        list.append(map);
    }
    m_clients = list;
    emit clientsChanged();
    emit openChanged(); // 状态栏文本包含客户端计数
}

NetworkManager::ClientSession *NetworkManager::sessionById(qint64 id)
{
    return m_sessions.value(static_cast<quintptr>(id), nullptr);
}

void NetworkManager::onSocketReadyRead(QTcpSocket *socket)
{
    const QByteArray incoming = socket->readAll();
    if (incoming.isEmpty())
        return;
    m_rxBytes += incoming.size();
    m_rxWindow += incoming.size();
    if (m_sessions.contains(reinterpret_cast<quintptr>(socket))) {
        ClientSession *session = m_sessions.value(reinterpret_cast<quintptr>(socket));
        session->rxBytes += incoming.size();
        updateClientsModel();
    }
    if (g_studio)
        g_studio->addTraffic(incoming.size(), 0, 0);
    processFrames(socket, incoming);
    emit statsChanged();
}

void NetworkManager::processFrames(QTcpSocket *socket, const QByteArray &incoming)
{
    const QString peer = m_sessions.contains(reinterpret_cast<quintptr>(socket))
                             ? QStringLiteral("%1:%2")
                                   .arg(m_sessions.value(reinterpret_cast<quintptr>(socket))->peer)
                                   .arg(m_sessions.value(reinterpret_cast<quintptr>(socket))->peerPort)
                             : QStringLiteral("%1:%2").arg(socket->peerAddress().toString())
                                   .arg(socket->peerPort());

    if (m_framing == QLatin1String("raw")) {
        emitFrame(incoming, peer);
        evaluateReplyRules(incoming, peer, [this, socket](const QByteArray &reply) {
            socket->write(reply);
        });
        return;
    }
    if (m_framing == QLatin1String("line")) {
        QByteArray &buffer = m_rxBuffers[socket];
        buffer += incoming;
        int index = 0;
        while (true) {
            const int newline = buffer.indexOf('\n', index);
            if (newline < 0)
                break;
            QByteArray frame = buffer.mid(index, newline - index + 1);
            emitFrame(frame, peer);
            evaluateReplyRules(frame, peer, [this, socket](const QByteArray &reply) {
                socket->write(reply);
            });
            index = newline + 1;
        }
        buffer.remove(0, index);
        return;
    }
    if (m_framing == QLatin1String("fixed")) {
        QByteArray &buffer = m_rxBuffers[socket];
        buffer += incoming;
        while (buffer.size() >= m_fixedFrameSize) {
            const QByteArray frame = buffer.left(m_fixedFrameSize);
            emitFrame(frame, peer);
            evaluateReplyRules(frame, peer, [this, socket](const QByteArray &reply) {
                socket->write(reply);
            });
            buffer.remove(0, m_fixedFrameSize);
        }
        return;
    }

    // 空闲间隔分帧：N ms 无新数据即认为一帧结束。
    QByteArray &buffer = m_rxBuffers[socket];
    buffer += incoming;
    QTimer *timer = m_idleTimers.value(socket, nullptr);
    if (!timer) {
        timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(50);
        connect(timer, &QTimer::timeout, this, [this, socket] {
            QByteArray &pending = m_rxBuffers[socket];
            if (!pending.isEmpty()) {
                const QString idlePeer = QStringLiteral("%1:%2")
                                             .arg(socket->peerAddress().toString())
                                             .arg(socket->peerPort());
                emitFrame(pending, idlePeer);
                const QByteArray frame = pending;
                pending.clear();
                evaluateReplyRules(frame, idlePeer, [this, socket](const QByteArray &reply) {
                    socket->write(reply);
                });
            }
        });
        m_idleTimers.insert(socket, timer);
    }
    timer->start();
}

void NetworkManager::emitFrame(const QByteArray &frame, const QString &peer)
{
    m_traffic->append(TrafficModel::Rx, frame, peer);
}

void NetworkManager::evaluateReplyRules(const QByteArray &data, const QString &peer,
                                        const std::function<void(const QByteArray &)> &send)
{
    for (const QVariant &entry : std::as_const(m_replyRules)) {
        const QVariantMap rule = entry.toMap();
        if (!rule.value(QStringLiteral("enabled"), true).toBool())
            continue;
        const QString matchType = rule.value(QStringLiteral("matchType")).toString();
        const QString pattern = rule.value(QStringLiteral("pattern")).toString();
        if (pattern.isEmpty())
            continue;
        bool matched = false;
        if (matchType == QLatin1String("hexPrefix")) {
            QByteArray patternBytes;
            if (HexCodec::parseHex(pattern, patternBytes))
                matched = data.startsWith(patternBytes);
        } else if (matchType == QLatin1String("regex")) {
            matched = QString::fromUtf8(data).contains(QRegularExpression(pattern));
        } else {
            matched = QString::fromUtf8(data).contains(pattern);
        }
        if (!matched)
            continue;
        QString error;
        const QByteArray response = HexCodec::buildPayload(
            rule.value(QStringLiteral("response")).toString(),
            rule.value(QStringLiteral("responseHex"), false).toBool(), true,
            rule.value(QStringLiteral("lineEnding"), QStringLiteral("none")).toString(), &error);
        if (!response.isEmpty()) {
            m_traffic->append(TrafficModel::Tx, response,
                              QStringLiteral("自动回复 → %1").arg(peer));
            send(response);
            m_txBytes += response.size();
            if (g_studio)
                g_studio->addTraffic(0, response.size(), 0);
        }
    }
}

void NetworkManager::onUdpReadyRead()
{
    while (m_udpSocket && m_udpSocket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(int(m_udpSocket->pendingDatagramSize()));
        QHostAddress from;
        quint16 port = 0;
        m_udpSocket->readDatagram(data.data(), data.size(), &from, &port);
        handleUdpDatagram(data, from, port);
    }
}

void NetworkManager::handleUdpDatagram(const QByteArray &data, const QHostAddress &from, quint16 port)
{
    m_rxBytes += data.size();
    m_rxWindow += data.size();
    if (g_studio)
        g_studio->addTraffic(data.size(), 0, 0);
    const QString peer = QStringLiteral("%1:%2 → 本地:%3")
                             .arg(from.toString()).arg(port)
                             .arg(m_udpSocket ? m_udpSocket->localPort() : 0);
    m_traffic->append(TrafficModel::Rx, data, peer);
    evaluateReplyRules(data, peer, [this, from, port](const QByteArray &reply) {
        if (m_udpSocket)
            m_udpSocket->writeDatagram(reply, from, port);
    });
    emit statsChanged();
}

QString NetworkManager::send(const QString &text, qint64 targetClientId)
{
    if (!isOpen()) {
        emit toastRequested(QStringLiteral("网络未连接，无法发送"), false);
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
    qint64 written = 0;

    if (m_mode == QLatin1String("tcpClient") && m_tcpSocket) {
        written = m_tcpSocket->write(payload);
    } else if (m_mode == QLatin1String("tcpServer")) {
        if (targetClientId >= 0) {
            ClientSession *session = sessionById(targetClientId);
            if (session) {
                written = session->socket->write(payload);
                session->txBytes += written;
            }
        } else {
            for (ClientSession *session : std::as_const(m_sessions)) {
                written += session->socket->write(payload);
                session->txBytes += written;
            }
        }
    } else if (m_udpSocket) {
        const QHostAddress target(m_broadcast ? QHostAddress::Broadcast : QHostAddress(m_remoteHost));
        written = m_udpSocket->writeDatagram(payload, target,
                                             static_cast<quint16>(m_remotePort));
    }

    if (written < 0) {
        m_traffic->appendInfo(QStringLiteral("发送失败"), true);
        return QString();
    }
    m_txBytes += written;
    m_traffic->append(TrafficModel::Tx, payload, QStringLiteral("网络"));
    if (g_studio)
        g_studio->addTraffic(0, written, 0);
    emit statsChanged();
    return text;
}

void NetworkManager::sendTo(const QString &host, int port, const QString &text)
{
    if (!m_udpSocket || m_udpSocket->state() != QAbstractSocket::BoundState) {
        emit toastRequested(QStringLiteral("UDP 未绑定本地端口"), false);
        return;
    }
    QString error;
    const QByteArray payload = HexCodec::buildPayload(text, m_hexSend, m_escapeEnabled,
                                                      m_lineEnding, &error);
    if (payload.isEmpty() && !error.isEmpty()) {
        emit toastRequested(error, false);
        return;
    }
    if (payload.isEmpty())
        return;
    const qint64 written = m_udpSocket->writeDatagram(payload, QHostAddress(host),
                                                      static_cast<quint16>(port));
    if (written < 0) {
        m_traffic->appendInfo(QStringLiteral("发送失败: %1 → %2:%3").arg(host).arg(port), true);
        return;
    }
    m_txBytes += written;
    m_traffic->append(TrafficModel::Tx, payload, QStringLiteral("%1:%2").arg(host).arg(port));
    if (g_studio)
        g_studio->addTraffic(0, written, 0);
    emit statsChanged();
}

void NetworkManager::disconnectClient(qint64 clientId)
{
    ClientSession *session = sessionById(clientId);
    if (!session)
        return;
    m_traffic->appendInfo(QStringLiteral("断开客户端 %1:%2").arg(session->peer).arg(session->peerPort));
    session->socket->disconnectFromHost();
}

void NetworkManager::joinMulticast(const QString &group)
{
    if (!m_udpSocket || m_udpSocket->state() != QAbstractSocket::BoundState) {
        emit toastRequested(QStringLiteral("请先绑定 UDP 本地端口"), false);
        return;
    }
    if (m_udpSocket->joinMulticastGroup(QHostAddress(group))) {
        m_udpJoined = true;
        m_multicastGroup = group;
        m_traffic->appendInfo(QStringLiteral("已加入组播组 %1").arg(group));
        emit multicastGroupChanged();
        emit openChanged();
    } else {
        m_traffic->appendInfo(QStringLiteral("加入组播组失败: %1").arg(group), true);
    }
}

void NetworkManager::leaveMulticast()
{
    if (!m_udpSocket || !m_udpJoined || m_multicastGroup.isEmpty())
        return;
    m_udpSocket->leaveMulticastGroup(QHostAddress(m_multicastGroup));
    m_udpJoined = false;
    m_traffic->appendInfo(QStringLiteral("已退出组播组 %1").arg(m_multicastGroup));
    emit openChanged();
}

void NetworkManager::startTimerSend(const QString &text, int intervalMs, int repeatCount)
{
    m_timerText = text;
    m_timerRemaining = repeatCount > 0 ? repeatCount : -1;
    m_timerSend.start(qMax(10, intervalMs));
    emit timerChanged();
}

void NetworkManager::stopTimerSend()
{
    if (m_timerSend.isActive())
        m_timerSend.stop();
    m_timerRemaining = 0;
    emit timerChanged();
}
