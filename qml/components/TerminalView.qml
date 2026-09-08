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
    property string emptyHint: "等待数据 · 连接后收发的数据会显示在这里"
    property int displayMode: 2 // 0 TEXT / 1 HEX / 2 HEX+ASCII
    property bool autoScroll: true
    property bool followTail: true
    property int unseenCount: 0

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
                text: "MESSAGE"
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
                text: root.model ? root.model.count + " items" : "0 items"
                accent: Theme.primary
                dot: false
                scaleFactor: 0.82
            }

            Text {
                visible: root.model && root.model.paused
                text: "PAUSED · RECEIVING IN BACKGROUND"
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
                text: root.model && root.model.paused ? "继续" : "暂停"
                accent: root.model && root.model.paused ? Theme.warning : Theme.primary
                variant: "text"
                onClicked: if (root.model) root.model.paused = !root.model.paused
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: "清屏"
                variant: "text"
                onClicked: if (root.model) root.model.clear()
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: "导出"
                variant: "text"
                onClicked: exportDialog.open()
            }

            AppButton {
                compact: true
                textPixelSize: 11
                text: root.followTail ? "自动跟随" : "手动浏览"
                accent: root.followTail ? Theme.success : Theme.warning
                variant: "text"
                onClicked: {
                    root.followTail = !root.followTail
                    if (root.followTail) {
                        root.unseenCount = 0
                        trafficList.positionViewAtEnd()
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
                if (root.followTail && root.autoScroll && !moving)
                    positionViewAtEnd()
                else if (!atYEnd)
                    root.unseenCount = Math.min(999, root.unseenCount + 1)
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
                text: root.emptyHint + "\n\nCtrl+Enter 发送 · ↑/↓ 浏览历史 · F1 查看快捷键"
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
                    text: "↓ " + (root.unseenCount >= 999 ? "999+" : root.unseenCount) + " new messages"
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
        title: "导出收发记录"
        nameFilters: ["文本文件 (*.txt *.log *.csv)", "所有文件 (*)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            if (root.model && root.model.exportToFile(selectedFile))
                root.toastRequested("收发记录已导出", true)
        }
    }

    signal toastRequested(string text, bool ok)
}
