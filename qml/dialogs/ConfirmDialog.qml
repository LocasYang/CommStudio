import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0

// Risk-graded confirmation dialog following the shared dialog skeleton:
// padding 0, surface shell with accent@42% border, tinted header band with
// squared corners, 72px footer band, Esc to close (mask click ignored).
Dialog {
    id: control

    property string titleText: "确认操作"
    property string messageText: ""
    property string noteText: ""
    property color accent: Theme.primary
    property string riskText: "常规操作"
    property string confirmText: "确认"
    property var infoRows: []
    readonly property bool errorTone: control.accent === Theme.error
    signal operationConfirmed()

    width: 520
    modal: true
    padding: 0
    closePolicy: Popup.CloseOnEscape
    parent: Overlay.overlay
    anchors.centerIn: parent

    Overlay.modal: Rectangle {
        color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42)
    }

    background: Rectangle {
        color: Theme.surface
        radius: Theme.radiusLarge
        border.width: 1
        border.color: Theme.alpha(control.accent, control.errorTone ? 0.55 : 0.42)

        Behavior on color { ColorAnimation { duration: Theme.durationNormal } }
    }

    function openFor(properties) {
        titleText = properties.titleText || "确认操作"
        messageText = properties.messageText || ""
        noteText = properties.noteText || ""
        accent = properties.accent || Theme.primary
        riskText = properties.riskText || "常规操作"
        confirmText = properties.confirmText || "确认"
        infoRows = properties.infoRows || []
        open()
    }

    contentItem: ColumnLayout {
        spacing: 0

        // Header band with squared bottom corners.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            // Error confirmations keep the surface neutral; the accent is
            // carried by the pill, border, indicator line and action button.
            color: control.errorTone ? Theme.surfaceRaised
                                     : Theme.alpha(control.accent, Theme.dark ? 0.18 : 0.10)
            radius: Theme.radiusLarge

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: Theme.radiusLarge
                color: parent.color
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 2
                color: Theme.alpha(control.accent, control.errorTone ? 0.72 : 0.42)
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 14
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    text: control.titleText
                    color: Theme.textPrimary
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                    elide: Text.ElideRight
                }

                StatusPill {
                    text: control.riskText
                    accent: control.accent
                    scaleFactor: 0.92
                }

                ToolButton {
                    id: closeButton
                    focusPolicy: Qt.NoFocus
                    width: 30
                    height: 30
                    ToolTip.visible: hovered
                    ToolTip.delay: 400
                    ToolTip.text: "关闭"

                    contentItem: Text {
                        text: "×"
                        color: closeButton.hovered || closeButton.pressed
                               ? Theme.textPrimary : Theme.textSecondary
                        font.pixelSize: 19
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 9
                        color: closeButton.pressed ? Theme.alpha(control.accent, 0.18)
                              : closeButton.hovered ? Theme.alpha(control.accent, 0.10)
                              : Theme.alpha(Theme.surface, 0.34)
                        border.width: 1
                        border.color: closeButton.hovered || closeButton.pressed
                                      ? Theme.alpha(control.accent, 0.40)
                                      : Theme.alpha(Theme.textSecondary, 0.20)

                        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
                    }

                    onClicked: control.reject()
                }
            }
        }

        // Body.
        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: 20
            spacing: 10

            Text {
                Layout.fillWidth: true
                text: control.messageText
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.DemiBold
                wrapMode: Text.WordWrap
                lineHeight: 1.25
            }

            Rectangle {
                Layout.fillWidth: true
                visible: control.noteText.length > 0
                implicitHeight: noteLabel.implicitHeight + 20
                radius: Theme.radius
                color: Theme.alpha(Theme.warning, Theme.dark ? 0.10 : 0.065)
                border.width: 1
                border.color: Theme.alpha(Theme.warning, 0.24)

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    Text {
                        text: "！"
                        color: Theme.warning
                        font.family: Theme.fontFamily
                        font.pixelSize: 17
                        font.weight: Font.Bold
                    }

                    Text {
                        id: noteLabel
                        Layout.fillWidth: true
                        text: control.noteText
                        color: Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        lineHeight: 1.25
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6

                Repeater {
                    model: control.infoRows

                    delegate: RowLayout {
                        id: infoRow
                        required property var modelData
                        Layout.fillWidth: true
                        spacing: 10

                        Text {
                            Layout.preferredWidth: 96
                            text: infoRow.modelData.label
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }

                        Text {
                            Layout.fillWidth: true
                            text: infoRow.modelData.value
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            elide: Text.ElideMiddle
                        }
                    }
                }
            }
        }

        // Footer band with squared top corners.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 72
            color: control.errorTone ? Theme.surfaceRaised
                                     : Theme.alpha(control.accent, Theme.dark ? 0.18 : 0.10)
            radius: Theme.radiusLarge

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: Theme.radiusLarge
                color: parent.color
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 1
                color: Theme.alpha(control.accent, control.errorTone ? 0.42 : 0.20)
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 24
                anchors.topMargin: 8
                spacing: 12

                Item { Layout.fillWidth: true }

                AppButton {
                    text: "取消"
                    variant: "outlined"
                    accent: Theme.textSecondary
                    onClicked: control.reject()
                }

                AppButton {
                    text: control.confirmText
                    variant: control.accent === Theme.error ? "danger" : "primary"
                    accent: control.accent
                    onClicked: {
                        control.accept()
                        control.operationConfirmed()
                    }
                }
            }
        }
    }
}
