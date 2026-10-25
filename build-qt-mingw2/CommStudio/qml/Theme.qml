pragma Singleton

import QtQuick

QtObject {
    id: theme

    // TuiSerial-inspired semantic palette: the UI is a workbench first, not a
    // collection of marketing cards. Light mode remains available, but the
    // dense serial workflow opens in the terminal-oriented dark palette.
    property bool dark: true
    readonly property color primary: "#9DAFEA"
    readonly property color success: "#58F08B"
    readonly property color warning: "#F4E56B"
    readonly property color error: "#FF7474"
    readonly property color cyan: "#71E7F0"
    readonly property color purple: "#E39BEF"

    readonly property color background: dark ? "#20232D" : "#F3F5FB"
    readonly property color backgroundSecondary: dark ? "#252936" : "#EAEEF8"
    readonly property color chrome: dark ? "#5D70A3" : "#EEF3FC"
    readonly property color chromeBody: dark ? "#46557D" : "#F8FAFF"
    readonly property color sidebar: dark ? "#20242F" : "#EFF1FA"
    readonly property color surface: dark ? "#272B37" : "#FFFFFF"
    readonly property color surfaceRaised: dark ? "#303646" : "#F4F6FB"
    readonly property color textPrimary: dark ? "#F3F4F7" : "#171A25"
    readonly property color textSecondary: dark ? "#C0C6D4" : "#50586A"
    readonly property color textTertiary: dark ? "#8E97AA" : "#727B8E"
    readonly property color border: dark ? alpha("#E8EDF8", 0.72) : alpha("#172033", 0.16)
    readonly property color divider: dark ? alpha("#E8EDF8", 0.24) : alpha("#172033", 0.10)
    readonly property color inputBackground: dark ? "#222733" : "#F1F3F8"
    readonly property color hoverFill: alpha(primary, dark ? 0.20 : 0.10)
    readonly property color selectedFill: dark ? "#687BAE" : alpha(primary, 0.15)
    readonly property color shadow: dark ? alpha("#080A10", 0.50) : alpha("#47536B", 0.16)

    // Traffic roles are named explicitly so RX/TX colors stay consistent in
    // the terminal, statistics bar, logs, and future protocol pages.
    readonly property color rx: dark ? cyan : "#0B8F96"
    readonly property color tx: dark ? success : "#21873D"
    readonly property color timestamp: dark ? textTertiary : "#727B8E"
    readonly property color payload: textPrimary

    readonly property int radiusSmall: 6
    readonly property int radius: 10
    readonly property int radiusLarge: 16
    readonly property int durationFast: 100
    readonly property int durationNormal: 200
    readonly property string fontFamily: "Microsoft YaHei UI"

    function alpha(value: color, opacity: real): color {
        return Qt.rgba(value.r, value.g, value.b, opacity)
    }

}
