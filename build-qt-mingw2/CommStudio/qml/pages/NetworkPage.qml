import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    signal toastRequested(string text, bool ok)

    readonly property bool connected: NetworkManager.open
    readonly property bool isTcpClient: NetworkManager.mode === "tcpClient"
    readonly property bool isTcpServer: NetworkManager.mode === "tcpServer"
    readonly property bool isUdp: NetworkManager.mode === "udp"

    property var ruleCache: [{}, {}]

    function updateRule(index, rule) {
        ruleCache[index] = rule
        NetworkManager.replyRules = ruleCache
    }

    function formatBytes(value) {
        if (value >= 1048576)
            return (value / 1048576).toFixed(2) + " MB"
        if (value >= 1024)
            return (value / 1024).toFixed(1) + " KB"
        return value + " B"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: I18n.t("Network", "网络")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.weight: Font.DemiBold
            }

            StatusPill {
                text: NetworkManager.stateText
                accent: root.connected ? Theme.success : Theme.textTertiary
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "RX " + root.formatBytes(NetworkManager.rxBytes)
                      + " (" + NetworkManager.rxRate + " B/s) · TX "
                      + root.formatBytes(NetworkManager.txBytes)
                color: Theme.textSecondary
                font.family: "Consolas"
                font.pixelSize: 11
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            // -------- 左：连接配置 --------
            AppCard {
                Layout.preferredWidth: 336
                Layout.maximumWidth: 336
                Layout.fillHeight: true
                padding: 16

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 9

                    Text {
                        text: I18n.t("Connection mode", "连接模式")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Repeater {
                            model: [
                                { label: "TCP Client", mode: "tcpClient" },
                                { label: "TCP Server", mode: "tcpServer" },
                                { label: "UDP", mode: "udp" }
                            ]

                            delegate: AppButton {
                                required property var modelData
                                compact: true
                                textPixelSize: 11
                                text: modelData.label
                                variant: NetworkManager.mode === modelData.mode ? "primary" : "default"
                                onClicked: NetworkManager.mode = modelData.mode
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    Text {
                        text: root.isTcpClient ? I18n.t("TCP client", "TCP 客户端")
                              : root.isTcpServer ? "TCP 服务端" : "UDP"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    FormRow {
                        visible: root.isTcpClient
                        label: I18n.t("Remote", "远端")
                        RowLayout {
                            spacing: 6

                            AppTextField {
                                id: remoteHostField
                                Layout.fillWidth: true
                                mono: true
                                text: NetworkManager.remoteHost
                                onEditingFinished: NetworkManager.remoteHost = text
                            }

                            AppTextField {
                                id: remotePortField
                                Layout.preferredWidth: 72
                                mono: true
                                text: NetworkManager.remotePort
                                onEditingFinished: NetworkManager.remotePort = parseInt(text) || 6000
                            }
                        }
                    }

                    FormRow {
                        visible: !root.isTcpClient
                        label: I18n.t("Local port", "本地端口")
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: NetworkManager.listenPort
                            onEditingFinished: NetworkManager.listenPort = parseInt(text) || 6000
                        }
                    }

                    FormRow {
                        label: I18n.t("Bind interface", "绑定网卡")
                        AppComboBox {
                            id: bindCombo
                            Layout.fillWidth: true
                            model: NetworkManager.interfaces
                            textRole: "label"
                            currentIndex: bindCombo.indexForName(NetworkManager.bindAddress)
                            onActivated: (index) => {
                                if (NetworkManager.interfaces.length > index)
                                    NetworkManager.bindAddress = NetworkManager.interfaces[index].name
                            }
                            function indexForName(name) {
                                const list = NetworkManager.interfaces
                                for (let i = 0; i < list.length; ++i)
                                    if (list[i].name === name)
                                        return i
                                return 0
                            }
                        }
                    }

                    AppCheckBox {
                        visible: root.isTcpClient
                        text: I18n.t("Auto reconnect after disconnect (2 s)", "断线自动重连（2 s）")
                        checked: NetworkManager.autoReconnect
                        onToggled: NetworkManager.autoReconnect = checked
                    }

                    AppCheckBox {
                        visible: root.isUdp
                        text: I18n.t("Send to broadcast address (255.255.255.255)", "发送到广播地址 (255.255.255.255)")
                        checked: NetworkManager.broadcast
                        onToggled: NetworkManager.broadcast = checked
                    }

                    RowLayout {
                        visible: root.isUdp
                        Layout.fillWidth: true
                        spacing: 6

                        AppTextField {
                            id: multicastField
                            Layout.fillWidth: true
                            mono: true
                            text: "239.1.1.1"
                            placeholder: "组播组地址"
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 11
                            text: I18n.t("Join multicast", "加入组播")
                            onClicked: NetworkManager.joinMulticast(multicastField.text)
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 11
                            text: I18n.t("Leave", "退出")
                            variant: "text"
                            onClicked: NetworkManager.leaveMulticast()
                        }
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: root.connected
                              ? (root.isTcpServer ? I18n.t("Stop listening", "停止监听") : root.isUdp ? I18n.t("Unbind port", "解绑端口") : I18n.t("Disconnect", "断开连接"))
                              : (root.isTcpServer ? I18n.t("Start listening", "开始监听") : root.isUdp ? I18n.t("Bind port", "绑定端口") : I18n.t("Connect", "连接"))
                        variant: root.connected ? "danger" : "primary"
                        onClicked: NetworkManager.openConnection()
                    }

                    // TCP 服务端客户端列表
                    ColumnLayout {
                        visible: root.isTcpServer
                        Layout.fillWidth: true
                        spacing: 6

                        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                        RowLayout {
                            Layout.fillWidth: true

                            Text {
                                text: I18n.t("Client sessions", "客户端会话")
                                color: Theme.textPrimary
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                font.weight: Font.DemiBold
                            }

                            Item { Layout.fillWidth: true }

                            AppButton {
                                compact: true
                                textPixelSize: 11
                                text: I18n.t("Disconnect selected", "断开选中")
                                variant: "danger"
                                onClicked: NetworkManager.disconnectClient(
                                               clientList.currentId)
                            }
                        }

                        ListView {
                            id: clientList
                            Layout.fillWidth: true
                            Layout.preferredHeight: Math.min(200, contentHeight)
                            clip: true
                            spacing: 2
                            model: NetworkManager.clients

                            property int currentId: -1

                            delegate: Rectangle {
                                id: clientRow
                                required property var modelData
                                required property int index
                                width: clientList.width
                                implicitHeight: 34
                                radius: Theme.radiusSmall
                                color: clientList.currentId === clientRow.modelData.id
                                       ? Theme.selectedFill
                                       : index % 2 === 0 ? "transparent"
                                                         : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.03 : 0.018)

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: clientList.currentId = clientRow.modelData.id
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 8
                                    anchors.rightMargin: 8
                                    spacing: 8

                                    Rectangle {
                                        width: 7; height: 7; radius: 4
                                        color: Theme.success
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: clientRow.modelData.peer + ":" + clientRow.modelData.peerPort
                                        color: Theme.textPrimary
                                        font.family: "Consolas"
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        text: "RX " + clientRow.modelData.rx + " · TX " + clientRow.modelData.tx
                                        color: Theme.textTertiary
                                        font.family: "Consolas"
                                        font.pixelSize: 10
                                    }
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                visible: NetworkManager.clients.length === 0
                                text: I18n.t("No clients connected", "暂无客户端接入")
                                color: Theme.textTertiary
                                font.family: Theme.fontFamily
                                font.pixelSize: 11
                            }
                        }
                    }

                    // TCP 分帧（粘包处理）
                    FormRow {
                        visible: root.isTcpClient || root.isTcpServer
                        label: I18n.t("Application framing", "应用分帧")
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["原始流", "按行 \\n", "固定长度", "空闲间隔 50ms"]
                            currentIndex: ["raw", "line", "fixed", "idle"]
                                            .indexOf(NetworkManager.framing)
                            onActivated: (index) => NetworkManager.framing =
                                             ["raw", "line", "fixed", "idle"][index]
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    Text {
                        text: I18n.t("Auto-reply rules", "自动回复规则")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    Text {
                        Layout.fillWidth: true
                        text: I18n.t("Reply automatically when received data matches a rule (useful for server-response simulation).", "收到的数据匹配规则时自动回复（可用于模拟服务器应答）。")
                        color: Theme.textTertiary
                        font.family: Theme.fontFamily
                        font.pixelSize: 10
                        wrapMode: Text.WordWrap
                    }

                    Repeater {
                        model: 2

                        delegate: ColumnLayout {
                            id: ruleCard
                            required property int index
                            Layout.fillWidth: true
                            spacing: 4

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                AppCheckBox {
                                    id: ruleEnable
                                    text: I18n.t("Rule ", "规则 ") + (ruleCard.index + 1)
                                    checked: false
                                    onToggled: ruleCard.pushRule()
                                }

                                AppComboBox {
                                    id: ruleMatch
                                    implicitWidth: 92
                                    implicitHeight: 30
                                    model: ["包含", "正则", "HEX前缀"]
                                    onActivated: ruleCard.pushRule()
                                }

                                AppTextField {
                                    id: rulePattern
                                    Layout.fillWidth: true
                                    mono: true
                                    placeholder: "匹配内容"
                                    onEditingFinished: ruleCard.pushRule()
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "→"
                                    color: Theme.textTertiary
                                    font.pixelSize: 12
                                }

                                AppComboBox {
                                    id: ruleFormat
                                    implicitWidth: 76
                                    implicitHeight: 30
                                    model: ["文本", "HEX"]
                                    onActivated: ruleCard.pushRule()
                                }

                                AppTextField {
                                    id: ruleResponse
                                    Layout.fillWidth: true
                                    mono: true
                                    placeholder: "回复内容"
                                    onEditingFinished: ruleCard.pushRule()
                                }
                            }

                            function pushRule() {
                                root.updateRule(ruleCard.index, {
                                    enabled: ruleEnable.checked,
                                    matchType: ["contains", "regex", "hexPrefix"][ruleMatch.currentIndex],
                                    pattern: rulePattern.text,
                                    response: ruleResponse.text,
                                    responseHex: ruleFormat.currentIndex === 1
                                })
                            }

                            Component.onCompleted: {
                                const rule = root.ruleCache[ruleCard.index]
                                if (rule) {
                                    ruleEnable.checked = rule.enabled === true
                                    ruleMatch.currentIndex =
                                            ["contains", "regex", "hexPrefix"].indexOf(rule.matchType)
                                    rulePattern.text = rule.pattern || ""
                                    ruleResponse.text = rule.response || ""
                                    ruleFormat.currentIndex = rule.responseHex ? 1 : 0
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // -------- 右：终端 + 发送 --------
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12

                AppCard {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    padding: 0

                    TerminalView {
                        anchors.fill: parent
                        model: NetworkManager.traffic
                        emptyHint: I18n.t("Waiting for network data · TCP retains the source address; UDP retains peer address and port", "等待网络数据 · TCP 保留来源地址，UDP 保留对端地址与端口")
                        onToastRequested: (text, ok) => root.toastRequested(text, ok)
                    }
                }

                AppCard {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 176
                    padding: 0

                    SendPanel {
                        anchors.fill: parent
                        manager: NetworkManager
                        showFile: false
                        sendLabel: root.isTcpServer ? I18n.t("Send to selected / all", "发送到选中 / 全部") : I18n.t("Send", "发送")
                        hint: root.isUdp ? I18n.t("UDP sends to the remote address", "UDP 按远端地址发送") : I18n.t("TCP client unicast · server can multicast", "TCP Client 单播 · Server 可多播")
                        onToastRequested: (text, ok) => root.toastRequested(text, ok)
                    }
                }
            }
        }
    }
}
