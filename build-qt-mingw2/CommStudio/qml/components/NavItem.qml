import QtQuick
import QtQuick.Controls
import CommStudio 1.0

Button {
    id: control

    property bool current: false
    property string symbol: "•"
    // Sub-level entry (plugin report views under 插件中心).
    property bool subLevel: false

    implicitHeight: subLevel ? 34 : 42
    leftPadding: subLevel ? 26 : 14
    rightPadding: 12

    contentItem: Row {
        spacing: 11

        Text {
            width: subLevel ? 16 : 20
            anchors.verticalCenter: parent.verticalCenter
            text: control.symbol
            color: control.current ? Theme.primary : Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: subLevel ? 13 : 17
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            width: control.width - (control.subLevel ? 54 : 70)
            text: control.text
            color: control.current ? Theme.primary : control.subLevel ? Theme.textSecondary
                                                                      : Theme.textPrimary
            font.family: Theme.fontFamily
            font.pixelSize: control.subLevel ? 13 : 14
            font.weight: control.current ? Font.DemiBold : Font.Normal
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        radius: Theme.radiusSmall
        color: control.current ? Theme.selectedFill
                               : control.hovered ? Theme.hoverFill : "transparent"
        border.width: control.current ? 1 : 0
        border.color: Theme.alpha(Theme.primary, 0.18)

        Rectangle {
            visible: control.current
            width: 3
            height: 20
            radius: 2
            color: Theme.primary
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.verticalCenter: parent.verticalCenter
        }

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
    }
}
