import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CommStudio 1.0

// A compact view of the selected channel's real traffic model. Pausing freezes
// presentation only; connection, polling and the underlying protocol stay active.
Rectangle {
    id: root
    property var backend
    readonly property var traffic: backend ? backend.traffic : null
    property bool followTail: true
    property string exportResult: ""
    property var exportModel: null
    signal closeRequested()
    signal statisticsRequested()
    function inspectExpanded() { expanded.open() }
    color: Theme.surface
    radius: 7; border.width: 1; border.color: Theme.divider

    function follow() {
        if (!visible || !followTail) return
        Qt.callLater(function() {
            if (!root.visible || !root.followTail) return
            messages.forceLayout()
            messages.positionViewAtEnd()
        })
    }
    onTrafficChanged: { followTail = true; exportResult = ""; follow() }
    onVisibleChanged: if (visible) follow()
    Connections {
        target: root.traffic
        function onRowsInserted() { root.follow() }
        function onModelReset() { root.follow() }
    }

    component DockButton: ToolButton {
        property string iconKind: ""
        property string tip: ""
        property color accent: Theme.textSecondary
        implicitWidth: 28; implicitHeight: 28; padding: 4
        Accessible.name: tip
        contentItem: AppIcon { kind: parent.iconKind; color: parent.enabled ? parent.accent : Theme.textTertiary }
        background: Rectangle { radius: 4; color: parent.hovered ? Theme.hoverFill : "transparent"; border.width: parent.activeFocus ? 1 : 0; border.color: Theme.primary }
        ToolTip.visible: hovered; ToolTip.delay: 400; ToolTip.text: tip
    }

    ColumnLayout {
        anchors.fill: parent; anchors.margins: 1; spacing: 0
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 41; Layout.minimumHeight: 41; Layout.maximumHeight: 41; color: Theme.surface; radius: 7
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider }
            Rectangle { anchors.bottom: parent.bottom; x: 13; width: 48; height: 2; color: Theme.cyan }
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 8; spacing: 6
                Text { text: I18n.t("Traffic monitor", "报文监视"); color: Theme.cyan; font.pixelSize: 11 }
                Text { text: root.traffic ? String(root.traffic.count) : "0"; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                AppButton { compact: true; variant: "text"; textPixelSize: 11; text: I18n.t("Statistics", "统计"); accent: Theme.textTertiary; onClicked: root.statisticsRequested() }
                Text { visible: root.traffic && root.traffic.paused; text: I18n.t("Paused", "显示已暂停"); color: Theme.warning; font.pixelSize: 10 }
                Item { Layout.fillWidth: true }
                Text { visible: root.width > 680; text: "HEX"; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                DockButton { iconKind: "down"; accent: root.followTail ? Theme.cyan : Theme.textTertiary; tip: I18n.t("Follow latest messages", "跟随最新报文"); onClicked: { root.followTail = !root.followTail; root.follow() } }
                DockButton { iconKind: root.traffic && root.traffic.paused ? "play" : "pause"; accent: root.traffic && root.traffic.paused ? Theme.warning : Theme.textSecondary; tip: I18n.t("Pause / resume display only", "暂停 / 继续显示，通信继续运行"); enabled: !!root.traffic; onClicked: root.traffic.paused = !root.traffic.paused }
                DockButton { iconKind: "clear"; tip: I18n.t("Clear traffic", "清空报文"); enabled: root.traffic && root.traffic.count > 0; onClicked: clearConfirm.open() }
                DockButton { iconKind: "save"; tip: I18n.t("Export traffic", "导出报文"); enabled: root.traffic && root.traffic.count > 0; onClicked: { root.exportModel = root.traffic; exportDialog.open() } }
                DockButton { iconKind: "maximize"; tip: I18n.t("Expand traffic monitor", "展开报文监视"); onClicked: expanded.open() }
                DockButton { iconKind: "close"; tip: I18n.t("Hide traffic monitor", "收起报文监视"); onClicked: root.closeRequested() }
            }
        }
        RowLayout {
            Layout.fillWidth: true; Layout.preferredHeight: 28; Layout.minimumHeight: 28; Layout.maximumHeight: 28; Layout.leftMargin: 12; Layout.rightMargin: 12; spacing: 10
            Text { Layout.preferredWidth: 96; text: I18n.t("TIME", "时间"); color: Theme.textTertiary; font.pixelSize: 10 }
            Text { Layout.preferredWidth: 36; text: I18n.t("DIR", "方向"); color: Theme.textTertiary; font.pixelSize: 10 }
            Text { Layout.fillWidth: true; text: I18n.t("FRAME / MESSAGE", "报文 / 消息"); color: Theme.textTertiary; font.pixelSize: 10 }
            Text { Layout.preferredWidth: 38; text: "BYTES"; color: Theme.textTertiary; font.pixelSize: 9; horizontalAlignment: Text.AlignRight }
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
        ListView {
            id: messages
            Layout.fillWidth: true; Layout.fillHeight: true; clip: true
            model: root.visible ? root.traffic : null
            ScrollBar.vertical: ScrollBar { }
            onMovementStarted: root.followTail = false
            onMovementEnded: if (atYEnd) root.followTail = true
            delegate: Rectangle {
                id: message
                required property string time
                required property int dir
                required property string dataHex
                required property string dataText
                required property int size
                required property bool plain
                required property int index
                width: messages.width; height: 25
                color: Theme.surface
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12; spacing: 10
                    Text { Layout.preferredWidth: 96; text: message.time; color: Theme.textTertiary; font.pixelSize: 11; font.family: "Consolas"; elide: Text.ElideRight }
                    Rectangle {
                        Layout.preferredWidth: 36; Layout.preferredHeight: 16; radius: 3
                        readonly property color accent: message.dir === 0 ? Theme.rx : message.dir === 1 ? Theme.tx : message.dir === 3 ? Theme.error : Theme.textSecondary
                        color: Theme.alpha(accent, 0.10)
                        Text { anchors.centerIn: parent; text: ["RX", "TX", "INFO", "ERR"][message.dir] || "--"; color: parent.accent; font.pixelSize: 9; font.family: "Consolas" }
                    }
                    Text { Layout.fillWidth: true; text: message.plain ? message.dataText : message.dataHex; color: message.dir === 3 ? Theme.error : Theme.textSecondary; font.pixelSize: 11; font.family: "Consolas"; elide: Text.ElideRight }
                    Text { Layout.preferredWidth: 38; text: message.size; color: Theme.textTertiary; font.pixelSize: 10; font.family: "Consolas"; horizontalAlignment: Text.AlignRight }
                }
                ToolTip.visible: hover.hovered; ToolTip.text: message.plain ? message.dataText : message.dataHex
                HoverHandler { id: hover }
            }
            Text { anchors.centerIn: parent; visible: messages.count === 0; text: I18n.t("Waiting for channel traffic", "等待通道收发报文"); color: Theme.textTertiary; font.pixelSize: 11 }
        }
        Text { visible: root.exportResult.length > 0; Layout.leftMargin: 12; Layout.preferredHeight: 20; text: root.exportResult; color: Theme.textSecondary; font.pixelSize: 10 }
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 28; Layout.minimumHeight: 28; Layout.maximumHeight: 28
            color: "transparent"
            Rectangle { width: parent.width; height: 1; color: Theme.divider }
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12; spacing: 18
                Text { text: root.backend ? "TX " + root.backend.txCount : "TX 0"; color: Theme.textTertiary; font.pixelSize: 10; font.family: "Consolas" }
                Text { text: root.backend ? "RX " + root.backend.rxCount : "RX 0"; color: Theme.textTertiary; font.pixelSize: 10; font.family: "Consolas" }
                Text { text: root.backend ? I18n.t("Errors ", "异常 ") + root.backend.errorCount : ""; color: root.backend && root.backend.errorCount > 0 ? Theme.error : Theme.textTertiary; font.pixelSize: 10 }
                Item { Layout.fillWidth: true }
                Text { text: root.followTail ? I18n.t("Following latest", "自动跟随") : I18n.t("Manual browse", "手动浏览"); color: Theme.textTertiary; font.pixelSize: 10 }
            }
        }
    }

    FileDialog {
        id: exportDialog
        title: I18n.t("Export traffic", "导出报文")
        fileMode: FileDialog.SaveFile; nameFilters: ["Text files (*.txt)"]
        onAccepted: root.exportResult = root.exportModel && root.exportModel.exportToFile(selectedFile)
                         ? I18n.t("Traffic exported", "报文已导出") : I18n.t("Export failed", "报文导出失败")
    }
    ConfirmDialog {
        id: clearConfirm
        titleText: I18n.t("Clear traffic", "清空报文")
        messageText: I18n.t("Clear the current channel's displayed traffic?", "清空当前通道已显示的报文？")
        noteText: I18n.t("Connection and polling continue.", "连接与轮询继续运行。")
        onOperationConfirmed: if (root.traffic) root.traffic.clear()
    }
    Dialog {
        id: expanded
        parent: Overlay.overlay; anchors.centerIn: parent
        width: Math.min(1200, parent.width - 48); height: Math.min(720, parent.height - 64)
        padding: 0; modal: true; closePolicy: Popup.CloseOnEscape
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.border }
        header: Rectangle {
            implicitHeight: 54; color: Theme.dialogHeader
            Text { anchors.left: parent.left; anchors.leftMargin: 16; anchors.verticalCenter: parent.verticalCenter; text: I18n.t("Traffic monitor", "报文监视"); color: Theme.textPrimary; font.pixelSize: 14; font.weight: Font.DemiBold }
            DockButton { anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter; width: 34; height: 34; iconKind: "close"; padding: 7; focusPolicy: Qt.NoFocus; tip: I18n.t("Close", "关闭"); onClicked: expanded.close() }
        }
        contentItem: TerminalView { model: expanded.visible ? root.traffic : null; decodeModbus: true }
    }
}
