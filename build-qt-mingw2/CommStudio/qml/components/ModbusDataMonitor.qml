import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root
    property var backend
    property string section: "data"
    signal newPointRequested()
    signal deleteRequested()
    signal addDeviceRequested()
    signal trendRequested()
    signal diagnosticsRequested()
    signal advancedRequested()
    signal controlRequested()
    signal editPointRequested(int row)

    function channelCaption() {
        if (!backend) return "MODBUS"
        if (backend.mode === "tcpMaster" || backend.mode === "tcpSlave")
            return (backend.host || "127.0.0.1") + ":" + backend.port
        return backend.portName || I18n.t("Serial channel", "串口通道")
    }
    function valueText(point) {
        if (point.value === undefined || point.value === null || String(point.value).length === 0) return "--"
        return String(point.value)
    }
    function rangeText(point) {
        const min = point.minValue === undefined ? "" : String(point.minValue)
        const max = point.maxValue === undefined ? "" : String(point.maxValue)
        if (!min && !max) return "--"
        return (min || "-") + " … " + (max || "-")
    }
    function areaText(fc) {
        return ["线圈", "离散输入", "保持寄存器", "输入寄存器"][Number(fc) - 1] || ("FC" + fc)
    }
    function writable(fc) {
        return Number(fc) === 1 || Number(fc) === 3
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            color: Theme.surfaceRaised
            border.color: Theme.divider
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 4
                Repeater {
                    model: [
                        {key: "data", label: I18n.t("Data", "数据"), icon: "▤"},
                        {key: "system", label: I18n.t("System", "系统"), icon: "◈"},
                        {key: "alarm", label: I18n.t("Alarm", "告警"), icon: "♧"},
                        {key: "history", label: I18n.t("History", "历史"), icon: "◷"},
                        {key: "control", label: I18n.t("Control", "控制"), icon: "⌁"}
                    ]
                    delegate: AppButton {
                        required property var modelData
                        compact: true
                        variant: "text"
                        text: modelData.icon + "  " + modelData.label
                        accent: root.section === modelData.key ? Theme.primary : Theme.textSecondary
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: 2
                            color: Theme.primary
                            visible: root.section === modelData.key
                        }
                        onClicked: {
                            root.section = modelData.key
                            if (modelData.key === "system") root.advancedRequested()
                            else if (modelData.key === "alarm") root.diagnosticsRequested()
                            else if (modelData.key === "history") root.trendRequested()
                            else if (modelData.key === "control") root.controlRequested()
                        }
                    }
                }
                Item { Layout.fillWidth: true }
                StatusPill {
                    text: backend && backend.polling ? I18n.t("RUNNING", "运行中") : I18n.t("IDLE", "空闲")
                    accent: backend && backend.polling ? Theme.success : Theme.textTertiary
                    scaleFactor: 0.78
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: Theme.chromeBody
            border.color: Theme.divider
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 6
                AppButton { compact: true; text: "↗"; ToolTip.visible: hovered; ToolTip.text: I18n.t("Share / menu", "分享 / 菜单") }
                Rectangle { width: 1; Layout.fillHeight: true; Layout.topMargin: 10; Layout.bottomMargin: 10; color: Theme.divider }
                AppButton { compact: true; text: "＋"; accent: Theme.primary; onClicked: root.newPointRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Add point", "新增数据点") }
                AppButton { compact: true; text: "×"; variant: "danger"; onClicked: root.deleteRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Delete selected", "删除选中数据点") }
                Rectangle { width: 1; Layout.fillHeight: true; Layout.topMargin: 10; Layout.bottomMargin: 10; color: Theme.divider }
                AppButton { compact: true; text: "▶"; accent: Theme.success; onClicked: backend.startPolling(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Start polling", "启动轮询") }
                AppButton { compact: true; text: "Ⅱ"; accent: Theme.warning; onClicked: backend.stopPolling(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Pause polling", "暂停轮询") }
                AppTextField { id: pointSearch; Layout.preferredWidth: 160; implicitHeight: 32; placeholder: I18n.t("Search name", "填写名称"); onTextChanged: if (backend) backend.points.search = text }
                AppButton { compact: true; text: "⌕"; variant: "outlined"; onClicked: pointSearch.forceActiveFocus() }
                Rectangle { width: 1; Layout.fillHeight: true; Layout.topMargin: 10; Layout.bottomMargin: 10; color: Theme.divider }
                AppButton { compact: true; text: "＋"; onClicked: root.newPointRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Add data point", "新增数据点") }
                AppButton { compact: true; text: "−"; variant: "danger"; onClicked: root.deleteRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Delete data point", "删除数据点") }
                AppButton { compact: true; text: "∿"; accent: Theme.cyan; onClicked: root.trendRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Trend", "趋势") }
                AppButton { compact: true; text: "▣"; onClicked: root.diagnosticsRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Statistics / diagnostics", "统计 / 诊断") }
                AppButton { compact: true; text: "⚙"; onClicked: root.advancedRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Settings", "设置") }
                Item { Layout.fillWidth: true }
                Text { text: backend ? "TX " + backend.txCount + " · RX " + backend.rxCount : ""; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                id: treePane
                Layout.preferredWidth: 224
                Layout.minimumWidth: 190
                Layout.fillHeight: true
                color: Theme.sidebar
                border.width: 1
                border.color: Theme.divider

                ScrollView {
                    anchors.fill: parent
                    clip: true
                    contentWidth: availableWidth
                    ColumnLayout {
                        width: parent.width
                        spacing: 0
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 38
                            color: Theme.surfaceRaised
                            Text { anchors.left: parent.left; anchors.leftMargin: 12; anchors.verticalCenter: parent.verticalCenter; text: I18n.t("CHANNEL", "通道"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            color: Theme.hoverFill
                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 8; spacing: 6
                                Text { text: "⌄"; color: Theme.textPrimary; font.pixelSize: 16 }
                                Rectangle { width: 18; height: 18; radius: 4; color: Theme.alpha(Theme.primary, 0.16); border.color: Theme.alpha(Theme.primary, 0.5); Text { anchors.centerIn: parent; text: "⌁"; color: Theme.primary; font.pixelSize: 12 } }
                                ColumnLayout { Layout.fillWidth: true; spacing: 0; Text { text: root.channelCaption(); color: Theme.textPrimary; font.family: "Consolas"; font.pixelSize: 12; elide: Text.ElideRight } Text { text: backend ? backend.mode : "MODBUS"; color: Theme.textTertiary; font.pixelSize: 10; elide: Text.ElideRight } }
                                StatusPill { text: backend && backend.open ? "ON" : "OFF"; accent: backend && backend.open ? Theme.success : Theme.textTertiary; dot: false; scaleFactor: 0.68 }
                            }
                        }
                        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
                        RowLayout {
                            Layout.fillWidth: true; Layout.preferredHeight: 36; Layout.leftMargin: 12; Layout.rightMargin: 8
                            Text { Layout.fillWidth: true; text: I18n.t("DEVICES", "设备"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                            AppButton { compact: true; text: "+"; onClicked: root.addDeviceRequested(); ToolTip.visible: hovered; ToolTip.text: I18n.t("Add device", "添加设备") }
                        }
                        Repeater {
                            model: backend ? backend.devices : []
                            delegate: Rectangle {
                                required property string name
                                required property int unitId
                                required property bool online
                                required property string deviceId
                                required property int index
                                Layout.fillWidth: true
                                Layout.preferredHeight: 38
                                color: index === backend.selectedDeviceIndex ? Theme.selectedFill : "transparent"
                                RowLayout {
                                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 8; spacing: 6
                                    Rectangle { width: 16; height: 16; radius: 3; color: index === backend.selectedDeviceIndex ? Theme.primary : Theme.inputBackground; border.color: Theme.border; Text { anchors.centerIn: parent; visible: index === backend.selectedDeviceIndex; text: "✓"; color: "#FFFFFF"; font.pixelSize: 11 } }
                                    Rectangle { width: 14; height: 14; radius: 7; color: online ? Theme.success : Theme.textTertiary }
                                    Text { Layout.fillWidth: true; text: (unitId < 10 ? "0" : "") + unitId + "  " + name; color: index === backend.selectedDeviceIndex ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 12; elide: Text.ElideRight }
                                }
                                MouseArea { anchors.fill: parent; onClicked: backend.selectDevice(index) }
                            }
                        }
                        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
                        RowLayout {
                            Layout.fillWidth: true; Layout.preferredHeight: 36; Layout.leftMargin: 12; Layout.rightMargin: 8
                            Text { Layout.fillWidth: true; text: I18n.t("POINT GROUPS", "数据点组"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                            Text { text: backend ? backend.points.rowCount() : 0; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                        }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 32
                            color: backend && backend.points.groupFilter === "" ? Theme.hoverFill : "transparent"
                            RowLayout { anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 8; Text { Layout.fillWidth: true; text: I18n.t("All points", "全部数据点"); color: Theme.textSecondary; font.pixelSize: 12 } Text { text: backend ? backend.points.rowCount() : 0; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 } }
                            MouseArea { anchors.fill: parent; onClicked: if (backend) backend.points.groupFilter = "" }
                        }
                        Repeater {
                            model: backend ? backend.points.groups : []
                            delegate: Rectangle {
                                required property string key
                                required property string name
                                required property int count
                                Layout.fillWidth: true; Layout.preferredHeight: 32
                                color: backend && backend.points.groupFilter === key ? Theme.hoverFill : "transparent"
                                RowLayout { anchors.fill: parent; anchors.leftMargin: 26; anchors.rightMargin: 8; Text { text: "•"; color: Theme.cyan; font.pixelSize: 14 } Text { Layout.fillWidth: true; text: name; color: Theme.textSecondary; font.pixelSize: 11; elide: Text.ElideRight } Text { text: count; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 } }
                                MouseArea { anchors.fill: parent; onClicked: if (backend) backend.points.groupFilter = key }
                            }
                        }
                        Item { Layout.fillHeight: true; Layout.minimumHeight: 12 }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.surface
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 38; color: Theme.surfaceRaised; border.bottom.color: Theme.divider
                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 0
                            Text { Layout.preferredWidth: 48; text: "#"; color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 190; text: I18n.t("Name", "名称"); color: Theme.textSecondary; font.pixelSize: 11 }
                            Text { Layout.preferredWidth: 132; text: I18n.t("Value", "数值"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 94; text: I18n.t("Command", "指令"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 54; text: I18n.t("Write", "写"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 70; text: I18n.t("Unit", "单位"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 122; text: I18n.t("Range", "范围"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 130; text: I18n.t("Area", "区块"); color: Theme.textSecondary; font.pixelSize: 11 }
                            Text { Layout.preferredWidth: 64; text: I18n.t("Address", "地址"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 54; text: I18n.t("Count", "数量"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 64; text: I18n.t("Bit off", "位偏移"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: 54; text: I18n.t("Bits", "位数"); color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                            Item { Layout.fillWidth: true }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true; Layout.fillHeight: true; color: Theme.surface
                        ListView {
                            id: pointList
                            anchors.fill: parent
                            clip: true
                            model: backend ? backend.points.filtered : null
                            ScrollBar.vertical: ScrollBar {}
                            delegate: Rectangle {
                                id: pointRow
                                required property var point
                                required property int index
                                width: pointList.width
                                height: 40
                                color: point.selected ? Theme.selectedFill : (index % 2 === 0 ? Theme.surface : Theme.surfaceRaised)
                                border.width: 1; border.color: Theme.divider
                                MouseArea { anchors.fill: parent; z: -1; onDoubleClicked: root.editPointRequested(point.sourceRow); onClicked: backend.points.selectPoint(point.sourceRow, (mouse.modifiers & Qt.ControlModifier) !== 0, (mouse.modifiers & Qt.ShiftModifier) !== 0) }
                                RowLayout {
                                    anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 0
                                    Text { Layout.preferredWidth: 48; text: String(index + 1); color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { Layout.preferredWidth: 190; text: point.name || "--"; color: Theme.textPrimary; font.pixelSize: 12; elide: Text.ElideRight }
                                    Rectangle { Layout.preferredWidth: 132; Layout.fillHeight: true; color: Theme.alpha(Theme.cyan, Theme.dark ? 0.12 : 0.16); border.left.width: 1; border.right.width: 1; border.color: Theme.alpha(Theme.cyan, 0.22); Text { anchors.centerIn: parent; text: root.valueText(point); color: point.status === "OUT OF RANGE" ? Theme.warning : Theme.cyan; font.family: "Consolas"; font.pixelSize: 12; elide: Text.ElideRight; width: parent.width - 10; horizontalAlignment: Text.AlignHCenter } }
                                    Text { Layout.preferredWidth: 94; text: "FC" + point.fc; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Item { Layout.preferredWidth: 54; Layout.fillHeight: true; AppButton { anchors.centerIn: parent; compact: true; text: "写"; variant: "text"; accent: root.writable(point.fc) ? Theme.warning : Theme.textTertiary; enabled: backend && root.writable(point.fc) && backend.open; onClicked: backend.writePoint(point.sourceRow, String(point.writeValue || "")) } }
                                    Text { Layout.preferredWidth: 70; text: point.unit || "--"; color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
                                    Text { Layout.preferredWidth: 122; text: root.rangeText(point); color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
                                    Text { Layout.preferredWidth: 130; text: root.areaText(point.fc); color: Theme.textSecondary; font.pixelSize: 11; elide: Text.ElideRight }
                                    Text { Layout.preferredWidth: 64; text: String(point.address); color: Theme.textPrimary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { Layout.preferredWidth: 54; text: String(point.count || 1); color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { Layout.preferredWidth: 64; text: String(point.bitOffset || 0); color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { Layout.preferredWidth: 54; text: String(point.bitCount || 0); color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Item { Layout.fillWidth: true }
                                }
                            }
                            Text { anchors.centerIn: parent; visible: pointList.count === 0; text: I18n.t("No data points for this device", "当前设备还没有数据点"); color: Theme.textTertiary; font.pixelSize: 12 }
                        }
                    }
                }
            }
        }
    }
}
