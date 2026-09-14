import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import CommStudio 1.0
import CommStudio.Backend 1.0

ApplicationWindow {
    id: root

    width: 1360
    height: 860
    minimumWidth: 1100
    minimumHeight: 680
    visible: true
    title: I18n.t("CommStudio · Integrated Communication Workbench", "CommStudio · 一体化通信调试平台")
    color: Theme.background
    font.family: Theme.fontFamily

    readonly property real resolutionRatio: Math.min(width / 1280.0, height / 800.0)
    readonly property real interfaceScale: Math.max(
                                               0.92,
                                               Math.min(
                                                   1.35,
                                                   1.0 + (resolutionRatio - 1.0) * 0.45))

    property int currentPage: 0
    property color toastAccent: Theme.primary

    function syncNativeTitleBar() {
        WindowChrome.apply(
            root,
            Theme.dark,
            Theme.chrome,
            Theme.textPrimary,
            Theme.chrome)
    }

    function showToast(text, accent) {
        if (!text || text.length === 0)
            return
        root.toastAccent = accent || Theme.primary
        toastText.text = text
        toast.open()
        toastTimer.restart()
    }

    function anyConnected() {
        return Studio.activeConnections > 0
    }

    palette.window: Theme.background
    palette.windowText: Theme.textPrimary
    palette.text: Theme.textPrimary
    palette.buttonText: Theme.textPrimary
    palette.button: Theme.surfaceRaised
    palette.base: Theme.inputBackground
    palette.highlight: Theme.primary
    palette.highlightedText: "#FFFFFF"

    Connections {
        target: SerialManager
        function onToastRequested(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) }
    }
    Connections {
        target: ModbusManager
        function onToastRequested(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) }
    }
    Connections {
        target: BluetoothManager
        function onToastRequested(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) }
    }
    Connections {
        target: NetworkManager
        function onToastRequested(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) }
    }
    Connections {
        target: Theme
        function onDarkChanged() {
            Qt.callLater(root.syncNativeTitleBar)
            Studio.saveTheme(Theme.dark)
        }
        function onPrimaryChanged() {
            Studio.saveTheme(Theme.dark)
        }
    }

    Item {
        anchors.fill: parent
        clip: true

        Rectangle {
            width: parent.width / root.interfaceScale
            height: parent.height / root.interfaceScale
            scale: root.interfaceScale
            transformOrigin: Item.TopLeft
            color: Theme.background

            Behavior on color { ColorAnimation { duration: Theme.durationNormal } }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    id: globalHeader
                    z: 4
                    Layout.fillWidth: true
                    Layout.preferredHeight: 54
                    color: Theme.chrome
                    border.width: 0

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.bottom
                        height: 9
                        gradient: Gradient {
                            orientation: Gradient.Vertical
                            GradientStop { position: 0.0; color: Theme.alpha(Theme.shadow, Theme.dark ? 0.16 : 0.075) }
                            GradientStop { position: 1.0; color: "transparent" }
                        }
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 18
                        anchors.rightMargin: 14
                        spacing: 12

                        Rectangle {
                            Layout.preferredWidth: 30
                            Layout.preferredHeight: 30
                            radius: 2
                            color: Theme.alpha(Theme.primary, Theme.dark ? 0.22 : 0.12)
                            border.width: 1
                            border.color: Theme.alpha(Theme.primary, 0.32)

                            Text {
                                anchors.centerIn: parent
                                text: "⌁"
                                color: Theme.primary
                                font.pixelSize: 19
                                font.weight: Font.DemiBold
                            }
                        }

                        ColumnLayout {
                            spacing: 0

                            Text {
                                text: "CommStudio"
                                color: Theme.textPrimary
                                font.family: Theme.fontFamily
                                font.pixelSize: 16
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: "SERIAL / PROTOCOL WORKBENCH"
                                color: Theme.textTertiary
                                font.family: Theme.fontFamily
                                font.pixelSize: 10
                            }
                        }

                        Rectangle {
                            Layout.leftMargin: 10
                            Layout.preferredWidth: 1
                            Layout.preferredHeight: 24
                            color: Theme.divider
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 1

                            Text {
                                Layout.fillWidth: true
                                text: "SERIAL  ·  MODBUS  ·  BLUETOOTH  ·  NETWORK"
                                color: Theme.textPrimary
                                font.family: "Consolas"
                                font.pixelSize: 11
                                font.weight: Font.Medium
                                elide: Text.ElideMiddle
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "RX " + (Studio.totalRx / 1024).toFixed(1) + " KB · TX "
                                      + (Studio.totalTx / 1024).toFixed(1) + " KB"
                                color: Theme.textTertiary
                                font.family: "Consolas"
                                font.pixelSize: 10
                                elide: Text.ElideRight
                            }
                        }

                        StatusPill {
                            text: root.anyConnected()
                                  ? I18n.count(Studio.activeConnections, "active", " 个连接活动")
                                  : I18n.t("No connection", "未建立连接")
                            accent: root.anyConnected() ? Theme.success : Theme.textTertiary
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 12
                            leadingSymbol: "⏻"
                            text: I18n.t("Disconnect all", "全部断开")
                            variant: "primary"
                            enabled: root.anyConnected()
                            onClicked: {
                                if (SerialManager.open)
                                    SerialManager.close()
                                ModbusManager.close()
                                if (BluetoothManager.leState !== "未连接")
                                    BluetoothManager.disconnectDevice()
                                if (NetworkManager.open)
                                    NetworkManager.close()
                                root.showToast(I18n.t("All connections disconnected", "已断开全部连接"), Theme.warning)
                            }
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 12
                            text: Theme.dark ? I18n.t("Light", "浅色") : I18n.t("Dark", "深色")
                            variant: "text"
                            onClicked: Theme.dark = !Theme.dark
                        }

                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

                    Rectangle {
                        Layout.preferredWidth: 218
                        Layout.fillHeight: true
                        color: Theme.alpha(Theme.sidebar, Theme.dark ? 0.96 : 0.92)
                        border.width: 1
                        border.color: Theme.divider

                        Behavior on color { ColorAnimation { duration: Theme.durationNormal } }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 6

                            AppCard {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 84
                                padding: 11
                                tint: Theme.surfaceRaised
                                outline: Theme.border

                                ColumnLayout {
                                    anchors.fill: parent
                                    spacing: 6

                                    RowLayout {
                                        Layout.fillWidth: true

                                        StatusPill {
                                            text: "SESSION"
                                            accent: Theme.primary
                                            dot: false
                                            scaleFactor: 0.9
                                        }

                                        Item { Layout.fillWidth: true }

                                        Text {
                                            text: I18n.count(Studio.activeConnections, "active", " 活动")
                                            color: Theme.textTertiary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 11
                                        }
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: root.anyConnected() ? "SESSION ACTIVE" : "NO ACTIVE SESSION"
                                        color: Theme.textPrimary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 15
                                        font.weight: Font.DemiBold
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: "SERIAL  ·  MODBUS  ·  BLE  ·  TCP/UDP"
                                        color: Theme.textSecondary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.currentPage = 0
                                }
                            }

                            Text {
                                Layout.leftMargin: 8
                                Layout.topMargin: 2
                                text: "WORKSPACES"
                                color: Theme.textTertiary
                                font.family: Theme.fontFamily
                                font.pixelSize: 11
                                font.weight: Font.DemiBold
                            }

                            Column {
                                width: 198
                                spacing: 3

                                Repeater {
                                    model: [
                                        { label: I18n.t("Serial", "串口调试"), symbol: "⌁", page: 0 },
                                        { label: "Modbus", symbol: "▦", page: 1 },
                                        { label: I18n.t("Bluetooth", "蓝牙"), symbol: "◈", page: 2 },
                                        { label: I18n.t("Network", "网络"), symbol: "⇌", page: 3 }
                                    ]

                                    delegate: NavItem {
                                        required property var modelData
                                        width: 198
                                        text: modelData.label
                                        symbol: modelData.symbol
                                        current: root.currentPage === modelData.page
                                        onClicked: root.currentPage = modelData.page
                                    }
                                }

                                Rectangle {
                                    width: 198
                                    height: 1
                                    color: Theme.divider
                                }

                                Text {
                                    width: 198
                                    height: 26
                                    text: "  TOOLS / SYSTEM"
                                    verticalAlignment: Text.AlignVCenter
                                    color: Theme.textTertiary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                }

                                Repeater {
                                    model: [
                                        { label: I18n.t("Tools", "工具箱"), symbol: "⚒", page: 4 },
                                        { label: I18n.t("Run log", "运行日志"), symbol: "▤", page: 5 },
                                        { label: I18n.t("Settings", "设置"), symbol: "⚙", page: 6 }
                                    ]

                                    delegate: NavItem {
                                        required property var modelData
                                        width: 198
                                        text: modelData.label
                                        symbol: modelData.symbol
                                        current: root.currentPage === modelData.page
                                        onClicked: root.currentPage = modelData.page
                                    }
                                }
                            }

                            Item { Layout.fillHeight: true }

                            Text {
                                Layout.fillWidth: true
                                text: "COMMSTUDIO 0.1.0  ·  QT 6.6"
                                font.family: "Consolas"
                                horizontalAlignment: Text.AlignHCenter
                                color: Theme.textTertiary
                                font.pixelSize: 10
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 18
                            spacing: 10

                            StackLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                currentIndex: root.currentPage

                                SerialPage {
                                    onToastRequested: function(text, ok) {
                                        root.showToast(text, ok ? Theme.success : Theme.error)
                                    }
                                }
                                ModbusPage {
                                    onToastRequested: function(text, ok) {
                                        root.showToast(text, ok ? Theme.success : Theme.error)
                                    }
                                }
                                BluetoothPage {
                                    onToastRequested: function(text, ok) {
                                        root.showToast(text, ok ? Theme.success : Theme.error)
                                    }
                                }
                                NetworkPage {
                                    onToastRequested: function(text, ok) {
                                        root.showToast(text, ok ? Theme.success : Theme.error)
                                    }
                                }
                                ToolboxPage { }
                                LogPage { }
                                SettingsPage { }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 28
                    color: Theme.surface
                    border.width: 1
                    border.color: Theme.divider

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 14

                        Rectangle {
                            width: 7
                            height: 7
                            radius: 4
                            color: root.anyConnected() ? Theme.success : Theme.textTertiary
                        }

                        Text {
                            text: Studio.statusMessage
                            color: Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: "RX " + Studio.totalRx + " B · TX " + Studio.totalTx
                                  + " B · " + I18n.t("Connections ", "连接 ")
                                  + I18n.count(Studio.activeConnections, "", " 个")
                            color: Theme.textTertiary
                            font.family: "Consolas"
                            font.pixelSize: 11
                        }
                    }
                }
            }
        }
    }

    // Result toast, centered, auto dismiss.
    Popup {
        id: toast
        parent: Overlay.overlay
        x: (root.width - width) / 2
        y: root.height - height - 46
        width: Math.min(350, root.width - 40)
        height: 72
        padding: 0
        modal: false
        closePolicy: Popup.NoAutoClose

        background: Rectangle {
            color: Theme.surface
            radius: Theme.radius
            border.width: 1
            border.color: Theme.border
        }

        contentItem: Item {
            Text {
                id: toastText
                anchors.fill: parent
                anchors.margins: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: ""
                color: root.toastAccent
                font.family: Theme.fontFamily
                font.pixelSize: 13
                font.weight: Font.Medium
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
            }
        }
    }

    Timer {
        id: toastTimer
        interval: 2600
        onTriggered: toast.close()
    }

    Component.onCompleted: {
        Theme.dark = Studio.startDark
        const args = Qt.application.arguments
        for (let i = 0; i < args.length; ++i) {
            if (args[i].indexOf("--page=") === 0)
                root.currentPage = Math.max(0, Math.min(6, Number(args[i].substring(7))))
        }
        if (Qt.application.arguments.indexOf("--dark") >= 0)
            Theme.dark = true
        Qt.callLater(root.syncNativeTitleBar)
    }

    onVisibilityChanged: {
        if (visible)
            Qt.callLater(root.syncNativeTitleBar)
    }
}
