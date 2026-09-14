import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CommStudio 1.0

// Dense traffic view inspired by TuiSerial's stable timestamp/direction/size
// columns. The model keeps raw bytes; this component only changes presentation.
Item {
    id: root

    property var model
    property string emptyHint: I18n.t("Waiting for data · received and transmitted traffic appears here after connecting", "等待数据 · 连接后收发的数据会显示在这里")
    property int displayMode: 2 // 0 TEXT / 1 HEX / 2 HEX+ASCII
    property bool autoScroll: true
    property bool followTail: true
    property int unseenCount: 0
    property bool tailScrollQueued: false

    // rowsInserted 时 ListView 的 contentHeight 可能尚未完成重算；延迟到下一轮
    // 布局后再定位，才能稳定贴住最新一行而不是停留在旧的滚动位置。
    function scheduleTailScroll() {
        if (tailScrollQueued || !followTail || !autoScroll)
            return
        tailScrollQueued = true
        Qt.callLater(function() {
            tailScrollQueued = false
            if (!followTail || !autoScroll)
                return
            trafficList.forceLayout()
            trafficList.positionViewAtEnd()
        })
    }

    function dirAccent(dir) {
        if (dir === 0) return Theme.rx
        if (dir === 1) return Theme.tx
        if (dir === 3) return Theme.error
        return Theme.warning
    }

    function dirLabel(dir) {
        if (dir === 0) return "◄ RX"
        if (dir === 1) return "► TX"
        if (dir === 3) return "! ERR"
        return "· INF"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 10
            Layout.topMargin: 8
            Layout.bottomMargin: 7
            spacing: 8

            Text {
                text: I18n.t("MESSAGE", "消息")
                color: Theme.textPrimary
                font.family: "Consolas"
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }

            Text {
                text: root.displayMode === 0 ? "TEXT" : root.displayMode === 1 ? "HEX" : "HEX + ASCII"
                color: Theme.cyan
                font.family: "Consolas"
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }

            StatusPill {
                text: root.model ? I18n.count(root.model.count, "items", " 条") : I18n.t("0 items", "0 条")
                accent: Theme.primary
                dot: false
                scaleFactor: 0.82
            }

            Text {
                visible: root.model && root.model.paused
                text: I18n.t("PAUSED · RECEIVING IN BACKGROUND", "已暂停 · 后台仍在接收")
                color: Theme.warning
                font.family: "Consolas"
                font.pixelSize: 10
            }

            Item { Layout.fillWidth: true }

            AppComboBox {
                implicitWidth: 122
                implicitHeight: 30
                font.family: "Consolas"
                font.pixelSize: 11
                model: ["TEXT", "HEX", "HEX+ASCII"]
                currentIndex: root.displayMode
                onActivated: (index) => root.displayMode = index
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: root.model && root.model.paused ? I18n.t("Resume", "继续") : I18n.t("Pause", "暂停")
                accent: root.model && root.model.paused ? Theme.warning : Theme.primary
                variant: "text"
                onClicked: if (root.model) root.model.paused = !root.model.paused
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: I18n.t("Clear", "清屏")
                variant: "text"
                onClicked: if (root.model) root.model.clear()
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: I18n.t("Export", "导出")
                variant: "text"
                onClicked: exportDialog.open()
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: root.followTail ? I18n.t("Follow tail", "自动跟随") : I18n.t("Manual browse", "手动浏览")
                accent: root.followTail ? Theme.success : Theme.warning
                variant: "text"
                onClicked: {
                    root.followTail = !root.followTail
                    if (root.followTail) {
                        root.unseenCount = 0
                        root.scheduleTailScroll()
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

        ListView {
            id: trafficList
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 5
            clip: true
            spacing: 0
            model: root.model

            ScrollBar.vertical: ScrollBar { }

            onCountChanged: {
                if (root.followTail && root.autoScroll)
                    root.scheduleTailScroll()
                else if (!atYEnd)
                    root.unseenCount = Math.min(999, root.unseenCount + 1)
            }
            onContentHeightChanged: {
                if (count > 0)
                    root.scheduleTailScroll()
            }
            onMovementStarted: if (!atYEnd) root.followTail = false
            onMovementEnded: if (atYEnd) { root.followTail = true; root.unseenCount = 0 }

            delegate: Rectangle {
                id: row
                required property int index
                required property string time
                required property int dir
                required property string dataHex
                required property string dataText
                required property string peer
                required property int size
                required property bool plain
                width: trafficList.width
                height: Math.max(25, dataTextItem.implicitHeight + 8)
                color: index % 2 === 0 ? "transparent" : Theme.alpha(Theme.textPrimary, 0.025)

                Rectangle {
                    width: 2
                    height: parent.height
                    color: root.dirAccent(row.dir)
                    opacity: row.dir === 2 ? 0.55 : 0.95
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 9
                    anchors.rightMargin: 8
                    spacing: 9

                    Text {
                        Layout.preferredWidth: 82
                        text: row.time
                        color: Theme.timestamp
                        font.family: "Consolas"
                        font.pixelSize: 11
                    }

                    Text {
                        Layout.preferredWidth: 48
                        text: root.dirLabel(row.dir)
                        color: root.dirAccent(row.dir)
                        font.family: "Consolas"
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                    }

                    Text {
                        Layout.preferredWidth: 52
                        text: row.size + " B"
                        color: Theme.warning
                        font.family: "Consolas"
                        font.pixelSize: 10
                    }

                    Text {
                        id: dataTextItem
                        Layout.fillWidth: true
                        text: {
                            if (row.plain)
                                return row.dataText
                            if (root.displayMode === 0)
                                return row.dataText
                            if (root.displayMode === 1)
                                return row.dataHex
                            return row.dataHex + "  |  " + row.dataText
                        }
                        color: row.plain && row.dir === 3 ? Theme.error
                               : row.plain ? Theme.textSecondary : Theme.payload
                        font.family: "Consolas"
                        font.pixelSize: 12
                        wrapMode: Text.WrapAnywhere
                        textFormat: Text.PlainText
                    }

                    Text {
                        visible: row.peer.length > 0
                        text: row.peer
                        color: Theme.textTertiary
                        font.family: "Consolas"
                        font.pixelSize: 10
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: root.model && root.model.count === 0
                text: root.emptyHint + "\n\n" + I18n.t("Ctrl+Enter send · ↑/↓ history · F1 shortcuts", "Ctrl+Enter 发送 · ↑/↓ 浏览历史 · F1 查看快捷键")
                color: Theme.textTertiary
                font.family: "Consolas"
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                visible: root.unseenCount > 0
                anchors.right: parent.right
                anchors.rightMargin: 14
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 12
                width: newMessagesText.implicitWidth + 22
                height: 28
                color: Theme.selectedFill
                border.width: 1
                border.color: Theme.primary

                Text {
                    id: newMessagesText
                    anchors.centerIn: parent
                    text: "↓ " + (root.unseenCount >= 999 ? "999+" : root.unseenCount)
                          + I18n.t(" new messages", " 条新消息")
                    color: Theme.textPrimary
                    font.family: "Consolas"
                    font.pixelSize: 11
                    font.weight: Font.DemiBold
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.followTail = true
                        root.unseenCount = 0
                        trafficList.positionViewAtEnd()
                    }
                }
            }
        }
    }

    FileDialog {
        id: exportDialog
        title: I18n.t("Export traffic log", "导出收发记录")
        nameFilters: [I18n.t("Text files (*.txt *.log *.csv)", "文本文件 (*.txt *.log *.csv)"),
                      I18n.t("All files (*)", "所有文件 (*)")]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            if (root.model && root.model.exportToFile(selectedFile))
                root.toastRequested(I18n.t("Traffic log exported", "收发记录已导出"), true)
        }
    }

    signal toastRequested(string text, bool ok)
}
