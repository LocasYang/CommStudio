import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import CommStudio 1.0

// 通用发送面板：格式（文本/HEX）、行尾追加、转义开关、发送历史（↑/↓）、
// 定时发送（间隔/次数，0 = 无限循环）、可选文件发送（串口助手）。
Item {
    id: root

    property var manager            // 须提供 send(text) / hexSend / lineEnding / escapeEnabled /
                                    // timerActive / startTimerSend / stopTimerSend
    property bool showFile: false
    property string sendLabel: "发送"
    property string hint: "Ctrl+Enter 快速发送"

    signal toastRequested(string text, bool ok)

    property var history: []
    property int historyIndex: -1

    readonly property bool hasTimer: manager && manager.timerActive !== undefined
    readonly property bool hasFile: manager && manager.fileActive !== undefined

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
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.topMargin: 10
            spacing: 8

            Text {
                text: "SEND"
                color: Theme.textPrimary
                font.family: "Consolas"
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }

            Text {
                text: formatCombo.currentIndex === 1 ? "HEX" : "TEXT"
                color: Theme.cyan
                font.family: "Consolas"
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }

            Text {
                text: "APPEND " + (lineEndingCombo.currentIndex === 0 ? "NONE"
                      : lineEndingCombo.currentIndex === 1 ? "CR"
                      : lineEndingCombo.currentIndex === 2 ? "LF"
                      : lineEndingCombo.currentIndex === 3 ? "CRLF" : "LFCR")
                color: Theme.warning
                font.family: "Consolas"
                font.pixelSize: 10
            }

            Item { Layout.fillWidth: true }

            Text {
                text: root.hint + "  ·  BUFFERED INPUT"
                color: Theme.textTertiary
                font.family: "Consolas"
                font.pixelSize: 10
            }
        }

        // 编辑与主发送按钮
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            spacing: 8

            AppComboBox {
                id: formatCombo
                implicitWidth: 92
                implicitHeight: 36
                model: ["TEXT", "HEX"]
                currentIndex: manager && manager.hexSend ? 1 : 0
                onActivated: (index) => { if (manager) manager.hexSend = (index === 1) }
            }

            AppComboBox {
                id: lineEndingCombo
                implicitWidth: 150
                implicitHeight: 36
                model: ["APPEND: NONE", "APPEND: CR (\\r)", "APPEND: LF (\\n)",
                        "APPEND: CRLF (\\r\\n)", "APPEND: LFCR (\\n\\r)"]
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
                Component.onCompleted: {
                    // manager 在实例化后才赋值时，重新同步显示
                }
            }

            AppTextField {
                id: sendField
                Layout.fillWidth: true
                Layout.minimumWidth: 120
                mono: true
                placeholder: formatCombo.currentIndex === 1 ? "AA 55 01 02 …（HEX）"
                                                            : "Input data...  支持 \\r  \\n  \\xNN"
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
                text: root.sendLabel === "发送" ? "SEND" : "SEND (OFFLINE)"
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
            spacing: 16

            AppCheckBox {
                id: escapeCheck
                text: "ESCAPE  \\r  \\n  \\xNN"
                checked: manager ? manager.escapeEnabled : true
                onToggled: if (manager) manager.escapeEnabled = checked
            }

            AppCheckBox {
                id: clearAfterCheck
                text: "发送后清空"
            }

            Item { Layout.fillWidth: true }

            // 定时发送
            AppTextField {
                id: intervalField
                implicitWidth: 76
                text: "1000"
                placeholder: "interval ms"
                mono: true
            }

            AppTextField {
                id: repeatField
                implicitWidth: 64
                text: "0"
                placeholder: "count"
                mono: true
            }

            AppButton {
                compact: true
                textPixelSize: 12
                visible: root.hasTimer ? !manager.timerActive : false
                text: "定时发送"
                variant: "outlined"
                onClicked: {
                    const interval = parseInt(intervalField.text)
                    if (isNaN(interval) || interval < 10) {
                        root.toastRequested("定时间隔至少 10 ms", false)
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
                      ? "停止 (" + manager.timerRemaining + ")"
                      : "停止定时"
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
                placeholder: "选择要发送的文件（原始分块发送）"
            }

            AppButton {
                compact: true
                textPixelSize: 12
                text: "浏览…"
                enabled: root.hasFile ? !manager.fileActive : false
                onClicked: fileDialog.open()
            }

            AppButton {
                id: sendFileButton
                compact: true
                textPixelSize: 12
                text: "发送文件"
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
                text: "取消"
                variant: "danger"
                onClicked: manager.cancelFileSend()
            }

            StatusPill {
                visible: root.hasFile && manager.fileActive
                text: manager && manager.fileActive
                      ? "发送中 " + manager.fileProgress + "% · " + manager.fileRate : ""
                accent: Theme.primary
                dot: false
                scaleFactor: 0.85
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "选择要发送的文件"
        nameFilters: ["所有文件 (*)"]
        onAccepted: {
            filePathField.text = selectedFile.toString().replace(/^file:\/\//, "")
            sendFileButton.fileUrl = selectedFile
        }
    }
}
