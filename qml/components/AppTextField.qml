import QtQuick
import QtQuick.Controls
import CommStudio 1.0

TextField {
    id: control

    property bool mono: false
    property string placeholder: ""
    implicitHeight: 36
    color: Theme.textPrimary
    placeholderText: placeholder
    placeholderTextColor: Theme.textTertiary
    font.family: mono ? "Consolas" : Theme.fontFamily
    font.pixelSize: mono ? 12 : 13
    selectByMouse: true
    verticalAlignment: TextInput.AlignVCenter

    background: Rectangle {
        color: Theme.inputBackground
        radius: Theme.radiusSmall
        border.width: control.activeFocus ? 2 : 1
        border.color: control.activeFocus ? Theme.primary : Theme.border

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
        Behavior on border.color { ColorAnimation { duration: Theme.durationFast } }
    }
}
