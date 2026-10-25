pragma Singleton

import QtQuick

QtObject {
    id: theme

    // Approved instrument-workbench palette: graphite surfaces, ice-blue
    // actions, cyan receive data, amber transmit data, and quiet separators.
    // Keep the same semantic hierarchy in both dark and light appearances.
    property bool dark: true
    readonly property color primary: dark ? "#59BFFF" : "#086DAD"
    readonly property color success: dark ? "#55D5AA" : "#147457"
    readonly property color warning: dark ? "#E7B970" : "#966014"
    readonly property color error: dark ? "#E9828D" : "#BD344D"
    readonly property color cyan: dark ? "#57D8E7" : "#007F91"
    readonly property color purple: dark ? "#AE9AD8" : "#7352A5"
    readonly property color onAccent: dark ? "#091725" : "#FFFFFF"

    readonly property color background: dark ? "#0B1018" : "#EEF3F7"
    readonly property color backgroundSecondary: dark ? "#101824" : "#E8EEF4"
    readonly property color chrome: dark ? "#101824" : "#F8FAFC"
    readonly property color chromeBody: dark ? "#121D29" : "#F2F6FA"
    readonly property color sidebar: dark ? "#0D151F" : "#F6F9FC"
    readonly property color surface: dark ? "#111A26" : "#FFFFFF"
    readonly property color surfaceRaised: dark ? "#172230" : "#F1F6FA"
    readonly property color tableHeader: dark ? "#182534" : "#EAF1F7"
    readonly property color toolbar: dark ? "#15212E" : "#F2F6FA"
    readonly property color alternateRow: dark ? "#14202D" : "#F7FAFC"
    // Opaque bands prevent corner fillers from applying the tint twice.
    readonly property color dialogHeader: dark ? "#172534" : "#EDF5FA"
    readonly property color dialogFooter: dark ? "#121E2B" : "#F5F8FB"
    readonly property color textPrimary: dark ? "#E7F0FA" : "#182B3E"
    readonly property color textSecondary: dark ? "#ACBDD0" : "#405A70"
    readonly property color textTertiary: dark ? "#8097AF" : "#607A90"
    readonly property color border: dark ? "#344B62" : "#BDCDDA"
    readonly property color divider: dark ? "#243143" : "#D9E3EC"
    readonly property color inputBackground: dark ? "#0E1824" : "#F8FBFD"
    readonly property color hoverFill: dark ? "#182B3C" : "#E7F1F8"
    readonly property color selectedFill: dark ? "#18374B" : "#DDEFF8"
    readonly property color shadow: dark ? alpha("#080A10", 0.50) : alpha("#47536B", 0.16)

    // Traffic roles are named explicitly so RX/TX colors stay consistent in
    // the terminal, statistics bar, logs, and future protocol pages.
    readonly property color rx: dark ? cyan : "#0B8F96"
    readonly property color tx: warning
    readonly property color timestamp: dark ? textTertiary : "#727B8E"
    readonly property color payload: textPrimary

    readonly property int radiusSmall: 6
    readonly property int radius: 8
    readonly property int radiusLarge: 12
    readonly property int durationFast: 100
    readonly property int durationNormal: 200
    readonly property string fontFamily: "Microsoft YaHei UI"

    function alpha(value: color, opacity: real): color {
        return Qt.rgba(value.r, value.g, value.b, opacity)
    }
    function blend(base: color, tint: color, amount: real): color {
        return Qt.rgba(base.r * (1 - amount) + tint.r * amount,
                       base.g * (1 - amount) + tint.g * amount,
                       base.b * (1 - amount) + tint.b * amount, 1)
    }

}
