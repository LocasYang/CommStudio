import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import CommStudio 1.0

// 通用发送面板：格式（文本/HEX）、行尾追加、转义开关、发送历史（↑/↓）、
// 定时发送（间隔/次数，0 = 无限循环）、可选文件发送（串口助手）。
Item {
    id: root
    objectName: "sendPanelRoot"

    property var manager            // 须提供 send(text) / hexSend / lineEnding / escapeEnabled /
                                    // timerActive / startTimerSend / stopTimerSend
    property bool showFile: false
    property string sendLabel: I18n.t("Send", "发送")
    property string hint: I18n.t("Ctrl+Enter quick send", "Ctrl+Enter 快速发送")

    signal toastRequested(string text, bool ok)

    property var history: []
    property int historyIndex: -1

    readonly property bool hasTimer: manager && manager.timerActive !== undefined
    readonly property bool hasFile: manager && manager.fileActive !== undefined
    readonly property bool compactLayout: width < 760
    readonly property var lineEndingLabels: [
        I18n.t("None", "不追加"),
        I18n.t("CR (\\r)", "回车 (CR)"),
        I18n.t("LF (\\n)", "换行 (LF)"),
        I18n.t("CRLF (\\r\\n)", "回车换行 (CRLF)"),
        I18n.t("LFCR (\\n\\r)", "换行回车 (LFCR)")
    ]
    readonly property var lineEndingBytes: ["", "0D", "0A", "0D 0A", "0A 0D"]
    readonly property string lineEndingSummary: lineEndingLabels[lineEndingCombo.currentIndex]
        + (lineEndingBytes[lineEndingCombo.currentIndex]
           ? " · " + lineEndingBytes[lineEndingCombo.currentIndex] : "")
    readonly property string lineEndingHint: I18n.t("Append to each message: ", "每次发送末尾追加：")
        + lineEndingSummary

    function doSend() {
        if (!manager || sendField.text.length === 0)
            return
        const sent = manager.send(sendField.text)
        if (sent !== undefined && sent !== null && sent !== "") {
            root.history.unshift(sendField.text)
            if (root.history.length > 50)
                root.history.pop()
            root.historyIndex = -1
            if (clearAfterCheck.checked)
                sendField.text = ""
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: root.compactLayout ? 4 : 8

        RowLayout {
            id: sendHeader
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.topMargin: root.compactLayout ? 8 : 10
            spacing: 8

            Text {
                text: I18n.t("SEND", "发送")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }

            Text {
                visible: !root.compactLayout
                text: formatCombo.currentIndex === 1 ? I18n.t("HEX", "十六进制") : I18n.t("TEXT", "文本")
                color: Theme.cyan
                font.family: Theme.fontFamily
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }

            Text {
                visible: !root.compactLayout
                text: I18n.t("Append: ", "追加：") + root.lineEndingSummary
                color: Theme.warning
                font.family: Theme.fontFamily
                font.pixelSize: 10
            }

            Item { Layout.fillWidth: true }

            Text {
                visible: !root.compactLayout
                Layout.maximumWidth: Math.max(100, root.width - 400)
                text: root.hint
                color: Theme.textTertiary
                font.family: Theme.fontFamily
                font.pixelSize: 10
                elide: Text.ElideRight
            }

            Item {
                id: headerSelectorsSlot
                visible: root.compactLayout
                Layout.minimumWidth: 280
                Layout.preferredWidth: 280
                Layout.maximumWidth: 280
                Layout.preferredHeight: 32
            }
        }

        // 编辑与主发送按钮
        RowLayout {
            id: sendRow
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            spacing: 8

            Item {
                id: editorSelectorsSlot
                visible: !root.compactLayout
                Layout.minimumWidth: 280
                Layout.preferredWidth: 280
                Layout.maximumWidth: 280
                Layout.preferredHeight: 36

                RowLayout {
                    // Keep stable slots in both rows when the window is resized.
                    parent: root.compactLayout ? headerSelectorsSlot : editorSelectorsSlot
                    anchors.fill: parent
                    spacing: 8

                    AppComboBox {
                        id: formatCombo
                        Layout.minimumWidth: 104
                        Layout.preferredWidth: 104
                        Layout.maximumWidth: 104
                        implicitHeight: root.compactLayout ? 32 : 36
                        leftPadding: 10; rightPadding: 34
                        font.pixelSize: 12
                        model: [I18n.t("Text", "文本"), I18n.t("Hex", "十六进制")]
                        Accessible.name: I18n.t("Send format", "发送格式")
                        currentIndex: manager && manager.hexSend ? 1 : 0
                        onActivated: (index) => { if (manager) manager.hexSend = (index === 1) }
                    }

                    AppComboBox {
                        id: lineEndingCombo
                        objectName: "sendLineEndingCombo"
                        Layout.minimumWidth: 168
                        Layout.preferredWidth: 168
                        Layout.maximumWidth: 168
                        implicitHeight: root.compactLayout ? 32 : 36
                        leftPadding: 10; rightPadding: 34
                        font.pixelSize: 12
                        model: root.lineEndingLabels
                        Accessible.name: I18n.t("Line ending", "追加结束符")
                        ToolTip.visible: hovered
                        ToolTip.delay: 400
                        ToolTip.text: root.lineEndingHint
                        currentIndex: {
                            if (!manager)
                                return 0
                            if (manager.lineEnding === "cr") return 1
                            if (manager.lineEnding === "lf") return 2
                            if (manager.lineEnding === "crlf") return 3
                            if (manager.lineEnding === "lfcr") return 4
                            return 0
                        }
                        onActivated: (index) => {
                            if (!manager)
                                return
                            manager.lineEnding = index === 1 ? "cr" : index === 2 ? "lf"
                                                : index === 3 ? "crlf" : index === 4 ? "lfcr" : "none"
                        }
                    }
                }
            }

            AppTextField {
                id: sendField
                objectName: "sendInput"
                Layout.fillWidth: true
                Layout.minimumWidth: 100
                mono: true
                placeholder: formatCombo.currentIndex === 1
                    ? I18n.t("Hex bytes, e.g. AA 55 01 02", "十六进制字节，如 AA 55 01 02")
                    : I18n.t("Enter text to send", "输入要发送的文本")
                Accessible.name: I18n.t("Send content", "发送内容")
                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: root.hint + "\n" + root.lineEndingHint
                Keys.onEnterPressed: root.doSend()
                Keys.onReturnPressed: root.doSend()
                Keys.onUpPressed: {
                    if (root.history.length === 0)
                        return
                    root.historyIndex = Math.min(root.historyIndex + 1, root.history.length - 1)
                    sendField.text = root.history[root.historyIndex]
                }
                Keys.onDownPressed: {
                    if (root.historyIndex <= 0) {
                        root.historyIndex = -1
                        sendField.text = ""
                        return
                    }
                    root.historyIndex--
                    sendField.text = root.history[root.historyIndex]
                }
            }

            AppButton {
                compact: true
                textPixelSize: 12
                text: manager && manager.open !== undefined && !manager.open
                      ? I18n.t("SEND (OFFLINE)", "发送（未连接）") : root.sendLabel
                variant: "primary"
                enabled: manager ? (manager.open === undefined ? true : manager.open) : false
                onClicked: root.doSend()
            }

        }

        // 选项行
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.bottomMargin: root.showFile ? 0 : 10
            spacing: root.compactLayout ? 6 : 16

            AppCheckBox {
                id: escapeCheck
                text: root.compactLayout ? I18n.t("Escapes", "转义字符")
                      : I18n.t("Escapes  \\r  \\n  \\xNN", "转义字符  \\r  \\n  \\xNN")
                font.pixelSize: root.compactLayout ? 11 : 12
                checked: manager ? manager.escapeEnabled : true
                onToggled: if (manager) manager.escapeEnabled = checked
                ToolTip.visible: hovered
                ToolTip.delay: 400
                ToolTip.text: I18n.t("In text mode, interpret \\r as CR (0D), \\n as LF (0A), and \\xNN as a hexadecimal byte.",
                                    "文本模式下，将 \\r 转为回车 (0D)、\\n 转为换行 (0A)、\\xNN 转为对应的十六进制字节。")
            }

            AppCheckBox {
                id: clearAfterCheck
                text: I18n.t("Clear after send", "发送后清空")
                font.pixelSize: root.compactLayout ? 11 : 12
            }

            Item { Layout.fillWidth: true }

            // 定时发送
            AppTextField {
                id: intervalField
                visible: root.hasTimer
                implicitWidth: root.compactLayout ? 60 : 76
                text: "1000"
                placeholder: I18n.t("ms", "毫秒")
                mono: true
                Accessible.name: I18n.t("Send interval in milliseconds", "发送间隔（毫秒）")
                ToolTip.visible: hovered
                ToolTip.text: Accessible.name
            }

            AppTextField {
                id: repeatField
                visible: root.hasTimer
                implicitWidth: root.compactLayout ? 50 : 64
                text: "0"
                placeholder: I18n.t("Count", "次数")
                mono: true
                Accessible.name: I18n.t("Repeat count; 0 means unlimited", "发送次数；0 表示不限次数")
                ToolTip.visible: hovered
                ToolTip.text: Accessible.name
            }

            AppButton {
                compact: true
                textPixelSize: 12
                visible: root.hasTimer ? !manager.timerActive : false
                text: I18n.t("Timed send", "定时发送")
                variant: "outlined"
                onClicked: {
                    const interval = parseInt(intervalField.text)
                    if (isNaN(interval) || interval < 10) {
                        root.toastRequested(I18n.t("Timer interval must be at least 10 ms", "定时间隔至少 10 ms"), false)
                        return
                    }
                    const repeat = parseInt(repeatField.text) || 0
                    manager.startTimerSend(sendField.text, interval, repeat)
                }
            }

            AppButton {
                compact: true
                textPixelSize: 12
                visible: root.hasTimer && manager.timerActive
                text: manager && manager.timerRemaining > 0
                      ? I18n.t("Stop (", "停止 (") + manager.timerRemaining + ")"
                      : I18n.t("Stop timer", "停止定时")
                variant: "danger"
                onClicked: manager.stopTimerSend()
            }
        }

        // 文件发送（仅串口）
        RowLayout {
            visible: root.showFile
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.bottomMargin: 10
            spacing: 8

            AppTextField {
                id: filePathField
                Layout.fillWidth: true
                readOnly: true
                placeholder: I18n.t("Choose a file to send (raw chunks)", "选择要发送的文件（原始分块发送）")
            }

            AppButton {
                compact: true
                textPixelSize: 12
                text: I18n.t("Browse…", "浏览…")
                enabled: root.hasFile ? !manager.fileActive : false
                onClicked: fileDialog.open()
            }

            AppButton {
                id: sendFileButton
                compact: true
                textPixelSize: 12
                text: I18n.t("Send file", "发送文件")
                variant: "outlined"
                accent: Theme.cyan
                enabled: root.hasFile ? (filePathField.text.length > 0 && !manager.fileActive) : false
                onClicked: manager.sendFile(sendFileButton.fileUrl, 4096, 0)
                property var fileUrl: ""
            }

            AppButton {
                compact: true
                textPixelSize: 12
                visible: root.hasFile && manager.fileActive
                text: I18n.t("Cancel", "取消")
                variant: "danger"
                onClicked: manager.cancelFileSend()
            }

            StatusPill {
                visible: root.hasFile && manager.fileActive
                text: manager && manager.fileActive
                      ? I18n.t("Sending ", "发送中 ") + manager.fileProgress + "% · " + manager.fileRate : ""
                accent: Theme.primary
                dot: false
                scaleFactor: 0.85
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: I18n.t("Choose a file to send", "选择要发送的文件")
        nameFilters: [I18n.t("All files (*)", "所有文件 (*)")]
        onAccepted: {
            filePathField.text = selectedFile.toString().replace(/^file:\/\//, "")
            sendFileButton.fileUrl = selectedFile
        }
    }
}
