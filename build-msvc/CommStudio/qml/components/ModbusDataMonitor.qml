import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root
    clip: true
    property var backend
    property string section: "data"
    property var expandedChannels: ({})
    property var checkedTreeDevices: ({})
    property bool showDeviceDetails: false
    property int selectedTreeChannelIndex: -1
    property int selectedTreeDeviceIndex: -1
    property int contextDeviceIndex: -1
    readonly property int pageHeaderHeight: 42
    readonly property int toolBarHeight: 50
    readonly property int tableHeaderHeight: 40
    readonly property int toolButtonWidth: 36
    readonly property int treePreferredWidth: showDeviceDetails ? 600 : 336
    readonly property int treeChannelColumnWidth: 92
    readonly property int treeNameColumnWidth: showDeviceDetails ? 220 : treePreferredWidth - treeChannelColumnWidth - 1
    readonly property int treeTypeColumnWidth: 120
    readonly property int treeIdColumnWidth: 82
    readonly property int treeAddressColumnWidth: 82
    readonly property int numberColumnWidth: 42
    readonly property int valueColumnWidth: 112
    readonly property int commandColumnWidth: 76
    readonly property int writeColumnWidth: 48
    readonly property int unitColumnWidth: 58
    readonly property int rangeColumnWidth: 110
    readonly property int areaColumnWidth: 112
    readonly property int addressColumnWidth: 64
    readonly property int countColumnWidth: 52
    readonly property int bitOffsetColumnWidth: 64
    readonly property int bitCountColumnWidth: 52
    readonly property bool showRangeColumn: tablePane.width >= 880
    readonly property bool showAreaColumn: tablePane.width >= 1000
    readonly property bool showCountColumn: tablePane.width >= 1110
    readonly property bool showBitColumns: tablePane.width >= 1260
    signal newPointRequested()
    signal deleteRequested()
    signal addDeviceRequested()
    signal removeDeviceRequested()
    signal removeDevicesRequested(var selections)
    signal editDeviceRequested(int index)
    signal channelManagerRequested()
    signal configureChannelRequested()
    signal trendRequested()
    signal diagnosticsRequested()
    signal advancedRequested()
    signal batchEditRequested()
    signal copyPointsRequested()
    signal importPointsRequested()
    signal exportPointsRequested()
    signal pastePointsRequested()
    signal sortPointsRequested()
    signal controlRequested()
    signal editPointRequested(int row)
    signal addChannelRequested()
    signal renameChannelRequested()
    signal removeChannelRequested()

    function currentChannel() {
        const channels = ModbusWorkspace.channels
        const index = ModbusWorkspace.currentIndex
        return index >= 0 && index < channels.length ? channels[index] : null
    }
    function treeChannels() {
        const channels = ModbusWorkspace.channels
        const serialChannels = []
        for (let index = 0; index < channels.length; ++index) {
            const channel = channels[index]
            if (channel.transport === "serial" && channel.available)
                serialChannels.push(channel)
        }
        return serialChannels
    }
    function isChannelExpanded(channelId) {
        const state = expandedChannels[String(channelId)]
        return state === undefined ? true : state
    }
    function toggleChannel(channelId) {
        const key = String(channelId)
        const next = Object.assign({}, expandedChannels)
        next[key] = !isChannelExpanded(key)
        expandedChannels = next
    }
    function deviceSelectionKey(channelId, deviceId) {
        return String(channelId) + "|" + String(deviceId)
    }
    function isDeviceChecked(channelId, deviceId) {
        return checkedTreeDevices[deviceSelectionKey(channelId, deviceId)] === true
    }
    function channelCheckState(channelId, deviceModel) {
        if (!deviceModel) return 0
        const devices = deviceModel.definitions()
        if (devices.length === 0) return 0
        let checked = 0
        for (let index = 0; index < devices.length; ++index) {
            if (isDeviceChecked(channelId, devices[index].id))
                ++checked
        }
        if (checked === 0) return 0
        return checked === devices.length ? 2 : 1
    }
    function toggleChannelChecked(channelId, deviceModel) {
        if (!deviceModel) return
        const devices = deviceModel.definitions()
        const shouldCheck = channelCheckState(channelId, deviceModel) !== 2
        const next = Object.assign({}, checkedTreeDevices)
        for (let index = 0; index < devices.length; ++index) {
            const key = deviceSelectionKey(channelId, devices[index].id)
            if (shouldCheck)
                next[key] = true
            else
                delete next[key]
        }
        checkedTreeDevices = next
    }
    function toggleDeviceChecked(channelId, deviceId) {
        const key = deviceSelectionKey(channelId, deviceId)
        const next = Object.assign({}, checkedTreeDevices)
        if (next[key] === true)
            delete next[key]
        else
            next[key] = true
        checkedTreeDevices = next
    }
    function checkedDeviceSelections() {
        const selections = []
        const channels = treeChannels()
        for (let channelOffset = 0; channelOffset < channels.length; ++channelOffset) {
            const channel = channels[channelOffset]
            const channelBackend = ModbusWorkspace.channelBackend(Number(channel.index))
            if (!channelBackend) continue
            const devices = channelBackend.devices.definitions()
            for (let deviceIndex = 0; deviceIndex < devices.length; ++deviceIndex) {
                const device = devices[deviceIndex]
                if (isDeviceChecked(channel.id, device.id)) {
                    selections.push({ channelIndex: Number(channel.index),
                                      channelId: String(channel.id),
                                      deviceIndex: deviceIndex,
                                      deviceId: String(device.id),
                                      name: String(device.name) })
                }
            }
        }
        return selections
    }
    function clearCheckedDevices() {
        checkedTreeDevices = ({})
        selectedTreeChannelIndex = -1
        selectedTreeDeviceIndex = -1
        contextDeviceIndex = -1
    }
    function channelCaptionFor(channel) {
        return channel ? (channel.portName || channel.channelName || "COM") : "COM"
    }
    function channelDetailFor(channel) {
        if (!channel) return "--"
        const modeLabels = {
            "rtuMaster": "RTU · MASTER", "asciiMaster": "ASCII · MASTER",
            "rtuSlave": "RTU · SLAVE", "asciiSlave": "ASCII · SLAVE"
        }
        const mode = modeLabels[channel.mode] || channel.mode
        const alias = String(channel.alias || "")
        return alias.length > 0 ? alias + "  ·  " + mode : mode
    }
    function channelCaption() {
        const channel = currentChannel()
        return channel ? (channel.channelName || channel.portName || channel.name || "MODBUS") : "MODBUS"
    }
    function channelDetail() {
        const channel = currentChannel()
        if (!channel) return "--"
        const modeLabels = {
            "rtuMaster": "RTU · MASTER", "asciiMaster": "ASCII · MASTER",
            "tcpMaster": "TCP · MASTER", "rtuSlave": "RTU · SLAVE",
            "asciiSlave": "ASCII · SLAVE", "tcpSlave": "TCP · SLAVE"
        }
        const mode = modeLabels[channel.mode] || channel.mode
        if (channel.transport === "serial") {
            const alias = String(channel.alias || "")
            return alias.length > 0 ? alias + "  ·  " + mode : mode
        }
        return channel.endpoint + "  ·  " + mode
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
        return [1, 5, 6, 15, 16].indexOf(Number(fc)) >= 0
    }

    component TreeMenuItem: MenuItem {
        id: treeMenuItem
        implicitWidth: 218
        implicitHeight: 40
        leftPadding: 12
        rightPadding: 12
        font.family: Theme.fontFamily
        font.pixelSize: 12
        contentItem: Text {
            text: treeMenuItem.text
            color: treeMenuItem.enabled ? Theme.textPrimary : Theme.textTertiary
            font: treeMenuItem.font
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        background: Rectangle {
            radius: Theme.radiusSmall
            color: treeMenuItem.highlighted ? Theme.hoverFill : "transparent"
            Behavior on color { ColorAnimation { duration: Theme.durationFast } }
        }
    }

    component ToolbarGlyph: Canvas {
        id: toolbarGlyph
        property string kind: ""
        property color accent: Theme.textPrimary
        implicitWidth: 30
        implicitHeight: 30
        antialiasing: true
        onKindChanged: requestPaint()
        onAccentChanged: requestPaint()
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        Component.onCompleted: requestPaint()
        function polygon(ctx, points) {
            ctx.beginPath()
            ctx.moveTo(points[0][0], points[0][1])
            for (let index = 1; index < points.length; ++index)
                ctx.lineTo(points[index][0], points[index][1])
            ctx.closePath()
            ctx.stroke()
        }
        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.strokeStyle = toolbarGlyph.accent
            ctx.fillStyle = "transparent"
            ctx.lineWidth = 2.2
            ctx.lineCap = "round"
            ctx.lineJoin = "round"
            if (kind === "search") {
                ctx.beginPath(); ctx.arc(12, 12, 8, 0, Math.PI * 2); ctx.stroke()
                ctx.beginPath(); ctx.moveTo(18, 18); ctx.lineTo(27, 27); ctx.stroke()
            } else if (kind === "wave") {
                ctx.beginPath(); ctx.moveTo(2, 17); ctx.lineTo(7, 17); ctx.lineTo(10, 6); ctx.lineTo(14, 27); ctx.lineTo(18, 9); ctx.lineTo(22, 21); ctx.lineTo(29, 21); ctx.stroke()
            } else if (kind === "inspect") {
                ctx.strokeRect(4, 4, 18, 22)
                ctx.beginPath(); ctx.moveTo(8, 9); ctx.lineTo(18, 9); ctx.moveTo(8, 14); ctx.lineTo(16, 14); ctx.stroke()
                ctx.beginPath(); ctx.arc(21, 21, 5, 0, Math.PI * 2); ctx.stroke()
                ctx.beginPath(); ctx.moveTo(25, 25); ctx.lineTo(29, 29); ctx.stroke()
            } else if (kind === "copy") {
                ctx.strokeRect(8, 4, 17, 21); ctx.strokeRect(4, 8, 17, 21)
                ctx.beginPath(); ctx.moveTo(12, 10); ctx.lineTo(20, 10); ctx.moveTo(9, 15); ctx.lineTo(17, 15); ctx.stroke()
            } else if (kind === "nodes") {
                polygon(ctx, [[9,3],[15,6],[15,13],[9,16],[3,13],[3,6]])
                polygon(ctx, [[22,10],[28,13],[28,20],[22,23],[16,20],[16,13]])
                polygon(ctx, [[9,17],[15,20],[15,27],[9,30],[3,27],[3,20]])
            } else if (kind === "grid") {
                ctx.strokeRect(3, 3, 11, 11); ctx.strokeRect(18, 3, 11, 11)
                ctx.strokeRect(3, 18, 11, 11); ctx.strokeRect(18, 18, 11, 11)
            } else if (kind === "rows") {
                ctx.beginPath(); ctx.moveTo(5, 7); ctx.lineTo(27, 7); ctx.moveTo(5, 16); ctx.lineTo(22, 16); ctx.moveTo(5, 25); ctx.lineTo(27, 25); ctx.stroke()
                ctx.strokeStyle = Theme.primary; ctx.beginPath(); ctx.moveTo(5, 16); ctx.lineTo(13, 16); ctx.stroke()
            } else if (kind === "columns") {
                ctx.beginPath(); ctx.moveTo(7, 5); ctx.lineTo(7, 27); ctx.moveTo(16, 5); ctx.lineTo(16, 22); ctx.moveTo(25, 5); ctx.lineTo(25, 27); ctx.stroke()
                ctx.strokeStyle = Theme.primary; ctx.beginPath(); ctx.moveTo(16, 5); ctx.lineTo(16, 15); ctx.stroke()
            } else if (kind === "edit") {
                ctx.beginPath(); ctx.moveTo(5, 25); ctx.lineTo(11, 23); ctx.lineTo(27, 7); ctx.lineTo(22, 2); ctx.lineTo(6, 18); ctx.closePath(); ctx.stroke()
                ctx.beginPath(); ctx.moveTo(5, 29); ctx.lineTo(27, 29); ctx.stroke()
            }
        }
    }

    component TreeToolButton: AbstractButton {
        id: treeToolButton
        property color accent: Theme.textPrimary
        property string tip: ""
        property string iconKind: ""
        property bool outlined: false
        implicitWidth: root.toolButtonWidth
        implicitHeight: root.toolButtonWidth
        Layout.minimumWidth: root.toolButtonWidth
        Layout.preferredWidth: root.toolButtonWidth
        Layout.maximumWidth: root.toolButtonWidth
        Layout.minimumHeight: root.toolButtonWidth
        Layout.preferredHeight: root.toolButtonWidth
        Layout.maximumHeight: root.toolButtonWidth
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        hoverEnabled: true
        contentItem: Item {
            ToolbarGlyph {
                anchors.centerIn: parent
                width: 30
                height: 30
                visible: treeToolButton.iconKind.length > 0
                kind: treeToolButton.iconKind
                accent: treeToolButton.enabled ? treeToolButton.accent : Theme.alpha(Theme.textTertiary, 0.48)
            }
            Text {
                anchors.fill: parent
                visible: treeToolButton.iconKind.length === 0
                text: treeToolButton.text
                color: treeToolButton.enabled
                       ? (treeToolButton.down ? Theme.primary : treeToolButton.accent)
                       : Theme.alpha(Theme.textTertiary, 0.48)
                font.family: Theme.fontFamily
                font.pixelSize: 19
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        background: Rectangle {
            radius: width / 2
            color: treeToolButton.down ? Theme.alpha(Theme.primary, 0.28)
                                       : (treeToolButton.hovered ? Theme.hoverFill : "transparent")
            border.width: treeToolButton.outlined ? 2 : (treeToolButton.activeFocus ? 1 : 0)
            border.color: treeToolButton.enabled ? treeToolButton.accent : Theme.alpha(Theme.textTertiary, 0.48)
        }
        ToolTip.visible: hovered && tip.length > 0
        ToolTip.delay: 550
        ToolTip.text: tip
    }

    component ChannelGlyph: Item {
        implicitWidth: 30
        implicitHeight: 28
        property color accent: Theme.primary
        Rectangle { x: 4; y: 4; width: 22; height: 7; radius: 1; color: "transparent"; border.width: 2; border.color: parent.accent }
        Rectangle { x: 8; y: 13; width: 14; height: 3; radius: 1; color: parent.accent }
        Rectangle { x: 10; y: 18; width: 10; height: 3; radius: 1; color: parent.accent }
        Rectangle { x: 13; y: 23; width: 4; height: 4; radius: 1; color: parent.accent }
        Rectangle { x: 8; y: 6; width: 2; height: 2; radius: 1; color: parent.accent }
        Rectangle { x: 13; y: 6; width: 2; height: 2; radius: 1; color: parent.accent }
    }

    component DeviceGlyph: Item {
        implicitWidth: 28
        implicitHeight: 28
        property bool online: false
        Text {
            anchors.centerIn: parent
            text: "⌁"
            rotation: -24
            color: parent.online ? Theme.success : Theme.error
            font.family: Theme.fontFamily
            font.pixelSize: 25
            font.weight: Font.DemiBold
        }
        Rectangle {
            visible: !parent.online
            anchors.centerIn: parent
            width: 27
            height: 2
            radius: 1
            rotation: -49
            color: Theme.error
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        radius: Theme.radius
        border.width: 1
        border.color: Theme.divider
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            visible: false
            Layout.fillWidth: true
            Layout.preferredHeight: 0
            color: Theme.surfaceRaised
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 10
                Text { text: I18n.t("Data monitor", "数据监控"); color: Theme.textPrimary; font.pixelSize: 14; font.weight: Font.DemiBold }
                StatusPill { text: root.currentChannel() && root.currentChannel().open ? I18n.t("CONNECTED", "已连接") : I18n.t("DISCONNECTED", "未连接"); accent: root.currentChannel() && root.currentChannel().open ? Theme.success : Theme.textTertiary; scaleFactor: 0.76 }
                Item { Layout.fillWidth: true }
                Text { text: backend ? ("TX " + backend.txCount + "  ·  RX " + backend.rxCount) : ""; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
            }
            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.toolBarHeight
            color: Theme.surfaceRaised
            RowLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.preferredWidth: root.treePreferredWidth
                    Layout.minimumWidth: root.treePreferredWidth
                    Layout.maximumWidth: root.treePreferredWidth
                    Layout.fillHeight: true
                    color: Theme.surfaceRaised

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 7
                        anchors.rightMargin: 7
                        spacing: 4

                        TreeToolButton {
                            id: channelManagerButton
                            tip: I18n.t("Channel manager", "通道管理")
                            onClicked: root.channelManagerRequested()
                            contentItem: Item {
                                Canvas {
                                    anchors.fill: parent
                                    onPaint: {
                                        const ctx = getContext("2d")
                                        ctx.clearRect(0, 0, width, height)
                                        ctx.strokeStyle = channelManagerButton.enabled ? Theme.textPrimary : Theme.textTertiary
                                        ctx.lineWidth = 2
                                        ctx.beginPath()
                                        ctx.moveTo(10, 17); ctx.lineTo(22, 9)
                                        ctx.moveTo(10, 17); ctx.lineTo(22, 25)
                                        ctx.stroke()
                                        const nodes = [[8,17],[24,8],[24,26]]
                                        for (let index = 0; index < nodes.length; ++index) {
                                            ctx.beginPath()
                                            ctx.arc(nodes[index][0], nodes[index][1], 4, 0, Math.PI * 2)
                                            ctx.stroke()
                                        }
                                    }
                                }
                            }
                        }
                        TreeToolButton { text: "+"; outlined: true; accent: Theme.textPrimary; tip: I18n.t("Add device", "添加设备"); enabled: root.treeChannels().length > 0; onClicked: root.addDeviceRequested() }
                        TreeToolButton {
                            text: "×"
                            outlined: true
                            accent: Theme.error
                            tip: I18n.t("Delete selected devices", "删除选中设备")
                            enabled: root.checkedDeviceSelections().length > 0
                            onClicked: root.removeDevicesRequested(root.checkedDeviceSelections())
                        }
                        TreeToolButton { text: "▷"; outlined: true; accent: Theme.cyan; tip: I18n.t("Start polling", "启动轮询"); enabled: backend && !backend.polling; onClicked: backend.startPolling() }
                        TreeToolButton { text: "Ⅱ"; outlined: true; accent: Theme.warning; tip: I18n.t("Pause polling", "暂停轮询"); enabled: backend && backend.polling; onClicked: backend.stopPolling() }
                        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 25; color: Theme.divider }
                        TreeToolButton {
                            text: root.showDeviceDetails ? "‹" : "›"
                            outlined: true
                            accent: Theme.textPrimary
                            tip: root.showDeviceDetails
                                 ? I18n.t("Hide device details", "收起设备详细信息")
                                 : I18n.t("Show device details", "展示设备详细信息")
                            onClicked: root.showDeviceDetails = !root.showDeviceDetails
                        }
                        Item { Layout.fillWidth: true }
                    }

                    Rectangle {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        width: 1
                        color: Theme.divider
                    }
                }

                Rectangle {
                    id: tableToolbar
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Theme.surfaceRaised

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 7

                        AppTextField { id: pointSearch; Layout.preferredWidth: Math.min(240, Math.max(140, tableToolbar.width * 0.28)); Layout.minimumWidth: 140; implicitHeight: 32; placeholder: I18n.t("Search name", "搜索名称"); onTextChanged: if (backend) backend.points.search = text }
                        TreeToolButton { iconKind: "search"; accent: Theme.textPrimary; tip: I18n.t("Search", "搜索"); onClicked: pointSearch.forceActiveFocus() }
                        TreeToolButton { text: "+"; outlined: true; accent: Theme.textPrimary; tip: I18n.t("Add point", "新增数据点"); onClicked: root.newPointRequested() }
                        TreeToolButton { text: "×"; outlined: true; accent: Theme.error; tip: I18n.t("Delete selected points", "删除选中数据点"); onClicked: root.deleteRequested() }
                        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 25; color: Theme.divider }
                        TreeToolButton { iconKind: "wave"; accent: Theme.textPrimary; tip: I18n.t("Real-time trend", "实时曲线"); onClicked: root.trendRequested() }
                        TreeToolButton { iconKind: "inspect"; accent: Theme.textPrimary; tip: I18n.t("Statistics and diagnostics", "统计与诊断"); onClicked: root.diagnosticsRequested() }
                        TreeToolButton { iconKind: "copy"; accent: Theme.textPrimary; tip: I18n.t("Copy data configuration", "复制数据配置"); onClicked: root.copyPointsRequested() }
                        TreeToolButton { iconKind: "nodes"; accent: Theme.textPrimary; tip: I18n.t("Import data template", "导入数据模板"); onClicked: root.importPointsRequested() }
                        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 25; color: Theme.divider }
                        TreeToolButton { iconKind: "grid"; accent: Theme.textPrimary; tip: I18n.t("Export data template", "导出数据模板"); onClicked: root.exportPointsRequested() }
                        TreeToolButton { iconKind: "rows"; accent: Theme.textPrimary; tip: I18n.t("Sort by address", "按地址排序"); onClicked: root.sortPointsRequested() }
                        TreeToolButton { iconKind: "columns"; accent: Theme.textPrimary; tip: I18n.t("Paste Excel rows", "粘贴 Excel 数据行"); onClicked: root.pastePointsRequested() }
                        TreeToolButton { iconKind: "edit"; accent: Theme.textPrimary; tip: I18n.t("Batch edit", "批量编辑"); onClicked: root.batchEditRequested() }
                        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 25; color: Theme.divider }
                        TreeToolButton { text: "⚙"; accent: Theme.textPrimary; tip: I18n.t("Device advanced parameters", "设备高级参数"); onClicked: root.advancedRequested() }
                    }
                }
            }
            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.alpha(Theme.primary, 0.34) }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                id: treePane
                Layout.preferredWidth: root.treePreferredWidth
                Layout.minimumWidth: root.treePreferredWidth
                Layout.maximumWidth: root.treePreferredWidth
                Layout.fillHeight: true
                color: Theme.sidebar

                ScrollView {
                    id: treeScroll
                    anchors.fill: parent
                    clip: true
                    contentWidth: availableWidth
                    ColumnLayout {
                        width: treeScroll.availableWidth
                        spacing: 0
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: root.tableHeaderHeight
                            color: Theme.surfaceRaised
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 0
                                anchors.rightMargin: 0
                                spacing: 0
                                Text {
                                    Layout.preferredWidth: root.treeChannelColumnWidth
                                    text: I18n.t("CHANNEL", "通道")
                                    color: Theme.textSecondary
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.divider }
                                Text {
                                    Layout.preferredWidth: root.treeNameColumnWidth
                                    text: I18n.t("NAME", "名称")
                                    color: Theme.textSecondary
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Rectangle { visible: root.showDeviceDetails; Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.divider }
                                Text {
                                    visible: root.showDeviceDetails
                                    Layout.preferredWidth: root.treeTypeColumnWidth
                                    text: I18n.t("DEVICE TYPE", "设备类型")
                                    color: Theme.textSecondary
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Rectangle { visible: root.showDeviceDetails; Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.divider }
                                Text {
                                    visible: root.showDeviceDetails
                                    Layout.preferredWidth: root.treeIdColumnWidth
                                    text: I18n.t("DEVICE ID", "设备 ID")
                                    color: Theme.textSecondary
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Rectangle { visible: root.showDeviceDetails; Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.divider }
                                Text {
                                    visible: root.showDeviceDetails
                                    Layout.preferredWidth: root.treeAddressColumnWidth
                                    text: I18n.t("ADDRESS", "地址")
                                    color: Theme.textSecondary
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
                        }
                        Repeater {
                            model: root.treeChannels()

                            delegate: ColumnLayout {
                                id: channelNode
                                required property var modelData
                                readonly property int sourceIndex: Number(modelData.index)
                                readonly property var channelBackend: ModbusWorkspace.channelBackend(sourceIndex)
                                readonly property bool expanded: root.isChannelExpanded(modelData.id)
                                readonly property bool focused: sourceIndex === root.selectedTreeChannelIndex
                                                               && root.selectedTreeDeviceIndex < 0
                                readonly property int checkState: root.channelCheckState(modelData.id,
                                                                                          channelBackend ? channelBackend.devices : null)
                                Layout.fillWidth: true
                                spacing: 0

                                Rectangle {
                                    id: channelRow
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 43
                                    color: channelNode.focused ? Theme.selectedFill
                                                                       : (channelMouse.containsMouse ? Theme.hoverFill : Theme.surfaceRaised)

                                    Rectangle {
                                        visible: channelNode.focused
                                        anchors.left: parent.left
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: 3
                                        height: 27
                                        radius: 2
                                        color: Theme.primary
                                    }

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.leftMargin: 7
                                        anchors.rightMargin: 8
                                        spacing: 5

                                        Item {
                                            Layout.preferredWidth: 14
                                            Layout.minimumWidth: 14
                                            Layout.maximumWidth: 14
                                            Layout.fillHeight: true
                                            Canvas {
                                                id: expandIndicator
                                                anchors.centerIn: parent
                                                width: 10
                                                height: 10
                                                onPaint: {
                                                    const ctx = getContext("2d")
                                                    ctx.clearRect(0, 0, width, height)
                                                    ctx.strokeStyle = Theme.textPrimary
                                                    ctx.lineWidth = 1.7
                                                    ctx.lineCap = "round"
                                                    ctx.lineJoin = "round"
                                                    ctx.beginPath()
                                                    if (channelNode.expanded) {
                                                        ctx.moveTo(1.5, 3.2); ctx.lineTo(5, 6.8); ctx.lineTo(8.5, 3.2)
                                                    } else {
                                                        ctx.moveTo(3.2, 1.5); ctx.lineTo(6.8, 5); ctx.lineTo(3.2, 8.5)
                                                    }
                                                    ctx.stroke()
                                                }
                                                Connections {
                                                    target: channelNode
                                                    function onExpandedChanged() { expandIndicator.requestPaint() }
                                                }
                                            }
                                        }
                                        Rectangle {
                                            Layout.preferredWidth: 18
                                            Layout.preferredHeight: 18
                                            radius: 4
                                            color: channelNode.checkState > 0 ? Theme.primary : Theme.inputBackground
                                            border.width: 1
                                            border.color: channelNode.checkState > 0 ? Theme.primary : Theme.border
                                            Text {
                                                anchors.centerIn: parent
                                                visible: channelNode.checkState > 0
                                                text: channelNode.checkState === 2 ? "✓" : "−"
                                                color: "#FFFFFF"
                                                font.pixelSize: 11
                                                font.weight: Font.Bold
                                            }
                                        }
                                        ChannelGlyph {
                                            Layout.preferredWidth: 30
                                            Layout.preferredHeight: 28
                                            accent: channelNode.modelData.open ? Theme.success : Theme.primary
                                        }
                                        Text {
                                            Layout.fillWidth: true
                                            text: root.channelCaptionFor(channelNode.modelData)
                                            color: channelNode.modelData.available ? Theme.textPrimary : Theme.textTertiary
                                            font.pixelSize: 13
                                            font.weight: Font.DemiBold
                                            elide: Text.ElideRight
                                        }
                                    }

                                    MouseArea {
                                        id: channelMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function(mouse) {
                                            if (mouse.button === Qt.LeftButton && mouse.x < 27) {
                                                root.toggleChannel(channelNode.modelData.id)
                                                return
                                            }
                                            root.selectedTreeChannelIndex = channelNode.sourceIndex
                                            root.selectedTreeDeviceIndex = -1
                                            ModbusWorkspace.select(channelNode.sourceIndex)
                                            if (mouse.button === Qt.LeftButton && mouse.x >= 27 && mouse.x <= 51) {
                                                root.toggleChannelChecked(channelNode.modelData.id,
                                                                          channelNode.channelBackend.devices)
                                                return
                                            }
                                            if (mouse.button === Qt.RightButton)
                                                channelMenu.popup()
                                        }
                                        onDoubleClicked: root.toggleChannel(channelNode.modelData.id)
                                    }
                                    Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
                                }

                                Repeater {
                                    id: deviceRepeater
                                    model: channelNode.expanded && channelNode.channelBackend ? channelNode.channelBackend.devices : null

                                    delegate: Rectangle {
                                        id: deviceRow
                                        required property string name
                                        required property int unitId
                                        required property bool online
                                        required property string deviceId
                                        required property int index
                                        readonly property bool focused: channelNode.sourceIndex === root.selectedTreeChannelIndex
                                                                        && index === root.selectedTreeDeviceIndex
                                        readonly property bool checked: root.isDeviceChecked(channelNode.modelData.id, deviceId)
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 40
                                        color: focused ? Theme.selectedFill : (deviceMouse.containsMouse ? Theme.hoverFill : Theme.sidebar)
                                        Rectangle {
                                            visible: focused
                                            anchors.left: parent.left
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: 3
                                            height: 25
                                            radius: 2
                                            color: Theme.primary
                                        }

                                        Rectangle {
                                            x: 40
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: 18
                                            height: 18
                                            radius: 4
                                            color: checked ? Theme.primary : Theme.inputBackground
                                            border.width: 1
                                            border.color: checked ? Theme.primary : Theme.border
                                            Text { anchors.centerIn: parent; visible: checked; text: "✓"; color: "#FFFFFF"; font.pixelSize: 11; font.weight: Font.Bold }
                                        }
                                        DeviceGlyph {
                                            x: root.treeChannelColumnWidth + 9
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: 28
                                            height: 28
                                            online: deviceRow.online
                                        }
                                        Text {
                                            x: root.treeChannelColumnWidth + 43
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: root.treeNameColumnWidth - 48
                                            text: name
                                            color: online ? Theme.textPrimary : Theme.error
                                            font.pixelSize: 12
                                            elide: Text.ElideRight
                                        }
                                        Rectangle {
                                            visible: root.showDeviceDetails
                                            x: root.treeChannelColumnWidth + root.treeNameColumnWidth + 1
                                            anchors.top: parent.top; anchors.bottom: parent.bottom
                                            width: 1; color: Theme.divider
                                        }
                                        Text {
                                            visible: root.showDeviceDetails
                                            x: root.treeChannelColumnWidth + root.treeNameColumnWidth + 2
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: root.treeTypeColumnWidth
                                            text: String(channelNode.modelData.mode).toLowerCase().indexOf("slave") >= 0
                                                  ? I18n.t("Slave simulator", "模拟从机")
                                                  : I18n.t("Master device", "模拟主机")
                                            color: Theme.textPrimary
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                            elide: Text.ElideRight
                                        }
                                        Rectangle {
                                            visible: root.showDeviceDetails
                                            x: root.treeChannelColumnWidth + root.treeNameColumnWidth + root.treeTypeColumnWidth + 2
                                            anchors.top: parent.top; anchors.bottom: parent.bottom
                                            width: 1; color: Theme.divider
                                        }
                                        Text {
                                            visible: root.showDeviceDetails
                                            x: root.treeChannelColumnWidth + root.treeNameColumnWidth + root.treeTypeColumnWidth + 3
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: root.treeIdColumnWidth
                                            text: String(index + 1)
                                            color: Theme.textPrimary
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                        Rectangle {
                                            visible: root.showDeviceDetails
                                            x: root.treeChannelColumnWidth + root.treeNameColumnWidth + root.treeTypeColumnWidth + root.treeIdColumnWidth + 3
                                            anchors.top: parent.top; anchors.bottom: parent.bottom
                                            width: 1; color: Theme.divider
                                        }
                                        Text {
                                            visible: root.showDeviceDetails
                                            x: root.treeChannelColumnWidth + root.treeNameColumnWidth + root.treeTypeColumnWidth + root.treeIdColumnWidth + 4
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: root.treeAddressColumnWidth
                                            text: String(unitId)
                                            color: Theme.textPrimary
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                        }

                                        MouseArea {
                                            id: deviceMouse
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: function(mouse) {
                                                root.selectedTreeChannelIndex = channelNode.sourceIndex
                                                root.selectedTreeDeviceIndex = index
                                                ModbusWorkspace.select(channelNode.sourceIndex)
                                                channelNode.channelBackend.selectDevice(index)
                                                if (mouse.button === Qt.LeftButton && mouse.x >= 36 && mouse.x <= 63) {
                                                    root.toggleDeviceChecked(channelNode.modelData.id, deviceId)
                                                    return
                                                }
                                                if (mouse.button === Qt.RightButton) {
                                                    root.contextDeviceIndex = index
                                                    deviceMenu.popup()
                                                }
                                            }
                                        }
                                        Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.alpha(Theme.divider, 0.74) }
                                    }
                                }
                            }
                        }
                        Text {
                            visible: root.treeChannels().length === 0
                            Layout.fillWidth: true
                            Layout.topMargin: 24
                            text: I18n.t("No available COM ports", "未扫描到可用 COM 口")
                            color: Theme.textTertiary
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Item { Layout.fillHeight: true; Layout.minimumHeight: 12 }
                    }
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    width: 1
                    color: Theme.divider
                }
            }

            Rectangle {
                id: tablePane
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.surface
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: root.tableHeaderHeight; color: Theme.surfaceRaised
                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 0
                            Text { Layout.preferredWidth: root.numberColumnWidth; text: "#"; color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.fillWidth: true; Layout.minimumWidth: 130; text: I18n.t("Name", "名称"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                            Text { Layout.preferredWidth: root.valueColumnWidth; text: I18n.t("Value", "数值"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: root.commandColumnWidth; text: I18n.t("Command", "指令"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: root.writeColumnWidth; text: I18n.t("Write", "写"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { Layout.preferredWidth: root.unitColumnWidth; text: I18n.t("Unit", "单位"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { visible: root.showRangeColumn; Layout.preferredWidth: root.rangeColumnWidth; text: I18n.t("Range", "范围"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { visible: root.showAreaColumn; Layout.preferredWidth: root.areaColumnWidth; text: I18n.t("Area", "区块"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                            Text { Layout.preferredWidth: root.addressColumnWidth; text: I18n.t("Address", "地址"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { visible: root.showCountColumn; Layout.preferredWidth: root.countColumnWidth; text: I18n.t("Count", "数量"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { visible: root.showBitColumns; Layout.preferredWidth: root.bitOffsetColumnWidth; text: I18n.t("Bit off", "位偏移"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                            Text { visible: root.showBitColumns; Layout.preferredWidth: root.bitCountColumnWidth; text: I18n.t("Bits", "位数"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                        }
                        Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
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
                                height: 42
                                color: point.selected ? Theme.selectedFill : (index % 2 === 0 ? Theme.surface : Theme.surfaceRaised)
                                MouseArea { anchors.fill: parent; onDoubleClicked: root.editPointRequested(point.sourceRow); onClicked: backend.points.selectPoint(point.sourceRow, (mouse.modifiers & Qt.ControlModifier) !== 0, (mouse.modifiers & Qt.ShiftModifier) !== 0) }
                                RowLayout {
                                    anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 0
                                    Text { Layout.preferredWidth: root.numberColumnWidth; text: String(index + 1); color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { Layout.fillWidth: true; Layout.minimumWidth: 130; text: point.name || "--"; color: Theme.textPrimary; font.pixelSize: 12; elide: Text.ElideRight }
                                    Rectangle { Layout.preferredWidth: root.valueColumnWidth; Layout.fillHeight: true; color: Theme.alpha(Theme.cyan, Theme.dark ? 0.12 : 0.16); border.width: 1; border.color: Theme.alpha(Theme.cyan, 0.22); Text { anchors.centerIn: parent; text: root.valueText(point); color: point.status === "OUT OF RANGE" ? Theme.warning : Theme.cyan; font.family: "Consolas"; font.pixelSize: 12; elide: Text.ElideRight; width: parent.width - 10; horizontalAlignment: Text.AlignHCenter } }
                                    Text { Layout.preferredWidth: root.commandColumnWidth; text: "FC" + point.fc; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Item { Layout.preferredWidth: root.writeColumnWidth; Layout.fillHeight: true; AppButton { width: 40; height: 28; anchors.centerIn: parent; compact: true; text: "写"; variant: "text"; accent: root.writable(point.fc) ? Theme.warning : Theme.textTertiary; enabled: backend && root.writable(point.fc) && backend.open; onClicked: backend.writePoint(point.sourceRow, String(point.writeValue || "")) } }
                                    Text { Layout.preferredWidth: root.unitColumnWidth; text: point.unit || "--"; color: Theme.textSecondary; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
                                    Text { visible: root.showRangeColumn; Layout.preferredWidth: root.rangeColumnWidth; text: root.rangeText(point); color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
                                    Text { visible: root.showAreaColumn; Layout.preferredWidth: root.areaColumnWidth; text: root.areaText(point.fc); color: Theme.textSecondary; font.pixelSize: 11; elide: Text.ElideRight }
                                    Text { Layout.preferredWidth: root.addressColumnWidth; text: String(point.address); color: Theme.primary; font.family: "Consolas"; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                    Text { visible: root.showCountColumn; Layout.preferredWidth: root.countColumnWidth; text: String(point.count || 1); color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { visible: root.showBitColumns; Layout.preferredWidth: root.bitOffsetColumnWidth; text: String(point.bitOffset || 0); color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                    Text { visible: root.showBitColumns; Layout.preferredWidth: root.bitCountColumnWidth; text: String(point.bitCount || 0); color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter }
                                }
                                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
                            }
                            Text { anchors.centerIn: parent; visible: pointList.count === 0; text: I18n.t("No data points for this device", "当前设备还没有数据点"); color: Theme.textTertiary; font.pixelSize: 12 }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        z: 100
        enabled: false
        color: "transparent"
        radius: Theme.radius
        border.width: 1
        border.color: Theme.alpha(Theme.textSecondary, 0.24)
    }

    Menu {
        id: channelMenu
        width: 230
        padding: 6
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.width: 1; border.color: Theme.border }
        TreeMenuItem {
            text: root.currentChannel() && root.currentChannel().open
                  ? I18n.t("⊘  Disconnect channel", "⊘  关闭通道")
                  : I18n.t("⌁  Connect channel", "⌁  连接通道")
            onTriggered: {
                if (!backend) return
                if (root.currentChannel() && root.currentChannel().open)
                    backend.close()
                else
                    backend.openConnection()
            }
        }
        TreeMenuItem { text: I18n.t("＋  Add device", "＋  添加设备"); onTriggered: root.addDeviceRequested() }
        TreeMenuItem { text: I18n.t("⚙  Configure parameters", "⚙  配置参数"); onTriggered: root.configureChannelRequested() }
        MenuSeparator { }
        TreeMenuItem {
            text: root.currentChannel() && root.currentChannel().transport === "serial"
                  ? I18n.t("✎  Edit alias", "✎  修改别名")
                  : I18n.t("✎  Rename channel", "✎  重命名通道")
            onTriggered: root.renameChannelRequested()
        }
        TreeMenuItem {
            visible: root.currentChannel() && root.currentChannel().transport === "network"
            text: I18n.t("×  Delete channel", "×  删除通道")
            enabled: ModbusWorkspace.channels.length > 1
            onTriggered: root.removeChannelRequested()
        }
    }

    Menu {
        id: deviceMenu
        width: 230
        padding: 6
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.width: 1; border.color: Theme.border }
        TreeMenuItem {
            text: I18n.t("✎  Edit device", "✎  编辑设备")
            enabled: root.contextDeviceIndex >= 0
            onTriggered: root.editDeviceRequested(root.contextDeviceIndex)
        }
        TreeMenuItem {
            text: I18n.t("＋  Add data point", "＋  添加数据点")
            enabled: root.contextDeviceIndex >= 0
            onTriggered: root.newPointRequested()
        }
        MenuSeparator { }
        TreeMenuItem {
            text: I18n.t("×  Delete device", "×  删除设备")
            enabled: root.contextDeviceIndex >= 0
            onTriggered: root.removeDeviceRequested()
        }
    }

}
