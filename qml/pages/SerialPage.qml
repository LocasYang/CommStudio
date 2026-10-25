import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    signal toastRequested(string text, bool ok)

    readonly property bool connected: SerialManager.open
    readonly property bool configLocked: SerialManager.configLocked

    function selectedPortDetails() {
        const infos = SerialManager.portInfos
        for (let i = 0; i < infos.length; ++i) {
            if (infos[i].name === SerialManager.portName) {
                let text = infos[i].description || "未知设备"
                if (infos[i].manufacturer)
                    text += " · " + infos[i].manufacturer
                if (infos[i].vid && infos[i].pid)
                    text += " · VID:PID " + Number(infos[i].vid).toString(16).toUpperCase().padStart(4, "0")
                           + ":" + Number(infos[i].pid).toString(16).toUpperCase().padStart(4, "0")
                return text
            }
        }
        return "请选择一个可用串口"
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
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: I18n.t("Serial", "串口调试")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 19
                font.weight: Font.DemiBold
            }

            StatusPill {
                text: SerialManager.reconnecting ? I18n.t("Reconnecting", "重连中")
                                                   : root.connected ? I18n.t("Connected", "已连接") : I18n.t("Disconnected", "未连接")
                accent: SerialManager.reconnecting ? Theme.warning
                                                    : root.connected ? Theme.success : Theme.textTertiary
            }

            StatusPill {
                text: root.configLocked ? I18n.t("🔒 Configuration locked", "🔒 配置已锁定")
                                        : I18n.t("🔓 Configuration editable", "🔓 配置可修改")
                accent: root.configLocked ? Theme.warning : Theme.textSecondary
                dot: false
                scaleFactor: 0.86
            }

            StatusPill {
                visible: SerialManager.timerActive
                text: I18n.t("Timed send active", "定时发送中")
                accent: Theme.warning
                dot: false
                scaleFactor: 0.9
            }

            Item { Layout.fillWidth: true }

            Text {
                text: SerialManager.configSummary + "    ·    RX " + root.formatBytes(SerialManager.rxBytes)
                      + "  TX " + root.formatBytes(SerialManager.txBytes)
                color: Theme.textSecondary
                font.family: "Consolas"
                font.pixelSize: 11
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            // -------- 左：连接配置 --------
            AppCard {
                Layout.preferredWidth: 292
                Layout.maximumWidth: 320
                Layout.fillHeight: true
                padding: 12
                cornerRadius: 7

                ScrollView {
                    id: configurationScroll
                    anchors.fill: parent
                    clip: true
                    contentWidth: availableWidth
                    contentHeight: configurationColumn.implicitHeight

                ColumnLayout {
                    id: configurationColumn
                    width: configurationScroll.availableWidth
                    spacing: 7

                    Text {
                        text: I18n.t("Serial configuration", "串口配置")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    Text {
                        Layout.fillWidth: true
                        text: SerialManager.configSummary
                        color: Theme.textSecondary
                        font.family: "Consolas"
                        font.pixelSize: 10
                        elide: Text.ElideRight
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        AppComboBox {
                            id: portCombo
                            enabled: !root.configLocked
                            Layout.fillWidth: true
                            model: SerialManager.portInfos
                            textRole: "label"
                            currentIndex: portCombo.indexForName(SerialManager.portName)
                            onActivated: (index) => {
                                if (SerialManager.portInfos.length > index)
                                    SerialManager.portName = SerialManager.portInfos[index].name
                            }
                            function indexForName(name) {
                                const infos = SerialManager.portInfos
                                for (let i = 0; i < infos.length; ++i)
                                    if (infos[i].name === name)
                                        return i
                                return -1
                            }
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 12
                            text: I18n.t("Refresh", "刷新")
                            enabled: !root.configLocked
                            onClicked: {
                                SerialManager.refreshPorts()
                                root.toastRequested(I18n.t("Port list refreshed (hot-plug detection supported)", "已刷新端口列表（支持热插拔自动检测）"), true)
                            }
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: root.selectedPortDetails()
                        color: Theme.textTertiary
                        font.family: Theme.fontFamily
                        font.pixelSize: 10
                        elide: Text.ElideRight
                    }

                    FormRow {
                        label: I18n.t("Baud rate", "波特率")
                        AppComboBox {
                            id: baudCombo
                            enabled: !root.configLocked
                            Layout.fillWidth: true
                            model: ["9600", "19200", "38400", "57600", "115200",
                                    "230400", "460800", "921600", I18n.t("Custom", "自定义")]
                            currentIndex: baudCombo.indexForRate(SerialManager.baudRate)
                            onActivated: (index) => {
                                if (index === 8)
                                    return
                                SerialManager.baudRate = baudCombo.model[index]
                            }
                            function indexForRate(rate) {
                                const list = ["9600", "19200", "38400", "57600", "115200",
                                              "230400", "460800", "921600"]
                                const idx = list.indexOf(rate)
                                return idx >= 0 ? idx : 8
                            }
                        }
                    }

                    FormRow {
                        visible: baudCombo.currentIndex === 8
                        label: I18n.t("Custom", "自定义")
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            enabled: !root.configLocked
                            text: SerialManager.baudRate
                            onEditingFinished: SerialManager.baudRate = text
                        }
                    }

                    FormRow {
                        label: I18n.t("Data bits", "数据位")
                        AppComboBox {
                            enabled: !root.configLocked
                            Layout.fillWidth: true
                            model: ["5", "6", "7", "8"]
                            currentIndex: [5, 6, 7, 8].indexOf(SerialManager.dataBits)
                            onActivated: (index) => SerialManager.dataBits = [5, 6, 7, 8][index]
                        }
                    }

                    FormRow {
                        label: I18n.t("Parity", "校验位")
                        AppComboBox {
                            enabled: !root.configLocked
                            Layout.fillWidth: true
                            model: [I18n.t("None", "无"), I18n.t("Even", "偶校验"),
                                    I18n.t("Odd", "奇校验"), I18n.t("Mark", "标记校验"),
                                    I18n.t("Space", "空格校验")]
                            currentIndex: ["none", "even", "odd", "mark", "space"]
                                            .indexOf(SerialManager.parity)
                            onActivated: (index) => SerialManager.parity =
                                             ["none", "even", "odd", "mark", "space"][index]
                        }
                    }

                    FormRow {
                        label: I18n.t("Stop bits", "停止位")
                        AppComboBox {
                            enabled: !root.configLocked
                            Layout.fillWidth: true
                            model: ["1", "1.5", "2"]
                            currentIndex: [1, 1.5, 2].indexOf(SerialManager.stopBits)
                            onActivated: (index) => SerialManager.stopBits = [1, 1.5, 2][index]
                        }
                    }

                    FormRow {
                        label: I18n.t("Flow control", "流控")
                        AppComboBox {
                            enabled: !root.configLocked
                            Layout.fillWidth: true
                            model: [I18n.t("None", "无"), "RTS/CTS", "XON/XOFF"]
                            currentIndex: ["none", "rtscts", "xonxoff"].indexOf(SerialManager.flowControl)
                            onActivated: (index) => SerialManager.flowControl =
                                             ["none", "rtscts", "xonxoff"][index]
                        }
                    }

                    AppCheckBox {
                        Layout.fillWidth: true
                        text: I18n.t("Auto reconnect after disconnect (0.5s → 5s backoff)", "断开后自动重连（0.5s → 5s 退避）")
                        checked: SerialManager.autoReconnect
                        onToggled: SerialManager.autoReconnect = checked
                    }

                    Text {
                        Layout.fillWidth: true
                        visible: root.configLocked
                        text: I18n.t("Communication parameters are read-only while connected.", "连接期间通信参数只读，断开后可修改")
                        color: Theme.warning
                        font.family: Theme.fontFamily
                        font.pixelSize: 10
                        wrapMode: Text.WordWrap
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: root.connected ? I18n.t("Disconnect", "断开连接") : I18n.t("Open serial port", "打开串口")
                        variant: root.connected ? "danger" : "primary"
                        onClicked: SerialManager.openConnection()
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    Text {
                        text: I18n.t("Control lines / pins", "控制线 / 引脚")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppCheckBox {
                            Layout.alignment: Qt.AlignVCenter
                            text: "DTR"
                            accent: Theme.cyan
                            enabled: root.connected
                            checked: SerialManager.dtr
                            onToggled: SerialManager.dtr = checked
                        }

                        AppCheckBox {
                            Layout.alignment: Qt.AlignVCenter
                            text: "RTS"
                            accent: Theme.cyan
                            enabled: root.connected
                            checked: SerialManager.rts
                            onToggled: SerialManager.rts = checked
                        }

                        Item { Layout.fillWidth: true }

                        AppButton {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredHeight: 30
                            compact: true
                            textPixelSize: 12
                            text: I18n.t("Break", "中断信号")
                            enabled: root.connected
                            onClicked: SerialManager.sendBreak(100)
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        StatusPill { text: "CTS"; accent: SerialManager.pinCts ? Theme.success : Theme.textTertiary; dot: false; scaleFactor: 0.82 }
                        StatusPill { text: "DSR"; accent: SerialManager.pinDsr ? Theme.success : Theme.textTertiary; dot: false; scaleFactor: 0.82 }
                        StatusPill { text: "DCD"; accent: SerialManager.pinDcd ? Theme.success : Theme.textTertiary; dot: false; scaleFactor: 0.82 }
                        StatusPill { text: "RI"; accent: SerialManager.pinRi ? Theme.success : Theme.textTertiary; dot: false; scaleFactor: 0.82 }
                    }

                    Item { Layout.preferredHeight: 12 }

                    Text {
                        Layout.fillWidth: true
                        text: I18n.t("The send panel supports HEX/text and escape sequences; files are sent as raw 4 KB chunks.", "发送区支持 HEX/文本与转义序列；文件以 4 KB 分块原始发送。")
                        color: Theme.textTertiary
                        font.family: Theme.fontFamily
                        font.pixelSize: 10
                        wrapMode: Text.WordWrap
                        lineHeight: 1.3
                    }
                }
                }
            }

            // -------- 右：终端 + 发送 --------
            ColumnLayout {
                Layout.fillWidth: true
                        Layout.fillHeight: true
                spacing: 10

                AppCard {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    padding: 0
                    cornerRadius: 7

                    TerminalView {
                        objectName: "serialTerminal"
                        anchors.fill: parent
                        model: SerialManager.traffic
                        displayMode: 0
                        allowHideSent: true
                        showSentMessages: true
                        emptyHint: root.connected
                                   ? SerialManager.configSummary + "\n\n等待串口数据 · RX 0 B"
                                   : I18n.t("Serial port is not connected\n\nChoose a port and select Open serial port.", "尚未连接串口\n\n请选择端口并点击“打开串口”")
                        onToastRequested: (text, ok) => root.toastRequested(text, ok)
                    }
                }

                AppCard {
                    Layout.fillWidth: true
                    // SendPanel 在串口页额外显示文件发送行；154 px 只能容纳
                    // 常规三行布局，第四行会越过卡片底部。
                    Layout.minimumHeight: 196
                    Layout.preferredHeight: 196
                    padding: 0
                    cornerRadius: 7

                    SendPanel {
                        anchors.fill: parent
                        manager: SerialManager
                        showFile: true
                        sendLabel: root.connected ? I18n.t("Send", "发送") : I18n.t("Send (offline)", "发送（未连接）")
                        hint: I18n.t("Ctrl+Enter quick send · ↑/↓ history", "Ctrl+Enter 快速发送 · ↑/↓ 切换历史")
                        onToastRequested: (text, ok) => root.toastRequested(text, ok)
                    }
                }
            }
        }
    }
}
