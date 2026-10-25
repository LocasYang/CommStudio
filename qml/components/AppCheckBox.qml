import QtQuick
import QtQuick.Controls
import CommStudio 1.0

CheckBox {
    id: control

    property color accent: Theme.primary

    spacing: 6
    font.family: Theme.fontFamily
    font.pixelSize: 12

    indicator: Rectangle {
        implicitWidth: 18
        implicitHeight: 18
        x: control.mirrored ? control.width - control.rightPadding - width : control.leftPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        radius: 5
        color: control.checked ? control.accent
                               : control.hovered ? Theme.hoverFill : Theme.inputBackground
        border.width: control.checked ? 0 : 1
        border.color: control.hovered ? Theme.alpha(control.accent, 0.6) : Theme.border

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }

        Text {
            anchors.centerIn: parent
            text: "✓"
            visible: control.checked
            color: Theme.onAccent
            font.family: Theme.fontFamily
            font.pixelSize: 12
            font.weight: Font.Bold
        }
    }

    contentItem: Text {
        text: control.text
        color: control.enabled ? Theme.textSecondary : Theme.textTertiary
        font: control.font
        verticalAlignment: Text.AlignVCenter
        // Control manages the content rectangle; center within its available
        // height instead of shifting text away from the indicator with anchors.
        // Basic 样式约定：框架不偏移 contentItem，需自行留出指示器宽度。
        leftPadding: control.indicator && !control.mirrored
                     ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored
                      ? control.indicator.width + control.spacing : 0
        elide: Text.ElideRight
    }
}
