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
    function inspectNewPoint() { pointEditor.edit(-1) }
    function inspectNewChannel() { channelNameDialog.openFor(-1) }
    function inspectRemoveChannel() { removeChannel.open() }
    function inspectChannelManager() { channelManager.openManager() }
    function inspectChannelConfiguration() {
        channelManager.openManager()
        Qt.callLater(function() { channelManager.openConfiguration(ModbusWorkspace.currentIndex) })
    }
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
            deviceUnitField.text = index >= 0 ? String(root.backend.devices.data(root.backend.devices.index(index, 0), 258) || "1") : "1"
            open()
            deviceNameField.forceActiveFocus()
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 62
                color: Theme.alpha(Theme.primary, Theme.dark ? 0.18 : 0.10)
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: Theme.radiusLarge; color: parent.color }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 14
                    Text { Layout.fillWidth: true; text: deviceNameDialog.editIndex >= 0 ? I18n.t("Edit device", "编辑设备") : I18n.t("Add device", "添加设备"); color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 18; font.weight: Font.DemiBold }
                     ToolButton {
                         id: deviceNameClose
                         width: 32
                         height: 32
                         focusPolicy: Qt.NoFocus
                         contentItem: Text { text: "×"; color: deviceNameClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                         background: Rectangle { radius: 9; color: deviceNameClose.hovered ? Theme.alpha(Theme.primary, 0.10) : "transparent" }
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
                RowLayout {
                    Layout.fillWidth: true
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
            if (editIndex >= 0 && root.backend.devices.renameDevice)
                root.backend.devices.renameDevice(editIndex, name)
            else
                root.backend.addDevice(name, unit)
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
                color: Theme.alpha(Theme.primary, Theme.dark ? 0.18 : 0.10)
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
                        width: 32; height: 32; focusPolicy: Qt.NoFocus
                        contentItem: Text { text: "×"; color: channelNameClose.hovered ? Theme.textPrimary : Theme.textSecondary; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        background: Rectangle { radius: 9; color: channelNameClose.hovered ? Theme.alpha(Theme.primary, 0.10) : Theme.alpha(Theme.surface, 0.34); border.width: 1; border.color: Theme.alpha(Theme.textSecondary, 0.20) }
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
                color: Theme.alpha(Theme.primary, Theme.dark ? 0.18 : 0.10)
                radius: Theme.radiusLarge
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: Theme.radiusLarge; color: parent.color }
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; height: 1; color: Theme.alpha(Theme.primary, 0.20) }
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
                Qt.callLater(function() { advanced.open() })
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
        id: batchPoints
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(520, parent ? parent.width - 48 : 520)
        height: Math.min(520, parent ? parent.height - 48 : 520)
        padding: 22
        modal: true
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        contentItem: ColumnLayout {
            Text { text: I18n.t("SELECTED POINTS", "已选数据点"); color: Theme.cyan; font.pixelSize: 18 }
            Text { text: I18n.t("Each Apply changes only its row.", "各应用按钮仅修改对应一项。"); color: Theme.textTertiary }
            RowLayout {
                Layout.fillWidth: true
                AppTextField { id: batchPeriod; text: "1000"; placeholder: "Period ms"; Layout.fillWidth: true; validator: IntValidator { bottom: 10; top: 3600000 } }
                AppButton { text: I18n.t("Apply period", "应用周期"); enabled: batchPeriod.acceptableInput; onClicked: root.backend.points.updateSelected({periodMs: Number(batchPeriod.text)}) }
            }
            RowLayout {
                Layout.fillWidth: true
                AppTextField { id: batchGroup; placeholder: I18n.t("Group", "分组"); Layout.fillWidth: true }
                AppButton { text: I18n.t("Apply group", "应用分组"); onClicked: root.backend.points.updateSelected({group: batchGroup.text}) }
            }
            RowLayout {
                AppButton { text: I18n.t("Enable polling", "参与轮询"); onClicked: root.backend.points.updateSelected({poll: true}) }
                AppButton { text: I18n.t("Disable polling", "取消轮询"); onClicked: root.backend.points.updateSelected({poll: false}) }
            }
            Text { text: I18n.t("Copy to device", "复制到设备"); color: Theme.textSecondary }
            AppComboBox { id: pointTarget; Layout.fillWidth: true; model: root.backend.devices; textRole: "label"; valueRole: "deviceId" }
            AppButton { text: I18n.t("Copy selected", "复制选中点"); onClicked: root.backend.points.copySelectedToDevice(pointTarget.currentValue) }
            AppButton { text: I18n.t("Delete selected", "删除选中点"); variant: "danger"; onClicked: deletePoints.open() }
            Item { Layout.fillHeight: true }
            AppButton { text: I18n.t("Close", "关闭"); onClicked: batchPoints.close() }
        }
    }
    ConfirmDialog {
        id: deletePoints
        titleText: I18n.t("Delete selected points", "删除选中数据点")
        messageText: I18n.t("Remove the selected definitions from this device?", "从当前设备删除已选中的数据点定义？")
        noteText: I18n.t("Existing generated tasks must be regenerated.", "现有自动生成的任务需要重新生成。")
        accent: Theme.error
        confirmText: I18n.t("Delete", "删除")
        onOperationConfirmed: root.backend.points.deleteSelected()
    }
    FileDialog {
        id: pointImport
        nameFilters: ["Point template (*.json *.csv)"]
        onAccepted: root.backend.points.importFile(selectedFile)
    }
    FileDialog {
        id: pointExport
        fileMode: FileDialog.SaveFile
        nameFilters: ["JSON (*.json)", "CSV (*.csv)"]
        defaultSuffix: "json"
        onAccepted: root.toastRequested(root.backend.points.exportFile(selectedFile) ? I18n.t("Exported", "已导出") : I18n.t("Export failed", "导出失败"), true)
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
                        const p = {name: pointName.text, address: Number(pointAddress.text), fc: pointArea.currentIndex + 1,
                            type: pointType.currentText, wordOrder: pointOrder.currentText, scale: Number(pointScale.text),
                            offset: Number(pointOffset.text), unit: pointUnit.text, periodMs: Number(pointPeriod.text), poll: pointPoll.checked,
                            count: Number(pointCount.text), displayType: pointDisplay.currentText, decimals: Number(pointDecimals.text),
                            bitOffset: Number(pointBitOffset.text), bitCount: Number(pointBits.text), group: pointGroup.text, notes: pointNotes.text,
                            timeoutMs: Number(pointTimeout.text), retries: Number(pointRetries.text),
                            minValue: pointMin.text.trim() === "" ? undefined : Number(pointMin.text),
                            maxValue: pointMax.text.trim() === "" ? undefined : Number(pointMax.text)}
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
        onOperationConfirmed: { root.backend.points.removePoint(pointEditor.editingRow); pointEditor.close() }
    }
    Dialog {
        id: advanced
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(720, parent ? parent.width - 48 : 720)
        height: Math.min(760, parent ? parent.height - 48 : 760)
        padding: 22
        modal: true
        closePolicy: Popup.CloseOnEscape
        Overlay.modal: Rectangle { color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42) }
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.width: 1; border.color: Theme.alpha(Theme.primary, 0.42) }
        contentItem: ScrollView {
            id: advancedScroll
            contentWidth: availableWidth
            clip: true
        ColumnLayout {
            width: advancedScroll.availableWidth
            spacing: 16
            Text { text: I18n.t("SESSION PARAMETERS", "会话参数"); color: Theme.cyan; font.pixelSize: 18 }
            FormRow {
                Layout.fillWidth: true
                label: root.isSerial ? I18n.t("Channel alias", "通道别名")
                                     : I18n.t("Channel name", "通道名称")
                AppTextField {
                    id: advancedChannelName
                    Layout.fillWidth: true
                    text: ModbusWorkspace.channels[ModbusWorkspace.currentIndex].alias || ""
                    placeholder: root.isSerial
                                 ? I18n.t("Optional, COM port remains the channel identity", "可选，COM 口始终是通道标识")
                                 : I18n.t("e.g. TCP Master", "例如：TCP 主站")
                    onEditingFinished: {
                        if (root.isSerial)
                            ModbusWorkspace.setAlias(ModbusWorkspace.currentIndex, text.trim())
                        else if (text.trim().length > 0)
                            ModbusWorkspace.rename(ModbusWorkspace.currentIndex, text.trim())
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }
                AppButton {
                    visible: !root.isSerial
                    text: I18n.t("Remove channel", "移除通道")
                    enabled: ModbusWorkspace.channels.length > 1
                    variant: "text"
                    accent: Theme.error
                    onClicked: removeChannel.open()
                }
            }
            Text { Layout.fillWidth: true; wrapMode: Text.WordWrap; text: I18n.t("Applied when connecting. Reconnect after changing parameters.", "连接时应用参数，修改后请重新连接。"); color: Theme.textTertiary; font.pixelSize: 12 }
            FormRow {
                label: I18n.t("Timeout ms", "超时 ms")
                    AppTextField {
                    Layout.fillWidth: true
                    text: String(root.backend.timeoutMs)
                    validator: IntValidator { bottom: 50; top: 60000 }
                    onEditingFinished: if (acceptableInput) root.backend.timeoutMs = Number(text)
                }
            }
            FormRow {
                label: I18n.t("Retries", "重试次数")
                AppTextField {
                    Layout.fillWidth: true
                    text: String(root.backend.retries)
                    validator: IntValidator { bottom: 0; top: 10 }
                    onEditingFinished: if (acceptableInput) root.backend.retries = Number(text)
                }
            }
            Repeater {
                model: [
                    {key: "dataBits", label: I18n.t("Data bits", "数据位"), min: 5, max: 8},
                    {key: "stopBits", label: I18n.t("Stop bits", "停止位"), min: 1, max: 2},
                    {key: "flowControl", label: I18n.t("Flow 0/1/2", "流控 0/1/2"), min: 0, max: 2},
                    {key: "interFrameUs", label: I18n.t("Frame gap µs", "帧间隔 µs"), min: -1, max: 1000000},
                    {key: "turnaroundMs", label: I18n.t("Turnaround ms", "响应间隔 ms"), min: 0, max: 60000},
                    {key: "addressOffset", label: I18n.t("Address offset", "地址偏移"), min: -65535, max: 65535},
                    {key: "writeCacheMs", label: I18n.t("Write cache ms", "写缓存 ms"), min: 0, max: 60000},
                    {key: "tcpConcurrency", label: I18n.t("TCP concurrency", "TCP 并发数"), min: 1, max: 16},
                    {key: "broadcastAddress", label: I18n.t("Broadcast address", "广播地址"), min: 0, max: 247},
                    {key: "reconnectMs", label: I18n.t("Reconnect ms", "重连 ms"), min: 100, max: 60000},
                    {key: "minimumPollMs", label: I18n.t("Min poll ms", "最小轮询 ms"), min: 10, max: 60000},
                    {key: "mergeGap", label: I18n.t("Merge gap", "合并空洞"), min: 0, max: 124},
                    {key: "maxRead", label: I18n.t("Max registers", "合并上限"), min: 1, max: 125}
                ]
                delegate: FormRow {
                    required property var modelData
                    Layout.fillWidth: true
                    label: modelData.label
                    AppTextField {
                        Layout.fillWidth: true
                        text: String(root.backend.channelOptions[modelData.key])
                        validator: IntValidator { bottom: modelData.min; top: modelData.max }
                        onEditingFinished: if (acceptableInput) root.backend.setChannelOption(modelData.key, Number(text))
                    }
                }
            }
            FormRow {
                Layout.fillWidth: true
                label: I18n.t("CRC byte order", "CRC 字节序")
                AppComboBox {
                    Layout.fillWidth: true
                    model: [I18n.t("Little endian", "低字节在前"), I18n.t("Big endian", "高字节在前")]
                    currentIndex: root.backend.channelOptions.crcByteOrder === "big" ? 1 : 0
                    onActivated: root.backend.setChannelOption("crcByteOrder", index === 1 ? "big" : "little")
                }
            }
            FormRow {
                Layout.fillWidth: true
                label: I18n.t("Coil bit order", "Coil 位序")
                AppComboBox {
                    Layout.fillWidth: true
                    model: ["LSB first", "MSB first"]
                    currentIndex: root.backend.channelOptions.coilBitOrder === "msb" ? 1 : 0
                    onActivated: root.backend.setChannelOption("coilBitOrder", index === 1 ? "msb" : "lsb")
                }
            }
            AppCheckBox { text: I18n.t("Auto reconnect", "自动重连"); checked: root.backend.channelOptions.autoReconnect; onToggled: root.backend.setChannelOption("autoReconnect", checked) }
            AppCheckBox { text: I18n.t("Poll after connecting", "连接后自动轮询"); checked: root.backend.channelOptions.autoPoll; onToggled: root.backend.setChannelOption("autoPoll", checked) }
            AppCheckBox { text: "TCP Keep Alive"; checked: root.backend.channelOptions.keepAlive; onToggled: root.backend.setChannelOption("keepAlive", checked) }
            Item { Layout.fillHeight: true }
            AppButton { Layout.fillWidth: true; text: I18n.t("Done", "完成"); onClicked: advanced.close() }
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
            spacing: 10

            Text {
                text: "Modbus"
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.weight: Font.DemiBold
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
            onNewPointRequested: pointEditor.edit(-1)
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
            onDiagnosticsRequested: { diagnostics.scanner = false; diagnostics.open() }
            onAdvancedRequested: advanced.open()
            onBatchEditRequested: batchPoints.open()
            onCopyPointsRequested: batchPoints.open()
            onImportPointsRequested: pointImport.open()
            onExportPointsRequested: pointExport.open()
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
            onControlRequested: advanced.open()
            onEditPointRequested: pointEditor.edit(row)
        }
    }


}
