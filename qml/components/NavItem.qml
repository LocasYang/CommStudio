import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0

Button {
    id: control
    property bool current: false
    property bool collapsed: false
    property string iconKind: "grid"
    property string symbol: ""
    property bool subLevel: false
    implicitHeight: collapsed ? 62 : 44
    Layout.minimumHeight: implicitHeight
    Layout.preferredHeight: implicitHeight
    Layout.maximumHeight: implicitHeight
    padding: 0
    hoverEnabled: true
    Accessible.name: text
    contentItem: Item {
        AppIcon {
            width: 20; height: 20
            x: control.collapsed ? (parent.width - width) / 2 : 14
            y: control.collapsed ? 10 : (parent.height - height) / 2
            kind: control.iconKind
            color: control.current ? Theme.cyan : Theme.textSecondary
        }
        Text {
            x: control.collapsed ? 2 : 46
            y: control.collapsed ? 37 : 0
            width: parent.width - x - (control.collapsed ? 2 : 10)
            height: control.collapsed ? 16 : parent.height
            text: control.text
            color: control.current ? Theme.cyan : Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: control.collapsed ? 10 : 13
            font.weight: control.current ? Font.DemiBold : Font.Normal
            horizontalAlignment: control.collapsed ? Text.AlignHCenter : Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
    background: Rectangle {
        radius: Theme.radiusSmall
        color: control.current ? Theme.selectedFill : control.hovered ? Theme.hoverFill : "transparent"
        border.width: control.current ? 1 : 0
        border.color: Theme.alpha(Theme.cyan, 0.20)
        Rectangle {
            visible: control.current
            width: 2; height: 24; radius: 1
            color: Theme.cyan
            anchors.left: parent.left; anchors.leftMargin: -7; anchors.verticalCenter: parent.verticalCenter
        }
        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
    }
    ToolTip.visible: hovered && collapsed
    ToolTip.delay: 450
    ToolTip.text: text
}
