import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root
    objectName: "modbusDataMonitor"
    clip: true
    property var backend
    property string section: "data"
    property var expandedChannels: ({})
    property var checkedTreeDevices: ({})
    property bool showDeviceDetails: false
    property bool forceDetailedColumns: false
    property int selectedTreeChannelIndex: -1
    property int selectedTreeDeviceIndex: -1
    property int contextDeviceIndex: -1
    property int searchMatchRow: -1
    property string activePointId: ""
    property string activeField: "count"
    property int contextRevision: 0
    readonly property string currentChannelName: currentChannel() ? channelCaptionFor(currentChannel()) : "--"
    readonly property string currentDeviceName: {
        contextRevision
        if (!backend || backend.selectedDeviceIndex < 0) return I18n.t("Data points", "数据点")
        return String(backend.devices.data(backend.devices.index(backend.selectedDeviceIndex, 0), 257) || I18n.t("Data points", "数据点"))
    }
    function protocolName(mode) {
        const name = String(mode).toLowerCase()
        return name.indexOf("tcp") >= 0 ? "MODBUS TCP" : name.indexOf("ascii") >= 0 ? "MODBUS ASCII" : "MODBUS RTU"
    }
    function serialFormat(channel) {
        if (!channel) return "--"
        const parity = String(channel.parity).toLowerCase()
        return String(channel.channelOptions.dataBits || 8) + " · " + (parity.indexOf("even") >= 0 ? "E" : parity.indexOf("odd") >= 0 ? "O" : "N") + " · " + String(channel.channelOptions.stopBits || 1)
    }
    Connections {
        target: backend ? backend.devices : null
        function onDefinitionsChanged() { root.contextRevision++ }
    }
    readonly property int tableHeaderHeight: 39
    readonly property int toolButtonWidth: 30
    readonly property int tableToolButtonWidth: tablePane.width < 680 ? 28 : 30
    readonly property int tableSearchWidth: tablePane.width < 680 ? 100 : (tablePane.width < 900 ? 124 : 153)
    readonly property int treePreferredWidth: showDeviceDetails ? Math.min(600, Math.max(272, width - 570)) : 272
    readonly property int treeContentWidth: showDeviceDetails ? 600 : treePreferredWidth - 2
    property bool trafficVisible: true
    signal statisticsRequested()
    readonly property int treeChannelColumnWidth: 92
    readonly property int treeNameColumnWidth: showDeviceDetails ? 220 : treePreferredWidth - treeChannelColumnWidth - 1
    readonly property int treeTypeColumnWidth: 120
    readonly property int treeIdColumnWidth: 82
    readonly property int treeAddressColumnWidth: 82
    readonly property int numberColumnWidth: 42
    readonly property real nameColumnWidth: 132 + Math.max(0, tablePane.width - 922) * 0.5
    readonly property real valueColumnWidth: 112 + Math.max(0, tablePane.width - 922) * 0.2
    readonly property int commandColumnWidth: 76
    readonly property int writeColumnWidth: 48
    readonly property int unitColumnWidth: 58
    readonly property int rangeColumnWidth: 110
    readonly property real areaColumnWidth: 112 + Math.max(0, tablePane.width - 922) * 0.3
    readonly property int addressColumnWidth: 64
    readonly property int countColumnWidth: 52
    readonly property int bitOffsetColumnWidth: 64
    readonly property int bitCountColumnWidth: 52
    readonly property int scaleColumnWidth: 62
    readonly property int offsetColumnWidth: 62
    readonly property int decimalsColumnWidth: 76
    readonly property int transferTypeColumnWidth: 104
    readonly property int displayTypeColumnWidth: 104
    readonly property int byteOrderColumnWidth: 76
    readonly property int wordOrderColumnWidth: 76
    readonly property int pollingColumnWidth: 88
    readonly property int periodColumnWidth: 108
    readonly property int timeoutColumnWidth: 108
    readonly property int retriesColumnWidth: 84
    readonly property int tableContentWidth: (columnVisible("number") ? numberColumnWidth : 0)
                                               + (columnVisible("name") ? nameColumnWidth : 0)
                                               + (columnVisible("value") ? valueColumnWidth : 0)
                                               + (columnVisible("command") ? commandColumnWidth : 0)
                                               + (columnVisible("write") ? writeColumnWidth : 0)
                                               + (columnVisible("unit") ? unitColumnWidth : 0)
                                               + (columnVisible("range") ? rangeColumnWidth : 0)
                                               + (columnVisible("area") ? areaColumnWidth : 0)
                                               + (columnVisible("address") ? addressColumnWidth : 0)
                                               + (columnVisible("count") ? countColumnWidth : 0)
                                               + (columnVisible("bitOffset") ? bitOffsetColumnWidth : 0)
                                               + (columnVisible("bitCount") ? bitCountColumnWidth : 0)
                                               + (columnVisible("scale") ? scaleColumnWidth : 0)
                                               + (columnVisible("offset") ? offsetColumnWidth : 0)
                                               + (columnVisible("decimals") ? decimalsColumnWidth : 0)
                                               + (columnVisible("transferType") ? transferTypeColumnWidth : 0)
                                               + (columnVisible("displayType") ? displayTypeColumnWidth : 0)
                                               + (columnVisible("byteOrder") ? byteOrderColumnWidth : 0)
                                               + (columnVisible("wordOrder") ? wordOrderColumnWidth : 0)
                                               + (columnVisible("polling") ? pollingColumnWidth : 0)
                                               + (columnVisible("batchWrite") ? pollingColumnWidth : 0)
                                               + (columnVisible("period") ? periodColumnWidth : 0)
                                               + (columnVisible("timeout") ? timeoutColumnWidth : 0)
                                               + (columnVisible("retries") ? retriesColumnWidth : 0)
    signal newPointRequested()
    signal deleteRequested()
    signal addDeviceRequested()
    signal removeDeviceRequested()
    signal removeDevicesRequested(var selections)
    signal editDeviceRequested(int index)
    signal channelManagerRequested()
    signal configureChannelRequested()
    signal trendRequested()
    signal parseFrameRequested()
    signal advancedRequested()
    signal batchEditRequested(string field, string label, string value)
    signal pasteCellsRequested(int sourceRow, var fields)
    signal copyPointsRequested()
    signal importPointsRequested()
    signal exportPointsRequested()
    signal pastePointsRequested()
    signal sortPointsRequested()
    signal controlRequested()
    signal editPointRequested(int row)
    signal editCommandRequested(int row, string currentValue)
    signal addChannelRequested()
    signal renameChannelRequested()
    signal removeChannelRequested()

    function inspectRowsFilter() { filterDialog.openRows() }
    function inspectColumnsFilter() { filterDialog.openColumns() }
    function inspectBatchWriteDetails() {
        forceDetailedColumns = true
        Qt.callLater(function() { tableHorizontal.contentX = Math.max(0, root.tableContentWidth - tableHorizontal.width) })
    }

    function activePoint() {
        if (!backend) return null
        for (let row = 0; row < backend.points.count; ++row) {
            const point = backend.points.pointAt(row)
            if (String(point.id) === activePointId) return point
        }
        return null
    }
    function fieldForColumn(key) {
        const fields = {command: "writeValue", period: "periodMs", timeout: "timeoutMs"}
        return fields[key] || key
    }
    function batchEditCurrentField() {
        const point = activePoint()
        const column = columnDefinitions().find(function(item) { return fieldForColumn(item.key) === activeField })
        let value = point && point[activeField] !== undefined ? String(point[activeField]) : ""
        if (activeField === "count" && !point) value = "1"
        if (activeField === "range") value = point && point.minValue !== undefined && point.maxValue !== undefined
                                               ? String(point.minValue) + "," + String(point.maxValue) : ""
        batchEditRequested(activeField, column ? column.name : activeField, value)
    }
    function pasteAtCurrentCell() {
        const point = activePoint()
        if (!point) return
        const columns = columnDefinitions().filter(function(column) { return columnVisible(column.key) })
        const start = columns.findIndex(function(column) { return fieldForColumn(column.key) === activeField })
        if (start >= 0)
            pasteCellsRequested(point.sourceRow, columns.slice(start).map(function(column) { return fieldForColumn(column.key) }))
    }
    onBackendChanged: activePointId = ""
    Connections {
        target: root.backend ? root.backend.points : null
        function onDeviceIdChanged() { root.activePointId = "" }
    }
    Keys.onPressed: function(event) {
        if (event.matches(StandardKey.Paste) && activePointId !== "") {
            pasteAtCurrentCell()
            event.accepted = true
        }
    }

    function currentChannel() {
        const channels = ModbusWorkspace.channels
        const index = ModbusWorkspace.currentIndex
        return index >= 0 && index < channels.length ? channels[index] : null
    }
    function columnVisible(key) {
        const detailed = ["scale", "offset", "decimals", "transferType", "displayType",
                          "byteOrder", "wordOrder", "polling", "batchWrite", "period", "timeout", "retries"]
        if (!forceDetailedColumns && detailed.indexOf(key) >= 0) return false
        return !backend || backend.visibleColumns[key] !== false
    }
    function columnDefinitions() {
        return [
            {key:"number", name:I18n.t("No.", "序号")},
            {key:"name", name:I18n.t("Name", "名称")},
            {key:"value", name:I18n.t("Value", "数值")},
            {key:"command", name:I18n.t("Command", "指令")},
            {key:"write", name:I18n.t("Write", "写")},
            {key:"unit", name:I18n.t("Unit", "单位")},
            {key:"range", name:I18n.t("Range", "范围")},
            {key:"area", name:I18n.t("Block", "区块")},
            {key:"address", name:I18n.t("Address", "地址")},
            {key:"count", name:I18n.t("Quantity", "数量")},
            {key:"bitOffset", name:I18n.t("Bit offset", "位偏移")},
            {key:"bitCount", name:I18n.t("Bits", "位数")},
            {key:"scale", name:I18n.t("Ratio", "系数")},
            {key:"offset", name:I18n.t("Offset", "偏移")},
            {key:"decimals", name:I18n.t("Decimals", "小数位数")},
            {key:"transferType", name:I18n.t("Transmission", "传输类型")},
            {key:"displayType", name:I18n.t("Presentation", "呈现类型")},
            {key:"byteOrder", name:I18n.t("Byte order", "字节序")},
            {key:"wordOrder", name:I18n.t("Word order", "字序")},
            {key:"polling", name:I18n.t("Polling", "轮询")},
            {key:"batchWrite", name:I18n.t("Batch write", "批量写")},
            {key:"period", name:I18n.t("Interval (ms)", "间隔时间(ms)")},
            {key:"timeout", name:I18n.t("Timeout (ms)", "超时时间(ms)")},
            {key:"retries", name:I18n.t("Retries", "重发次数")}
        ]
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
                                      name: String(device.name),
                                      deviceEnabled: device.enabled !== false })
                }
            }
        }
        return selections
    }
    function hasStartableCheckedChannel() {
        const selections = checkedDeviceSelections()
        for (let index = 0; index < selections.length; ++index) {
            const channelBackend = ModbusWorkspace.channelBackend(selections[index].channelIndex)
            if (selections[index].deviceEnabled && channelBackend && !channelBackend.polling)
                return true
        }
        return false
    }
    function anyChannelPolling() {
        const channels = treeChannels()
        for (let index = 0; index < channels.length; ++index) {
            const channelBackend = ModbusWorkspace.channelBackend(Number(channels[index].index))
            if (channelBackend && channelBackend.polling)
                return true
        }
        return false
    }
    function startCheckedPolling() {
        const grouped = ({})
        const selections = checkedDeviceSelections()
        for (let index = 0; index < selections.length; ++index) {
            const channel = String(selections[index].channelIndex)
            if (selections[index].deviceEnabled) {
                if (!grouped[channel]) grouped[channel] = []
                grouped[channel].push(selections[index].deviceId)
            }
        }
        for (const channel in grouped) {
            const channelBackend = ModbusWorkspace.channelBackend(Number(channel))
            if (channelBackend && !channelBackend.polling)
                channelBackend.startPollingForDevices(grouped[channel])
        }
    }
    function stopActivePolling() {
        const channels = treeChannels()
        for (let index = 0; index < channels.length; ++index) {
            const channelBackend = ModbusWorkspace.channelBackend(Number(channels[index].index))
            if (channelBackend && channelBackend.polling)
                channelBackend.stopPolling()
        }
    }
    function clearCheckedDevices() {
        checkedTreeDevices = ({})
        selectedTreeChannelIndex = -1
        selectedTreeDeviceIndex = -1
        contextDeviceIndex = -1
    }
    function anyChannelBatchWriting() {
        const channels = ModbusWorkspace.channels
        for (let i = 0; i < channels.length; ++i) {
            const manager = ModbusWorkspace.channelBackend(i)
            if (manager && manager.batchWriting) return true
        }
        return false
    }
    function toggleCheckedBatchWriting() {
        if (anyChannelBatchWriting()) {
            const channels = ModbusWorkspace.channels
            for (let i = 0; i < channels.length; ++i) {
                const manager = ModbusWorkspace.channelBackend(i)
                if (manager) manager.stopBatchWriting()
            }
            return
        }
        const grouped = ({})
        const selected = checkedDeviceSelections()
        for (let i = 0; i < selected.length; ++i) {
            const key = String(selected[i].channelIndex)
            if (!grouped[key]) grouped[key] = []
            grouped[key].push(selected[i].deviceId)
        }
        // Validate all selected channels before starting any timer.
        for (const key in grouped) {
            const manager = ModbusWorkspace.channelBackend(Number(key))
            if (!manager || !manager.canStartBatchWriting(grouped[key])) return
        }
        for (const key in grouped)
            ModbusWorkspace.channelBackend(Number(key)).startBatchWriting(grouped[key])
    }
    function findNextPoint() {
        if (!backend || pointSearch.text.trim().length === 0) {
            searchMatchRow = -1
            return
        }
        const row = backend.points.findNext(pointSearch.text, searchMatchRow)
        searchMatchRow = row
        if (row >= 0) {
            pointList.currentIndex = row
            pointList.positionViewAtIndex(row, ListView.Center)
        }
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
    function transferTypeText(type) {
        const labels = {
            "bit": "位数据", "uint16": "无符号整数", "uint32": "无符号整数",
            "uint64": "无符号整数", "int16": "有符号整数", "int32": "有符号整数",
            "int64": "有符号整数", "float32": "浮点数", "float64": "浮点数",
            "bcd": "BCD", "bytes": "字节流", "string": "字符串"
        }
        return labels[String(type || "").toLowerCase()] || String(type || "--").toUpperCase()
    }
    function displayTypeText(displayType) {
        const labels = {
            "DEC": "十进制", "HEX": "十六进制", "BIN": "二进制", "FLOAT": "浮点数",
            "STRING": "字符串", "ENUM": "枚举", "TIMESTAMP": "时间戳", "BIT FIELD": "位域"
        }
        const key = String(displayType || "DEC").toUpperCase()
        return labels[key] || key
    }
    function byteOrderText(wordOrder) {
        const order = String(wordOrder || "ABCD").toUpperCase()
        return order === "BADC" || order === "DCBA" ? "小端" : "大端"
    }
    function wordOrderText(wordOrder) {
        const order = String(wordOrder || "ABCD").toUpperCase()
        return order === "CDAB" || order === "DCBA" ? "小端" : "大端"
    }
    function inheritedNumber(value, fallback) {
        const number = Number(value)
        return isNaN(number) || number < 0 ? fallback : String(number)
    }
    function writable(fc) {
        // Point FC represents the MODBUS data block, not the write function code.
        return Number(fc) === 1 || Number(fc) === 3
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

    component PanelTab: AbstractButton {
        property bool current: false
        implicitWidth: tabText.implicitWidth + 14; implicitHeight: 48
        Layout.minimumHeight: 48; Layout.preferredHeight: 48; Layout.maximumHeight: 48
        hoverEnabled: true
        contentItem: Text { id: tabText; text: parent.text; color: parent.current ? Theme.cyan : parent.hovered ? Theme.textPrimary : Theme.textTertiary; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        background: Item { Rectangle { visible: parent.parent.current; anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter; width: parent.width - 12; height: 2; color: Theme.cyan } }
    }

    component ToolbarGlyph: AppIcon {
        property color accent: Theme.textSecondary
        color: accent
    }

    component TreeToolButton: AbstractButton {
        id: treeToolButton
        property color accent: Theme.textSecondary
        property string tip: ""
        property string iconKind: ({"+":"plus", "×":"close", "▷":"play", "Ⅱ":"pause", "‹":"collapse", "›":"expand"})[text] || ""
        property bool outlined: false
        property bool framed: false
        property int buttonSize: root.toolButtonWidth
        implicitWidth: buttonSize
        implicitHeight: buttonSize
        Layout.minimumWidth: buttonSize
        Layout.preferredWidth: buttonSize
        Layout.maximumWidth: buttonSize
        Layout.minimumHeight: buttonSize
        Layout.preferredHeight: buttonSize
        Layout.maximumHeight: buttonSize
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        hoverEnabled: true
        contentItem: Item {
            ToolbarGlyph {
                anchors.centerIn: parent
                width: 21
                height: width
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
                font.pixelSize: 14
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        background: Rectangle {
            radius: 4
            color: treeToolButton.down ? Theme.alpha(Theme.primary, 0.28)
                                       : (treeToolButton.hovered ? Theme.hoverFill : "transparent")
            border.width: treeToolButton.activeFocus || treeToolButton.framed ? 1 : 0
            border.color: Theme.border
        }
        Accessible.name: tip
        ToolTip.visible: hovered && tip.length > 0
        ToolTip.delay: 550
        ToolTip.text: tip
    }

    component DenseToolButton: TreeToolButton {
        buttonSize: root.tableToolButtonWidth
    }

    component ToolSeparator: Item {
        implicitWidth: 7
        implicitHeight: 25
        Rectangle {
            anchors.centerIn: parent
            width: 1
            height: 25
            color: Theme.divider
        }
    }

    component TableHeaderCell: Rectangle {
        property string cellText: ""
        property int textAlignment: Text.AlignHCenter
        property bool interactive: false
        signal doubleActivated()
        height: root.tableHeaderHeight
        color: Theme.tableHeader
        border.width: 0
        Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.alpha(Theme.divider, 0.45) }
        Text {
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            text: parent.cellText
            color: Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: 11
            font.weight: Font.Medium
            horizontalAlignment: parent.textAlignment
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        MouseArea {
            anchors.fill: parent
            enabled: parent.interactive
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onDoubleClicked: parent.doubleActivated()
        }
    }

    component TableValueCell: Rectangle {
        property string cellText: "--"
        property color textColor: Theme.textSecondary
        property color fillColor: "transparent"
        property bool mono: false
        property bool bold: false
        property bool interactive: false
        property int textAlignment: Text.AlignHCenter
        signal activated(int modifiers)
        signal doubleActivated()
        color: fillColor
        border.width: 0
        Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.alpha(Theme.divider, 0.45) }
        Text {
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            text: parent.cellText
            color: parent.textColor
            font.family: parent.mono ? "Consolas" : Theme.fontFamily
            font.pixelSize: 12
            font.weight: parent.bold ? Font.DemiBold : Font.Normal
            horizontalAlignment: parent.textAlignment
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        MouseArea {
            anchors.fill: parent
            enabled: parent.interactive
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: parent.activated(mouse.modifiers)
            onDoubleClicked: parent.doubleActivated()
        }
    }

    component EditableValueCell: TableValueCell {
        required property string fieldKey
        required property var definition
        interactive: true
        border.width: root.activePointId === String(definition.id) && root.activeField === fieldKey ? 1 : 0
        border.color: root.activePointId === String(definition.id) && root.activeField === fieldKey
                      ? Theme.primary : Theme.divider
        onActivated: function(modifiers) {
            root.activePointId = String(definition.id)
            root.activeField = fieldKey
            root.forceActiveFocus()
            backend.points.selectPoint(definition.sourceRow, (modifiers & Qt.ControlModifier) !== 0,
                                       (modifiers & Qt.ShiftModifier) !== 0)
        }
        onDoubleActivated: root.editPointRequested(definition.sourceRow)
    }

    component ChannelGlyph: AppIcon {
        property color accent: Theme.primary
        kind: "port"; color: accent
    }

    component DeviceGlyph: AppIcon {
        property bool online: false
        property bool deviceEnabled: true
        kind: online ? "link" : "unlink"
        color: online && deviceEnabled ? Theme.success : Theme.textTertiary
    }

    RowLayout {
        anchors.fill: parent
        spacing: 14
        Rectangle {
            id: treePanel
            objectName: "modbusTreePanel"
            Layout.preferredWidth: root.treePreferredWidth
            Layout.minimumWidth: root.treePreferredWidth; Layout.maximumWidth: root.treePreferredWidth
            Layout.fillHeight: true
            color: Theme.surface; radius: 7; border.width: 1; border.color: Theme.divider
            ColumnLayout {
                anchors.fill: parent; anchors.margins: 1; spacing: 0
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 48; Layout.minimumHeight: 48; Layout.maximumHeight: 48
                    color: Theme.surface; radius: 7
                    RowLayout {
                        anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 12; spacing: 9
                        Text { text: I18n.t("Channels & devices", "通道与设备"); color: Theme.textPrimary; font.pixelSize: 13; font.weight: Font.DemiBold }
                        Rectangle {
                            width: channelCount.implicitWidth + 10; height: 17; radius: 3; color: "transparent"; border.width: 1; border.color: Theme.border
                            Text { id: channelCount; anchors.centerIn: parent; text: root.treeChannels().length; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                        }
                        Item { Layout.fillWidth: true }
                        TreeToolButton { iconKind: "more"; tip: I18n.t("Channel manager", "通道管理"); onClicked: root.channelManagerRequested() }
                    }
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider }
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 45; Layout.minimumHeight: 45; Layout.maximumHeight: 45
                    color: Theme.toolbar

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 7
                        anchors.rightMargin: 7
                        spacing: 2

                        TreeToolButton {
                            id: channelManagerButton
                            tip: I18n.t("Channel manager", "通道管理")
                            onClicked: root.channelManagerRequested()
                            iconKind: "nodes"
                        }
                        TreeToolButton { text: "+"; outlined: true; accent: Theme.textSecondary; tip: I18n.t("Add device", "添加设备"); enabled: root.treeChannels().length > 0; onClicked: root.addDeviceRequested() }
                        TreeToolButton {
                            text: "×"
                            outlined: true
                            accent: Theme.error
                            tip: I18n.t("Delete selected devices", "删除选中设备")
                            enabled: root.checkedDeviceSelections().length > 0
                            onClicked: root.removeDevicesRequested(root.checkedDeviceSelections())
                        }
                        TreeToolButton { text: "▷"; outlined: true; accent: Theme.cyan; tip: I18n.t("Poll checked devices", "轮询勾选设备"); enabled: root.hasStartableCheckedChannel(); onClicked: root.startCheckedPolling() }
                        TreeToolButton { text: "Ⅱ"; outlined: true; accent: Theme.warning; tip: I18n.t("Stop polling", "停止轮询"); enabled: root.anyChannelPolling(); onClicked: root.stopActivePolling() }
                        TreeToolButton {
                            text: "W"; outlined: true
                            accent: root.anyChannelBatchWriting() ? Theme.warning : Theme.textPrimary
                            tip: root.anyChannelBatchWriting() ? I18n.t("Stop cyclic batch write", "停止循环批量写")
                                                              : I18n.t("Cyclic write for checked devices", "对勾选设备启动循环批量写")
                            enabled: root.anyChannelBatchWriting() || root.checkedDeviceSelections().length > 0
                            onClicked: root.toggleCheckedBatchWriting()
                        }
                        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: 25; color: Theme.divider }
                        TreeToolButton {
                            text: root.showDeviceDetails ? "‹" : "›"
                            outlined: true
                            accent: Theme.textSecondary
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
                id: treePane
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.surface

                ScrollView {
                    id: treeScroll
                    anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top
                    anchors.topMargin: 10; anchors.bottom: connectionFooter.top
                    clip: true
                    contentWidth: root.treeContentWidth
                    ColumnLayout {
                        width: root.treeContentWidth
                        spacing: 0
                        Rectangle {
                            Layout.fillWidth: true
                            visible: root.showDeviceDetails
                            Layout.preferredHeight: root.tableHeaderHeight
                            color: Theme.tableHeader
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
                                    Layout.preferredHeight: 57
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
                                        anchors.fill: parent; anchors.leftMargin: 13; anchors.rightMargin: 14; spacing: 9
                                        Item {
                                            Layout.preferredWidth: 13; Layout.minimumWidth: 13; Layout.maximumWidth: 13; Layout.fillHeight: true
                                            AppIcon { anchors.centerIn: parent; width: 13; height: 13; kind: channelNode.expanded ? "down" : "expand"; color: Theme.textSecondary }
                                        }
                                        Rectangle {
                                            id: channelCheckbox
                                            Layout.preferredWidth: 13; Layout.preferredHeight: 13; radius: 3
                                            color: channelNode.checkState > 0 ? Theme.primary : Theme.inputBackground
                                            border.width: 1; border.color: channelNode.checkState > 0 ? Theme.primary : Theme.border
                                            Text { anchors.centerIn: parent; visible: channelNode.checkState > 0; text: channelNode.checkState === 2 ? "✓" : "−"; color: Theme.onAccent; font.pixelSize: 10; font.weight: Font.Bold }
                                        }
                                        ChannelGlyph { Layout.preferredWidth: 25; Layout.preferredHeight: 25; accent: Theme.textTertiary }
                                        ColumnLayout {
                                            Layout.fillWidth: true; spacing: 4
                                            Text { Layout.fillWidth: true; text: root.channelCaptionFor(channelNode.modelData); color: Theme.textPrimary; font.family: "Consolas"; font.pixelSize: 14; font.weight: Font.DemiBold; elide: Text.ElideRight }
                                            Text {
                                                Layout.fillWidth: true
                                                text: channelNode.channelBackend ? (String(channelNode.channelBackend.mode).indexOf("tcp") < 0
                                                    ? channelNode.channelBackend.baudRate + " · " + root.serialFormat(channelNode.channelBackend).replace(/ · /g, "") + " · " + root.protocolName(channelNode.channelBackend.mode).replace("MODBUS ", "")
                                                    : channelNode.channelBackend.host + ":" + channelNode.channelBackend.port) : "--"
                                                color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10; elide: Text.ElideRight
                                            }
                                        }
                                        Rectangle { width: 6; height: 6; radius: 3; color: channelNode.modelData.open ? Theme.success : Theme.textTertiary }
                                    }

                                    MouseArea {
                                        id: channelMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: function(mouse) {
                                            if (mouse.button === Qt.LeftButton && mouse.x < 30) {
                                                root.toggleChannel(channelNode.modelData.id)
                                                return
                                            }
                                            root.selectedTreeChannelIndex = channelNode.sourceIndex
                                            root.selectedTreeDeviceIndex = -1
                                            ModbusWorkspace.select(channelNode.sourceIndex)
                                            if (mouse.button === Qt.LeftButton && mouse.x >= 30 && mouse.x <= 53) {
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
                                        required property bool deviceEnabled
                                        required property string deviceId
                                        required property int index
                                        readonly property bool focused: channelNode.sourceIndex === root.selectedTreeChannelIndex
                                                                        && index === root.selectedTreeDeviceIndex
                                        readonly property bool checked: root.isDeviceChecked(channelNode.modelData.id, deviceId)
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 46
                                        Layout.topMargin: 1; Layout.bottomMargin: 1
                                        color: focused ? Theme.selectedFill : (deviceMouse.containsMouse ? Theme.hoverFill : Theme.surface)
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
                                            x: 35
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: 13
                                            height: 13
                                            radius: 3
                                            color: checked ? Theme.primary : Theme.inputBackground
                                            border.width: 1
                                            border.color: checked ? Theme.primary : Theme.border
                                            Text { anchors.centerIn: parent; visible: checked; text: "✓"; color: Theme.onAccent; font.pixelSize: 11; font.weight: Font.Bold }
                                        }
                                        Rectangle { x: 22; y: 0; width: 1; height: parent.height / 2; color: Theme.border }
                                        Rectangle { x: 22; y: parent.height / 2; width: 10; height: 1; color: Theme.border }
                                        Text { visible: !root.showDeviceDetails; anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter; text: String(unitId).padStart(2, "0"); color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                                        DeviceGlyph {
                                            x: root.showDeviceDetails ? root.treeChannelColumnWidth + 9 : 59
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: 16
                                            height: 16
                                            online: deviceRow.online
                                            deviceEnabled: deviceRow.deviceEnabled
                                        }
                                        Text {
                                            x: root.showDeviceDetails ? root.treeChannelColumnWidth + 43 : 85
                                            anchors.verticalCenter: parent.verticalCenter
                                            width: root.showDeviceDetails ? root.treeNameColumnWidth - 48 : parent.width - 124
                                            text: name
                                            color: deviceEnabled ? Theme.textPrimary : Theme.textTertiary
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
                                                if (mouse.button === Qt.LeftButton && mouse.x >= 30 && mouse.x <= 53) {
                                                    root.toggleDeviceChecked(channelNode.modelData.id, deviceId)
                                                    return
                                                }
                                                if (mouse.button === Qt.RightButton) {
                                                    root.contextDeviceIndex = index
                                                    deviceMenu.popup()
                                                }
                                            }
                                        }
                                        Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.alpha(Theme.divider, 0.2) }
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
            
                Rectangle {
                    id: connectionFooter
                    anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
                    height: treePane.height > 500 ? 200 : 100
                    color: Theme.chromeBody
                    Rectangle { width: parent.width; height: 1; color: Theme.divider }
                    ColumnLayout {
                        anchors.fill: parent; anchors.margins: 16; spacing: 9
                        Text { text: "CURRENT CHANNEL"; color: Theme.textTertiary; font.pixelSize: 10; font.family: "Consolas"; font.letterSpacing: 1 }
                        RowLayout {
                            AppIcon { kind: "port"; width: 20; height: 20; color: Theme.cyan }
                            Text { text: root.currentChannelName; color: Theme.textPrimary; font.family: "Consolas"; font.pixelSize: 13; font.weight: Font.DemiBold }
                            Text { text: backend ? root.protocolName(backend.mode) : "--"; color: Theme.textTertiary; font.pixelSize: 10; font.family: "Consolas" }
                            Item { Layout.fillWidth: true }
                        }
                        GridLayout {
                            visible: connectionFooter.height > 120
                            Layout.fillWidth: true; columns: 2; columnSpacing: 12; rowSpacing: 6
                            Text { text: I18n.t("Role", "设备角色"); color: Theme.textTertiary; font.pixelSize: 11; Layout.fillWidth: true }
                            Text { text: I18n.t("Address", "设备地址"); color: Theme.textTertiary; font.pixelSize: 11; Layout.fillWidth: true }
                            Text { text: backend && String(backend.mode).toLowerCase().indexOf("slave") >= 0 ? "Slave" : "Master"; color: Theme.textSecondary; font.pixelSize: 12; font.family: "Consolas" }
                            Text { text: backend ? "Unit ID · " + backend.unitId : "--"; color: Theme.textSecondary; font.pixelSize: 12; font.family: "Consolas" }
                            Text { text: I18n.t("Baud rate", "波特率"); color: Theme.textTertiary; font.pixelSize: 11 }
                            Text { text: I18n.t("Data format", "数据格式"); color: Theme.textTertiary; font.pixelSize: 11 }
                            Text { text: backend && String(backend.mode).indexOf("tcp") < 0 ? backend.baudRate + " bps" : "--"; color: Theme.textSecondary; font.pixelSize: 12; font.family: "Consolas" }
                            Text { text: backend && String(backend.mode).indexOf("tcp") < 0 ? root.serialFormat(backend) : "--"; color: Theme.textSecondary; font.pixelSize: 12; font.family: "Consolas" }
                        }
                        Item { Layout.fillHeight: true }
                        RowLayout {
                            Rectangle { width: 4; height: 4; radius: 2; color: backend && backend.open ? Theme.success : Theme.textTertiary }
                            Text { text: backend && backend.open ? I18n.t("Connected", "已连接") : I18n.t("Disconnected", "未连接"); color: backend && backend.open ? Theme.success : Theme.textTertiary; font.pixelSize: 10 }
                        }
                    }
                }
            }
            }
        }
        ColumnLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 14
            Rectangle {
                id: pointPanel
                objectName: "modbusPointPanel"
                Layout.fillWidth: true; Layout.fillHeight: true
                color: Theme.surface; radius: 7; border.width: 1; border.color: Theme.divider
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 1; spacing: 0
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 48; Layout.minimumHeight: 48; Layout.maximumHeight: 48
                        color: Theme.surface; radius: 7
                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 15; anchors.rightMargin: 14; spacing: 10
                            AppIcon { kind: "grid"; width: 17; height: 17; color: Theme.textTertiary }
                            Text { Layout.maximumWidth: Math.max(100, pointPanel.width - 315); text: root.currentDeviceName; color: Theme.textPrimary; font.pixelSize: 13; font.weight: Font.DemiBold; elide: Text.ElideRight }
                            Text { text: backend ? "UNIT " + backend.unitId : ""; color: Theme.textTertiary; font.pixelSize: 10; font.family: "Consolas" }
                            Item { Layout.fillWidth: true }
                            PanelTab { text: I18n.t("Data", "数据监视"); current: true; onClicked: pointList.forceActiveFocus() }
                            PanelTab { text: I18n.t("Traffic", "报文"); current: false; onClicked: root.trafficVisible = !root.trafficVisible }
                            PanelTab { text: I18n.t("Statistics", "统计"); onClicked: root.statisticsRequested() }
                        }
                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider }
                    }
                Rectangle {
                    id: tableToolbar
                    objectName: "modbusPointToolbar"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 49; Layout.minimumHeight: 49; Layout.maximumHeight: 49
                    color: Theme.toolbar

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 0

                        AppTextField {
                            id: pointSearch
                            width: root.tableSearchWidth
                            height: 30
                            leftPadding: 29; rightPadding: 8; font.pixelSize: 11
                            placeholder: I18n.t("Search name", "搜索名称")
                            onTextChanged: root.searchMatchRow = -1
                            onAccepted: root.findNextPoint()
                            ToolButton {
                                anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                                width: 29; height: 28; padding: 7
                                contentItem: AppIcon { kind: "search"; color: Theme.textTertiary }
                                background: Item { }
                                Accessible.name: I18n.t("Find next matching name", "查找下一个匹配名称")
                                ToolTip.visible: hovered; ToolTip.text: Accessible.name
                                onClicked: root.findNextPoint()
                            }
                        }
                        DenseToolButton { text: "+"; outlined: true; accent: Theme.textSecondary; tip: I18n.t("Add point", "新增数据点"); onClicked: root.newPointRequested() }
                        DenseToolButton { text: "×"; outlined: true; accent: Theme.error; tip: I18n.t("Delete selected points", "删除选中数据点"); enabled: backend && backend.points.selectedCount > 0; onClicked: root.deleteRequested() }
                        ToolSeparator { }
                        DenseToolButton { iconKind: "wave"; accent: Theme.textSecondary; tip: I18n.t("Real-time trend", "实时曲线"); onClicked: root.trendRequested() }
                        DenseToolButton { iconKind: "inspect"; accent: Theme.textSecondary; tip: I18n.t("Parse PDU frame", "解析 PDU 报文"); onClicked: root.parseFrameRequested() }
                        DenseToolButton { iconKind: "copy"; accent: Theme.textSecondary; tip: I18n.t("Copy data configuration", "复制数据配置"); onClicked: root.copyPointsRequested() }
                        DenseToolButton { iconKind: "import"; accent: Theme.textSecondary; tip: I18n.t("Import data template", "导入数据模板"); onClicked: root.importPointsRequested() }
                        DenseToolButton { iconKind: "export"; accent: Theme.textSecondary; tip: I18n.t("Export data template", "导出数据模板"); onClicked: root.exportPointsRequested() }
                        ToolSeparator { }
                        DenseToolButton { iconKind: "grid"; accent: Theme.textSecondary; tip: I18n.t("Filter data blocks", "筛选数据区块"); onClicked: blockFilterMenu.popup() }
                        DenseToolButton { iconKind: "rows"; accent: Theme.textSecondary; tip: I18n.t("Filter rows", "筛选数据行"); onClicked: filterDialog.openRows() }
                        DenseToolButton { iconKind: "columns"; accent: Theme.textSecondary; tip: I18n.t("Filter columns", "筛选数据列"); onClicked: filterDialog.openColumns() }
                        DenseToolButton {
                            iconKind: "edit"
                            accent: Theme.textSecondary
                            tip: I18n.t("Batch modify selected column", "批量修改当前列")
                            enabled: backend && backend.points.selectedCount > 0 && root.activeField !== ""
                            onClicked: root.batchEditCurrentField()
                        }
                        DenseToolButton { iconKind: "gear"; accent: Theme.textSecondary; tip: I18n.t("Device advanced parameters", "设备高级参数"); onClicked: root.advancedRequested() }
                        ToolSeparator { }
                        DenseToolButton {
                            iconKind: root.forceDetailedColumns ? "collapse" : "expand"
                            objectName: "modbusPointDetailsToggle"
                            framed: true
                            outlined: root.forceDetailedColumns
                            accent: Theme.textSecondary
                            tip: root.forceDetailedColumns
                                 ? I18n.t("Show brief columns", "显示简略信息")
                                 : I18n.t("Show detailed columns", "显示详细信息")
                            onClicked: {
                                root.forceDetailedColumns = !root.forceDetailedColumns
                                if (!root.forceDetailedColumns)
                                    tableHorizontal.contentX = 0
                            }
                        }
                    }
                }
            Rectangle {
                id: tablePane
                objectName: "modbusTablePane"
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.surface
                clip: true

                Flickable {
                    id: tableHorizontal
                    anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top
                    anchors.bottom: pointFooter.top
                    contentWidth: Math.max(width, root.tableContentWidth)
                    contentHeight: height
                    flickableDirection: Flickable.HorizontalFlick
                    boundsBehavior: Flickable.StopAtBounds
                    clip: true
                    ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }

                    Column {
                        width: tableHorizontal.contentWidth
                        height: tableHorizontal.height
                        spacing: 0

                        Rectangle {
                            width: parent.width
                            height: root.tableHeaderHeight
                            color: Theme.tableHeader
                            Row {
                                height: parent.height
                                spacing: 0
                                TableHeaderCell { visible: root.columnVisible("number"); width: root.numberColumnWidth; cellText: I18n.t("No.", "序号"); interactive: true; onDoubleActivated: if (backend) backend.points.toggleSelectAll() }
                                TableHeaderCell { visible: root.columnVisible("name"); width: root.nameColumnWidth; cellText: I18n.t("Name", "名称"); textAlignment: Text.AlignLeft }
                                TableHeaderCell { visible: root.columnVisible("value"); width: root.valueColumnWidth; cellText: I18n.t("Value", "数值"); interactive: true; onDoubleActivated: if (backend) backend.readAllPoints() }
                                TableHeaderCell { visible: root.columnVisible("command"); width: root.commandColumnWidth; cellText: I18n.t("Command", "指令") }
                                TableHeaderCell { visible: root.columnVisible("write"); width: root.writeColumnWidth; cellText: I18n.t("Write", "写"); interactive: true; onDoubleActivated: if (backend) backend.writeBatchOnce() }
                                TableHeaderCell { visible: root.columnVisible("unit"); width: root.unitColumnWidth; cellText: I18n.t("Unit", "单位") }
                                TableHeaderCell { visible: root.columnVisible("range"); width: root.rangeColumnWidth; cellText: I18n.t("Range", "范围") }
                                TableHeaderCell { visible: root.columnVisible("area"); width: root.areaColumnWidth; cellText: I18n.t("Block", "区块") }
                                TableHeaderCell { visible: root.columnVisible("address"); width: root.addressColumnWidth; cellText: I18n.t("Address", "地址"); interactive: true; onDoubleActivated: root.sortPointsRequested() }
                                TableHeaderCell { visible: root.columnVisible("count"); width: root.countColumnWidth; cellText: I18n.t("Quantity", "数量") }
                                TableHeaderCell { visible: root.columnVisible("bitOffset"); width: root.bitOffsetColumnWidth; cellText: I18n.t("Bit offset", "位偏移") }
                                TableHeaderCell { visible: root.columnVisible("bitCount"); width: root.bitCountColumnWidth; cellText: I18n.t("Bits", "位数") }
                                TableHeaderCell { visible: root.columnVisible("scale"); width: root.scaleColumnWidth; cellText: I18n.t("Ratio", "系数") }
                                TableHeaderCell { visible: root.columnVisible("offset"); width: root.offsetColumnWidth; cellText: I18n.t("Offset", "偏移") }
                                TableHeaderCell { visible: root.columnVisible("decimals"); width: root.decimalsColumnWidth; cellText: I18n.t("Decimals", "小数位数") }
                                TableHeaderCell { visible: root.columnVisible("transferType"); width: root.transferTypeColumnWidth; cellText: I18n.t("Transmission", "传输类型") }
                                TableHeaderCell { visible: root.columnVisible("displayType"); width: root.displayTypeColumnWidth; cellText: I18n.t("Presentation", "呈现类型") }
                                TableHeaderCell { visible: root.columnVisible("byteOrder"); width: root.byteOrderColumnWidth; cellText: I18n.t("Byte order", "字节序") }
                                TableHeaderCell { visible: root.columnVisible("wordOrder"); width: root.wordOrderColumnWidth; cellText: I18n.t("Word order", "字序") }
                                TableHeaderCell { visible: root.columnVisible("polling"); width: root.pollingColumnWidth; cellText: I18n.t("Polling", "轮询") }
                                TableHeaderCell { visible: root.columnVisible("batchWrite"); width: root.pollingColumnWidth; cellText: I18n.t("Batch write", "批量写") }
                                TableHeaderCell { visible: root.columnVisible("period"); width: root.periodColumnWidth; cellText: I18n.t("Interval (ms)", "间隔时间(ms)") }
                                TableHeaderCell { visible: root.columnVisible("timeout"); width: root.timeoutColumnWidth; cellText: I18n.t("Timeout (ms)", "超时时间(ms)") }
                                TableHeaderCell { visible: root.columnVisible("retries"); width: root.retriesColumnWidth; cellText: I18n.t("Retries", "重发次数") }
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: Math.max(0, parent.height - root.tableHeaderHeight)
                            color: Theme.surface
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
                                height: 45
                                color: point.selected ? Theme.selectedFill : (index % 2 === 0 ? Theme.surface : Theme.alpha(Theme.alternateRow, 0.45))
                                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.alpha(Theme.divider, 0.6) }
                                Rectangle { visible: point.selected; width: 2; height: parent.height; color: Theme.cyan; z: 2 }
                                MouseArea {
                                    anchors.fill: parent
                                    onDoubleClicked: root.editPointRequested(point.sourceRow)
                                    onClicked: function(mouse) {
                                        root.activeField = ""
                                        root.activePointId = ""
                                        backend.points.selectPoint(point.sourceRow, (mouse.modifiers & Qt.ControlModifier) !== 0, (mouse.modifiers & Qt.ShiftModifier) !== 0)
                                    }
                                }
                                Row {
                                    anchors.fill: parent
                                    spacing: 0
                                    TableValueCell {
                                        visible: root.columnVisible("number"); width: root.numberColumnWidth; height: parent.height; cellText: String(index + 1); mono: true; textColor: Theme.textTertiary; interactive: true
                                        onActivated: function(modifiers) { backend.points.selectPoint(point.sourceRow, (modifiers & Qt.ControlModifier) !== 0, (modifiers & Qt.ShiftModifier) !== 0) }
                                    }
                                    EditableValueCell { definition: point; fieldKey: "name"; visible: root.columnVisible("name"); width: root.nameColumnWidth; height: parent.height; cellText: point.name || "--"; textAlignment: Text.AlignLeft; textColor: index === root.searchMatchRow ? Theme.error : Theme.textPrimary; bold: index === root.searchMatchRow }
                                    TableValueCell {
                                        visible: root.columnVisible("value"); width: root.valueColumnWidth; height: parent.height; cellText: root.valueText(point); mono: true; textColor: point.status === "OUT OF RANGE" ? Theme.warning : Theme.cyan; fillColor: "transparent"; textAlignment: Text.AlignRight; bold: true; interactive: true
                                        onActivated: function(modifiers) {
                                            root.activePointId = String(point.id); root.activeField = ""
                                            backend.points.selectPoint(point.sourceRow, (modifiers & Qt.ControlModifier) !== 0, (modifiers & Qt.ShiftModifier) !== 0)
                                        }
                                        onDoubleActivated: if (backend) backend.readPoint(point.sourceRow)
                                    }
                                    TableValueCell {
                                        visible: root.columnVisible("command"); width: root.commandColumnWidth; height: parent.height; cellText: String(point.writeValue === undefined || point.writeValue === null || point.writeValue === "" ? "--" : point.writeValue); mono: true; textColor: root.writable(point.fc) ? Theme.warning : Theme.textTertiary; interactive: root.writable(point.fc)
                                        onActivated: function(modifiers) {
                                            root.activePointId = String(point.id); root.activeField = "writeValue"; root.forceActiveFocus()
                                            backend.points.selectPoint(point.sourceRow, (modifiers & Qt.ControlModifier) !== 0, (modifiers & Qt.ShiftModifier) !== 0)
                                        }
                                        onDoubleActivated: root.editCommandRequested(point.sourceRow, String(point.writeValue || ""))
                                    }
                                    TableValueCell {
                                        visible: root.columnVisible("write"); width: root.writeColumnWidth; height: parent.height
                                        cellText: I18n.t("Write", "写")
                                        interactive: backend && root.writable(point.fc) && backend.open
                                        textColor: interactive ? Theme.warning : Theme.textTertiary
                                        onActivated: function(modifiers) {
                                            root.activePointId = String(point.id); root.activeField = ""
                                            backend.points.selectPoint(point.sourceRow, (modifiers & Qt.ControlModifier) !== 0, (modifiers & Qt.ShiftModifier) !== 0)
                                        }
                                        onDoubleActivated: backend.writePoint(point.sourceRow, String(point.writeValue || ""))
                                    }
                                    EditableValueCell { definition: point; fieldKey: "unit"; visible: root.columnVisible("unit"); width: root.unitColumnWidth; height: parent.height; cellText: point.unit || "--" }
                                    EditableValueCell { definition: point; fieldKey: "range"; visible: root.columnVisible("range"); width: root.rangeColumnWidth; height: parent.height; cellText: root.rangeText(point); mono: true; textColor: Theme.textTertiary }
                                    TableValueCell { visible: root.columnVisible("area"); width: root.areaColumnWidth; height: parent.height; cellText: root.areaText(point.fc); textAlignment: Text.AlignLeft }
                                    EditableValueCell { definition: point; fieldKey: "address"; visible: root.columnVisible("address"); width: root.addressColumnWidth; height: parent.height; cellText: String(point.address); mono: true; bold: true; textColor: Theme.primary }
                                    EditableValueCell { definition: point; fieldKey: "count"; visible: root.columnVisible("count"); width: root.countColumnWidth; height: parent.height; cellText: String(point.count || 1); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "bitOffset"; visible: root.columnVisible("bitOffset"); width: root.bitOffsetColumnWidth; height: parent.height; cellText: String(point.bitOffset || 0); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "bitCount"; visible: root.columnVisible("bitCount"); width: root.bitCountColumnWidth; height: parent.height; cellText: String(Number(point.bitCount || 0) > 0 ? point.bitCount : Math.max(1, Number(point.count || 1)) * 16); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "scale"; visible: root.columnVisible("scale"); width: root.scaleColumnWidth; height: parent.height; cellText: String(point.scale === undefined ? 1 : point.scale); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "offset"; visible: root.columnVisible("offset"); width: root.offsetColumnWidth; height: parent.height; cellText: String(point.offset === undefined ? 0 : point.offset); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "decimals"; visible: root.columnVisible("decimals"); width: root.decimalsColumnWidth; height: parent.height; cellText: String(point.decimals === undefined ? 0 : point.decimals); mono: true }
                                    TableValueCell { visible: root.columnVisible("transferType"); width: root.transferTypeColumnWidth; height: parent.height; cellText: root.transferTypeText(point.type) }
                                    TableValueCell { visible: root.columnVisible("displayType"); width: root.displayTypeColumnWidth; height: parent.height; cellText: root.displayTypeText(point.displayType) }
                                    TableValueCell { visible: root.columnVisible("byteOrder"); width: root.byteOrderColumnWidth; height: parent.height; cellText: root.byteOrderText(point.wordOrder) }
                                    TableValueCell { visible: root.columnVisible("wordOrder"); width: root.wordOrderColumnWidth; height: parent.height; cellText: root.wordOrderText(point.wordOrder) }
                                    Rectangle {
                                        visible: root.columnVisible("polling"); width: root.pollingColumnWidth; height: parent.height
                                        color: point.poll ? Theme.alpha(Theme.cyan, Theme.dark ? 0.14 : 0.18) : "transparent"
                                        border.width: 1; border.color: Theme.divider
                                        Row {
                                            anchors.centerIn: parent; spacing: 6
                                            Rectangle {
                                                width: 14; height: 14; radius: 3
                                                color: point.poll ? Theme.primary : Theme.inputBackground
                                                border.width: point.poll ? 0 : 1; border.color: Theme.border
                                                Text { anchors.centerIn: parent; visible: point.poll; text: "✓"; color: Theme.onAccent; font.pixelSize: 10; font.weight: Font.Bold }
                                            }
                                            Text { text: point.poll ? I18n.t("Selected", "已选择") : I18n.t("Off", "未选择"); color: point.poll ? Theme.textPrimary : Theme.textTertiary; font.pixelSize: 11 }
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: if (backend) backend.points.togglePointPolling(point.sourceRow)
                                        }
                                    }
                                    Rectangle {
                                        visible: root.columnVisible("batchWrite"); width: root.pollingColumnWidth; height: parent.height
                                        readonly property bool writable: Number(point.fc) === 1 || Number(point.fc) === 3
                                        color: point.batchWrite ? Theme.alpha(Theme.cyan, Theme.dark ? 0.14 : 0.18) : "transparent"
                                        border.width: 1; border.color: Theme.divider
                                        Row {
                                            anchors.centerIn: parent; spacing: 6
                                            Rectangle {
                                                visible: parent.parent.writable
                                                width: 14; height: 14; radius: 3
                                                color: point.batchWrite ? Theme.primary : Theme.inputBackground
                                                border.width: point.batchWrite ? 0 : 1; border.color: Theme.border
                                                Text { anchors.centerIn: parent; visible: point.batchWrite === true; text: "✓"; color: Theme.onAccent; font.pixelSize: 10; font.weight: Font.Bold }
                                            }
                                            Text {
                                                text: !parent.parent.writable ? "--" : point.batchWrite ? I18n.t("Selected", "已选择") : I18n.t("Off", "未选择")
                                                color: point.batchWrite ? Theme.textPrimary : Theme.textTertiary; font.pixelSize: 11
                                            }
                                        }
                                        MouseArea {
                                            anchors.fill: parent; enabled: parent.writable
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: if (backend) backend.points.togglePointBatchWrite(point.sourceRow)
                                        }
                                    }
                                    EditableValueCell { definition: point; fieldKey: "periodMs"; visible: root.columnVisible("period"); width: root.periodColumnWidth; height: parent.height; cellText: String(point.periodMs === undefined ? 1000 : point.periodMs); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "timeoutMs"; visible: root.columnVisible("timeout"); width: root.timeoutColumnWidth; height: parent.height; cellText: root.inheritedNumber(point.timeoutMs, backend ? String(backend.timeoutMs) : I18n.t("Inherit", "继承")); mono: true }
                                    EditableValueCell { definition: point; fieldKey: "retries"; visible: root.columnVisible("retries"); width: root.retriesColumnWidth; height: parent.height; cellText: root.inheritedNumber(point.retries, backend ? String(backend.retries) : I18n.t("Inherit", "继承")); mono: true }
                                }
                            }
                            Text { anchors.centerIn: parent; visible: pointList.count === 0; text: I18n.t("No data points for this device", "当前设备还没有数据点"); color: Theme.textTertiary; font.pixelSize: 12 }
                        }
                        }
                    }
                }
                Rectangle {
                    id: pointFooter
                    anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
                    height: 33; color: Theme.chromeBody
                    Rectangle { width: parent.width; height: 1; color: Theme.divider }
                    RowLayout {
                        anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                        Text { text: backend ? I18n.t("Points ", "数据点 ") + backend.points.count : ""; color: Theme.textTertiary; font.pixelSize: 10 }
                        Text { text: backend ? I18n.t("Selected ", "已选择 ") + backend.points.selectedCount : ""; color: Theme.cyan; font.pixelSize: 10 }
                        Item { Layout.fillWidth: true }
                        Text { text: root.forceDetailedColumns ? I18n.t("Detailed view", "详细视图") : I18n.t("Brief view", "简略视图"); color: Theme.textTertiary; font.pixelSize: 10 }
                    }
                }

            }
                }
            }
            ModbusTrafficDock {
                id: trafficDock
                objectName: "modbusTrafficDock"
                Layout.fillWidth: true; Layout.fillHeight: false
                Layout.preferredHeight: Math.min(272, Math.max(180, root.height * 0.28))
                Layout.minimumHeight: Layout.preferredHeight; Layout.maximumHeight: Layout.preferredHeight
                visible: root.trafficVisible
                backend: root.backend
                onCloseRequested: root.trafficVisible = false
                onStatisticsRequested: root.statisticsRequested()
            }
        }
    }

    Menu {
        id: blockFilterMenu
        title: I18n.t("Filter data blocks", "筛选数据区块")
        Repeater {
            model: [I18n.t("Coil", "线圈"), I18n.t("Discrete input", "离散输入"),
                    I18n.t("Holding register", "保持寄存器"), I18n.t("Input register", "输入寄存器")]
            delegate: TreeMenuItem {
                required property string modelData
                required property int index
                text: modelData
                checkable: true
                checked: backend ? (backend.points.areaMask & (1 << index)) !== 0 : true
                onTriggered: if (backend) backend.points.areaMask = backend.points.areaMask ^ (1 << index)
            }
        }
        MenuSeparator { }
        TreeMenuItem { text: I18n.t("Show all blocks", "显示全部区块"); onTriggered: if (backend) backend.points.areaMask = 15 }
    }

    Dialog {
        id: filterDialog
        property string kind: "rows"
        property var entries: []
        property var draftSelection: ({})
        property string query: filterSearch.text
        readonly property var filteredEntries: entries.filter(function(entry) {
            return String(entry.name).toLowerCase().indexOf(filterDialog.query.trim().toLowerCase()) >= 0
        })
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(530, parent ? parent.width - 48 : 530)
        height: Math.min(kind === "rows" ? 710 : 650, parent ? parent.height - 40 : 710)
        modal: true
        padding: 0
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle {
            color: Theme.surface
            radius: Theme.radiusLarge
            border.width: 1
            border.color: Theme.alpha(Theme.primary, 0.42)
        }
        function openRows() {
            if (!backend) return
            kind = "rows"
            const deviceId = backend.points.deviceId
            const points = backend.points.definitions().filter(function(point) { return point.deviceId === deviceId })
            entries = points.map(function(point, index) {
                return {key: String(point.id), name: String(point.name), number: index + 1}
            })
            const draft = ({})
            for (let index = 0; index < points.length; ++index)
                draft[String(points[index].id)] = points[index].rowVisible !== false
            draftSelection = draft
            filterSearch.text = ""
            open()
        }
        function openColumns() {
            if (!backend) return
            kind = "columns"
            entries = root.columnDefinitions().map(function(column, index) {
                return {key: column.key, name: column.name, number: index + 1}
            })
            const draft = ({})
            for (let index = 0; index < entries.length; ++index)
                draft[entries[index].key] = backend.visibleColumns[entries[index].key] !== false
            draftSelection = draft
            filterSearch.text = ""
            open()
        }
        function toggle(key) {
            const draft = Object.assign({}, draftSelection)
            draft[key] = draft[key] === false
            draftSelection = draft
        }
        onOpened: filterSearch.forceActiveFocus()
        onAccepted: {
            if (!backend) return
            if (kind === "rows") {
                const visibleIds = []
                for (let index = 0; index < entries.length; ++index)
                    if (draftSelection[entries[index].key] !== false)
                        visibleIds.push(entries[index].key)
                backend.points.setVisiblePointIds(visibleIds)
            } else {
                backend.setVisibleColumns(draftSelection)
                tableHorizontal.contentX = 0
            }
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 56
                color: Theme.dialogHeader
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 12
                    Text {
                        Layout.fillWidth: true
                        text: filterDialog.kind === "rows" ? I18n.t("Filter rows", "筛选行") : I18n.t("Filter columns", "筛选列")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 17
                        font.weight: Font.DemiBold
                    }
                    ToolButton {
                        id: filterClose
                        Layout.preferredWidth: 34
                        Layout.preferredHeight: 34
                        focusPolicy: Qt.NoFocus
                        contentItem: Text { text: "×"; color: filterClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: 9; color: filterClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                        onClicked: filterDialog.reject()
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 58
                Layout.leftMargin: 14
                Layout.rightMargin: 14
                spacing: 7
                AppTextField {
                    id: filterSearch
                    Layout.preferredWidth: 220
                    height: 36
                    placeholder: I18n.t("Enter name", "填写名称")
                }
                ToolbarGlyph { kind: "search"; accent: Theme.textSecondary; width: 30; height: 30 }
                Item { Layout.fillWidth: true }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 14
                Layout.rightMargin: 14
                color: Theme.surface
                border.width: 1
                border.color: Theme.border
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 38
                        color: Theme.tableHeader
                        Row {
                            anchors.fill: parent
                            Rectangle { width: 62; height: parent.height; color: "transparent"; border.width: 1; border.color: Theme.divider; Text { anchors.centerIn: parent; text: I18n.t("No.", "序号"); color: Theme.textPrimary; font.pixelSize: 13 } }
                            Rectangle { width: Math.max(0, filterList.width - 62 - 142); height: parent.height; color: "transparent"; border.width: 1; border.color: Theme.divider; Text { anchors.centerIn: parent; text: I18n.t("Name", "名称"); color: Theme.textPrimary; font.pixelSize: 13 } }
                            Rectangle { width: 142; height: parent.height; color: "transparent"; border.width: 1; border.color: Theme.divider; Text { anchors.centerIn: parent; text: I18n.t("Select", "选择"); color: Theme.textPrimary; font.pixelSize: 13 } }
                        }
                    }
                    ListView {
                        id: filterList
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        model: filterDialog.filteredEntries
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                        Text {
                            anchors.centerIn: parent
                            visible: filterList.count === 0
                            text: filterDialog.entries.length === 0
                                  ? I18n.t("No data points", "当前设备没有数据点")
                                  : I18n.t("No matching names", "没有匹配的名称")
                            color: Theme.textTertiary
                            font.pixelSize: 12
                        }
                        delegate: Rectangle {
                            required property var modelData
                            required property int index
                            width: filterList.width
                            height: 38
                            color: index % 2 === 0 ? Theme.surface : Theme.surfaceRaised
                            Row {
                                anchors.fill: parent
                                Rectangle { width: 62; height: parent.height; color: "transparent"; border.width: 1; border.color: Theme.divider; Text { anchors.centerIn: parent; text: String(modelData.number); color: Theme.textPrimary; font.pixelSize: 12 } }
                                Rectangle { width: Math.max(0, filterList.width - 62 - 142); height: parent.height; color: "transparent"; border.width: 1; border.color: Theme.divider; Text { anchors.fill: parent; anchors.leftMargin: 8; text: modelData.name; color: Theme.textPrimary; font.pixelSize: 12; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight } }
                                Rectangle {
                                    width: 142; height: parent.height
                                    color: filterDialog.draftSelection[modelData.key] !== false ? Theme.alpha(Theme.cyan, Theme.dark ? 0.23 : 0.18) : "transparent"
                                    border.width: 1; border.color: Theme.divider
                                    Row {
                                        anchors.centerIn: parent
                                        spacing: 10
                                        Rectangle {
                                            width: 16; height: 16; radius: 3
                                            color: filterDialog.draftSelection[modelData.key] !== false ? Theme.primary : Theme.inputBackground
                                            border.width: 1; border.color: filterDialog.draftSelection[modelData.key] !== false ? Theme.primary : Theme.border
                                            Text { anchors.centerIn: parent; visible: filterDialog.draftSelection[modelData.key] !== false; text: "✓"; color: Theme.onAccent; font.pixelSize: 11; font.weight: Font.Bold }
                                        }
                                        Text { text: filterDialog.draftSelection[modelData.key] !== false ? I18n.t("Selected", "已选择") : I18n.t("Hidden", "未选择"); color: Theme.textPrimary; font.pixelSize: 12 }
                                    }
                                }
                            }
                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: filterDialog.toggle(modelData.key) }
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                color: Theme.dialogFooter
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 18
                    Item { Layout.fillWidth: true }
                    AppButton { compact: true; text: I18n.t("Cancel", "取消"); variant: "outlined"; accent: Theme.textSecondary; onClicked: filterDialog.reject() }
                    AppButton { compact: true; text: I18n.t("OK", "确定"); variant: "primary"; onClicked: filterDialog.accept() }
                }
            }
        }
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
        TreeMenuItem {
            text: root.backend && root.contextDeviceIndex >= 0
                  && root.contextDeviceIndex < root.backend.devices.definitions().length
                  && root.backend.devices.definitions()[root.contextDeviceIndex].enabled === false
                  ? I18n.t("↻  Restore device", "↻  恢复设备")
                  : I18n.t("⊘  Disable device", "⊘  停用设备")
            enabled: root.backend && root.contextDeviceIndex >= 0
            onTriggered: {
                if (!root.backend || root.contextDeviceIndex < 0) return
                const device = root.backend.devices.definitions()[root.contextDeviceIndex]
                if (device) root.backend.setDeviceEnabled(root.contextDeviceIndex, device.enabled === false)
            }
        }
        MenuSeparator { }
        TreeMenuItem {
            text: I18n.t("×  Delete device", "×  删除设备")
            enabled: root.contextDeviceIndex >= 0
            onTriggered: root.removeDeviceRequested()
        }
    }

}
