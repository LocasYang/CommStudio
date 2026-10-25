import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import CommStudio 1.0
import CommStudio.Backend 1.0

ApplicationWindow {
    id: root
    width: 1460; height: 900
    minimumWidth: 1100; minimumHeight: 700
    visible: true
    title: I18n.t("CommStudio · Communication Workbench", "CommStudio · 通信调试工作台")
    color: Theme.background
    font.family: Theme.fontFamily
    property int currentPage: 0
    property bool navigationCollapsed: true
    property color toastAccent: Theme.primary
    function syncNativeTitleBar() {
        WindowChrome.apply(root, Theme.dark, Theme.chrome, Theme.textPrimary, Theme.divider)
    }
    function toggleNavigation() {
        navigationCollapsed = !navigationCollapsed
        Studio.saveValue("ui/navigationCollapsed", navigationCollapsed)
    }
    function showToast(text, accent) {
        if (!text || text.length === 0) return
        root.toastAccent = accent || Theme.primary
        toastText.text = text; toast.open(); toastTimer.restart()
    }
    function anyConnected() { return Studio.activeConnections > 0 }
    palette.window: Theme.background
    palette.windowText: Theme.textPrimary
    palette.text: Theme.textPrimary
    palette.buttonText: Theme.textPrimary
    palette.button: Theme.surfaceRaised
    palette.base: Theme.inputBackground
    palette.highlight: Theme.primary
    palette.highlightedText: Theme.onAccent
    Connections {
        target: SerialManager
        function onToastRequested(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) }
    }
    Connections {
        target: ModbusWorkspace
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


    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 65
            color: Theme.chrome
            Behavior on color { ColorAnimation { duration: Theme.durationNormal } }
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider }
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 22; anchors.rightMargin: 20
                spacing: 14
                Rectangle {
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32; radius: 7
                    color: Theme.selectedFill; border.width: 1; border.color: Theme.alpha(Theme.cyan, 0.3)
                    AppIcon { anchors.centerIn: parent; kind: "wave"; color: Theme.cyan; width: 23; height: 23 }
                }
                Text { text: "CommStudio"; color: Theme.textPrimary; font.pixelSize: 20; font.weight: Font.DemiBold }
                Rectangle { Layout.leftMargin: 4; Layout.preferredWidth: 1; Layout.preferredHeight: 22; color: Theme.divider }
                Text { text: I18n.t("Communication workbench", "通信调试工作台"); color: Theme.textTertiary; font.pixelSize: 12 }
                Row {
                    visible: root.width > 1200
                    Layout.leftMargin: 15; spacing: 6
                    Repeater {
                        model: ["SERIAL", "MODBUS", "TCP / IP"]
                        delegate: Rectangle {
                            required property string modelData
                            required property int index
                            width: label.implicitWidth + 14; height: 23; radius: 4
                            color: [0, 1, 3][index] === root.currentPage ? Theme.selectedFill : "transparent"
                            border.width: 1; border.color: Theme.divider
                            Text { id: label; anchors.centerIn: parent; text: modelData; color: [0, 1, 3][index] === root.currentPage ? Theme.cyan : Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                        }
                    }
                }
                Item { Layout.fillWidth: true }
                StatusPill {
                    text: root.anyConnected() ? I18n.count(Studio.activeConnections, "connections", " 个连接在线") : I18n.t("Disconnected", "未建立连接")
                    accent: root.anyConnected() ? Theme.success : Theme.textTertiary
                    scaleFactor: 0.9
                }
                AppButton {
                    compact: true; textPixelSize: 11
                    text: I18n.t("Disconnect all", "全部断开")
                    variant: "outlined"; accent: Theme.textSecondary
                    enabled: root.anyConnected()
                    onClicked: {
                        if (SerialManager.open) SerialManager.close()
                        ModbusWorkspace.closeAll()
                        if (BluetoothManager.leState !== "未连接") BluetoothManager.disconnectDevice()
                        if (NetworkManager.open) NetworkManager.close()
                        root.showToast(I18n.t("All connections disconnected", "已断开全部连接"), Theme.warning)
                    }
                }
                ToolButton {
                    objectName: "themeToggle"
                    implicitWidth: 32; implicitHeight: 32
                    Accessible.name: Theme.dark ? I18n.t("Light theme", "浅色主题") : I18n.t("Dark theme", "深色主题")
                    contentItem: AppIcon { kind: "sun"; color: Theme.textSecondary }
                    background: Rectangle { radius: 6; color: parent.hovered ? Theme.hoverFill : "transparent" }
                    ToolTip.visible: hovered; ToolTip.text: Accessible.name
                    onClicked: Theme.dark = !Theme.dark
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0
            Rectangle {
                id: navigation
                objectName: "navigationRail"
                Layout.preferredWidth: root.navigationCollapsed ? 72 : 210
                Layout.minimumWidth: Layout.preferredWidth
                Layout.maximumWidth: Layout.preferredWidth
                Layout.fillHeight: true
                color: Theme.sidebar
                clip: true
                Rectangle { anchors.right: parent.right; height: parent.height; width: 1; color: Theme.divider }
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 10; spacing: 4
                    ToolButton {
                        objectName: "navigationToggle"
                        Layout.fillWidth: true; Layout.preferredHeight: 35; Layout.bottomMargin: 10
                        Accessible.name: root.navigationCollapsed ? I18n.t("Expand navigation", "展开导航") : I18n.t("Collapse navigation", "收起导航")
                        contentItem: Item {
                            AppIcon { x: root.navigationCollapsed ? (parent.width - width) / 2 : 12; anchors.verticalCenter: parent.verticalCenter; kind: "sidebar"; color: Theme.textSecondary }
                            Text { visible: !root.navigationCollapsed; x: 46; anchors.verticalCenter: parent.verticalCenter; text: I18n.t("Collapse navigation", "收起导航"); color: Theme.textSecondary; font.pixelSize: 12 }
                        }
                        background: Rectangle { radius: 6; color: parent.hovered ? Theme.hoverFill : "transparent" }
                        ToolTip.visible: hovered; ToolTip.text: Accessible.name
                        onClicked: root.toggleNavigation()
                    }
                    Text { visible: !root.navigationCollapsed; Layout.leftMargin: 14; Layout.bottomMargin: 6; text: "WORKSPACE"; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10; font.letterSpacing: 1 }
                    Repeater {
                        model: [
                            {label: I18n.t("Serial", "串口"), icon: "port", page: 0},
                            {label: "Modbus", icon: "grid", page: 1},
                            {label: I18n.t("Bluetooth", "蓝牙"), icon: "bluetooth", page: 2},
                            {label: I18n.t("Network", "网络"), icon: "network", page: 3}
                        ]
                        delegate: NavItem {
                            required property var modelData
                            Layout.fillWidth: true
                            collapsed: root.navigationCollapsed
                            text: modelData.label; iconKind: modelData.icon; current: root.currentPage === modelData.page
                            onClicked: root.currentPage = modelData.page
                        }
                    }
                    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; Layout.topMargin: 12; Layout.bottomMargin: 12; color: Theme.divider }
                    Repeater {
                        model: [
                            {label: I18n.t("Tools", "工具箱"), icon: "tools", page: 4},
                            {label: I18n.t("Logs", "日志"), icon: "log", page: 5},
                            {label: I18n.t("Settings", "设置"), icon: "gear", page: 6}
                        ]
                        delegate: NavItem {
                            required property var modelData
                            Layout.fillWidth: true
                            collapsed: root.navigationCollapsed
                            text: modelData.label; iconKind: modelData.icon; current: root.currentPage === modelData.page
                            onClicked: root.currentPage = modelData.page
                        }
                    }
                    Item { Layout.fillHeight: true }
                    Text { Layout.fillWidth: true; text: root.navigationCollapsed ? "v" + Studio.version : "COMMSTUDIO · " + Studio.version; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 9; horizontalAlignment: Text.AlignHCenter }
                }
            }
            StackLayout {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.leftMargin: 22; Layout.rightMargin: 22; Layout.topMargin: 10; Layout.bottomMargin: 17
                currentIndex: root.currentPage
                SerialPage { onToastRequested: function(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) } }
                ModbusPage { onToastRequested: function(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) } }
                BluetoothPage { onToastRequested: function(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) } }
                NetworkPage { onToastRequested: function(text, ok) { root.showToast(text, ok ? Theme.success : Theme.error) } }
                ToolboxPage { }
                LogPage { }
                SettingsPage { }
            }
        }
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 26
            color: Theme.chrome
            Rectangle { width: parent.width; height: 1; color: Theme.divider }
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 18; spacing: 9
                Rectangle { width: 5; height: 5; radius: 3; color: root.anyConnected() ? Theme.success : Theme.textTertiary }
                Text { Layout.fillWidth: true; text: Studio.statusMessage; color: Theme.textSecondary; font.pixelSize: 10; elide: Text.ElideRight }
                Text { text: "RX " + Studio.totalRx + " B   ·   TX " + Studio.totalTx + " B   ·   " + I18n.t("Connections ", "连接 ") + Studio.activeConnections; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
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
        // QSettings INI profiles can return boolean values as strings.
        const savedNavigation = String(Studio.loadValue("ui/navigationCollapsed", true)).toLowerCase()
        navigationCollapsed = savedNavigation === "true" || savedNavigation === "1"
        const args = Qt.application.arguments
        for (let i = 0; i < args.length; ++i) {
            if (args[i].indexOf("--page=") === 0)
                root.currentPage = Math.max(0, Math.min(6, Number(args[i].substring(7))))
        }
        if (args.indexOf("--dark") >= 0) Theme.dark = true
        Qt.callLater(root.syncNativeTitleBar)
    }
    onVisibilityChanged: if (visible) Qt.callLater(root.syncNativeTitleBar)
}
