import QtQuick
import QtQuick.Controls
import CommStudio 1.0

Button {
    id: control

    property string variant: "default"
    property color accent: Theme.primary
    property bool compact: false
    property real scaleFactor: 1.0
    property int textPixelSize: Math.round(14 * scaleFactor)
    property string leadingSymbol: ""
    property bool preserveAccentWhenDisabled: false

    implicitHeight: Math.round((compact ? 32 : 38) * scaleFactor)
    implicitWidth: Math.max(Math.round((compact ? 72 : 88) * scaleFactor),
                            contentItem.implicitWidth + Math.round(28 * scaleFactor))
    padding: Math.round((compact ? 8 : 12) * scaleFactor)

    contentItem: Text {
        text: control.leadingSymbol.length > 0
              ? control.leadingSymbol + "  " + control.text
              : control.text
        color: {
            if (!control.enabled)
                return control.preserveAccentWhenDisabled
                       ? Theme.alpha(control.accent, Theme.dark ? 0.72 : 0.78)
                       : Theme.dark ? Theme.textTertiary : "#6F788B"
            if (control.variant === "primary" || control.variant === "danger")
                return Theme.onAccent
            if (control.variant === "text" || control.variant === "outlined")
                return control.accent
            return Theme.textPrimary
        }
        font.family: Theme.fontFamily
        font.pixelSize: control.textPixelSize
        font.weight: control.variant === "primary" ? Font.DemiBold : Font.Normal
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: Theme.radiusSmall
        color: {
            if (!control.enabled)
                return control.preserveAccentWhenDisabled
                       ? Theme.alpha(control.accent, Theme.dark ? 0.10 : 0.075)
                       : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.05 : 0.075)
            if (control.variant === "primary")
                return control.down ? Qt.darker(control.accent, 1.18)
                                    : control.hovered ? Qt.lighter(control.accent, 1.08) : control.accent
            if (control.variant === "danger")
                return control.down ? Qt.darker(Theme.error, 1.18)
                                    : control.hovered ? Qt.lighter(Theme.error, 1.08) : Theme.error
            if (control.variant === "text")
                return control.hovered ? Theme.alpha(control.accent, 0.12) : "transparent"
            if (control.variant === "outlined")
                return control.down ? Theme.alpha(control.accent, 0.18)
                                    : control.hovered ? Theme.alpha(control.accent, 0.10) : "transparent"
            return control.down ? Theme.selectedFill
                                : control.hovered ? Theme.hoverFill : Theme.surfaceRaised
        }
        border.width: control.variant === "default" || control.variant === "outlined" ? 1 : 0
        border.color: control.variant === "outlined"
                      ? Theme.alpha(control.accent,
                                    !control.enabled && control.preserveAccentWhenDisabled
                                    ? 0.48 : control.hovered ? 0.90 : 0.58)
                      : control.hovered ? control.accent
                                        : !control.enabled && !Theme.dark
                                          ? Theme.alpha(Theme.textPrimary, 0.12)
                                          : Theme.border

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
        Behavior on border.color { ColorAnimation { duration: Theme.durationFast } }
    }
}
