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
    property string manualDecodeResult: ""
    function inspectNewPoint() { pointEditor.edit(-1) }
    function inspectNewTask() { taskEditor.openFor(-1) }
    function inspectNewChannel() { channelNameDialog.openFor(-1) }
    function inspectRemoveChannel() { removeChannel.open() }
    function decodeManualResult() {
        const rows = root.backend.registerMap || []
        if (rows.length < 2) { root.manualDecodeResult = I18n.t("Need at least two registers", "至少需要两个寄存器"); return }
        let a = parseInt(rows[0].rawHex, 16) & 0xffff
        let b = parseInt(rows[1].rawHex, 16) & 0xffff
        const order = manualDecodeOrder.model[manualDecodeOrder.currentIndex]
        if (order === "CDAB" || order === "DCBA") { const t = a; a = b; b = t }
        if (order === "BADC" || order === "DCBA") {
            a = ((a & 0xff) << 8) | ((a >> 8) & 0xff)
            b = ((b & 0xff) << 8) | ((b >> 8) & 0xff)
        }
        const raw = ((a * 65536) + b) >>> 0
        const type = manualDecodeType.model[manualDecodeType.currentIndex]
        if (type === "UINT32") root.manualDecodeResult = String(raw)
        else if (type === "INT32") root.manualDecodeResult = String(raw >= 0x80000000 ? raw - 0x100000000 : raw)
        else {
            const sign = (raw >>> 31) ? -1 : 1
            const exponent = (raw >>> 23) & 0xff
            const mantissa = raw & 0x7fffff
            const value = exponent === 0 ? sign * (mantissa / 0x800000) * Math.pow(2, -126)
                : sign * (1 + mantissa / 0x800000) * Math.pow(2, exponent - 127)
            root.manualDecodeResult = type + " = " + value.toFixed(6)
        }
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
    Dialog {
        id: channelNameDialog
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
            channelNameField.text = index >= 0 && index < ModbusWorkspace.channels.length
                                     ? ModbusWorkspace.channels[index].name : ""
            open()
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
                        text: channelNameDialog.editIndex >= 0 ? I18n.t("Rename channel", "重命名通道") : I18n.t("New channel", "新增通道")
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
                    label: I18n.t("Name", "名称")
                    AppTextField { id: channelNameField; Layout.fillWidth: true; placeholder: I18n.t("e.g. Inverter line", "例如：逆变器总线"); onAccepted: channelNameDialog.accept() }
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
                        enabled: channelNameField.text.trim().length > 0
                        onClicked: channelNameDialog.accept()
                    }
                }
            }
        }

        onAccepted: {
            const name = channelNameField.text.trim()
            if (!name) return
            if (editIndex >= 0)
                ModbusWorkspace.rename(editIndex, name)
            else
                ModbusWorkspace.add(name)
        }
    }
    ConfirmDialog {
        id: taskDelete
        titleText: I18n.t("Delete polling task", "删除轮询任务")
        messageText: I18n.t("Delete the selected polling task?", "确认删除选中的轮询任务？")
        noteText: root.pendingTaskDelete >= 0 && root.pendingTaskDelete < root.backend.tasks.length
                  ? (root.backend.taskAt(root.pendingTaskDelete).name || I18n.t("Unnamed task", "未命名任务"))
                  : ""
        confirmText: I18n.t("Delete", "删除")
        accent: Theme.error
        onOperationConfirmed: {
            if (root.pendingTaskDelete >= 0)
                root.backend.removeTask(root.pendingTaskDelete)
            root.pendingTaskDelete = -1
            root.selectedTask = -1
            taskEditor.close()
        }
    }
    ConfirmDialog {
        id: removeDevice
        titleText: I18n.t("Remove device", "移除设备")
        messageText: I18n.t("Remove this device? Its points and tasks must be removed first.", "移除此设备？请先移除它的数据点和任务。")
        confirmText: I18n.t("Remove", "移除")
        accent: Theme.error
        onOperationConfirmed: root.backend.removeDevice(root.backend.selectedDeviceIndex)
    }
    Drawer {
        id: batchPoints
        parent: Overlay.overlay
        edge: Qt.RightEdge
        width: Math.min(440, parent.width)
        height: parent.height
        padding: 18
        modal: true
        closePolicy: Popup.CloseOnEscape
        background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.color: Theme.border }
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
    Drawer {
        id: pointEditor
        parent: Overlay.overlay
        edge: Qt.RightEdge
        width: Math.min(460, parent.width)
        height: parent.height
        modal: true
        padding: 18
        closePolicy: Popup.CloseOnEscape
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
    function writeManual() {
        root.backend.manualWrite(Number(manualSlave.text), [5, 6, 15, 16][manualFc.currentIndex - 4],
                                  Number(manualAddress.text), manualValues.text)
    }
    ConfirmDialog {
        id: writeConfirm
        titleText: I18n.t("Confirm batch / broadcast write", "确认批量 / 广播写入")
        messageText: I18n.t("Write values to the selected device?", "确认向当前站号写入这些数据？")
        noteText: "ID " + manualSlave.text + " · @" + manualAddress.text + " · " + manualValues.text
        accent: Theme.warning
        confirmText: I18n.t("Write", "写入")
        onOperationConfirmed: root.writeManual()
    }
    Drawer {
        id: advanced
        parent: Overlay.overlay
        edge: Qt.RightEdge
        width: Math.min(420, parent ? parent.width : 420)
        height: parent ? parent.height : 700
        padding: 24
        closePolicy: Popup.CloseOnEscape
        background: Rectangle { color: Theme.surface; border.color: Theme.divider }
        contentItem: ScrollView {
            id: advancedScroll
            contentWidth: availableWidth
            clip: true
        ColumnLayout {
            width: advancedScroll.availableWidth
            spacing: 16
            Text { text: I18n.t("SESSION PARAMETERS", "会话参数"); color: Theme.cyan; font.pixelSize: 18 }
            AppTextField { Layout.fillWidth: true; text: ModbusWorkspace.channels[ModbusWorkspace.currentIndex].name; onEditingFinished: ModbusWorkspace.rename(ModbusWorkspace.currentIndex, text) }
            AppButton { Layout.fillWidth: true; text: I18n.t("Remove channel", "移除通道"); enabled: ModbusWorkspace.channels.length > 1; variant: "text"; onClicked: removeChannel.open() }
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

    readonly property bool isMaster: root.backend.mode === "rtuMaster" || root.backend.mode === "asciiMaster" || root.backend.mode === "tcpMaster"

    readonly property bool isSerial: root.backend.mode === "rtuMaster" || root.backend.mode === "asciiMaster"
                                      || root.backend.mode === "rtuSlave" || root.backend.mode === "asciiSlave"
    readonly property bool isRtu: root.backend.mode === "rtuMaster" || root.backend.mode === "rtuSlave"
    readonly property bool connected: root.backend.open || root.backend.simulatorRunning

    onIsMasterChanged: if (isMaster && workspaceTab === 3) workspaceTab = 1

    property int selectedTask: -1
    property int pendingTaskDelete: -1
    property int workspaceTab: 5   // 0 轮询任务 / 1 手动读写 / 2 寄存器表 / 3 仿真 / 4 Raw / 5 数据监控

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

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            // -------- 左：连接配置 --------
            AppCard {
                outline: Theme.alpha(Theme.textSecondary, 0.16)
                cornerRadius: Theme.radius
                Layout.preferredWidth: 312
                Layout.maximumWidth: 312
                Layout.fillHeight: true
                padding: 16

                ScrollView {
                    id: contextScroll
                    anchors.fill: parent
                    clip: true
                    contentWidth: availableWidth
                ColumnLayout {
                    width: contextScroll.availableWidth
                    spacing: 9

                    Text {
                        text: I18n.t("CHANNEL / DEVICE", "通道 / 设备")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Item {
                            id: channelPicker
                            Layout.fillWidth: true
                            implicitHeight: channelCombo.implicitHeight
                            AppComboBox {
                                id: channelCombo
                                anchors.fill: parent
                                model: ModbusWorkspace.channels
                                textRole: "name"
                                currentIndex: ModbusWorkspace.currentIndex
                                onActivated: function(index) { ModbusWorkspace.select(index) }
                            }
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.RightButton
                                propagateComposedEvents: true
                                onClicked: function(mouse) {
                                    if (mouse.button === Qt.RightButton) {
                                        channelMenu.popup()
                                        mouse.accepted = true
                                    }
                                }
                            }
                        }
                        AppButton {
                            compact: true
                            text: "+"
                            ToolTip.visible: hovered
                            ToolTip.delay: 350
                            ToolTip.text: I18n.t("Add channel", "新增通道")
                            onClicked: channelNameDialog.openFor(-1)
                        }
                        AppButton {
                            compact: true
                            text: "−"
                            textPixelSize: 18
                            variant: "danger"
                            enabled: ModbusWorkspace.channels.length > 1
                            ToolTip.visible: hovered
                            ToolTip.delay: 350
                            ToolTip.text: I18n.t("Delete current channel", "删除当前通道")
                            onClicked: removeChannel.open()
                        }
                        Menu {
                            id: channelMenu
                            MenuItem { text: I18n.t("Add channel", "新增通道"); onTriggered: channelNameDialog.openFor(-1) }
                            MenuItem { text: I18n.t("Rename channel", "重命名通道"); onTriggered: channelNameDialog.openFor(ModbusWorkspace.currentIndex) }
                            MenuSeparator { }
                            MenuItem { text: I18n.t("Edit channel parameters", "编辑通道参数"); onTriggered: advanced.open() }
                            MenuItem {
                                text: I18n.t("Delete channel", "删除通道")
                                enabled: ModbusWorkspace.channels.length > 1
                                onTriggered: removeChannel.open()
                            }
                        }
                    }

                    FormRow {
                        label: I18n.t("Mode", "模式")
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["RTU 主站", "ASCII 主站", "TCP 主站", "RTU 从站模拟", "ASCII 从站模拟", "TCP 从站模拟"]
                            currentIndex: ["rtuMaster", "asciiMaster", "tcpMaster", "rtuSlave", "asciiSlave", "tcpSlave"]
                                            .indexOf(root.backend.mode)
                            onActivated: (index) => root.backend.mode =
                                             ["rtuMaster", "asciiMaster", "tcpMaster", "rtuSlave", "asciiSlave", "tcpSlave"][index]
                        }
                    }

                    FormRow {
                        visible: root.isSerial
                        label: I18n.t("Port", "端口")
                        AppComboBox {
                            id: modbusPortCombo
                            Layout.fillWidth: true
                            model: root.backend.portInfos
                            textRole: "label"
                            currentIndex: modbusPortCombo.indexForName(root.backend.portName)
                            onAboutToShowPopup: root.backend.refreshPorts()
                            onActivated: (index) => {
                                if (root.backend.portInfos.length > index)
                                    root.backend.portName = root.backend.portInfos[index].name
                            }
                            function indexForName(name) {
                                const infos = root.backend.portInfos
                                for (let i = 0; i < infos.length; ++i)
                                    if (infos[i].name === name)
                                        return i
                                return -1
                            }
                        }
                    }

                    FormRow {
                        visible: root.isSerial
                        label: I18n.t("Baud rate", "波特率")
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"]
                            currentIndex: Math.max(0, model.indexOf(root.backend.baudRate))
                            onActivated: (index) => root.backend.baudRate = model[index]
                        }
                    }

                    FormRow {
                        visible: root.isSerial
                        label: I18n.t("Parity", "校验位")
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["无 (None)", "偶 (Even)", "奇 (Odd)"]
                            currentIndex: ["none", "even", "odd"].indexOf(root.backend.parity)
                            onActivated: (index) => root.backend.parity =
                                             ["none", "even", "odd"][index]
                        }
                    }

                    FormRow {
                        visible: !root.isSerial
                        label: I18n.t("Host", "主机")
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: root.backend.host
                            onEditingFinished: root.backend.host = text
                        }
                    }

                    FormRow {
                        visible: !root.isSerial
                        label: root.isMaster ? I18n.t("Remote port", "远端口") : I18n.t("Listen port", "监听端口")
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: root.backend.port
                            onEditingFinished: root.backend.port = parseInt(text) || 502
                        }
                    }

                    FormRow {
                        visible: !root.isMaster
                        label: root.isMaster ? "Unit ID" : I18n.t("Slave address", "从站地址")
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: root.isMaster ? root.backend.unitId : root.backend.slaveAddress
                            onEditingFinished: {
                                const value = parseInt(text) || 1
                                if (root.isMaster)
                                    root.backend.unitId = Math.min(255, Math.max(0, value))
                                else
                                    root.backend.slaveAddress = Math.min(247, Math.max(1, value))
                            }
                        }
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: I18n.t("Session parameters", "会话参数")
                        variant: "text"
                        onClicked: advanced.open()
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: root.backend.connectionState === 1 ? I18n.t("Connecting…", "连接中…")
                              : root.backend.connectionState === 4 ? I18n.t("Reconnect", "重新连接")
                              : root.connected ? I18n.t("Disconnect / stop simulation", "断开 / 停止模拟")
                              : (root.isMaster ? I18n.t("Connect master", "连接主站") : I18n.t("Start simulator", "启动从站模拟"))
                        enabled: root.backend.connectionState !== 1 && root.backend.connectionState !== 3
                        variant: root.connected ? "danger" : "primary"
                        onClicked: root.backend.openConnection()
                    }

                    Rectangle {
                        Layout.fillWidth: true; height: 2
                        color: root.backend.connectionState === 4 ? Theme.error : root.backend.connectionState === 1 ? Theme.warning : root.connected ? Theme.cyan : Theme.divider
                    }

                    ColumnLayout {
                        visible: root.isMaster
                        Layout.fillWidth: true
                        spacing: 8
                        Text { text: I18n.t("DEVICE CONTEXT", "设备上下文"); color: Theme.cyan; font.pixelSize: 12; font.family: "Consolas" }
                        Item {
                            id: devicePicker
                            Layout.fillWidth: true
                            implicitHeight: deviceCombo.implicitHeight
                            AppComboBox {
                                id: deviceCombo
                                anchors.fill: parent
                                model: root.backend.devices
                                textRole: "label"
                                valueRole: "online"
                                currentIndex: root.backend.selectedDeviceIndex
                                onActivated: root.backend.selectDevice(index)
                            }
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.RightButton
                                propagateComposedEvents: true
                                onClicked: function(mouse) {
                                    if (mouse.button === Qt.RightButton) {
                                        deviceMenu.popup()
                                        mouse.accepted = true
                                    }
                                }
                            }
                        }
                        Rectangle {
                            Layout.fillWidth: true; height: 44; radius: Theme.radiusSmall
                            color: Theme.alpha(Theme.cyan, 0.07); border.width: 1; border.color: Theme.alpha(Theme.cyan, 0.25)
                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                                Text { text: "●"; color: deviceCombo.currentValue ? Theme.success : Theme.textTertiary; font.pixelSize: 13 }
                                Text { text: deviceCombo.currentText || I18n.t("Target device", "目标设备"); color: Theme.textSecondary; font.pixelSize: 11; elide: Text.ElideRight }
                                Item { Layout.fillWidth: true }
                                AppTextField {
                                    Layout.preferredWidth: 58
                                    implicitHeight: 30
                                    text: String(root.backend.unitId)
                                    mono: true
                                    horizontalAlignment: TextInput.AlignHCenter
                                    validator: IntValidator { bottom: 1; top: 247 }
                                    onEditingFinished: if (acceptableInput) root.backend.unitId = Number(text)
                                }
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6
                            AppTextField {
                                id: newDeviceNameField
                                Layout.fillWidth: true
                                implicitHeight: 30
                                placeholder: I18n.t("New device name", "新设备名称")
                            }
                            AppTextField {
                                id: newDeviceIdField
                                Layout.preferredWidth: 52
                                implicitHeight: 30
                                mono: true
                                text: "2"
                                horizontalAlignment: TextInput.AlignHCenter
                                validator: IntValidator { bottom: 1; top: 247 }
                            }
                            AppButton {
                                compact: true
                                text: "+"
                                textPixelSize: 18
                                variant: "outlined"
                                accent: Theme.cyan
                                enabled: root.isMaster && newDeviceIdField.acceptableInput
                                onClicked: {
                                    root.backend.addDevice(newDeviceNameField.text, Number(newDeviceIdField.text))
                                    newDeviceNameField.clear()
                                }
                            }
                        }
                        Text {
                            Layout.fillWidth: true; wrapMode: Text.WordWrap
                            text: I18n.t("Unit ID belongs to the selected device context. Add a device here before creating a task.", "Unit ID 属于当前设备上下文，可在这里新增设备后再创建任务。")
                            color: Theme.textTertiary; font.pixelSize: 10
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            AppButton { compact: true; text: I18n.t("Scan", "扫描"); onClicked: { diagnostics.scanner = true; diagnostics.open() } }
                            AppButton { compact: true; text: I18n.t("Statistics", "统计"); onClicked: { diagnostics.scanner = false; diagnostics.open() } }
                            AppButton { compact: true; text: I18n.t("Manage", "管理"); onClicked: deviceMenu.open()
                                Menu {
                                    id: deviceMenu
                                    MenuItem { text: I18n.t("Add device", "添加设备"); onTriggered: newDeviceNameField.forceActiveFocus() }
                                    MenuItem { text: I18n.t("Rename using name field", "用名称输入框重命名"); enabled: newDeviceNameField.text.trim().length > 0; onTriggered: root.backend.devices.renameDevice(root.backend.selectedDeviceIndex, newDeviceNameField.text) }
                                    MenuItem { text: I18n.t("Remove device", "移除设备"); enabled: root.backend.selectedDeviceIndex > 0; onTriggered: removeDevice.open() }
                                }
                            }
                        }
                    }

                    // 从站模拟器配置
                    ColumnLayout {
                        visible: !root.isMaster
                        Layout.fillWidth: true
                        spacing: 7

                        Text {
                            text: I18n.t("Register area data sources", "寄存器区数据来源")
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 13
                            font.weight: Font.DemiBold
                        }

                        Repeater {
                            model: [
                                { key: "holding", label: "保持寄存器" },
                                { key: "input", label: "输入寄存器" },
                                { key: "coils", label: "线圈" },
                                { key: "discrete", label: "离散输入" }
                            ]

                            delegate: RowLayout {
                                required property var modelData
                                Layout.fillWidth: true
                                spacing: 5

                                Text {
                                    Layout.preferredWidth: 62
                                    text: modelData.label
                                    color: Theme.textSecondary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 11
                                }

                                 AppComboBox {
                                     id: sourceCombo
                                     property string areaKey: modelData.key
                                     property int areaCount: 100
                                     property double areaBase: 100
                                     property double areaAmplitude: 10
                                    Layout.preferredWidth: 72
                                     model: ["static", "random", "counter", "ramp", "sine", "parabola", "square", "sequence"]
                                     function restoreConfig() {
                                         const saved = root.backend.simulatorAreas ? root.backend.simulatorAreas[areaKey] : undefined
                                         if (!saved) return
                                         areaCount = Number(saved.count || areaCount)
                                         areaBase = Number(saved.base || 0)
                                         areaAmplitude = Number(saved.amplitude || 0)
                                         const strategy = String(saved.source || "static")
                                         currentIndex = Math.max(0, model.indexOf(strategy))
                                         const row = parent
                                         if (row && row.children) {
                                             for (let i = 0; i < row.children.length; ++i) {
                                                 const field = row.children[i]
                                                 if (field.areaRole === "count") field.text = String(areaCount)
                                                 else if (field.areaRole === "base") field.text = String(areaBase)
                                                 else if (field.areaRole === "amplitude") field.text = String(areaAmplitude)
                                             }
                                         }
                                     }
                                     Component.onCompleted: restoreConfig()
                                     onActivated: applyConfig()
                                    function applyConfig() {
                                        root.backend.configureSimulatorArea(
                                            areaKey, areaCount, model[currentIndex],
                                            areaBase, areaAmplitude)
                                    }
                                }

                                 AppTextField {
                                     property string areaRole: "count"
                                     Layout.preferredWidth: 42
                                    mono: true
                                    text: "100"
                                    onEditingFinished: {
                                        sourceCombo.areaCount = parseInt(text) || 0
                                        sourceCombo.applyConfig()
                                    }
                                }
                                 AppTextField {
                                     property string areaRole: "base"
                                     Layout.preferredWidth: 42
                                    mono: true
                                    text: "100"
                                    placeholder: I18n.t("Base", "基值")
                                    onEditingFinished: {
                                        sourceCombo.areaBase = Number(text) || 0
                                        sourceCombo.applyConfig()
                                    }
                                }
                                 AppTextField {
                                     property string areaRole: "amplitude"
                                     Layout.preferredWidth: 42
                                    mono: true
                                    text: "10"
                                    placeholder: I18n.t("Amp", "幅值")
                                    onEditingFinished: {
                                        sourceCombo.areaAmplitude = Number(text) || 0
                                        sourceCombo.applyConfig()
                                    }
                                }
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: I18n.t("For each area, value = base ± amplitude and is refreshed by source every second; static areas can be edited in the register table below.", "各区的 value = base ± amplitude，按来源每秒刷新；static 区可用下方寄存器表手改。")
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 10
                            wrapMode: Text.WordWrap
                            lineHeight: 1.3
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }
            }

            // -------- 右：工作区 + 终端 --------
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                // 工作区切换
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Repeater {
                        model: [
                            { label: I18n.t("Polling", "轮询任务"), tab: 0 },
                            { label: I18n.t("Manual", "手动读写"), tab: 1 },
                            { label: I18n.t("Data points", "数据点"), tab: 2 },
                            { label: I18n.t("Simulator data", "仿真数据"), tab: 3, simulatorOnly: true },
                            { label: "Raw Test Center", tab: 4 },
                            { label: I18n.t("Data monitor", "数据监控"), tab: 5 }
                        ]

                        delegate: AppButton {
                            required property var modelData
                            compact: true
                            text: modelData.label
                            visible: !modelData.simulatorOnly || !root.isMaster
                            enabled: visible
                            variant: "text"
                            accent: root.workspaceTab === modelData.tab ? Theme.cyan : Theme.textSecondary
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width; height: 2
                                visible: root.workspaceTab === modelData.tab
                                gradient: Gradient {
                                    orientation: Gradient.Horizontal
                                    GradientStop { position: 0; color: Theme.cyan }
                                    GradientStop { position: 1; color: Theme.purple }
                                }
                            }
                            onClicked: root.workspaceTab = modelData.tab
                        }
                    }

                    Item { Layout.fillWidth: true }
                }

                AppCard {
                outline: Theme.alpha(Theme.textSecondary, 0.16)
                cornerRadius: Theme.radius
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    padding: 0

                    StackLayout {
                        anchors.fill: parent
                        currentIndex: root.workspaceTab

                        // ---- 0 轮询任务 ----
                        Item {
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Text {
                                        text: I18n.t("Polling tasks", "轮询任务")
                                        color: Theme.textPrimary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 14
                                        font.weight: Font.DemiBold
                                    }

                                    StatusPill {
                                        text: I18n.count(root.backend.tasks.length, "tasks", " 个")
                                        accent: Theme.primary
                                        dot: false
                                        scaleFactor: 0.85
                                    }

                                    Item { Layout.fillWidth: true }

                                    AppButton { compact: true; textPixelSize: 12; text: I18n.t("New task", "新增任务"); onClicked: taskEditor.openFor(-1) }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: I18n.t("Import CSV", "导入 CSV")
                                        onClicked: taskCsvDialog.open()
                                    }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: I18n.t("Delete selected", "删除选中")
                                        variant: "danger"
                                        enabled: root.selectedTask >= 0
                                        onClicked: {
                                            root.pendingTaskDelete = root.selectedTask
                                            taskDelete.open()
                                        }
                                    }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: I18n.t("Start polling", "启动轮询")
                                        variant: "primary"
                                        enabled: root.isMaster && !root.backend.polling
                                                 && root.backend.tasks.length > 0
                                        onClicked: root.backend.startPolling()
                                    }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: I18n.t("Stop polling", "停止轮询")
                                        variant: "danger"
                                        enabled: root.backend.polling
                                        onClicked: root.backend.stopPolling()
                                    }
                                }

                                // 表头
                                Rectangle {
                                    Layout.fillWidth: true
                                    implicitHeight: taskHeaderRow.implicitHeight + 14
                                    color: Theme.surfaceRaised
                                    radius: Theme.radiusSmall

                                    RowLayout {
                                        id: taskHeaderRow
                                        anchors.fill: parent
                                        anchors.leftMargin: 12
                                        anchors.rightMargin: 12
                                        spacing: 8

                                        Text { Layout.preferredWidth: 30; text: "启"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold }
                                        Text { Layout.preferredWidth: 110; text: "名称"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold }
                                        Text { Layout.preferredWidth: 42; text: "从站"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                        Text { Layout.preferredWidth: 42; text: "FC"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                        Text { Layout.preferredWidth: 58; text: "地址"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                        Text { Layout.preferredWidth: 48; text: "数量"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                        Text { Layout.preferredWidth: 66; text: "周期"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                        Text { Layout.preferredWidth: 92; text: "类型"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold }
                                        Item { Layout.fillWidth: true }
                                        Text { Layout.preferredWidth: 120; text: "状态"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                        Text { Layout.preferredWidth: 56; text: "操作"; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
                                    }
                                }

                                ListView {
                                    id: taskList
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    spacing: 2
                                    model: root.backend.tasks

                                    ScrollBar.vertical: ScrollBar { }

                                    delegate: Rectangle {
                                        id: taskRow
                                        required property var modelData
                                        required property int index
                                        width: taskList.width
                                        implicitHeight: 36
                                        radius: Theme.radiusSmall
                                        color: root.selectedTask === index
                                               ? Theme.selectedFill
                                               : index % 2 === 0 ? "transparent"
                                                 : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.03 : 0.018)

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: root.selectedTask = taskRow.index
                                            onDoubleClicked: taskEditor.openFor(taskRow.index)
                                        }

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.leftMargin: 12
                                            anchors.rightMargin: 12
                                            spacing: 8

                                            Rectangle {
                                                Layout.preferredWidth: 14
                                                Layout.preferredHeight: 14
                                                radius: 4
                                                color: taskRow.modelData.enabled !== false
                                                       ? Theme.success : Theme.alpha(Theme.textPrimary, 0.2)
                                            }

                                            Text { Layout.preferredWidth: 110; text: taskRow.modelData.name || "未命名任务"; color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 12; elide: Text.ElideRight }
                                            Text { Layout.preferredWidth: 42; text: taskRow.modelData.slave; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter }
                                            Text { Layout.preferredWidth: 42; text: "0x" + Number(taskRow.modelData.fc).toString(16).toUpperCase().padStart(2, "0"); color: Theme.primary; font.family: "Consolas"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter }
                                            Text { Layout.preferredWidth: 58; text: taskRow.modelData.address; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter }
                                            Text { Layout.preferredWidth: 48; text: taskRow.modelData.count; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter }
                                            Text { Layout.preferredWidth: 66; text: taskRow.modelData.periodMs + " ms"; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter }
                                            Text { Layout.preferredWidth: 92; text: (taskRow.modelData.type || "uint16") + (taskRow.modelData.unit ? " " + taskRow.modelData.unit : ""); color: Theme.textSecondary; font.family: Theme.fontFamily; font.pixelSize: 11; elide: Text.ElideRight }
                                            Item { Layout.fillWidth: true }
                                            Text {
                                                Layout.preferredWidth: 120
                                                text: {
                                                    const state = taskRow.modelData.status || "待开始"
                                                    const rate = taskRow.modelData.successRate
                                                    const rtt = taskRow.modelData.lastRttMs
                                                    return state + (rate === undefined ? "" : " · " + Number(rate).toFixed(1) + "%")
                                                        + (rtt === undefined || rtt <= 0 ? "" : " · " + rtt + " ms")
                                                }
                                                color: taskRow.modelData.ok === true ? Theme.success : taskRow.modelData.ok === false ? Theme.error : Theme.textTertiary
                                                font.family: "Consolas"; font.pixelSize: 10; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight
                                            }
                                            AppButton {
                                                Layout.preferredWidth: 56
                                                compact: true
                                                textPixelSize: 11
                                                text: I18n.t("Delete", "删除")
                                                variant: "danger"
                                                onClicked: {
                                                    root.selectedTask = taskRow.index
                                                    root.pendingTaskDelete = taskRow.index
                                                    taskDelete.open()
                                                }
                                            }
                                        }
                                    }
                                }

                                Text {
                                    visible: root.backend.tasks.length === 0
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignHCenter
                                    text: I18n.t("No polling tasks · select New task or import a CSV register table", "暂无轮询任务 · 点击「新增任务」创建，或从 CSV 寄存器表导入")
                                    color: Theme.textTertiary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 12
                                    topPadding: 20
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: "单行调度：同一时刻只有一个未完成请求（RTU 严格串行）；双击行可编辑任务。"
                                    color: Theme.textTertiary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 10
                                }
                            }
                        }

                        // ---- 1 手动读写 ----
                        Item {
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 10

                                Text {
                                    text: I18n.t("Manual read / write", "手动读写")
                                    color: Theme.textPrimary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }

                                GridLayout {
                                    Layout.fillWidth: true
                                    columns: width < 900 ? 2 : 4
                                    columnSpacing: 14
                                    rowSpacing: 8

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: I18n.t("Slave", "从站")
                                        AppTextField { id: manualSlave; Layout.fillWidth: true; mono: true; text: String(root.backend.unitId) }
                                    }

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: I18n.t("Function code", "功能码")
                                        AppComboBox {
                                            id: manualFc
                                            Layout.fillWidth: true
                                            model: ["01 读线圈", "02 读离散输入", "03 读保持寄存器", "04 读输入寄存器",
                                                    "05 写单线圈", "06 写单寄存器", "0F 写多线圈", "10 写多寄存器"]
                                            currentIndex: 2
                                        }
                                    }

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: I18n.t("Start address", "起始地址")
                                        AppTextField { id: manualAddress; Layout.fillWidth: true; mono: true; text: "0" }
                                    }

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: I18n.t("Quantity / value", "数量 / 值")
                                        AppTextField { id: manualValues; Layout.fillWidth: true; mono: true; text: "1" }
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    AppButton {
                                        text: I18n.t("Read", "读取")
                                        variant: "primary"
                                        accent: Theme.cyan
                                        enabled: root.isMaster && root.backend.open && manualFc.currentIndex < 4
                                        onClicked: {
                                            const fc = [1, 2, 3, 4][manualFc.currentIndex]
                                            root.backend.manualRead(parseInt(manualSlave.text) || 1, fc,
                                                                     parseInt(manualAddress.text) || 0,
                                                                     parseInt(manualValues.text) || 1)
                                        }
                                    }

                                    AppButton {
                                        text: I18n.t("Write", "写入")
                                        variant: "primary"
                                        enabled: root.isMaster && root.backend.open && manualFc.currentIndex >= 4
                                        onClicked: {
                                            const fc = [5, 6, 15, 16][manualFc.currentIndex - 4]
                                            if (fc === 15 || fc === 16 || Number(manualSlave.text) === 0) {
                                                writeConfirm.open()
                                            } else {
                                                root.writeManual()
                                            }
                                        }
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                        text: I18n.t("0-based address · decimal or 0x HEX values", "地址从 0 开始 · 支持十进制与 0x 十六进制")
                                        color: Theme.textTertiary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }

                                    Item { Layout.fillWidth: true }
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    implicitHeight: previewColumn.implicitHeight + 22
                                    radius: Theme.radius
                                    color: Theme.surfaceRaised
                                    border.width: 1
                                    border.color: Theme.divider

                                    ColumnLayout {
                                        id: previewColumn
                                        anchors.fill: parent
                                        anchors.margins: 11
                                        spacing: 5

                                        Text {
                                            text: "写前预览（工程量 → 原始值 → 寄存器编码）"
                                            color: Theme.textPrimary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 12
                                            font.weight: Font.DemiBold
                                        }

                                        RowLayout {
                                            spacing: 8

                                            AppTextField { id: previewValue; Layout.preferredWidth: 110; mono: true; text: "230.5"; placeholder: "工程量" }
                                            AppTextField { id: previewScale; Layout.preferredWidth: 80; mono: true; text: "0.1"; placeholder: "Scale" }
                                            AppComboBox { id: previewOrder; implicitWidth: 96; model: ["ABCD", "BADC", "CDAB", "DCBA"] }

                                            AppButton {
                                                compact: true
                                                textPixelSize: 12
                                                text: "预览编码"
                                                onClicked: {
                                                    const result = root.backend.previewWrite(
                                                        parseFloat(previewValue.text) || 0,
                                                        parseFloat(previewScale.text) || 1, 0,
                                                        "float32", previewOrder.model[previewOrder.currentIndex])
                                                    previewResult.text = result.error
                                                        ? result.error
                                                        : "原始值 " + result.raw + " · 寄存器 " + result.registers
                                                          + " (" + result.wordOrder + ")"
                                                }
                                            }

                                            Text {
                                                id: previewResult
                                                Layout.fillWidth: true
                                                color: Theme.success
                                                font.family: "Consolas"
                                                font.pixelSize: 12
                                                elide: Text.ElideRight
                                            }
                                        }
                                    }
                                }

                                Text {
                                    text: I18n.t("READ RESULT    ADDRESS / HEX / UINT16 / INT16", "读取结果    地址 / HEX / UINT16 / INT16")
                                    color: Theme.cyan; font.family: "Consolas"; font.pixelSize: 12
                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Text { text: I18n.t("Temporary decode", "临时解码"); color: Theme.textSecondary; font.pixelSize: 11 }
                                    AppComboBox { id: manualDecodeType; implicitWidth: 110; model: ["UINT32", "INT32", "FLOAT32"] }
                                    AppComboBox { id: manualDecodeOrder; implicitWidth: 96; model: ["ABCD", "BADC", "CDAB", "DCBA"] }
                                    AppButton { compact: true; text: I18n.t("Decode first 2", "解码前 2 个"); enabled: root.backend.registerMap.length >= 2; onClicked: root.decodeManualResult() }
                                    Text { Layout.fillWidth: true; text: root.manualDecodeResult; color: Theme.success; font.family: "Consolas"; font.pixelSize: 11; elide: Text.ElideRight }
                                }
                                ListView {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    model: root.backend.registerMap
                                    ScrollBar.vertical: ScrollBar {}
                                    delegate: Rectangle {
                                        required property var modelData
                                        required property int index
                                        width: ListView.view.width; height: 32
                                        color: index % 2 ? Theme.surfaceRaised : Theme.inputBackground
                                        Row {
                                            anchors.verticalCenter: parent.verticalCenter
                                            leftPadding: 10; spacing: 20
                                            Repeater {
                                                model: [modelData.protocolAddress, "0x" + modelData.rawHex,
                                                        parseInt(modelData.rawHex, 16),
                                                        parseInt(modelData.rawHex, 16) > 32767 ? parseInt(modelData.rawHex, 16) - 65536 : parseInt(modelData.rawHex, 16)]
                                                Text { required property var modelData; width: 100; text: modelData; color: Theme.textPrimary; font.family: "Consolas"; font.pixelSize: 12 }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // ---- 2 寄存器表 ----
                        Item {
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: I18n.t("DATA POINTS / LATEST VALUES", "数据点 / 最新值")
                                        color: Theme.textPrimary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 14
                                        font.weight: Font.DemiBold
                                    }

                                    Item { Layout.fillWidth: true }

                                    Text {
                                        text: I18n.t("Protocol / reference address", "协议地址 / 参考地址")
                                        color: Theme.textTertiary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 10
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    AppTextField { id: pointSearch; Layout.fillWidth: true; placeholder: I18n.t("Search name / address", "搜索名称 / 地址"); onTextChanged: root.backend.points.search = text }
                                    AppComboBox {
                                        id: areaFilter; Layout.preferredWidth: 145
                                        model: [I18n.t("All areas", "全部区域"), "01 · Coil", "02 · Discrete", "03 · Holding", "04 · Input"]
                                        onCurrentIndexChanged: root.backend.points.area = currentIndex
                                    }
                                    AppButton { compact: true; text: I18n.t("Add point", "新增点"); onClicked: pointEditor.edit(-1) }
                                    AppButton { compact: true; text: I18n.t("Generate tasks", "生成任务"); onClicked: root.backend.createPointTasks(readStrategy.currentIndex, root.backend.channelOptions.mergeGap, root.backend.channelOptions.maxRead) }
                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    AppButton { compact: true; text: I18n.t("Import", "导入"); onClicked: pointImport.open() }
                                    AppButton { compact: true; text: I18n.t("Export", "导出"); onClicked: pointExport.open() }
                                    AppButton { compact: true; text: I18n.t("More", "更多"); onClicked: pointMenu.open()
                                        Menu {
                                            id: pointMenu
                                            MenuItem { text: I18n.t("Paste CSV / Excel", "粘贴 CSV / Excel"); onTriggered: root.backend.points.pasteClipboard() }
                                            MenuItem { text: I18n.t("Batch edit selected", "批量编辑选中点"); onTriggered: batchPoints.open() }
                                            MenuItem { text: I18n.t("Sort by area / address", "按区域 / 地址排序"); onTriggered: root.backend.points.sortByAddress() }
                                            MenuItem { text: I18n.t("Trends", "趋势"); onTriggered: { pointTrends.show(); pointTrends.raise() } }
                                        }
                                    }
                                    AppComboBox { id: readStrategy; Layout.preferredWidth: 160; model: [I18n.t("Auto optimize", "自动优化"), I18n.t("Independent", "每点独立"), I18n.t("Table order", "表格顺序合并")] }
                                    Text { Layout.fillWidth: true; text: I18n.t("Double-click to edit · Saved per device", "双击编辑 · 按设备保存"); color: Theme.textTertiary; font.pixelSize: 10 }
                                }
                                ListView {
                                    id: registerList
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    spacing: 2
                                    model: root.backend.points.filtered

                                    ScrollBar.vertical: ScrollBar { }

                                    delegate: Rectangle {
                                        id: regRow
                                        required property var point
                                        property var modelData: point
                                        required property int index
                                        width: registerList.width
                                        height: 36
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: function(mouse) { root.backend.points.selectPoint(regRow.point.sourceRow, (mouse.modifiers & Qt.ControlModifier) !== 0, (mouse.modifiers & Qt.ShiftModifier) !== 0) }
                                            onDoubleClicked: pointEditor.edit(regRow.point.sourceRow)
                                        }
                                        radius: Theme.radiusSmall
                                        color: point.selected ? Theme.selectedFill : index % 2 === 0 ? "transparent"
                                                               : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.03 : 0.018)

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.leftMargin: 10
                                            anchors.rightMargin: 10
                                            spacing: 10

                                            Text { Layout.preferredWidth: 86; text: regRow.point.name; color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 12; elide: Text.ElideRight }
                                            Text { Layout.preferredWidth: 38; text: "FC" + regRow.point.fc; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 58; text: regRow.point.address; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 58; text: regRow.modelData.type; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11; elide: Text.ElideRight }
                                            Text { Layout.preferredWidth: 82; text: regRow.point.rawHex || "—"; color: Theme.primary; font.family: "Consolas"; font.pixelSize: 12; elide: Text.ElideRight }
                                            AppTextField { Layout.preferredWidth: 86; mono: true; text: regRow.point.writeValue || ""; placeholder: I18n.t("write", "写入"); onEditingFinished: root.backend.points.setWriteValue(regRow.point.sourceRow, text) }
                                            Text { Layout.fillWidth: true; text: regRow.point.value === undefined ? "—" : String(regRow.point.value); color: regRow.point.status === "OUT OF RANGE" ? Theme.warning : Theme.success; font.family: "Consolas"; font.pixelSize: 13; elide: Text.ElideRight }
                                            Text { Layout.preferredWidth: 34; text: regRow.modelData.unit || ""; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11; elide: Text.ElideRight }
                                            Text { Layout.preferredWidth: 74; text: regRow.point.status || "—"; color: regRow.point.status === "OK" ? Theme.success : regRow.point.status ? Theme.warning : Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10; elide: Text.ElideRight }
                                            AppButton { compact: true; text: I18n.t("Read", "读取"); enabled: root.connected; onClicked: root.backend.readPoint(regRow.point.sourceRow) }
                                            AppButton { compact: true; text: I18n.t("Write", "写入"); enabled: root.connected && (regRow.point.fc === 1 || regRow.point.fc === 3) && String(regRow.point.writeValue || "").trim().length > 0; onClicked: root.backend.writePoint(regRow.point.sourceRow, String(regRow.point.writeValue || "")) }
                                        }
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        visible: registerList.count === 0
                                        text: I18n.t("No points · Add a point for the selected device", "暂无数据点 · 请为当前设备新增数据点")
                                        color: Theme.textTertiary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 12
                                    }
                                }
                            }
                        }

                        // ---- 4 从站仿真数据 ----
                        Item {
                            visible: !root.isMaster
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 8
                                RowLayout {
                                    Layout.fillWidth: true
                                    Text { text: I18n.t("Simulator data", "仿真数据"); color: Theme.textPrimary; font.pixelSize: 14; font.weight: Font.DemiBold }
                                    StatusPill { text: root.backend.simulatorRunning ? I18n.t("RUNNING", "运行中") : I18n.t("STOPPED", "已停止"); accent: root.backend.simulatorRunning ? Theme.success : Theme.textTertiary; dot: true; scaleFactor: 0.8 }
                                    Item { Layout.fillWidth: true }
                                    Text { text: I18n.t("Strategies: static · random · counter · ramp · sine · parabola · square · sequence", "策略：固定值 · 随机 · 计数 · 线性 · 正弦 · 抛物线 · 方波 · 序列"); color: Theme.textTertiary; font.pixelSize: 10 }
                                }
                                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
                                ListView {
                                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true; spacing: 2
                                    model: root.backend.simulatorRegisters
                                    delegate: Rectangle {
                                        required property var modelData
                                        required property int index
                                        width: parent ? parent.width : 0; height: 32; radius: Theme.radiusSmall
                                        color: index % 2 ? Theme.alpha(Theme.textPrimary, 0.025) : "transparent"
                                        RowLayout { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 12
                                            Text { Layout.preferredWidth: 90; text: modelData.area; color: Theme.textSecondary; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 80; text: "0x" + Number(modelData.address).toString(16).toUpperCase().padStart(4, "0"); color: Theme.primary; font.family: "Consolas"; font.pixelSize: 12 }
                                             AppTextField {
                                                 Layout.preferredWidth: 100
                                                 implicitHeight: 28
                                                 mono: true
                                                 text: String(modelData.value)
                                                 validator: IntValidator { bottom: 0; top: 65535 }
                                                 onEditingFinished: if (acceptableInput)
                                                     root.backend.writeSimulatorRegister(modelData.areaKey, modelData.address, Number(text))
                                             }
                                            Item { Layout.fillWidth: true }
                                        }
                                    }
                                }
                                Text { visible: root.backend.simulatorRegisters.length === 0; Layout.fillWidth: true; text: I18n.t("Start a slave simulator to inspect live values", "启动从站模拟器后可查看实时值"); color: Theme.textTertiary; horizontalAlignment: Text.AlignHCenter; font.pixelSize: 12 }
                            }
                        }

                        // ---- 3 Raw Test Center ----
                        Item {
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 10

                                Text {
                                    text: "Raw Test Center"
                                    color: Theme.textPrimary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: root.isSerial
                                          ? "输入「从站地址 + PDU」，勾选自动 CRC 后发出，例如 01 03 00 00 00 02。"
                                          : "输入「Unit ID + PDU」自动补 MBAP 头发出，例如 01 03 00 00 00 02。"
                                    color: Theme.textSecondary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 11
                                    wrapMode: Text.WordWrap
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    AppTextField {
                                        id: rawRequest
                                        Layout.fillWidth: true
                                        mono: true
                                        text: root.isRtu ? "01 03 00 00 00 02" : "01 03 00 00 00 02"
                                        placeholder: "从站地址 + PDU"
                                    }

                                    AppCheckBox {
                                        id: rawAutoCrc
                                        visible: root.isRtu
                                        text: "自动 CRC16"
                                        checked: true
                                    }

                                    AppButton {
                                        text: "发送原始帧"
                                        variant: "primary"
                                        enabled: !root.backend.rawBusy && !root.backend.rawLoopActive
                                        onClicked: root.backend.sendRawFrame(rawRequest.text,
                                                                              !root.isSerial,
                                                                              rawAutoCrc.checked)
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    AppTextField { id: rawLoopCount; Layout.preferredWidth: 75; text: "10"; placeholder: I18n.t("Count", "次数"); validator: IntValidator { bottom: 1; top: 1000000 } }
                                    AppTextField { id: rawLoopInterval; Layout.preferredWidth: 85; text: "1000"; placeholder: "Interval ms"; validator: IntValidator { bottom: 10; top: 3600000 } }
                                    AppTextField { id: rawJitter; Layout.preferredWidth: 75; text: "0"; placeholder: "Jitter ms"; validator: IntValidator { bottom: 0; top: 60000 } }
                                    AppButton { compact: true; text: I18n.t("Loop", "循环"); enabled: !root.backend.rawLoopActive && rawLoopCount.acceptableInput && rawLoopInterval.acceptableInput && rawJitter.acceptableInput; onClicked: root.backend.startRawLoop(rawRequest.text, !root.isSerial, rawAutoCrc.checked, Number(rawLoopCount.text), Number(rawLoopInterval.text), Number(rawJitter.text)) }
                                    AppButton { compact: true; text: I18n.t("Stop", "停止"); enabled: root.backend.rawLoopActive; onClicked: root.backend.stopRawLoop() }
                                    Text { text: I18n.t("Sent: ", "已发送：") + root.backend.rawLoopSent; color: Theme.textSecondary }
                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    AppComboBox {
                                        id: rawTemplate
                                        Layout.fillWidth: true
                                        model: root.backend.rawTemplates
                                        textRole: "name"
                                        onActivated: {
                                            const template = root.backend.rawTemplates[index]
                                            rawRequest.text = template.request; rawAutoCrc.checked = template.crc
                                            if (template.tcp === !root.isSerial) root.toastRequested(I18n.t("Template transport differs from current mode", "模板通信类型与当前模式不同"), false)
                                        }
                                    }
                                    AppTextField { id: rawTemplateName; Layout.preferredWidth: 135; placeholder: I18n.t("Template name", "模板名称") }
                                    AppButton { compact: true; text: I18n.t("Save", "保存"); onClicked: root.backend.saveRawTemplate(rawTemplateName.text, rawRequest.text, !root.isSerial, rawAutoCrc.checked) }
                                    AppButton { compact: true; text: I18n.t("Remove", "移除"); enabled: rawTemplate.currentIndex >= 0; onClicked: root.backend.removeRawTemplate(rawTemplate.currentIndex) }
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    implicitHeight: rawResultColumn.implicitHeight + 22
                                    radius: Theme.radius
                                    color: Theme.surfaceRaised
                                    border.width: 1
                                    border.color: Theme.divider

                                    ColumnLayout {
                                        id: rawResultColumn
                                        anchors.fill: parent
                                        anchors.margins: 11
                                        spacing: 6

                                        Text {
                                            text: I18n.t("Response parser", "响应解析")
                                            color: Theme.textPrimary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 12
                                            font.weight: Font.DemiBold
                                        }

                                        Text {
                                            Layout.fillWidth: true
                                            visible: !root.backend.rawResult
                                                     || Object.keys(root.backend.rawResult).length === 0
                                            text: I18n.t("No raw request sent", "尚未发送 Raw 请求")
                                            color: Theme.textTertiary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 11
                                        }

                                        Flow {
                                            Layout.fillWidth: true
                                            spacing: 16

                                            Text {
                                                visible: root.backend.rawResult.ok === true
                                                text: "从站 " + root.backend.rawResult.slave
                                                      + " · FC 0x" + Number(root.backend.rawResult.functionCode || 0).toString(16)
                                                      + " · " + (root.backend.rawResult.elapsedMs || 0) + " ms"
                                                color: Theme.textPrimary
                                                font.family: "Consolas"
                                                font.pixelSize: 12
                                            }

                                            Text {
                                                visible: root.backend.rawResult.words !== undefined
                                                text: "数据 [" + (root.backend.rawResult.words || []).join(" ") + "]"
                                                color: Theme.success
                                                font.family: "Consolas"
                                                font.pixelSize: 12
                                            }

                                            Text {
                                                visible: root.backend.rawResult.exception === true
                                                text: root.backend.rawResult.exceptionText || "异常响应"
                                                color: Theme.error
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                font.weight: Font.DemiBold
                                            }

                                            Text {
                                                visible: root.backend.rawResult.timeout === true
                                                text: "超时 · 2 s 无响应"
                                                color: Theme.error
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                font.weight: Font.DemiBold
                                            }

                                            Text {
                                                visible: root.backend.rawResult.crcOk === false
                                                text: "CRC 校验失败"
                                                color: Theme.error
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                font.weight: Font.DemiBold
                                            }
                                        }
                                    }
                                }

                                Item { Layout.fillHeight: true }
                            }
                        }
                        // 参考 MThings 的“数据 / 设备树 / 高密度点表”工作台，保留在 CommStudio 主工作区内。
                        Item {
                            visible: root.workspaceTab === 5
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            ModbusDataMonitor {
                                anchors.fill: parent
                                backend: root.backend
                                onNewPointRequested: pointEditor.edit(-1)
                                onDeleteRequested: deletePoints.open()
                                onAddDeviceRequested: newDeviceNameField.forceActiveFocus()
                                onTrendRequested: { pointTrends.show(); pointTrends.raise() }
                                onDiagnosticsRequested: { diagnostics.scanner = false; diagnostics.open() }
                                onAdvancedRequested: advanced.open()
                                onControlRequested: root.workspaceTab = 1
                                onEditPointRequested: pointEditor.edit(row)
                            }
                        }

                    }

                }

                AppCard {
                outline: Theme.alpha(Theme.textSecondary, 0.16)
                cornerRadius: Theme.radius
                    Layout.fillWidth: true
                    Layout.preferredHeight: 220
                    padding: 0

                    TerminalView {
                        anchors.fill: parent
                        model: root.backend.traffic
                        emptyHint: I18n.t("Modbus transaction traffic (reconstructed ADU) appears here", "Modbus 事务流量（重建 ADU）会显示在这里")
                        onToastRequested: (text, ok) => root.toastRequested(text, ok)
                    }
                }
            }
        }
    }

    // 任务编辑弹窗：居中模态表单，避免右侧抽屉造成标签与控件挤压错位。
    Dialog {
        id: taskEditor
        property int editIndex: -1

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(680, parent ? parent.width - 48 : 680)
        height: Math.min(660, parent ? parent.height - 40 : 660)
        modal: true
        padding: 0
        closePolicy: Popup.CloseOnEscape

        Overlay.modal: Rectangle {
            color: Theme.alpha("#050716", Theme.dark ? 0.72 : 0.42)
        }

        background: Rectangle {
            color: Theme.surface
            radius: Theme.radiusLarge
            border.width: 1
            border.color: Theme.alpha(Theme.primary, 0.42)
        }

        function openFor(index) {
            editIndex = index
            if (index >= 0) {
                const t = root.backend.taskAt(index)
                nameField.text = t.name || ""
                slaveField.text = t.slave || 1
                fcCombo.currentIndex = [1, 2, 3, 4].indexOf(t.fc) >= 0 ? [1, 2, 3, 4].indexOf(t.fc) : 2
                addrField.text = t.address || 0
                countField.text = t.count || 1
                periodField.text = t.periodMs || 1000
                priorityCombo.currentIndex = t.priority === 2 ? 0 : t.priority === 4 ? 2 : 1
                timeoutField.text = t.timeoutMs === undefined ? "-1" : String(t.timeoutMs)
                retryField.text = t.retries === undefined ? "-1" : String(t.retries)
                typeCombo.currentIndex = ["uint16", "int16", "uint32", "int32", "float32", "bool", "hex"]
                                          .indexOf(t.type) >= 0
                                          ? ["uint16", "int16", "uint32", "int32", "float32", "bool", "hex"].indexOf(t.type) : 0
                orderCombo.currentIndex = ["ABCD", "BADC", "CDAB", "DCBA"].indexOf(t.wordOrder) >= 0
                                          ? ["ABCD", "BADC", "CDAB", "DCBA"].indexOf(t.wordOrder) : 0
                scaleField.text = t.scale !== undefined ? t.scale : 1
                offsetField.text = t.offset !== undefined ? t.offset : 0
                unitField.text = t.unit || ""
                enableCheck.checked = t.enabled !== false
            } else {
                nameField.text = ""
                slaveField.text = String(root.backend.unitId)
                fcCombo.currentIndex = 2
                addrField.text = "0"
                countField.text = "1"
                periodField.text = "1000"
                priorityCombo.currentIndex = 1
                timeoutField.text = "-1"
                retryField.text = "-1"
                typeCombo.currentIndex = 0
                orderCombo.currentIndex = 0
                scaleField.text = "1"
                offsetField.text = "0"
                unitField.text = ""
                enableCheck.checked = true
            }
            open()
        }

        contentItem: ColumnLayout {
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                color: Theme.alpha(Theme.primary, Theme.dark ? 0.18 : 0.10)
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
                    anchors.rightMargin: 14
                    spacing: 10

                    Text {
                        Layout.fillWidth: true
                        text: taskEditor.editIndex >= 0 ? I18n.t("Edit polling task", "编辑轮询任务") : I18n.t("New polling task", "新增轮询任务")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        elide: Text.ElideRight
                    }

                    ToolButton {
                        id: taskEditorClose
                        focusPolicy: Qt.NoFocus
                        width: 32
                        height: 32
                        ToolTip.visible: hovered
                        ToolTip.delay: 400
                        ToolTip.text: I18n.t("Close", "关闭")
                        contentItem: Text {
                            text: "×"
                            color: taskEditorClose.hovered || taskEditorClose.pressed ? Theme.textPrimary : Theme.textSecondary
                            font.pixelSize: 20
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 9
                            color: taskEditorClose.pressed ? Theme.alpha(Theme.primary, 0.18)
                                  : taskEditorClose.hovered ? Theme.alpha(Theme.primary, 0.10)
                                  : Theme.alpha(Theme.surface, 0.34)
                            border.width: 1
                            border.color: taskEditorClose.hovered || taskEditorClose.pressed
                                          ? Theme.alpha(Theme.primary, 0.40)
                                          : Theme.alpha(Theme.textSecondary, 0.20)
                        }
                        onClicked: taskEditor.reject()
                    }
                }
            }

            ScrollView {
                id: taskFormScroll
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    x: 22
                    y: 18
                    width: Math.max(0, taskFormScroll.availableWidth - 44)
                    spacing: 14

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 18
                        rowSpacing: 10

                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Name", "名称")
                            AppTextField { id: nameField; Layout.fillWidth: true; placeholder: "如 DC Bus 电压" }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Slave", "从站")
                            AppTextField { id: slaveField; Layout.fillWidth: true; mono: true; text: "1"; validator: IntValidator { bottom: 1; top: 247 } }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Function code", "功能码")
                            AppComboBox { id: fcCombo; Layout.fillWidth: true; model: ["01 读线圈", "02 读离散输入", "03 读保持寄存器", "04 读输入寄存器"] }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Start address", "起始地址")
                            AppTextField { id: addrField; Layout.fillWidth: true; mono: true; text: "0"; validator: IntValidator { bottom: 0; top: 65535 } }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Quantity", "数量")
                            AppTextField { id: countField; Layout.fillWidth: true; mono: true; text: "1"; validator: IntValidator { bottom: 1; top: 125 } }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Period ms", "周期 ms")
                            AppTextField { id: periodField; Layout.fillWidth: true; mono: true; text: "1000"; validator: IntValidator { bottom: 10; top: 3600000 } }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Priority", "优先级")
                            AppComboBox { id: priorityCombo; Layout.fillWidth: true; model: [I18n.t("High", "高"), I18n.t("Normal", "普通"), I18n.t("Low", "低")]; currentIndex: 1 }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Timeout ms", "超时 ms")
                            AppTextField { id: timeoutField; Layout.fillWidth: true; mono: true; text: "-1"; validator: IntValidator { bottom: -1; top: 60000 } }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Retries", "重试次数")
                            AppTextField { id: retryField; Layout.fillWidth: true; mono: true; text: "-1"; validator: IntValidator { bottom: -1; top: 10 } }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Type", "类型")
                            AppComboBox { id: typeCombo; Layout.fillWidth: true; model: ["uint16", "int16", "uint32", "int32", "float32", "bool", "hex"] }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Byte order", "字序")
                            AppComboBox { id: orderCombo; Layout.fillWidth: true; model: ["ABCD", "BADC", "CDAB", "DCBA"] }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Scale", "比例")
                            AppTextField { id: scaleField; Layout.fillWidth: true; mono: true; text: "1"; validator: DoubleValidator {} }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Offset", "偏移")
                            AppTextField { id: offsetField; Layout.fillWidth: true; mono: true; text: "0"; validator: DoubleValidator {} }
                        }
                        FormRow {
                            Layout.columnSpan: 1; Layout.fillWidth: true; labelWidth: 78
                            label: I18n.t("Unit", "单位")
                            AppTextField { id: unitField; Layout.fillWidth: true; placeholder: "V / A / ℃" }
                        }
                        AppCheckBox {
                            id: enableCheck
                            Layout.columnSpan: 2
                            text: I18n.t("Enable this task", "启用该任务")
                            checked: true
                        }
                    }
                    Item { Layout.preferredHeight: 4 }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                color: Theme.alpha(Theme.primary, Theme.dark ? 0.18 : 0.10)
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
                    color: Theme.alpha(Theme.primary, 0.20)
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 24
                    anchors.rightMargin: 24
                    anchors.topMargin: 8
                    spacing: 12

                    AppButton {
                        visible: taskEditor.editIndex >= 0
                        compact: true
                        text: I18n.t("Delete", "删除")
                        variant: "danger"
                        onClicked: {
                            root.pendingTaskDelete = taskEditor.editIndex
                            taskDelete.open()
                        }
                    }
                    Item { Layout.fillWidth: true }
                    AppButton {
                        compact: true
                        text: I18n.t("Cancel", "取消")
                        variant: "outlined"
                        accent: Theme.textSecondary
                        onClicked: taskEditor.reject()
                    }
                    AppButton {
                        compact: true
                        text: I18n.t("Save", "保存")
                        variant: "primary"
                        onClicked: {
                            const task = {
                                name: nameField.text || ("任务 " + (taskEditor.editIndex + 1)),
                                slave: parseInt(slaveField.text) || 1,
                                fc: [1, 2, 3, 4][fcCombo.currentIndex],
                                address: parseInt(addrField.text) || 0,
                                count: parseInt(countField.text) || 1,
                                periodMs: parseInt(periodField.text) || 1000,
                                priority: [2, 3, 4][priorityCombo.currentIndex],
                                timeoutMs: parseInt(timeoutField.text),
                                retries: parseInt(retryField.text),
                                type: typeCombo.model[typeCombo.currentIndex],
                                wordOrder: orderCombo.model[orderCombo.currentIndex],
                                scale: Number(scaleField.text),
                                offset: Number(offsetField.text),
                                unit: unitField.text,
                                enabled: enableCheck.checked
                            }
                            if (taskEditor.editIndex >= 0)
                                root.backend.updateTask(taskEditor.editIndex, task)
                            else
                                root.backend.addTask(task)
                            taskEditor.accept()
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: taskCsvDialog
        title: I18n.t("Import register table CSV", "导入寄存器表 CSV")
        nameFilters: ["CSV 文件 (*.csv)", "所有文件 (*)"]
        onAccepted: root.backend.importTasksFromCsv(selectedFile)
    }


}
