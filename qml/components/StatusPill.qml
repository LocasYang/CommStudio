import QtQuick
import CommStudio 1.0

Rectangle {
    id: root

    property string text: ""
    property color accent: Theme.primary
    property bool dot: true
    property real scaleFactor: 1.0

    implicitWidth: row.implicitWidth + Math.round(18 * scaleFactor)
    implicitHeight: Math.round(26 * scaleFactor)
    radius: height / 2
    color: Theme.alpha(accent, Theme.dark ? 0.17 : 0.10)
    border.width: 1
    border.color: Theme.alpha(accent, 0.24)

    Row {
        id: row
        anchors.centerIn: parent
        spacing: Math.round(6 * root.scaleFactor)

        Rectangle {
            visible: root.dot
            anchors.verticalCenter: parent.verticalCenter
            width: Math.round(7 * root.scaleFactor)
            height: width
            radius: width / 2
            color: root.accent
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.text
            color: root.accent
            font.family: Theme.fontFamily
            font.pixelSize: Math.round(12 * root.scaleFactor)
            font.weight: Font.DemiBold
        }
    }
}
