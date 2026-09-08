import QtQuick
import CommStudio 1.0

Rectangle {
    id: root

    default property alias contentData: content.data
    property int padding: 20
    property int cornerRadius: 0
    property color tint: Theme.surface
    property color outline: Theme.border

    color: tint
    radius: root.cornerRadius
    border.width: 1
    border.color: outline

    Behavior on color { ColorAnimation { duration: Theme.durationNormal } }
    Behavior on border.color { ColorAnimation { duration: Theme.durationNormal } }

    Item {
        id: content
        anchors.fill: parent
        anchors.margins: root.padding
    }
}
