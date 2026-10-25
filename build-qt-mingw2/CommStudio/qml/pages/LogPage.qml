import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    function levelAccent(level) {
        if (level === "success")
            return Theme.success
        if (level === "warning")
            return Theme.warning
        if (level === "error")
            return Theme.error
        return Theme.primary
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: I18n.t("Run log", "运行日志")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.weight: Font.DemiBold
            }

            StatusPill {
                text: I18n.count(Studio.logs.length, "entries", " 条")
                accent: Theme.primary
                dot: false
            }

            Item { Layout.fillWidth: true }

            AppButton {
                compact: true
                textPixelSize: 12
                text: I18n.t("Export log", "导出日志")
                enabled: Studio.logs.length > 0
                onClicked: logExportDialog.open()
            }

            AppButton {
                compact: true
                textPixelSize: 12
                text: I18n.t("Clear log", "清空日志")
                enabled: Studio.logs.length > 0
                onClicked: Studio.clearLogs()
            }
        }

        AppCard {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0

            ListView {
                id: logList
                anchors.fill: parent
                anchors.margins: 12
                clip: true
                spacing: 4
                model: Studio.logs

                ScrollBar.vertical: ScrollBar { }

                onCountChanged: {
                    if (moving === false)
                        positionViewAtEnd()
                }

                delegate: Rectangle {
                    id: logRow
                    required property var modelData
                    required property int index
                    width: logList.width
                    height: Math.max(28, logText.implicitHeight + 12)
                    radius: Theme.radiusSmall
                    color: index % 2 === 0 ? "transparent"
                                           : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.035 : 0.02)

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 10

                        Text {
                            text: logRow.modelData.time
                            color: Theme.textTertiary
                            font.family: "Consolas"
                            font.pixelSize: 11
                        }

                        Rectangle {
                            width: 3
                            height: 16
                            radius: 1.5
                            color: root.levelAccent(logRow.modelData.level)
                        }

                        Text {
                            id: logText
                            Layout.fillWidth: true
                            text: logRow.modelData.message
                            color: logRow.modelData.level === "error"
                                   ? Theme.error : Theme.textPrimary
                            font.family: "Consolas"
                            font.pixelSize: 12
                            wrapMode: Text.WrapAnywhere
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: Studio.logs.length === 0
                    text: I18n.t("No log entries · connection, traffic, and errors will be recorded here", "暂无日志 · 连接 / 收发 / 异常事件会记录到这里")
                    color: Theme.textTertiary
                    font.family: Theme.fontFamily
                    font.pixelSize: 12
                }
            }
        }
    }

    FileDialog {
        id: logExportDialog
        title: I18n.t("Export run log", "导出运行日志")
        nameFilters: ["文本文件 (*.txt *.log)", "所有文件 (*)"]
        fileMode: FileDialog.SaveFile
        onAccepted: Studio.saveLogs(selectedFile)
    }
}
