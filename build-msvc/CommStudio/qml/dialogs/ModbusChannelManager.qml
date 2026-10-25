import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0

Dialog {
    id: control

    property var workspace
    property int selectedIndex: -1
    property int pendingRemoveIndex: -1
    property string transportFilter: "serial"
    readonly property real bandTint: Theme.dark ? 0.17 : 0.09
    readonly property color bandColor: control.mixedBandColor(Theme.primary)

    function mixedBandColor(accent) {
        const amount = control.bandTint
        return Qt.rgba(Theme.surface.r * (1 - amount) + accent.r * amount,
                       Theme.surface.g * (1 - amount) + accent.g * amount,
                       Theme.surface.b * (1 - amount) + accent.b * amount,
                       1.0)
    }

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min(920, parent ? parent.width - 56 : 920)
    height: Math.min(640, parent ? parent.height - 48 : 640)
    modal: true
    padding: 0
    closePolicy: Popup.CloseOnEscape
    onClosed: channelEditor.close()

    Overlay.modal: Rectangle {
        color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42)
    }

    background: Rectangle {
        color: Theme.surface
        radius: Theme.radiusLarge
        border.width: 1
        border.color: Theme.alpha(Theme.primary, 0.42)
    }

    function openManager() {
        if (workspace)
            workspace.refreshPorts()
        selectedIndex = workspace ? workspace.currentIndex : -1
        const current = selectedIndex >= 0 && workspace
                        ? workspace.channels[selectedIndex] : null
        setTransportFilter(current && current.transport === "network" ? "network" : "serial")
        open()
    }

    function openConfiguration(index) {
        if (!workspace || index < 0 || index >= workspace.channels.length)
            return
        selectChannel(index)
        const channel = workspace.channels[index]
        const backend = workspace.current
        channelEditor.creating = false
        channelEditor.sourceIndex = index
        channelEditor.transport = channel.transport
        channelEditor.pendingName = channel.alias || ""
        channelEditor.pendingMode = channel.mode
        channelEditor.pendingPortName = channel.portName || ""
        channelEditor.pendingBaudRate = channel.baudRate || "9600"
        channelEditor.pendingParity = channel.parity || "none"
        channelEditor.pendingDataBits = backend ? Number(backend.channelOptions.dataBits || 8) : 8
        channelEditor.pendingStopBits = backend ? Number(backend.channelOptions.stopBits || 1) : 1
        channelEditor.pendingFlowControl = backend ? Number(backend.channelOptions.flowControl || 0) : 0
        channelEditor.pendingHost = channel.host || "127.0.0.1"
        channelEditor.pendingTcpPort = Number(channel.port || 502)
        channelEditor.reloadPortChoices(backend)
        channelEditor.syncControls()
        channelEditor.open()
    }

    function openNewConfiguration(transport) {
        if (!workspace)
            return
        const backend = workspace ? workspace.current : null
        channelEditor.creating = true
        channelEditor.sourceIndex = -1
        channelEditor.transport = transport
        channelEditor.pendingName = transport === "network"
                                  ? I18n.t("TCP Channel ", "TCP 通道 ") + (workspace.channels.length + 1)
                                  : I18n.t("Serial Channel ", "串口通道 ") + (workspace.channels.length + 1)
        channelEditor.pendingMode = transport === "network" ? "tcpMaster" : "rtuMaster"
        channelEditor.pendingPortName = ""
        channelEditor.pendingBaudRate = "9600"
        channelEditor.pendingParity = "none"
        channelEditor.pendingDataBits = 8
        channelEditor.pendingStopBits = 1
        channelEditor.pendingFlowControl = 0
        channelEditor.pendingHost = "127.0.0.1"
        channelEditor.pendingTcpPort = 502
        channelEditor.reloadPortChoices(backend)
        channelEditor.syncControls()
        channelEditor.open()
    }

    function filteredChannels() {
        const result = []
        const channels = workspace ? workspace.channels : []
        for (let index = 0; index < channels.length; ++index) {
            const channel = channels[index]
            if (channel.transport === transportFilter
                    && (channel.transport === "network"
                        || (String(channel.portName || "").length > 0
                            && channel.available === true)))
                result.push(channel)
        }
        return result
    }

    function modeLabel(mode) {
        const labels = {
            "rtuMaster": I18n.t("RTU master", "RTU 主站"),
            "asciiMaster": I18n.t("ASCII master", "ASCII 主站"),
            "tcpMaster": I18n.t("TCP master", "TCP 主站"),
            "rtuSlave": I18n.t("RTU slave simulator", "RTU 从站模拟"),
            "asciiSlave": I18n.t("ASCII slave simulator", "ASCII 从站模拟"),
            "tcpSlave": I18n.t("TCP slave simulator", "TCP 从站模拟")
        }
        return labels[mode] || mode
    }

    function setTransportFilter(filter) {
        transportFilter = filter
        const rows = filteredChannels()
        let selectedVisible = false
        for (let index = 0; index < rows.length; ++index) {
            if (Number(rows[index].index) === selectedIndex) {
                selectedVisible = true
                break
            }
        }
        if (!selectedVisible) {
            selectedIndex = rows.length > 0 ? Number(rows[0].index) : -1
            if (selectedIndex >= 0 && workspace)
                workspace.select(selectedIndex)
        }
    }

    function statusText(channel) {
        if (channel.open)
            return I18n.t("Connected", "已连接")
        if (channel.connectionState === 1)
            return I18n.t("Connecting", "连接中")
        if (channel.connectionState === 3)
            return I18n.t("Closing", "断开中")
        return I18n.t("Closed", "已关闭")
    }

    function statusAccent(channel) {
        if (channel.open)
            return Theme.success
        if (channel.connectionState === 1 || channel.connectionState === 3)
            return Theme.warning
        return Theme.textTertiary
    }

    function selectChannel(index) {
        selectedIndex = index
        if (workspace)
            workspace.select(index)
    }

    contentItem: ColumnLayout {
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 76
            color: control.bandColor
            radius: Theme.radiusLarge

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: Theme.radiusLarge
                color: parent.color
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 22
                anchors.rightMargin: 68
                spacing: 12

                Rectangle {
                    Layout.preferredWidth: 42
                    Layout.preferredHeight: 42
                    radius: Theme.radius
                    color: Theme.alpha(Theme.primary, 0.16)
                    border.width: 1
                    border.color: Theme.alpha(Theme.primary, 0.42)
                    Text { anchors.centerIn: parent; text: "⌘"; color: Theme.primary; font.pixelSize: 22; font.weight: Font.DemiBold }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text { text: I18n.t("Channel manager", "通道管理"); color: Theme.textPrimary; font.pixelSize: 20; font.weight: Font.DemiBold }
                    Text {
                        text: control.transportFilter === "serial"
                              ? I18n.t("COM ports detected by Windows", "扫描 Windows 当前识别到的 COM 口")
                              : I18n.t("Configured TCP communication endpoints", "管理已配置的 TCP 通信端点")
                        color: Theme.textSecondary
                        font.pixelSize: 11
                    }
                }

                StatusPill {
                    text: I18n.count(control.filteredChannels().length,
                                     "channels", " 个通道")
                    accent: Theme.primary
                    dot: false
                    scaleFactor: 0.88
                }

            }

            ToolButton {
                id: closeButton
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                width: 34
                height: 34
                focusPolicy: Qt.NoFocus
                padding: 0
                ToolTip.visible: hovered
                ToolTip.delay: 400
                ToolTip.text: I18n.t("Close", "关闭")
                contentItem: Text {
                    text: "×"
                    color: closeButton.hovered || closeButton.pressed ? Theme.textPrimary : Theme.textSecondary
                    font.family: Theme.fontFamily
                    font.pixelSize: 21
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: Theme.radius
                    color: closeButton.pressed ? Theme.alpha(Theme.primary, 0.18)
                          : closeButton.hovered ? Theme.alpha(Theme.primary, 0.10)
                          : Theme.alpha(Theme.surface, 0.34)
                    border.width: 1
                    border.color: closeButton.hovered || closeButton.pressed
                                  ? Theme.alpha(Theme.primary, 0.40)
                                  : Theme.alpha(Theme.textSecondary, 0.20)
                    Behavior on color { ColorAnimation { duration: Theme.durationFast } }
                    Behavior on border.color { ColorAnimation { duration: Theme.durationFast } }
                }
                onClicked: control.reject()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 58
            color: Theme.surfaceRaised

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 8

                AppButton {
                    Layout.preferredWidth: 112
                    Layout.minimumWidth: 112
                    compact: true
                    text: "▤  " + I18n.t("SERIAL", "串口")
                    variant: control.transportFilter === "serial" ? "primary" : "outlined"
                    accent: Theme.primary
                    onClicked: control.setTransportFilter("serial")
                }
                AppButton {
                    Layout.preferredWidth: 112
                    Layout.minimumWidth: 112
                    compact: true
                    text: "⌁  " + I18n.t("NETWORK", "网络")
                    variant: control.transportFilter === "network" ? "primary" : "outlined"
                    accent: Theme.primary
                    onClicked: control.setTransportFilter("network")
                }

                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; Layout.topMargin: 12; Layout.bottomMargin: 12; color: Theme.divider }

                AppButton {
                    Layout.preferredWidth: 48
                    Layout.minimumWidth: 48
                    compact: true
                    text: "↻"
                    variant: "outlined"
                    accent: Theme.textSecondary
                    onClicked: if (control.workspace) control.workspace.refreshPorts()
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: control.transportFilter === "serial"
                                      ? I18n.t("Scan COM ports again", "重新扫描 COM 口")
                                      : I18n.t("Refresh TCP channels", "刷新网络通道")
                }
                AppButton {
                    visible: control.transportFilter === "network"
                    Layout.preferredWidth: visible ? 48 : 0
                    Layout.minimumWidth: visible ? 48 : 0
                    compact: true
                    text: "+"
                    variant: "outlined"
                    accent: Theme.primary
                    onClicked: control.openNewConfiguration(control.transportFilter)
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: I18n.t("Add channel", "新增通道")
                }
                AppButton {
                    visible: control.transportFilter === "network"
                    Layout.preferredWidth: visible ? 48 : 0
                    Layout.minimumWidth: visible ? 48 : 0
                    compact: true
                    text: "×"
                    variant: "outlined"
                    accent: Theme.error
                    enabled: control.selectedIndex >= 0 && control.workspace && control.workspace.channels.length > 1
                    onClicked: {
                        control.pendingRemoveIndex = control.selectedIndex
                        removeConfirm.open()
                    }
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: I18n.t("Remove selected channel", "删除选中通道")
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: control.transportFilter === "network"
                          ? I18n.t("TCP transport", "TCP 通信链路")
                          : I18n.t("Detected COM ports", "已扫描到的 COM 口")
                    color: Theme.textTertiary
                    font.pixelSize: 11
                }
            }

            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 16
            Layout.rightMargin: 16
            Layout.topMargin: 14
            color: Theme.surface
            radius: Theme.radius
            border.width: 1
            border.color: Theme.divider
            clip: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: Theme.surfaceRaised
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 0
                        Item { Layout.preferredWidth: 34 }
                        Text { Layout.fillWidth: true; Layout.minimumWidth: 180; text: I18n.t("Channel", "通道"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                        Text { Layout.preferredWidth: 190; text: I18n.t("Alias", "别名"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold }
                        Text { Layout.preferredWidth: 112; text: I18n.t("Status", "状态"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                        Text { Layout.preferredWidth: 96; text: I18n.t("Parameters", "参数"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                        Text { Layout.preferredWidth: 108; text: I18n.t("Control", "控制"); color: Theme.textSecondary; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                    }
                    Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
                }

                ListView {
                    id: channelList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: control.filteredChannels()
                    ScrollBar.vertical: ScrollBar {}

                    delegate: Rectangle {
                        id: channelRow
                        required property var modelData
                        required property int index
                        readonly property int sourceIndex: Number(modelData.index)
                        width: channelList.width
                        height: 58
                        color: sourceIndex === control.selectedIndex
                               ? Theme.selectedFill
                               : (index % 2 === 0 ? Theme.surface : Theme.surfaceRaised)

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: control.selectChannel(channelRow.sourceIndex)
                            onDoubleClicked: {
                                control.selectChannel(channelRow.sourceIndex)
                                control.openConfiguration(channelRow.sourceIndex)
                            }
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 0

                            Item {
                                Layout.preferredWidth: 34
                                Layout.fillHeight: true
                                Rectangle {
                                    width: 18
                                    height: 18
                                    anchors.centerIn: parent
                                    radius: 5
                                    color: channelRow.sourceIndex === control.selectedIndex ? Theme.primary : Theme.inputBackground
                                    border.width: 1
                                    border.color: channelRow.sourceIndex === control.selectedIndex ? Theme.primary : Theme.border
                                    Text { anchors.centerIn: parent; visible: channelRow.sourceIndex === control.selectedIndex; text: "✓"; color: "#FFFFFF"; font.pixelSize: 11; font.weight: Font.Bold }
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 180
                                spacing: 1
                                Text { Layout.fillWidth: true; text: channelRow.modelData.channelName; color: Theme.textPrimary; font.family: "Consolas"; font.pixelSize: 13; font.weight: channelRow.sourceIndex === control.selectedIndex ? Font.DemiBold : Font.Normal; elide: Text.ElideRight }
                                Text { Layout.fillWidth: true; text: channelRow.modelData.endpoint; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10; elide: Text.ElideMiddle }
                            }

                            ColumnLayout {
                                Layout.preferredWidth: 190
                                spacing: 1
                                Text {
                                    Layout.fillWidth: true
                                    text: String(channelRow.modelData.alias || "").length > 0
                                          ? channelRow.modelData.alias : "--"
                                    color: String(channelRow.modelData.alias || "").length > 0
                                           ? Theme.textPrimary : Theme.textTertiary
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }
                                Text { Layout.fillWidth: true; text: control.modeLabel(channelRow.modelData.mode); color: Theme.textTertiary; font.pixelSize: 9; elide: Text.ElideRight }
                            }
                            Item {
                                Layout.preferredWidth: 112
                                Layout.fillHeight: true
                                StatusPill { anchors.centerIn: parent; text: control.statusText(channelRow.modelData); accent: control.statusAccent(channelRow.modelData); dot: true; scaleFactor: 0.76 }
                            }
                            Item {
                                Layout.preferredWidth: 96
                                Layout.fillHeight: true
                                AppButton {
                                    anchors.centerIn: parent
                                    width: 80
                                    height: 30
                                    compact: true
                                    text: I18n.t("Configure", "配置")
                                    variant: "default"
                                    accent: Theme.textSecondary
                                    onClicked: control.openConfiguration(channelRow.sourceIndex)
                                }
                            }
                            Item {
                                Layout.preferredWidth: 108
                                Layout.fillHeight: true
                                AppButton {
                                    anchors.centerIn: parent
                                    width: 92
                                    height: 30
                                    compact: true
                                    text: channelRow.modelData.open ? I18n.t("Stop", "停止") : I18n.t("Start", "启动")
                                    variant: "outlined"
                                    accent: channelRow.modelData.open ? Theme.error : Theme.primary
                                    onClicked: {
                                        control.selectChannel(channelRow.sourceIndex)
                                        if (channelRow.modelData.open)
                                            control.workspace.closeChannel(channelRow.sourceIndex)
                                        else
                                            control.workspace.openChannel(channelRow.sourceIndex)
                                    }
                                }
                            }
                        }

                        Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: Theme.divider }
                    }

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        visible: channelList.count === 0
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: control.transportFilter === "network" ? "⌁" : "▤"; color: Theme.textTertiary; font.pixelSize: 32 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: I18n.t("No channels of this type", "暂无此类型通道"); color: Theme.textSecondary; font.pixelSize: 13 }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: control.transportFilter === "network"
                                  ? I18n.t("Use + to create one", "点击＋创建通道")
                                  : I18n.t("Connect a serial adapter, then refresh", "连接串口设备后点击刷新")
                            color: Theme.textTertiary
                            font.pixelSize: 11
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 72
            color: control.bandColor
            radius: Theme.radiusLarge

            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.alpha(Theme.primary, 0.20) }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 22
                anchors.rightMargin: 22
                anchors.topMargin: 8
                spacing: 12

                Text {
                    Layout.fillWidth: true
                    text: I18n.t("Channels contain transport parameters; Unit ID belongs to devices.", "通道只保存通信链路参数，Unit ID 归属于设备。")
                    color: Theme.textSecondary
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
                AppButton { text: I18n.t("Close", "关闭"); variant: "outlined"; accent: Theme.textSecondary; onClicked: control.accept() }
            }
        }
    }

    Popup {
        id: channelEditor

        property bool creating: false
        property int sourceIndex: -1
        property string transport: "serial"
        property string pendingName: ""
        property string pendingMode: "rtuMaster"
        property string pendingPortName: ""
        property string pendingBaudRate: "9600"
        property string pendingParity: "none"
        property int pendingDataBits: 8
        property int pendingStopBits: 1
        property int pendingFlowControl: 0
        property string pendingHost: "127.0.0.1"
        property int pendingTcpPort: 502
        property var portChoices: []
        readonly property bool serial: transport === "serial"
        readonly property bool channelActive: !creating && sourceIndex >= 0
                                              && control.workspace
                                              && sourceIndex < control.workspace.channels.length
                                              && control.workspace.channels[sourceIndex].open

        parent: control.contentItem
        x: 0
        y: 0
        width: control.contentItem ? control.contentItem.width : control.width
        height: control.contentItem ? control.contentItem.height : control.height
        modal: false
        dim: false
        padding: 0
        focus: true
        closePolicy: Popup.CloseOnEscape

        function modeValues() {
            return serial
                    ? ["rtuMaster", "asciiMaster", "rtuSlave", "asciiSlave"]
                    : ["tcpMaster", "tcpSlave"]
        }

        function protocolLabels() {
            return serial
                    ? [I18n.t("MODBUS-RTU master", "MODBUS-RTU 主站"),
                       I18n.t("MODBUS-ASCII master", "MODBUS-ASCII 主站"),
                       I18n.t("MODBUS-RTU slave simulator", "MODBUS-RTU 从站模拟"),
                       I18n.t("MODBUS-ASCII slave simulator", "MODBUS-ASCII 从站模拟")]
                    : [I18n.t("MODBUS-TCP master", "MODBUS-TCP 主站"),
                       I18n.t("MODBUS-TCP slave simulator", "MODBUS-TCP 从站模拟")]
        }

        function modeIndex() {
            const values = modeValues()
            const index = values.indexOf(pendingMode)
            return index >= 0 ? index : 0
        }

        function reloadPortChoices(backend) {
            if (control.workspace)
                control.workspace.refreshPorts()
            const values = []
            const infos = backend ? backend.portInfos : []
            for (let index = 0; index < infos.length; ++index) {
                const name = String(infos[index].name || "")
                if (name.length > 0 && values.indexOf(name) < 0)
                    values.push(name)
            }
            if (pendingPortName.length > 0 && values.indexOf(pendingPortName) < 0)
                values.unshift(pendingPortName)
            portChoices = values
        }

        function syncControls() {
            Qt.callLater(function() {
                editorName.text = channelEditor.pendingName
                protocolCombo.currentIndex = channelEditor.modeIndex()
                const portIndex = channelEditor.portChoices.indexOf(channelEditor.pendingPortName)
                serialPortCombo.currentIndex = portIndex >= 0 ? portIndex : (channelEditor.portChoices.length > 0 ? 0 : -1)
                if (channelEditor.pendingPortName.length === 0 && serialPortCombo.currentIndex >= 0)
                    channelEditor.pendingPortName = serialPortCombo.currentText
                baudCombo.currentIndex = Math.max(0, baudCombo.find(channelEditor.pendingBaudRate))
                parityCombo.currentIndex = channelEditor.pendingParity === "odd" ? 2 : (channelEditor.pendingParity === "even" ? 1 : 0)
                dataBitsCombo.currentIndex = Math.max(0, dataBitsCombo.find(String(channelEditor.pendingDataBits)))
                stopBitsCombo.currentIndex = channelEditor.pendingStopBits === 2 ? 1 : 0
                flowCombo.currentIndex = Math.max(0, Math.min(2, channelEditor.pendingFlowControl))
                hostField.text = channelEditor.pendingHost
                tcpPortField.text = String(channelEditor.pendingTcpPort)
                editorName.selectAll()
                editorName.forceActiveFocus()
            })
        }

        function validConfiguration() {
            if (serial)
                return pendingPortName.length > 0
            return pendingName.trim().length > 0
                    && pendingHost.trim().length > 0
                    && pendingTcpPort >= 1 && pendingTcpPort <= 65535
        }

        function saveConfiguration() {
            if (!control.workspace || channelActive || !validConfiguration())
                return
            let index = sourceIndex
            if (creating) {
                control.workspace.addChannel(pendingName.trim(), pendingMode)
                index = control.workspace.currentIndex
            } else {
                control.workspace.select(index)
                if (serial)
                    control.workspace.setAlias(index, pendingName.trim())
                else
                    control.workspace.rename(index, pendingName.trim())
            }
            const backend = control.workspace.current
            if (!backend)
                return
            backend.mode = pendingMode
            if (serial) {
                backend.portName = pendingPortName
                backend.baudRate = pendingBaudRate
                backend.parity = pendingParity
                backend.setChannelOption("dataBits", pendingDataBits)
                backend.setChannelOption("stopBits", pendingStopBits)
                backend.setChannelOption("flowControl", pendingFlowControl)
            } else {
                backend.host = pendingHost.trim()
                backend.port = pendingTcpPort
            }
            control.selectedIndex = index
            control.transportFilter = transport
            close()
        }

        background: Rectangle { color: "transparent" }

        contentItem: Item {
            Rectangle {
                anchors.fill: parent
                color: Theme.alpha("#050716", Theme.dark ? 0.68 : 0.38)
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons
                onWheel: function(wheel) { wheel.accepted = true }
            }

            Rectangle {
                id: editorCard
                anchors.centerIn: parent
                width: Math.min(620, parent.width - 36)
                height: Math.min(channelEditor.serial ? 584 : 454, parent.height - 28)
                color: Theme.surface
                radius: Theme.radiusLarge
                border.width: 1
                border.color: Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.52)

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 68
                        color: control.mixedBandColor(channelEditor.serial ? Theme.primary : Theme.cyan)
                        radius: Theme.radiusLarge

                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: Theme.radiusLarge
                            color: parent.color
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 62
                            spacing: 12

                            Rectangle {
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                radius: Theme.radius
                                color: Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.16)
                                border.width: 1
                                border.color: Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.42)
                                Text {
                                    anchors.centerIn: parent
                                    text: channelEditor.serial ? "▤" : "⌁"
                                    color: channelEditor.serial ? Theme.primary : Theme.cyan
                                    font.pixelSize: 21
                                    font.weight: Font.DemiBold
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 1
                                Text {
                                    text: channelEditor.serial
                                          ? I18n.t("Serial channel settings", "串口参数配置")
                                          : I18n.t("TCP channel settings", "网络参数配置")
                                    color: Theme.textPrimary
                                    font.pixelSize: 18
                                    font.weight: Font.DemiBold
                                }
                                Text {
                                    text: I18n.t("Settings are applied only after saving", "参数暂存，保存后应用到当前通道")
                                    color: Theme.textSecondary
                                    font.pixelSize: 10
                                }
                            }

                            StatusPill {
                                text: channelEditor.channelActive
                                      ? I18n.t("Stop channel first", "请先停止通道")
                                      : (channelEditor.creating ? I18n.t("New", "新增") : I18n.t("Editing", "编辑中"))
                                accent: channelEditor.channelActive ? Theme.warning : (channelEditor.serial ? Theme.primary : Theme.cyan)
                                dot: true
                                scaleFactor: 0.80
                            }

                        }

                        ToolButton {
                            id: editorCloseButton
                            anchors.right: parent.right
                            anchors.rightMargin: 14
                            anchors.verticalCenter: parent.verticalCenter
                            width: 32
                            height: 32
                            focusPolicy: Qt.NoFocus
                            padding: 0
                            ToolTip.visible: hovered
                            ToolTip.delay: 400
                            ToolTip.text: I18n.t("Close", "关闭")
                            contentItem: Text {
                                text: "×"
                                color: editorCloseButton.hovered || editorCloseButton.pressed
                                       ? Theme.textPrimary : Theme.textSecondary
                                font.family: Theme.fontFamily
                                font.pixelSize: 20
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                radius: 9
                                color: editorCloseButton.pressed
                                       ? Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.18)
                                       : editorCloseButton.hovered
                                         ? Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.10)
                                         : Theme.alpha(Theme.surface, 0.34)
                                border.width: 1
                                border.color: editorCloseButton.hovered || editorCloseButton.pressed
                                              ? Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.40)
                                              : Theme.alpha(Theme.textSecondary, 0.20)
                                Behavior on color { ColorAnimation { duration: Theme.durationFast } }
                                Behavior on border.color { ColorAnimation { duration: Theme.durationFast } }
                            }
                            onClicked: channelEditor.close()
                        }
                    }

                    ScrollView {
                        id: editorScroll
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        contentWidth: availableWidth
                        clip: true

                        ColumnLayout {
                            width: editorScroll.availableWidth
                            spacing: 9
                            enabled: !channelEditor.channelActive

                            Item { Layout.preferredHeight: 8 }

                            FormRow {
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: channelEditor.serial ? I18n.t("Alias", "别名")
                                                            : I18n.t("Channel name", "通道名称")
                                AppTextField {
                                    id: editorName
                                    Layout.fillWidth: true
                                    implicitHeight: 42
                                    placeholder: channelEditor.serial
                                                 ? I18n.t("Optional, e.g. Inverter bus", "可选，例如：逆变器总线")
                                                 : I18n.t("e.g. Inverter bus", "例如：逆变器总线")
                                    onTextChanged: channelEditor.pendingName = text
                                }
                            }

                            FormRow {
                                visible: channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Serial port", "串口端口")
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8
                                    AppComboBox {
                                        id: serialPortCombo
                                        Layout.fillWidth: true
                                        enabled: channelEditor.creating
                                        model: channelEditor.portChoices
                                        displayText: currentIndex >= 0
                                                     ? currentText
                                                     : I18n.t("No serial port detected", "未检测到可用串口")
                                        onActivated: channelEditor.pendingPortName = currentText
                                    }
                                    AppButton {
                                        Layout.preferredWidth: 44
                                        Layout.minimumWidth: 44
                                        compact: true
                                        text: "↻"
                                        variant: "outlined"
                                        accent: Theme.textSecondary
                                        onClicked: {
                                            channelEditor.reloadPortChoices(control.workspace ? control.workspace.current : null)
                                            channelEditor.syncControls()
                                        }
                                    }
                                }
                            }

                            FormRow {
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Protocol", "传输协议")
                                AppComboBox {
                                    id: protocolCombo
                                    Layout.fillWidth: true
                                    model: channelEditor.protocolLabels()
                                    onActivated: channelEditor.pendingMode = channelEditor.modeValues()[index]
                                }
                            }

                            FormRow {
                                visible: channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Baud rate", "波特率")
                                AppComboBox {
                                    id: baudCombo
                                    Layout.fillWidth: true
                                    model: ["1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"]
                                    onActivated: channelEditor.pendingBaudRate = currentText
                                }
                            }

                            FormRow {
                                visible: channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Parity", "校验方式")
                                AppComboBox {
                                    id: parityCombo
                                    Layout.fillWidth: true
                                    model: [I18n.t("None", "无 (None)"), I18n.t("Even", "偶校验 (Even)"), I18n.t("Odd", "奇校验 (Odd)")]
                                    onActivated: channelEditor.pendingParity = index === 2 ? "odd" : (index === 1 ? "even" : "none")
                                }
                            }

                            FormRow {
                                visible: channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Data bits", "数据位")
                                AppComboBox {
                                    id: dataBitsCombo
                                    Layout.fillWidth: true
                                    model: ["5", "6", "7", "8"]
                                    onActivated: channelEditor.pendingDataBits = Number(currentText)
                                }
                            }

                            FormRow {
                                visible: channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Stop bits", "停止位")
                                AppComboBox {
                                    id: stopBitsCombo
                                    Layout.fillWidth: true
                                    model: [I18n.t("1 bit", "1 位"), I18n.t("2 bits", "2 位")]
                                    onActivated: channelEditor.pendingStopBits = index === 1 ? 2 : 1
                                }
                            }

                            FormRow {
                                visible: channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("Flow control", "流控类型")
                                AppComboBox {
                                    id: flowCombo
                                    Layout.fillWidth: true
                                    model: [I18n.t("None", "无"), I18n.t("Hardware", "硬件流控"), I18n.t("Software", "软件流控")]
                                    onActivated: channelEditor.pendingFlowControl = index
                                }
                            }

                            FormRow {
                                visible: !channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: channelEditor.pendingMode === "tcpSlave"
                                       ? I18n.t("Listen address", "监听地址")
                                       : I18n.t("Target host", "目标主机")
                                AppTextField {
                                    id: hostField
                                    Layout.fillWidth: true
                                    implicitHeight: 42
                                    mono: true
                                    placeholder: "127.0.0.1"
                                    onTextChanged: channelEditor.pendingHost = text
                                }
                            }

                            FormRow {
                                visible: !channelEditor.serial
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                labelWidth: 116
                                label: I18n.t("TCP port", "TCP 端口")
                                AppTextField {
                                    id: tcpPortField
                                    Layout.fillWidth: true
                                    implicitHeight: 42
                                    mono: true
                                    validator: IntValidator { bottom: 1; top: 65535 }
                                    onTextChanged: if (acceptableInput) channelEditor.pendingTcpPort = Number(text)
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.leftMargin: 22
                                Layout.rightMargin: 22
                                Layout.preferredHeight: 38
                                radius: Theme.radiusSmall
                                color: Theme.alpha(channelEditor.channelActive ? Theme.warning : (channelEditor.serial ? Theme.primary : Theme.cyan), Theme.dark ? 0.12 : 0.07)
                                border.width: 1
                                border.color: Theme.alpha(channelEditor.channelActive ? Theme.warning : (channelEditor.serial ? Theme.primary : Theme.cyan), 0.22)
                                Text {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 12
                                    verticalAlignment: Text.AlignVCenter
                                    text: channelEditor.channelActive
                                          ? I18n.t("Stop this channel before changing transport parameters.", "通道运行中，请先停止后再修改通信参数。")
                                          : I18n.t("Unit ID is configured on the device, not on the channel.", "Unit ID 属于设备配置，不放在通道参数中。")
                                    color: channelEditor.channelActive ? Theme.warning : Theme.textSecondary
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                }
                            }

                            Item { Layout.preferredHeight: 8 }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 68
                        color: control.mixedBandColor(channelEditor.serial ? Theme.primary : Theme.cyan)
                        radius: Theme.radiusLarge

                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            height: Theme.radiusLarge
                            color: parent.color
                        }
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            height: 1
                            color: Theme.alpha(channelEditor.serial ? Theme.primary : Theme.cyan, 0.20)
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 22
                            anchors.rightMargin: 22
                            anchors.topMargin: 6
                            spacing: 10

                            Text {
                                Layout.fillWidth: true
                                text: channelEditor.serial
                                      ? I18n.t("Only essential serial settings are shown.", "仅保留必要的串口配置。")
                                      : I18n.t("Only essential TCP settings are shown.", "仅保留必要的网络配置。")
                                color: Theme.textSecondary
                                font.pixelSize: 10
                                elide: Text.ElideRight
                            }
                            AppButton {
                                compact: true
                                text: I18n.t("Cancel", "取消")
                                variant: "outlined"
                                accent: Theme.textSecondary
                                onClicked: channelEditor.close()
                            }
                            AppButton {
                                compact: true
                                text: channelEditor.creating ? I18n.t("Create", "创建") : I18n.t("Save", "保存")
                                variant: "primary"
                                accent: channelEditor.serial ? Theme.primary : Theme.cyan
                                enabled: !channelEditor.channelActive && channelEditor.validConfiguration()
                                onClicked: channelEditor.saveConfiguration()
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: control.workspace
        function onChannelsChanged() {
            if (!control.workspace || control.workspace.channels.length === 0)
                control.selectedIndex = -1
            else if (control.selectedIndex >= control.workspace.channels.length)
                control.selectedIndex = control.workspace.channels.length - 1
        }
    }

    ConfirmDialog {
        id: removeConfirm
        titleText: I18n.t("Remove channel", "移除通道")
        messageText: I18n.t("Stop and remove the selected channel?", "停止并移除选中的通道？")
        noteText: control.pendingRemoveIndex >= 0 && control.workspace
                  ? control.workspace.channels[control.pendingRemoveIndex].name : ""
        confirmText: I18n.t("Remove", "移除")
        riskText: I18n.t("Destructive action", "删除操作")
        accent: Theme.error
        onOperationConfirmed: {
            if (control.workspace && control.pendingRemoveIndex >= 0)
                control.workspace.remove(control.pendingRemoveIndex)
            control.selectedIndex = control.workspace ? control.workspace.currentIndex : -1
            control.pendingRemoveIndex = -1
        }
    }
}
