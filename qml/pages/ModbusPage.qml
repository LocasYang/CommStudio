import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root
    objectName: "modbusPage"
    property var backend: ModbusWorkspace.current
    property var pendingDeviceRemovals: []
    function inspectNewPoint() { addPoints.openForCurrentDevice() }
    function inspectNewChannel() { channelNameDialog.openFor(-1) }
    function inspectRemoveChannel() { removeChannel.open() }
    function inspectChannelManager() { channelManager.openManager() }
    function inspectChannelConfiguration() {
        channelManager.openManager()
        Qt.callLater(function() { channelManager.openConfiguration(ModbusWorkspace.currentIndex) })
    }
    function inspectDeviceAdvancedParameters() { advanced.openForCurrentDevice() }
    function inspectStartupWriteParameters() {
        advanced.openForCurrentDevice()
        startupInspectionScroll.restart()
    }
    Timer {
        id: startupInspectionScroll
        interval: 250
        onTriggered: {
            const flick = advancedTableScroll.contentItem
            flick.contentY = Math.max(0, flick.contentHeight - flick.height)
        }
    }
    function inspectBatchQuantity() {
        if (backend && backend.points.count > 0) {
            backend.points.selectPoint(0, false, false)
            batchPoints.openForSelection()
        } else {
            batchPoints.quantityLimit = 125
            batchQuantity.text = "1"
            batchPoints.open()
        }
    }
    function inspectRowsFilter() { dataMonitor.inspectRowsFilter() }
    function inspectColumnsFilter() { dataMonitor.inspectColumnsFilter() }
    function inspectBatchWriteDetails() { dataMonitor.inspectBatchWriteDetails() }
    function inspectCopyConfiguration() { copyConfiguration.openSelector() }
    function inspectTemplateExport() { templateExport.openForCurrentDevice() }
    function inspectStatistics() { diagnostics.scanner = false; diagnostics.open() }
    signal toastRequested(string text, bool ok)

    ConfirmDialog {
        id: removeChannel
        titleText: I18n.t("Remove channel", "移除通道")
        messageText: I18n.t("Stop and remove this channel from the workspace?", "停止当前通道并从工作区移除？")
        confirmText: I18n.t("Remove", "移除")
        riskText: I18n.t("Destructive action", "删除操作")
        accent: Theme.error
        onOperationConfirmed: { advanced.close(); ModbusWorkspace.remove(ModbusWorkspace.currentIndex) }
    }
    ModbusChannelManager {
        id: channelManager
        workspace: ModbusWorkspace
    }
    Dialog {
        id: deviceNameDialog
        property int editIndex: -1
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(440, parent ? parent.width - 48 : 440)
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
        function openFor(index) {
            editIndex = index
            deviceNameField.text = index >= 0 ? String(root.backend.devices.data(root.backend.devices.index(index, 0), 257) || "") : ""
            if (index >= 0) {
                deviceUnitField.text = String(root.backend.devices.data(root.backend.devices.index(index, 0), 258) || "1")
            } else {
                const used = ({})
                const devices = root.backend.devices.definitions()
                for (let item = 0; item < devices.length; ++item)
                    used[Number(devices[item].unitId)] = true
                let next = 1
                while (next <= 247 && used[next]) ++next
                deviceUnitField.text = String(Math.min(next, 247))
            }
            open()
            deviceNameField.forceActiveFocus()
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 62
                color: Theme.dialogHeader
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 14
                    Text { Layout.fillWidth: true; text: deviceNameDialog.editIndex >= 0 ? I18n.t("Edit device", "编辑设备") : I18n.t("Add device", "添加设备"); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 18; font.weight: Font.DemiBold }
                     ToolButton {
                         id: deviceNameClose
                         width: 34
                         height: 34
                         focusPolicy: Qt.NoFocus
                         contentItem: Text { text: "×"; color: deviceNameClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                         background: Rectangle { radius: 9; color: deviceNameClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                         onClicked: deviceNameDialog.reject()
                     }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                Layout.margins: 22
                spacing: 12
                FormRow { label: I18n.t("Name", "设备名称"); AppTextField { id: deviceNameField; Layout.fillWidth: true; placeholder: I18n.t("e.g. COM8 inverter", "例如：COM8 逆变器"); onAccepted: deviceNameDialog.accept() } }
                 FormRow {
                     label: "Unit ID"
                     AppTextField {
                         id: deviceUnitField
                         Layout.preferredWidth: 110
                         mono: true
                         text: "1"
                         validator: IntValidator { bottom: 1; top: 247 }
                         onAccepted: deviceNameDialog.accept()
                     }
                }
                Text { Layout.fillWidth: true; wrapMode: Text.WordWrap; text: I18n.t("The Unit ID is the Modbus slave address used by polling tasks and points.", "Unit ID 是轮询任务和数据点使用的 Modbus 从站地址。"); color: Theme.textTertiary; font.pixelSize: 11 }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                color: Theme.dialogFooter
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 22
                    anchors.rightMargin: 22
                    Item { Layout.fillWidth: true }
                    AppButton { compact: true; text: I18n.t("Cancel", "取消"); variant: "outlined"; accent: Theme.textSecondary; onClicked: deviceNameDialog.reject() }
                    AppButton { compact: true; text: I18n.t("Save", "保存"); variant: "primary"; enabled: deviceNameField.text.trim().length > 0 && deviceUnitField.acceptableInput; onClicked: deviceNameDialog.accept() }
                }
            }
        }
        onAccepted: {
            const name = deviceNameField.text.trim()
            const unit = Math.max(1, Math.min(247, parseInt(deviceUnitField.text) || 1))
            if (!name) return
            if (editIndex >= 0) {
                root.backend.updateDevice(editIndex, name, unit)
            } else {
                root.backend.addDevice(name, unit)
            }
        }
    }

    Dialog {
        id: channelNameDialog
        property int editIndex: -1
        property string pendingMode: "rtuMaster"
        readonly property bool editingSerialAlias: editIndex >= 0
                                                    && editIndex < ModbusWorkspace.channels.length
                                                    && ModbusWorkspace.channels[editIndex].transport === "serial"

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(440, parent ? parent.width - 48 : 440)
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

        function openFor(index, mode) {
            editIndex = index
            pendingMode = mode === undefined || mode === "" ? "rtuMaster" : mode
            channelNameField.text = index >= 0 && index < ModbusWorkspace.channels.length
                                     ? (ModbusWorkspace.channels[index].alias || "")
                                     : (pendingMode === "tcpMaster"
                                        ? I18n.t("TCP Channel ", "TCP 通道 ") + (ModbusWorkspace.channels.length + 1)
                                        : I18n.t("Serial Channel ", "串口通道 ") + (ModbusWorkspace.channels.length + 1))
            open()
            channelNameField.selectAll()
            channelNameField.forceActiveFocus()
        }

        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 62
                color: Theme.dialogHeader
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 14
                    Text {
                        Layout.fillWidth: true
                        text: channelNameDialog.editingSerialAlias
                              ? I18n.t("Edit channel alias", "修改通道别名")
                              : (channelNameDialog.editIndex >= 0
                                 ? I18n.t("Rename channel", "重命名通道")
                                 : I18n.t("New channel", "新增通道"))
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                    }
                    ToolButton {
                        id: channelNameClose
                        width: 34; height: 34; focusPolicy: Qt.NoFocus
                        contentItem: Text { text: "×"; color: channelNameClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: 9; color: channelNameClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                        onClicked: channelNameDialog.reject()
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                Layout.margins: 22
                spacing: 14
                FormRow {
                    Layout.fillWidth: true
                    label: channelNameDialog.editingSerialAlias ? I18n.t("Alias", "别名") : I18n.t("Name", "名称")
                    AppTextField { id: channelNameField; Layout.fillWidth: true; placeholder: channelNameDialog.editingSerialAlias ? I18n.t("Optional, e.g. Inverter line", "可选，例如：逆变器总线") : I18n.t("e.g. Inverter line", "例如：逆变器总线"); onAccepted: channelNameDialog.accept() }
                }
                Text {
                    Layout.fillWidth: true
                    text: I18n.t("Channel communication parameters can be adjusted from the context menu or Session parameters.", "通道通信参数可通过右键菜单或“会话参数”弹窗调整。")
                    color: Theme.textTertiary
                    font.family: Theme.fontFamily
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                color: Theme.dialogFooter
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 24; anchors.rightMargin: 24; anchors.topMargin: 8
                    Item { Layout.fillWidth: true }
                    AppButton { compact: true; text: I18n.t("Cancel", "取消"); variant: "outlined"; accent: Theme.textSecondary; onClicked: channelNameDialog.reject() }
                    AppButton {
                        compact: true; text: I18n.t("Save", "保存"); variant: "primary"
                        enabled: channelNameDialog.editingSerialAlias || channelNameField.text.trim().length > 0
                        onClicked: channelNameDialog.accept()
                    }
                }
            }
        }

        onAccepted: {
            const name = channelNameField.text.trim()
            if (editingSerialAlias)
                ModbusWorkspace.setAlias(editIndex, name)
            else if (editIndex >= 0 && name)
                ModbusWorkspace.rename(editIndex, name)
            else {
                if (!name) return
                ModbusWorkspace.addChannel(name, pendingMode)
                Qt.callLater(function() { channelManager.openConfiguration(ModbusWorkspace.currentIndex) })
            }
        }
    }
    ConfirmDialog {
        id: removeDevice
        titleText: I18n.t("Remove device", "移除设备")
        messageText: I18n.t("Remove this device and clear its points and polling tasks?", "删除此设备并清空其数据点和轮询任务？")
        confirmText: I18n.t("Remove", "移除")
        accent: Theme.error
        onOperationConfirmed: root.backend.removeDevice(root.backend.selectedDeviceIndex)
    }
    ConfirmDialog {
        id: removeSelectedDevices
        titleText: I18n.t("Delete selected devices", "删除选中设备")
        messageText: I18n.t("Delete all selected devices? Their points and polling tasks will also be cleared.",
                            "删除所有选中的设备？这些设备的数据点和轮询任务也会一并清空。")
        noteText: I18n.t("This operation cannot be undone.", "此操作无法撤销。")
        confirmText: I18n.t("Delete", "删除")
        riskText: I18n.t("Destructive action", "删除操作")
        accent: Theme.error
        infoRows: [{ label: I18n.t("Selected", "已选"),
                     value: I18n.count(root.pendingDeviceRemovals.length, "devices", " 个设备") }]
        onOperationConfirmed: {
            const removals = root.pendingDeviceRemovals.slice(0)
            removals.sort(function(left, right) {
                if (left.channelIndex !== right.channelIndex)
                    return right.channelIndex - left.channelIndex
                return right.deviceIndex - left.deviceIndex
            })
            for (let index = 0; index < removals.length; ++index) {
                const removal = removals[index]
                const channelBackend = ModbusWorkspace.channelBackend(removal.channelIndex)
                if (channelBackend)
                    channelBackend.removeDevice(removal.deviceIndex)
            }
            root.pendingDeviceRemovals = []
            dataMonitor.clearCheckedDevices()
            root.toastRequested(I18n.t("Selected devices deleted", "已删除选中设备"), true)
        }
    }
    Dialog {
        id: addPoints
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(520, parent ? parent.width - 48 : 520)
        modal: true
        padding: 0
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        function openForCurrentDevice() {
            dataCount.text = "1"
            dataBlock.currentIndex = 2
            startAddress.text = root.backend ? String(root.backend.points.suggestedAddress(3)) : "0"
            open()
            dataCount.forceActiveFocus()
            dataCount.selectAll()
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 62
                color: Theme.dialogHeader; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 14
                    Text { Layout.fillWidth: true; text: I18n.t("Add data configuration", "新增数据配置"); color: Theme.textPrimary; font.pixelSize: 18; font.weight: Font.DemiBold }
                    ToolButton {
                        id: addPointsClose; width: 34; height: 34; focusPolicy: Qt.NoFocus
                        contentItem: Text { text: "×"; color: addPointsClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: 9; color: addPointsClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                        onClicked: addPoints.reject()
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.margins: 22; spacing: 14
                Text { Layout.fillWidth: true; text: I18n.t("New rows are appended to the current device table.", "新数据会追加到当前设备点表末尾。"); color: Theme.textSecondary; font.pixelSize: 12 }
                FormRow { label: I18n.t("Data count", "数据条数"); AppTextField { id: dataCount; Layout.fillWidth: true; text: "1"; validator: IntValidator { bottom: 1; top: 4096 } } }
                FormRow {
                    label: I18n.t("Block", "区块")
                    AppComboBox {
                        id: dataBlock; Layout.fillWidth: true
                        model: [I18n.t("Coil (RW)", "线圈 (RW)"), I18n.t("Discrete input (RO)", "离散输入 (RO)"), I18n.t("Holding register (RW)", "保持寄存器 (RW)"), I18n.t("Input register (RO)", "输入寄存器 (RO)")]
                        currentIndex: 2
                        onActivated: if (root.backend) startAddress.text = String(root.backend.points.suggestedAddress(index + 1))
                    }
                }
                FormRow { label: I18n.t("Starting address", "起始数据地址"); AppTextField { id: startAddress; Layout.fillWidth: true; text: "0"; validator: IntValidator { bottom: 0; top: 65535 } } }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 68
                color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20; spacing: 10
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: addPoints.reject() }
                    AppButton {
                        text: I18n.t("Add", "新增"); variant: "primary"
                        enabled: root.backend && root.backend.points.deviceId.length > 0 && dataCount.acceptableInput && startAddress.acceptableInput
                                 && Number(startAddress.text) + Number(dataCount.text) <= 65536
                        onClicked: {
                            const ok = root.backend.points.addPoints(Number(dataCount.text), dataBlock.currentIndex + 1, Number(startAddress.text))
                            root.toastRequested(ok ? I18n.t("Data configuration added", "已新增数据配置") : I18n.t("Unable to add data configuration", "新增数据配置失败"), ok)
                            if (ok) addPoints.accept()
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: parseFrame
        property var parseResult: ({})
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(680, parent ? parent.width - 48 : 680)
        height: Math.min(600, parent ? parent.height - 48 : 600)
        modal: true; padding: 0; closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        function openParser() { parseResult = ({}); parseStart.text = "0"; parsePdu.text = ""; open(); parsePdu.forceActiveFocus() }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 62; color: Theme.dialogHeader; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 14
                    Text { Layout.fillWidth: true; text: I18n.t("Parse PDU frame", "解析 PDU 报文"); color: Theme.textPrimary; font.pixelSize: 18; font.weight: Font.DemiBold }
                    ToolButton {
                        id: parseClose
                        width: 34; height: 34
                        contentItem: Text { text: "×"; color: parseClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: 9; color: parseClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                        onClicked: parseFrame.reject()
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.margins: 22; spacing: 12
                Text { Layout.fillWidth: true; wrapMode: Text.WordWrap; text: I18n.t("Enter a PDU beginning with the function code. For read responses, also enter the request start address.", "输入以功能码开头的 PDU；解析读响应时还需填写请求的起始地址。"); color: Theme.textSecondary; font.pixelSize: 12 }
                FormRow { label: I18n.t("Start address", "起始地址"); AppTextField { id: parseStart; Layout.fillWidth: true; text: "0"; validator: IntValidator { bottom: 0; top: 65535 } } }
                FormRow { label: "PDU (HEX)"; AppTextField { id: parsePdu; Layout.fillWidth: true; placeholder: "03 04 00 64 00 C8" } }
                Text { visible: parseFrame.parseResult.ok === false; Layout.fillWidth: true; text: String(parseFrame.parseResult.error || ""); color: Theme.error; wrapMode: Text.WordWrap }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: Theme.surfaceRaised; radius: Theme.radius; border.width: 1; border.color: Theme.border
                    ListView {
                        anchors.fill: parent; anchors.margins: 1; clip: true
                        model: parseFrame.parseResult.results || []
                        ScrollBar.vertical: ScrollBar {}
                        delegate: Rectangle {
                            required property var modelData; required property int index
                            width: ListView.view.width; height: 42; color: index % 2 === 0 ? Theme.surface : Theme.surfaceRaised
                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                                Text { Layout.fillWidth: true; text: modelData.name + "  ·  " + modelData.address; color: Theme.textPrimary; elide: Text.ElideRight }
                                Text { text: String(modelData.value); color: Theme.cyan; font.family: "Consolas" }
                                Text { text: modelData.raw || ""; color: Theme.textTertiary; font.family: "Consolas" }
                            }
                        }
                        Text { anchors.centerIn: parent; visible: parent.count === 0 && parseFrame.parseResult.ok !== false; text: I18n.t("Parsed values will appear here", "解析结果将在这里显示"); color: Theme.textTertiary }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 68; color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Close", "关闭"); variant: "outlined"; onClicked: parseFrame.reject() }
                    AppButton { text: I18n.t("Parse", "解析"); variant: "primary"; enabled: root.backend && parseStart.acceptableInput && parsePdu.text.trim().length > 0; onClicked: parseFrame.parseResult = root.backend.points.parsePdu(Number(parseStart.text), parsePdu.text) }
                }
            }
        }
    }

    Dialog {
        id: copyConfiguration
        property var selectedTargets: ({})
        parent: Overlay.overlay; anchors.centerIn: parent
        width: Math.min(600, parent ? parent.width - 48 : 600); height: Math.min(560, parent ? parent.height - 48 : 560)
        modal: true; padding: 0; closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        function allTargetItems() {
            const result = []
            const channels = ModbusWorkspace.channels
            const currentDeviceId = root.backend ? root.backend.points.deviceId : ""
            for (let channelIndex = 0; channelIndex < channels.length; ++channelIndex) {
                const channel = channels[channelIndex]
                const channelBackend = ModbusWorkspace.channelBackend(Number(channel.index))
                if (!channelBackend) continue
                const devices = channelBackend.devices.definitions()
                for (let deviceIndex = 0; deviceIndex < devices.length; ++deviceIndex) {
                    const device = devices[deviceIndex]
                    if (Number(channel.index) === ModbusWorkspace.currentIndex && device.id === currentDeviceId)
                        continue
                    result.push({key: String(channel.index) + "|" + String(device.id),
                                 channelIndex: Number(channel.index), deviceIndex: deviceIndex,
                                 channelName: String(channel.channelName), mode: String(channel.mode),
                                 name: String(device.name),
                                 unitId: Number(device.unitId)})
                }
            }
            return result
        }
        function targetItems() {
            const all = allTargetItems()
            const channels = ModbusWorkspace.channels
            const selectedChannel = channelFilter.currentIndex > 0
                                    && channelFilter.currentIndex <= channels.length
                                    ? Number(channels[channelFilter.currentIndex - 1].index) : -1
            return all.filter(function(item) {
                const typeMatch = typeFilter.currentIndex === 0
                                  || (typeFilter.currentIndex === 1 && item.mode.toLowerCase().indexOf("master") >= 0)
                                  || (typeFilter.currentIndex === 2 && item.mode.toLowerCase().indexOf("slave") >= 0)
                return typeMatch && (selectedChannel < 0 || item.channelIndex === selectedChannel)
            })
        }
        function openSelector() {
            selectedTargets = ({})
            typeFilter.currentIndex = 0
            channelFilter.currentIndex = 0
            open()
        }
        function toggleTarget(id) { const next = Object.assign({}, selectedTargets); if (next[id]) delete next[id]; else next[id] = true; selectedTargets = next }
        function targetCount() { return Object.keys(selectedTargets).length }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 62; color: Theme.dialogHeader; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 14
                    Text { Layout.fillWidth: true; text: I18n.t("Copy data configuration", "复制数据配置"); color: Theme.textPrimary; font.pixelSize: 18; font.weight: Font.DemiBold }
                    ToolButton {
                        id: copyClose; Layout.preferredWidth: 34; Layout.preferredHeight: 34; focusPolicy: Qt.NoFocus
                        contentItem: Text { text: "×"; color: copyClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: 9; color: copyClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                        onClicked: copyConfiguration.reject()
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.margins: 22; spacing: 10
                Text { Layout.fillWidth: true; text: I18n.t("Select target devices. Their existing point tables will be fully overwritten.", "选择目标设备；目标设备现有点表会被完整覆盖。"); color: Theme.warning; wrapMode: Text.WordWrap }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    AppComboBox {
                        id: typeFilter
                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 36
                        model: [I18n.t("All types", "全部类型"), I18n.t("Master", "主站"), I18n.t("Slave", "从站")]
                    }
                    AppComboBox {
                        id: channelFilter
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        model: [I18n.t("All channels", "全部通道")].concat(ModbusWorkspace.channels.map(function(channel) { return String(channel.channelName) }))
                    }
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: Theme.surfaceRaised; radius: Theme.radius; border.width: 1; border.color: Theme.border
                    ListView {
                        anchors.fill: parent; clip: true; model: copyConfiguration.targetItems()
                        ScrollBar.vertical: ScrollBar {}
                        delegate: Rectangle {
                            required property var modelData; required property int index
                            width: ListView.view.width; height: 46
                            color: index % 2 === 0 ? Theme.surface : Theme.surfaceRaised
                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                                AppCheckBox { checked: copyConfiguration.selectedTargets[modelData.key] === true; onClicked: copyConfiguration.toggleTarget(modelData.key) }
                                Text { Layout.fillWidth: true; text: modelData.channelName + " / " + modelData.name; color: Theme.textPrimary; elide: Text.ElideRight }
                                Text { text: "Unit ID " + modelData.unitId; color: Theme.textTertiary; font.family: "Consolas" }
                            }
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 68; color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20
                    Text { text: I18n.count(copyConfiguration.targetCount(), "targets", " 个目标设备"); color: Theme.textSecondary }
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: copyConfiguration.reject() }
                    AppButton { text: I18n.t("Overwrite and copy", "覆盖复制"); variant: "primary"; enabled: copyConfiguration.targetCount() > 0; onClicked: copyConfigurationConfirm.open() }
                }
            }
        }
    }
    ConfirmDialog {
        id: copyConfigurationConfirm
        titleText: I18n.t("Overwrite data configuration", "覆盖数据配置")
        messageText: I18n.t("Replace the complete point table of every selected target device?", "用当前设备点表完整覆盖所有已选目标设备？")
        noteText: I18n.t("Existing target point configurations will be lost.", "目标设备原有数据配置将丢失。")
        confirmText: I18n.t("Overwrite", "覆盖")
        riskText: I18n.t("Overwrite operation", "覆盖操作")
        accent: Theme.warning
        onOperationConfirmed: {
            const targets = copyConfiguration.allTargetItems()
            let copied = 0
            for (let index = 0; index < targets.length; ++index) {
                const target = targets[index]
                if (copyConfiguration.selectedTargets[target.key]
                    && ModbusWorkspace.copyDevicePoints(ModbusWorkspace.currentIndex,
                                                         root.backend.selectedDeviceIndex,
                                                         target.channelIndex, target.deviceIndex))
                    ++copied
            }
            copyConfiguration.close()
            root.toastRequested(I18n.count(copied, "device configurations copied", " 个设备配置已复制"), copied > 0)
        }
    }
    Dialog {
        id: batchPoints
        property int quantityLimit: 125
        property string fieldKey: "count"
        property string fieldLabel: I18n.t("Quantity", "数量")
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(480, parent ? parent.width - 48 : 480)
        height: Math.min(292, parent ? parent.height - 48 : 292)
        padding: 0
        modal: true
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        function openForSelection(field, label, value) {
            if (!root.backend || root.backend.points.selectedCount <= 0) {
                root.toastRequested(I18n.t("Select one or more data points first", "请先选择一个或多个数据点"), false)
                return
            }
            quantityLimit = Math.max(1, root.backend.points.selectedQuantityLimit())
            fieldKey = field || "count"
            fieldLabel = label || I18n.t("Quantity", "数量")
            batchQuantity.text = value === undefined ? "1" : value
            open()
            batchQuantity.forceActiveFocus()
            batchQuantity.selectAll()
        }
        function applyQuantity() {
            if (root.backend.points.updateSelectedField(fieldKey, batchQuantity.text)) {
                root.toastRequested(I18n.t("Updated selected column: ", "已批量修改所选列：") + fieldLabel, true)
                close()
            } else {
                root.toastRequested(I18n.t("Batch modification failed", "批量修改失败"), false)
            }
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                color: Theme.dialogHeader
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                ToolButton {
                    id: batchPointsClose
                    anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter
                    width: 34; height: 34; focusPolicy: Qt.NoFocus
                    contentItem: Text { text: "×"; color: batchPointsClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 9; color: batchPointsClose.hovered ? Theme.hoverFill : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                    onClicked: batchPoints.reject()
                }
                Text {
                    anchors.left: parent.left; anchors.leftMargin: 20; anchors.right: batchPointsClose.left; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
                    text: I18n.t("Batch modify data", "批量修改数据")
                    color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 18; font.weight: Font.DemiBold; elide: Text.ElideRight
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 24; Layout.rightMargin: 24; Layout.topMargin: 18; Layout.bottomMargin: 16
                spacing: 10
                Text {
                    Layout.fillWidth: true
                    text: I18n.t("Set the target value (", "批量设置参数目标值（") + batchPoints.fieldLabel + I18n.t(")", "）")
                    color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 17; font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
                }
                AppTextField {
                    id: batchQuantity
                    Layout.fillWidth: true
                    Layout.preferredHeight: 42
                    text: "1"
                    mono: batchPoints.fieldKey !== "name" && batchPoints.fieldKey !== "unit"
                    font.pixelSize: 18
                    horizontalAlignment: TextInput.AlignHCenter
                    onAccepted: batchPoints.applyQuantity()
                }
                Text {
                    Layout.fillWidth: true
                    text: batchPoints.fieldKey === "range"
                          ? I18n.t("Enter minimum,maximum; leave blank to clear the range", "输入 最小值,最大值；留空清除范围")
                          : batchPoints.fieldKey === "count" && batchPoints.quantityLimit === 1
                          ? I18n.t("Bit data quantity is fixed at 1", "所选数据包含位数据，数量固定为 1")
                          : I18n.count(root.backend ? root.backend.points.selectedCount : 0, "selected points", " 个数据点已选择")
                    color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 72
                color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 18; spacing: 10
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: batchPoints.reject() }
                    AppButton { text: I18n.t("OK", "确定"); variant: "primary"; onClicked: batchPoints.applyQuantity() }
                }
            }
        }
    }
    Dialog {
        id: commandEditor
        property int pointRow: -1
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(480, parent ? parent.width - 48 : 480)
        height: Math.min(292, parent ? parent.height - 48 : 292)
        padding: 0
        modal: true
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        function openFor(row, currentValue) {
            pointRow = row
            commandValue.text = currentValue
            open()
            commandValue.forceActiveFocus()
            commandValue.selectAll()
        }
        function applyValue() {
            if (!root.backend || pointRow < 0 || commandValue.text.trim().length === 0) return
            if (root.backend.points.setWriteValue(pointRow, commandValue.text.trim())) {
                root.toastRequested(I18n.t("Write target updated", "写入目标值已更新"), true)
                close()
            }
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 64
                color: Theme.dialogHeader; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                ToolButton {
                    id: commandEditorClose
                    anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter
                    width: 34; height: 34; focusPolicy: Qt.NoFocus
                    contentItem: Text { text: "×"; color: commandEditorClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 9; color: commandEditorClose.hovered ? Theme.hoverFill : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                    onClicked: commandEditor.reject()
                }
                Text {
                    anchors.left: parent.left; anchors.leftMargin: 20; anchors.right: commandEditorClose.left; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
                    text: I18n.t("Input data", "输入数据")
                    color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 18; font.weight: Font.DemiBold; elide: Text.ElideRight
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.leftMargin: 24; Layout.rightMargin: 24; Layout.topMargin: 18; Layout.bottomMargin: 16
                spacing: 10
                Text {
                    Layout.fillWidth: true
                    text: I18n.t("Set the write command target value", "设置写入指令目标值")
                    color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 17; font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                }
                AppTextField {
                    id: commandValue
                    Layout.fillWidth: true; Layout.preferredHeight: 42
                    mono: true; font.pixelSize: 18; horizontalAlignment: TextInput.AlignHCenter
                    placeholder: I18n.t("Value or comma-separated values", "数值或逗号分隔的多个数值")
                    onAccepted: commandEditor.applyValue()
                }
                Text {
                    Layout.fillWidth: true
                    text: I18n.t("Click Write to send the command", "设置后点击“写”即可发送指令")
                    color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 72
                color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 18; spacing: 10
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: commandEditor.reject() }
                    AppButton { text: I18n.t("OK", "确定"); variant: "primary"; enabled: commandValue.text.trim().length > 0; onClicked: commandEditor.applyValue() }
                }
            }
        }
    }
    ConfirmDialog {
        id: deletePoints
        titleText: I18n.t("Delete selected points", "删除选中数据点")
        messageText: I18n.t("Remove the selected definitions from this device?", "从当前设备删除已选中的数据点定义？")
        noteText: I18n.t("Polling will stop. Related generated tasks and point history are cleared automatically; independent tasks are kept.", "将停止轮询，自动清理关联的生成任务和点历史，保留独立任务。")
        accent: Theme.error
        confirmText: I18n.t("Delete", "删除")
        onOperationConfirmed: if (root.backend.deleteSelectedPoints()) root.toastRequested(I18n.t("Selected points deleted", "已删除选中数据点"), true)
    }
    FileDialog {
        id: pointImport
        title: I18n.t("Select device data template", "选择设备数据模板")
        nameFilters: ["Device template (*.dtmthings *.json *.csv)", "MThings (*.dtmthings)", "JSON (*.json)", "CSV (*.csv)"]
        onAccepted: {
            const ok = root.backend.points.importFile(selectedFile)
            root.toastRequested(ok ? I18n.t("Imported", "已导入") : I18n.t("Import failed", "导入失败"), ok)
        }
    }
    Dialog {
        id: templateExport
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(560, parent ? parent.width - 48 : 560)
        height: Math.min(480, parent ? parent.height - 40 : 480)
        padding: 0; modal: true; closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        function openForCurrentDevice() {
            if (!root.backend || root.backend.selectedDeviceIndex < 0) return
            const devices = root.backend.devices.definitions()
            profileName.text = String(devices[root.backend.selectedDeviceIndex].name)
            open()
        }
        function metadata() {
            return {manufacturer: profileFactory.text.trim(), deviceName: profileName.text.trim(),
                    model: profileModel.text.trim(), version: profileVersion.text.trim(), notes: profileNotes.text,
                    deviceType: root.backend.mode.toLowerCase().indexOf("slave") >= 0 ? 2 : 1,
                    timeoutMs: root.backend.timeoutMs, retries: root.backend.retries}
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 60
                color: Theme.dialogHeader; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                Text {
                    anchors.left: parent.left; anchors.leftMargin: 20; anchors.right: exportProfileClose.left; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
                    text: I18n.t("Export device profile", "导出设备数据模板"); color: Theme.textPrimary; font.pixelSize: 18; font.weight: Font.DemiBold; elide: Text.ElideRight
                }
                ToolButton {
                    id: exportProfileClose
                    anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
                    width: 34; height: 34; focusPolicy: Qt.NoFocus
                    contentItem: Text { text: "×"; color: exportProfileClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: Theme.radius; color: exportProfileClose.down ? Theme.selectedFill : exportProfileClose.hovered ? Theme.hoverFill : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                    onClicked: templateExport.reject()
                    ToolTip.visible: hovered; ToolTip.text: I18n.t("Close", "关闭")
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.margins: 18; spacing: 8
                FormRow { label: I18n.t("Manufacturer *", "厂家名称 *"); AppTextField { id: profileFactory; Layout.fillWidth: true; Layout.preferredHeight: 36 } }
                FormRow { label: I18n.t("Device name *", "设备名称 *"); AppTextField { id: profileName; Layout.fillWidth: true; Layout.preferredHeight: 36 } }
                FormRow { label: I18n.t("Model *", "设备型号 *"); AppTextField { id: profileModel; Layout.fillWidth: true; Layout.preferredHeight: 36 } }
                FormRow { label: I18n.t("Version *", "配置版本 *"); AppTextField { id: profileVersion; Layout.fillWidth: true; Layout.preferredHeight: 36; text: "1.0" } }
                FormRow { label: I18n.t("Notes", "备注"); AppTextField { id: profileNotes; Layout.fillWidth: true; Layout.preferredHeight: 36 } }
                Text { Layout.fillWidth: true; text: I18n.t("Exports the current device's complete point table, including hidden rows.", "导出当前设备的完整点表，包含已隐藏的数据行。"); color: Theme.textSecondary; wrapMode: Text.WordWrap; font.pixelSize: 12 }
                Item { Layout.fillHeight: true }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 72; color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20; spacing: 10
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: templateExport.reject() }
                    AppButton {
                        text: I18n.t("Export", "导出"); variant: "primary"
                        enabled: profileFactory.text.trim() !== "" && profileName.text.trim() !== "" && profileModel.text.trim() !== "" && profileVersion.text.trim() !== ""
                        onClicked: pointExport.open()
                    }
                }
            }
        }
    }
    FileDialog {
        id: pointExport
        fileMode: FileDialog.SaveFile
        nameFilters: ["MThings Modbus (*.dtmthings)", "CommStudio JSON (*.json)", "CSV point table (*.csv)"]
        defaultSuffix: selectedNameFilter.index === 0 ? "dtmthings" : selectedNameFilter.index === 1 ? "json" : "csv"
        onAccepted: {
            const ok = root.backend.points.exportFile(selectedFile, templateExport.metadata())
            root.toastRequested(ok ? I18n.t("Exported", "已导出") : I18n.t("Export failed. Check the file path or use JSON for unsupported MThings data types.", "导出失败，请检查路径；MThings 不支持的类型与数量组合请使用 JSON 格式。"), ok)
            if (ok) templateExport.close()
        }
    }
    Dialog {
        id: pointEditor
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(620, parent ? parent.width - 48 : 620)
        height: Math.min(720, parent ? parent.height - 48 : 720)
        modal: true
        padding: 22
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        property int editingRow: -1
        property var draft: ({})
        function edit(row) {
            editingRow = row
            draft = row < 0 ? {name: "Point", fc: 3, address: 0, type: "uint16", wordOrder: "ABCD", scale: 1, offset: 0, unit: "", periodMs: 1000, poll: true} : root.backend.points.pointAt(row)
            pointName.text = draft.name
            pointAddress.text = String(draft.address)
            pointArea.currentIndex = draft.fc - 1
            pointType.currentIndex = pointType.model.indexOf(draft.type)
            pointOrder.currentIndex = pointOrder.model.indexOf(draft.wordOrder)
            pointScale.text = String(draft.scale)
            pointOffset.text = String(draft.offset)
            pointUnit.text = draft.unit || ""
            pointPeriod.text = String(draft.periodMs)
            pointPoll.checked = draft.poll
            pointCount.text = String(draft.count || 1)
            pointDisplay.currentIndex = Math.max(0, pointDisplay.model.indexOf(draft.displayType || "DEC"))
            pointDecimals.text = String(draft.decimals === undefined ? 3 : draft.decimals)
            pointEnums.text = JSON.stringify(draft.enumMap || {})
            pointBits.text = String(draft.bitCount || 0)
            pointBitOffset.text = String(draft.bitOffset || 0)
            pointGroup.text = draft.group || ""
            pointNotes.text = draft.notes || ""
            pointTimeout.text = String(draft.timeoutMs === undefined ? -1 : draft.timeoutMs)
            pointRetries.text = String(draft.retries === undefined ? -1 : draft.retries)
            pointMin.text = draft.minValue === undefined ? "" : String(draft.minValue)
            pointMax.text = draft.maxValue === undefined ? "" : String(draft.maxValue)
            open()
        }
        background: Rectangle { color: Theme.surface; border.color: Theme.border; radius: Theme.radiusLarge }
        contentItem: ColumnLayout {
            Text { text: I18n.t("DATA POINT INSPECTOR", "数据点编辑"); color: Theme.cyan; font.pixelSize: 18 }
            ScrollView {
                id: pointFormScroll
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: availableWidth
                ColumnLayout {
                    width: pointFormScroll.availableWidth
                    spacing: 10
                    FormRow { label: I18n.t("Name", "名称"); AppTextField { id: pointName; Layout.fillWidth: true } }
                    FormRow { label: I18n.t("Area", "区域"); AppComboBox { id: pointArea; Layout.fillWidth: true; model: ["Coil", "Discrete Input", "Holding Register", "Input Register"] } }
                    FormRow { label: I18n.t("Address", "地址"); AppTextField { id: pointAddress; Layout.fillWidth: true; placeholder: "0 / 0x0100" } }
                    FormRow { label: I18n.t("Type", "类型"); AppComboBox { id: pointType; Layout.fillWidth: true; model: ["bit", "uint16", "int16", "uint32", "int32", "float32", "uint64", "int64", "float64", "bcd", "bytes", "string"] } }
                    FormRow { label: I18n.t("Length", "长度"); visible: ["bcd", "bytes", "string"].indexOf(pointType.currentText) >= 0; AppTextField { id: pointCount; Layout.fillWidth: true; validator: IntValidator { bottom: 1; top: 125 } } }
                    FormRow { label: I18n.t("Display", "显示"); AppComboBox { id: pointDisplay; Layout.fillWidth: true; model: ["DEC", "HEX", "BIN", "FLOAT", "STRING", "ENUM", "TIMESTAMP", "BIT FIELD"] } }
                    FormRow { label: I18n.t("Decimals", "小数位"); AppTextField { id: pointDecimals; Layout.fillWidth: true; validator: IntValidator { bottom: 0; top: 12 } } }
                    FormRow { label: I18n.t("Bit offset", "位偏移"); AppTextField { id: pointBitOffset; Layout.fillWidth: true; validator: IntValidator { bottom: 0; top: 63 } } }
                    FormRow { label: I18n.t("Bit count", "位数"); AppTextField { id: pointBits; Layout.fillWidth: true; placeholder: "0 = full value"; validator: IntValidator { bottom: 0; top: 64 } } }
                    FormRow { label: I18n.t("Enum JSON", "枚举 JSON"); visible: pointDisplay.currentText === "ENUM"; AppTextField { id: pointEnums; Layout.fillWidth: true; placeholder: '{"0":"STOP","1":"RUN"}' } }
                    FormRow { label: I18n.t("Byte order", "字节序"); AppComboBox { id: pointOrder; Layout.fillWidth: true; model: ["ABCD", "BADC", "CDAB", "DCBA"] } }
                    FormRow { label: I18n.t("Scale", "系数"); AppTextField { id: pointScale; Layout.fillWidth: true; validator: DoubleValidator {} } }
                    FormRow { label: I18n.t("Offset", "偏移"); AppTextField { id: pointOffset; Layout.fillWidth: true; validator: DoubleValidator {} } }
                    FormRow { label: I18n.t("Unit", "单位"); AppTextField { id: pointUnit; Layout.fillWidth: true } }
                    FormRow { label: I18n.t("Min", "下限"); AppTextField { id: pointMin; Layout.fillWidth: true; placeholder: I18n.t("optional", "可选"); validator: DoubleValidator {} } }
                    FormRow { label: I18n.t("Max", "上限"); AppTextField { id: pointMax; Layout.fillWidth: true; placeholder: I18n.t("optional", "可选"); validator: DoubleValidator {} } }
                    FormRow { label: I18n.t("Period (ms)", "周期 (ms)"); AppTextField { id: pointPeriod; Layout.fillWidth: true; validator: IntValidator { bottom: 10; top: 3600000 } } }
                    FormRow { label: "Timeout ms"; AppTextField { id: pointTimeout; Layout.fillWidth: true; placeholder: "-1 = inherit"; validator: IntValidator { bottom: -1; top: 60000 } } }
                    FormRow { label: "Retry"; AppTextField { id: pointRetries; Layout.fillWidth: true; placeholder: "-1 = inherit"; validator: IntValidator { bottom: -1; top: 10 } } }
                    FormRow { label: I18n.t("Group", "分组"); AppTextField { id: pointGroup; Layout.fillWidth: true } }
                    FormRow { label: I18n.t("Notes", "备注"); AppTextField { id: pointNotes; Layout.fillWidth: true } }
                    AppCheckBox { id: pointPoll; text: I18n.t("Include in polling", "参与轮询") }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                AppButton { compact: true; text: I18n.t("Copy", "复制"); enabled: pointEditor.editingRow >= 0; onClicked: { root.backend.points.copyPoint(pointEditor.editingRow); pointEditor.close() } }
                AppButton { compact: true; text: I18n.t("Delete", "删除"); enabled: pointEditor.editingRow >= 0; onClicked: pointDelete.open() }
                Item { Layout.fillWidth: true }
                AppButton { compact: true; text: I18n.t("Cancel", "取消"); onClicked: pointEditor.close() }
                AppButton {
                    compact: true
                    variant: "primary"
                    text: I18n.t("Apply", "应用")
                    enabled: pointScale.acceptableInput && pointOffset.acceptableInput && pointPeriod.acceptableInput
                             && pointMin.acceptableInput && pointMax.acceptableInput
                             && pointName.text.trim().length > 0 && isFinite(Number(pointAddress.text))
                    onClicked: {
                        const p = Object.assign({}, pointEditor.draft, {name: pointName.text, address: Number(pointAddress.text), fc: pointArea.currentIndex + 1,
                            type: pointType.currentText, wordOrder: pointOrder.currentText, scale: Number(pointScale.text),
                            offset: Number(pointOffset.text), unit: pointUnit.text, periodMs: Number(pointPeriod.text), poll: pointPoll.checked,
                            count: Number(pointCount.text), displayType: pointDisplay.currentText, decimals: Number(pointDecimals.text),
                            bitOffset: Number(pointBitOffset.text), bitCount: Number(pointBits.text), group: pointGroup.text, notes: pointNotes.text,
                            timeoutMs: Number(pointTimeout.text), retries: Number(pointRetries.text),
                            minValue: pointMin.text.trim() === "" ? undefined : Number(pointMin.text),
                            maxValue: pointMax.text.trim() === "" ? undefined : Number(pointMax.text)})
                        try { p.enumMap = JSON.parse(pointEnums.text) } catch(e) { root.toastRequested(I18n.t("Invalid enum JSON", "枚举 JSON 无效"), false); return }
                        if (root.backend.points.savePoint(pointEditor.editingRow, p)) pointEditor.close()
                    }
                }
            }
        }
    }
    ConfirmDialog {
        id: pointDelete
        titleText: I18n.t("Delete point", "删除数据点")
        messageText: I18n.t("Delete this point definition?", "删除此数据点定义？")
        noteText: pointName.text
        confirmText: I18n.t("Delete", "删除")
        accent: Theme.error
        onOperationConfirmed: {
            root.backend.points.selectPoint(pointEditor.editingRow, false, false)
            if (root.backend.deleteSelectedPoints()) pointEditor.close()
        }
    }
    Dialog {
        id: advanced
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(760, parent ? parent.width - 40 : 760)
        height: Math.min(780, parent ? parent.height - 32 : 780)
        padding: 0
        modal: true
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        property int deviceIndex: -1
        property var draft: ({})
        readonly property var parameterRows: [
            { key: "addressOffset", label: I18n.t("Address offset", "地址偏移"), kind: "number", minimum: -65535, maximum: 65535, fallback: 0 },
            { key: "displayAddress", label: I18n.t("Address display mode", "地址显示模式"), kind: "choice", options: [
                { label: I18n.t("Decimal", "十进制"), value: "decimal" }, { label: I18n.t("Hexadecimal", "十六进制"), value: "hex" } ] },
            { key: "crcByteOrder", label: I18n.t("CRC byte order (RTU only)", "CRC字节序（RTU模式生效）"), kind: "choice", options: [
                { label: I18n.t("Little endian", "小端"), value: "little" }, { label: I18n.t("Big endian", "大端"), value: "big" } ] },
            { key: "coilBitOrder", label: I18n.t("Coil/discrete bit order", "线圈/离散量位序"), kind: "choice", options: [
                { label: I18n.t("High bit on left", "高位在左"), value: "msb" }, { label: I18n.t("Low bit on left", "低位在左"), value: "lsb" } ] },
            { key: "packageMode", label: I18n.t("Package method", "组包方式"), kind: "choice", options: [
                { label: I18n.t("By line", "按行"), value: "line" }, { label: I18n.t("By address", "按地址大小"), value: "address" },
                { label: I18n.t("By list order", "按列表顺序"), value: "list" } ] },
            { key: "maxBitGap", label: I18n.t("Maximum bit-data gap", "位数据最大允许间隔"), kind: "number", minimum: 0, maximum: 1999, fallback: 0 },
            { key: "maxRegisterGap", label: I18n.t("Maximum register gap", "寄存器最大允许间隔"), kind: "number", minimum: 0, maximum: 124, fallback: 5 },
            { key: "maxBitRead", label: I18n.t("Maximum continuous bit read", "位数据最大连续读个数"), kind: "number", minimum: 1, maximum: 2000, fallback: 2000 },
            { key: "maxRegisterRead", label: I18n.t("Maximum continuous register read", "寄存器最大连续读个数"), kind: "number", minimum: 1, maximum: 125, fallback: 125 },
            { key: "singleRegisterWriteCode", label: I18n.t("Single-register write function", "单寄存器写功能码"), kind: "choice", options: [
                { label: "0x06", value: 6 }, { label: "0x10", value: 16 } ] },
            { key: "singleCoilWriteCode", label: I18n.t("Single-coil write function", "单线圈写功能码"), kind: "choice", options: [
                { label: "0x05", value: 5 }, { label: "0x0F", value: 15 } ] },
            { key: "broadcast", label: I18n.t("Broadcast device", "广播设备"), kind: "choice", options: [
                { label: I18n.t("No", "否"), value: false }, { label: I18n.t("Yes", "是"), value: true } ] },
            { key: "pollAfterStartup", label: I18n.t("Poll immediately after startup", "启动后立即轮询"), kind: "choice", options: [
                { label: I18n.t("Off", "关闭"), value: false }, { label: I18n.t("On", "开启"), value: true } ] },
            { key: "writeAfterStartup", label: I18n.t("Batch write after startup", "启动后批量写"), kind: "choice", options: [
                { label: I18n.t("Off", "关闭"), value: "off" }, { label: I18n.t("Once", "单次"), value: "once" },
                { label: I18n.t("Cycle", "循环"), value: "cycle" } ] }
        ]
        function openForCurrentDevice() { openForDevice(root.backend ? root.backend.selectedDeviceIndex : -1) }
        function openForDevice(index) {
            if (!root.backend || index < 0 || index >= root.backend.devices.definitions().length) {
                root.toastRequested(I18n.t("Select a device first", "请先选择设备"), false)
                return
            }
            deviceIndex = index
            draft = root.backend.devices.advancedOptions(index)
            open()
        }
        function setDraftValue(key, value) {
            let next = {}
            for (let item in draft) next[item] = draft[item]
            next[key] = value
            draft = next
        }
        function optionLabels(options) {
            let result = []
            for (let i = 0; i < options.length; ++i) result.push(options[i].label)
            return result
        }
        function optionIndex(options, value) {
            for (let i = 0; i < options.length; ++i)
                if (options[i].value === value) return i
            return 0
        }
        function saveDeviceOptions() {
            if (!root.backend || deviceIndex < 0) return
            if (!root.backend.devices.setAdvancedOptions(deviceIndex, draft)) {
                root.toastRequested(I18n.t("Unable to save device parameters", "设备高级参数保存失败"), false)
                return
            }
            root.toastRequested(I18n.t("Device parameters saved", "设备高级参数已保存"), true)
            close()
        }
        background: Rectangle {
            color: Theme.surface
            radius: Theme.radiusLarge
            border.width: 1
            border.color: Theme.alpha(Theme.primary, 0.42)
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                color: Theme.dialogHeader
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                ToolButton {
                    id: advancedClose
                    anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter
                    width: 34; height: 34; focusPolicy: Qt.NoFocus
                    contentItem: Text { text: "×"; color: advancedClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 9; color: advancedClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                    onClicked: advanced.reject()
                }
                RowLayout {
                    anchors.left: parent.left; anchors.leftMargin: 20
                    anchors.right: advancedClose.left; anchors.rightMargin: 12
                    anchors.top: parent.top; anchors.bottom: parent.bottom
                    spacing: 12
                    Rectangle {
                        Layout.preferredWidth: 34; Layout.preferredHeight: 34; radius: 9
                        color: Theme.alpha(Theme.primary, 0.10)
                        border.width: 1; border.color: Theme.alpha(Theme.primary, 0.30)
                        Text { anchors.centerIn: parent; text: "⌘"; color: Theme.primary; font.pixelSize: 18 }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 1
                        Text { text: I18n.t("Edit device advanced parameters", "修改设备高级参数"); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 18; font.weight: Font.DemiBold }
                        Text {
                            text: {
                                if (!root.backend || advanced.deviceIndex < 0) return ""
                                const item = root.backend.devices.definitions()[advanced.deviceIndex]
                                return item ? String(item.name) + "  ·  Unit ID " + String(item.unitId) : ""
                            }
                            color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 42
                Layout.leftMargin: 14; Layout.rightMargin: 14; Layout.topMargin: 12
                color: Theme.surfaceRaised
                border.width: 1; border.color: Theme.border
                RowLayout {
                    anchors.fill: parent; spacing: 0
                    Text { Layout.preferredWidth: parent.width * 0.46; Layout.fillHeight: true; text: I18n.t("Parameter", "参数"); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 15; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.border }
                    Text { Layout.fillWidth: true; Layout.fillHeight: true; text: I18n.t("Value", "数值"); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 15; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                }
            }
            ScrollView {
                id: advancedTableScroll
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.leftMargin: 14; Layout.rightMargin: 14
                clip: true; contentWidth: availableWidth
                background: Rectangle { color: Theme.surface }
                Column {
                    width: advancedTableScroll.availableWidth
                    Repeater {
                        model: advanced.parameterRows
                        delegate: Rectangle {
                            required property int index
                            required property var modelData
                            width: advancedTableScroll.availableWidth
                            height: 43
                            color: index % 2 === 0 ? Theme.surfaceRaised : Theme.surface
                            border.width: 1; border.color: Theme.border
                            RowLayout {
                                anchors.fill: parent; spacing: 0
                                Text {
                                    Layout.preferredWidth: parent.width * 0.46; Layout.fillHeight: true
                                    leftPadding: 12; rightPadding: 10
                                    text: modelData.label; color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 13
                                    verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
                                }
                                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.border }
                                Loader {
                                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.margins: 3
                                    sourceComponent: modelData.kind === "choice" ? advancedChoiceEditor : advancedNumberEditor
                                }
                                Component {
                                    id: advancedNumberEditor
                                    AppTextField {
                                        text: String(advanced.draft[modelData.key] === undefined ? modelData.fallback : advanced.draft[modelData.key])
                                        validator: IntValidator { bottom: modelData.minimum; top: modelData.maximum }
                                        onTextEdited: if (acceptableInput) advanced.setDraftValue(modelData.key, Number(text))
                                    }
                                }
                                Component {
                                    id: advancedChoiceEditor
                                    AppComboBox {
                                        enabled: modelData.key !== "writeAfterStartup" || (root.backend && (root.backend.mode === "rtuMaster" || root.backend.mode === "tcpMaster"))
                                        model: advanced.optionLabels(modelData.options)
                                        currentIndex: advanced.optionIndex(modelData.options, advanced.draft[modelData.key])
                                        onActivated: advanced.setDraftValue(modelData.key, modelData.options[index].value)
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Text {
                Layout.fillWidth: true; Layout.leftMargin: 18; Layout.rightMargin: 18
                Layout.topMargin: 6; Layout.bottomMargin: 6
                text: I18n.t("Restart cyclic writing after parameter changes. Startup writing is off by default (RTU/TCP masters only).",
                             "循环写参数变更后需停止并重启；启动批量写默认关闭，仅适用于 RTU/TCP 主站。")
                color: Theme.textSecondary; font.pixelSize: 11; wrapMode: Text.Wrap
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 72
                color: Theme.dialogFooter
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 18; spacing: 10
                    AppButton {
                        text: I18n.t("Sync to other devices", "同步至其它设备")
                        variant: "outlined"
                        enabled: root.backend && root.backend.devices.definitions().length > 1
                        onClicked: syncAdvanced.openFor(advanced.deviceIndex)
                    }
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: advanced.reject() }
                    AppButton { text: I18n.t("OK", "确定"); variant: "primary"; onClicked: advanced.saveDeviceOptions() }
                }
            }
        }
    }

    Dialog {
        id: syncAdvanced
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(520, parent ? parent.width - 56 : 520)
        height: Math.min(470, parent ? parent.height - 72 : 470)
        padding: 0
        modal: true
        closePolicy: Popup.CloseOnEscape
        property int sourceIndex: -1
        property var selectedTargets: ({})
        function openFor(index) {
            sourceIndex = index
            selectedTargets = ({})
            open()
        }
        function setTarget(index, checked) {
            let next = {}
            for (let item in selectedTargets) next[item] = selectedTargets[item]
            next[index] = checked
            selectedTargets = next
        }
        function applyToTargets() {
            let copied = 0
            const devices = root.backend.devices.definitions()
            for (let i = 0; i < devices.length; ++i) {
                if (i !== sourceIndex && selectedTargets[i] && root.backend.devices.copyAdvancedOptions(sourceIndex, i))
                    ++copied
            }
            if (copied === 0) {
                root.toastRequested(I18n.t("Select at least one target device", "请至少选择一个目标设备"), false)
                return
            }
            root.toastRequested(I18n.t("Parameters synchronized", "高级参数已同步到其它设备"), true)
            close()
        }
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.48 : 0.26) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 62
                color: Theme.dialogHeader; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                ToolButton {
                    id: syncAdvancedClose
                    anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter
                    width: 34; height: 34; focusPolicy: Qt.NoFocus
                    contentItem: Text { text: "×"; color: syncAdvancedClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 21; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 9; color: syncAdvancedClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent"; border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.28) }
                    onClicked: syncAdvanced.reject()
                }
                Text {
                    anchors.left: parent.left; anchors.leftMargin: 20; anchors.right: syncAdvancedClose.left; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
                    text: I18n.t("Sync advanced parameters", "同步设备高级参数"); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 18; font.weight: Font.DemiBold; elide: Text.ElideRight
                }
            }
            Text {
                Layout.fillWidth: true; Layout.margins: 18
                text: I18n.t("Choose the devices that should receive the current device parameters.", "选择需要同步的目标设备；当前设备保持不变。")
                color: Theme.textSecondary; font.family: Theme.fontFamily; font.pixelSize: 13; wrapMode: Text.WordWrap
            }
            ScrollView {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.leftMargin: 18; Layout.rightMargin: 18; clip: true
                ListView {
                    model: root.backend ? root.backend.devices.definitions() : []
                    spacing: 6
                    delegate: Rectangle {
                        required property int index
                        required property var modelData
                        width: ListView.view.width
                        height: index === syncAdvanced.sourceIndex ? 0 : 50
                        visible: index !== syncAdvanced.sourceIndex
                        radius: Theme.radiusSmall; color: targetMouse.containsMouse ? Theme.hoverFill : Theme.surfaceRaised
                        border.width: 1; border.color: Theme.border
                        MouseArea { id: targetMouse; anchors.fill: parent; hoverEnabled: true; onClicked: syncAdvanced.setTarget(index, !Boolean(syncAdvanced.selectedTargets[index])) }
                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12; spacing: 10
                            AppCheckBox { checked: Boolean(syncAdvanced.selectedTargets[index]); onToggled: syncAdvanced.setTarget(index, checked) }
                            Text { Layout.fillWidth: true; text: String(modelData.name); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 14 }
                            Text { text: "Unit ID " + String(modelData.unitId); color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 12 }
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 66
                color: Theme.dialogFooter; radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 18; spacing: 10
                    Item { Layout.fillWidth: true }
                    AppButton { text: I18n.t("Cancel", "取消"); variant: "outlined"; onClicked: syncAdvanced.reject() }
                    AppButton { text: I18n.t("Sync", "同步"); variant: "primary"; onClicked: syncAdvanced.applyToTargets() }
                }
            }
        }
    }

    ModbusDiagnosticsDrawer {
        id: diagnostics
        backend: root.backend
    }

    ModbusTrendWindow {
        id: pointTrends
        backend: root.backend
    }

    readonly property bool isMaster: root.backend.mode === "rtuMaster" || root.backend.mode === "asciiMaster" || root.backend.mode === "tcpMaster"

    readonly property bool isSerial: root.backend.mode === "rtuMaster" || root.backend.mode === "asciiMaster"
                                      || root.backend.mode === "rtuSlave" || root.backend.mode === "asciiSlave"
    readonly property bool connected: root.backend.open || root.backend.simulatorRunning


    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 44; Layout.minimumHeight: 44; Layout.maximumHeight: 44
            spacing: 10

            Text {
                text: "Modbus"
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 23
                font.weight: Font.DemiBold
            }

            Text {
                visible: root.width > 900
                Layout.maximumWidth: Math.max(150, root.width * 0.24)
                text: "/  " + dataMonitor.currentChannelName + "  /  " + dataMonitor.currentDeviceName
                color: Theme.textTertiary; font.pixelSize: 12; elide: Text.ElideRight
            }

            StatusPill {
                text: root.connected
                      ? (root.isMaster ? "主站已连接" : "从站模拟中")
                      : "未连接"
                accent: root.connected ? Theme.success : Theme.textTertiary
            }

            StatusPill {
                visible: root.backend.polling
                text: I18n.t("Polling · ", "轮询中 · ") + I18n.count(root.backend.tasks.length, "tasks", " 任务")
                accent: Theme.warning
                dot: false
                scaleFactor: 0.9
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "TX " + root.backend.txCount + " · RX " + root.backend.rxCount
                      + " · 异常 " + root.backend.errorCount
                      + " · 响应 " + root.backend.lastResponseMs + " ms"
                color: Theme.textSecondary
                font.family: "Consolas"
                font.pixelSize: 11
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: { diagnostics.scanner = false; diagnostics.open() }
                }
            }
        }

        ModbusDataMonitor {
            id: dataMonitor
            Layout.fillWidth: true
            Layout.fillHeight: true
            backend: root.backend
            onNewPointRequested: addPoints.openForCurrentDevice()
            onStatisticsRequested: { diagnostics.scanner = false; diagnostics.open() }
            onDeleteRequested: deletePoints.open()
            onAddDeviceRequested: deviceNameDialog.openFor(-1)
            onRemoveDeviceRequested: removeDevice.open()
            onRemoveDevicesRequested: function(selections) {
                if (!selections || selections.length === 0) return
                root.pendingDeviceRemovals = selections
                removeSelectedDevices.open()
            }
            onEditDeviceRequested: function(index) { deviceNameDialog.openFor(index) }
            onChannelManagerRequested: channelManager.openManager()
            onConfigureChannelRequested: {
                channelManager.openManager()
                Qt.callLater(function() { channelManager.openConfiguration(ModbusWorkspace.currentIndex) })
            }
            onTrendRequested: { pointTrends.show(); pointTrends.raise() }
            onParseFrameRequested: parseFrame.openParser()
            onAdvancedRequested: advanced.openForCurrentDevice()
            onBatchEditRequested: function(field, label, value) { batchPoints.openForSelection(field, label, value) }
            onPasteCellsRequested: function(sourceRow, fields) {
                const ok = root.backend.points.pasteCellsFromClipboard(sourceRow, fields)
                if (ok) root.toastRequested(I18n.t("Cells filled from clipboard", "已从剪贴板填入单元格"), true)
            }
            onCopyPointsRequested: copyConfiguration.openSelector()
            onImportPointsRequested: pointImport.open()
            onExportPointsRequested: templateExport.openForCurrentDevice()
            onPastePointsRequested: {
                const ok = root.backend.points.pasteClipboard()
                root.toastRequested(ok ? I18n.t("Pasted", "已粘贴")
                                       : I18n.t("No valid point rows in clipboard", "剪贴板中没有有效的数据点行"), ok)
            }
            onSortPointsRequested: {
                root.backend.points.sortByAddress()
                root.toastRequested(I18n.t("Sorted by area and address", "已按区块和地址排序"), true)
            }
            onAddChannelRequested: channelNameDialog.openFor(-1)
            onRenameChannelRequested: channelNameDialog.openFor(ModbusWorkspace.currentIndex)
            onRemoveChannelRequested: removeChannel.open()
            onControlRequested: advanced.openForCurrentDevice()
            onEditPointRequested: pointEditor.edit(row)
            onEditCommandRequested: function(row, currentValue) { commandEditor.openFor(row, currentValue) }
        }
    }


}
