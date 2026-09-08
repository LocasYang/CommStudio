import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    signal toastRequested(string text, bool ok)

    readonly property bool isMaster: ModbusManager.mode === "rtuMaster" || ModbusManager.mode === "tcpMaster"

    readonly property bool isRtu: ModbusManager.mode === "rtuMaster" || ModbusManager.mode === "rtuSlave"
    readonly property bool connected: ModbusManager.open || ModbusManager.simulatorRunning

    property int selectedTask: -1
    property int workspaceTab: 0   // 0 轮询任务 / 1 手动读写 / 2 寄存器表 / 3 Raw

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
                visible: ModbusManager.polling
                text: "轮询中 · " + ModbusManager.tasks.length + " 任务"
                accent: Theme.warning
                dot: false
                scaleFactor: 0.9
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "TX " + ModbusManager.txCount + " · RX " + ModbusManager.rxCount
                      + " · 异常 " + ModbusManager.errorCount
                      + " · 响应 " + ModbusManager.lastResponseMs + " ms"
                color: Theme.textSecondary
                font.family: "Consolas"
                font.pixelSize: 11
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            // -------- 左：连接配置 --------
            AppCard {
                Layout.preferredWidth: 312
                Layout.maximumWidth: 312
                Layout.fillHeight: true
                padding: 16

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 9

                    Text {
                        text: "连接配置"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    FormRow {
                        label: "模式"
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["RTU 主站", "TCP 主站", "RTU 从站模拟", "TCP 从站模拟"]
                            currentIndex: ["rtuMaster", "tcpMaster", "rtuSlave", "tcpSlave"]
                                            .indexOf(ModbusManager.mode)
                            onActivated: (index) => ModbusManager.mode =
                                             ["rtuMaster", "tcpMaster", "rtuSlave", "tcpSlave"][index]
                        }
                    }

                    FormRow {
                        visible: root.isRtu
                        label: "端口"
                        AppComboBox {
                            id: modbusPortCombo
                            Layout.fillWidth: true
                            model: ModbusManager.portInfos
                            textRole: "label"
                            currentIndex: modbusPortCombo.indexForName(ModbusManager.portName)
                            onAboutToShowPopup: ModbusManager.refreshPorts()
                            onActivated: (index) => {
                                if (ModbusManager.portInfos.length > index)
                                    ModbusManager.portName = ModbusManager.portInfos[index].name
                            }
                            function indexForName(name) {
                                const infos = ModbusManager.portInfos
                                for (let i = 0; i < infos.length; ++i)
                                    if (infos[i].name === name)
                                        return i
                                return infos.length > 0 ? 0 : -1
                            }
                        }
                    }

                    FormRow {
                        visible: root.isRtu
                        label: "波特率"
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"]
                            currentIndex: Math.max(0, model.indexOf(ModbusManager.baudRate))
                            onActivated: (index) => ModbusManager.baudRate = model[index]
                        }
                    }

                    FormRow {
                        visible: root.isRtu
                        label: "校验位"
                        AppComboBox {
                            Layout.fillWidth: true
                            model: ["无 (None)", "偶 (Even)", "奇 (Odd)"]
                            currentIndex: ["none", "even", "odd"].indexOf(ModbusManager.parity)
                            onActivated: (index) => ModbusManager.parity =
                                             ["none", "even", "odd"][index]
                        }
                    }

                    FormRow {
                        visible: !root.isRtu
                        label: "主机"
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: ModbusManager.host
                            onEditingFinished: ModbusManager.host = text
                        }
                    }

                    FormRow {
                        visible: !root.isRtu
                        label: root.isMaster ? "远端口" : "监听端口"
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: ModbusManager.port
                            onEditingFinished: ModbusManager.port = parseInt(text) || 502
                        }
                    }

                    FormRow {
                        label: root.isMaster ? "Unit ID" : "从站地址"
                        AppTextField {
                            Layout.fillWidth: true
                            mono: true
                            text: root.isMaster ? ModbusManager.unitId : ModbusManager.slaveAddress
                            onEditingFinished: {
                                const value = parseInt(text) || 1
                                if (root.isMaster)
                                    ModbusManager.unitId = Math.min(255, Math.max(0, value))
                                else
                                    ModbusManager.slaveAddress = Math.min(247, Math.max(1, value))
                            }
                        }
                    }

                    FormRow {
                        visible: root.isMaster
                        label: "超时/重试"
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            AppTextField {
                                Layout.fillWidth: true
                                mono: true
                                text: ModbusManager.timeoutMs
                                onEditingFinished: ModbusManager.timeoutMs = parseInt(text) || 1000
                            }

                            AppTextField {
                                Layout.fillWidth: true
                                mono: true
                                text: ModbusManager.retries
                                onEditingFinished: ModbusManager.retries = parseInt(text) || 0
                            }
                        }
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: root.connected ? "断开 / 停止模拟"
                                             : (root.isMaster ? "连接主站" : "启动从站模拟")
                        variant: root.connected ? "danger" : "primary"
                        onClicked: ModbusManager.open()
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    // 从站模拟器配置
                    ColumnLayout {
                        visible: !root.isMaster
                        Layout.fillWidth: true
                        spacing: 7

                        Text {
                            text: "寄存器区数据来源"
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
                                    Layout.preferredWidth: 86
                                    model: ["static", "random", "counter", "ramp", "sine"]
                                    onActivated: applyConfig()
                                    function applyConfig() {
                                        ModbusManager.configureSimulatorArea(
                                            areaKey, areaCount, model[currentIndex],
                                            areaBase, areaAmplitude)
                                    }
                                }

                                AppTextField {
                                    Layout.preferredWidth: 48
                                    mono: true
                                    text: "100"
                                    onEditingFinished: {
                                        sourceCombo.areaCount = parseInt(text) || 0
                                        sourceCombo.applyConfig()
                                    }
                                }
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: "各区的 value = base ± amplitude，按来源每秒刷新；static 区可用下方寄存器表手改。"
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
                            { label: "轮询任务", tab: 0 },
                            { label: "手动读写", tab: 1 },
                            { label: "寄存器表", tab: 2 },
                            { label: "Raw Test Center", tab: 3 }
                        ]

                        delegate: AppButton {
                            required property var modelData
                            compact: true
                            text: modelData.label
                            variant: root.workspaceTab === modelData.tab ? "primary" : "default"
                            onClicked: root.workspaceTab = modelData.tab
                        }
                    }

                    Item { Layout.fillWidth: true }
                }

                AppCard {
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
                                        text: "轮询任务"
                                        color: Theme.textPrimary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 14
                                        font.weight: Font.DemiBold
                                    }

                                    StatusPill {
                                        text: ModbusManager.tasks.length + " 个"
                                        accent: Theme.primary
                                        dot: false
                                        scaleFactor: 0.85
                                    }

                                    Item { Layout.fillWidth: true }

                                    AppButton { compact: true; textPixelSize: 12; text: "新增任务"; onClicked: taskEditor.openFor(-1) }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: "导入 CSV"
                                        onClicked: taskCsvDialog.open()
                                    }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: "启动轮询"
                                        variant: "primary"
                                        enabled: root.isMaster && !ModbusManager.polling
                                                 && ModbusManager.tasks.length > 0
                                        onClicked: ModbusManager.startPolling()
                                    }
                                    AppButton {
                                        compact: true; textPixelSize: 12; text: "停止轮询"
                                        variant: "danger"
                                        enabled: ModbusManager.polling
                                        onClicked: ModbusManager.stopPolling()
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
                                    }
                                }

                                ListView {
                                    id: taskList
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    spacing: 2
                                    model: ModbusManager.tasks

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
                                            Text { Layout.preferredWidth: 120; text: taskRow.modelData.status || "待开始"; color: taskRow.modelData.ok === true ? Theme.success : taskRow.modelData.ok === false ? Theme.error : Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
                                        }
                                    }
                                }

                                Text {
                                    visible: ModbusManager.tasks.length === 0
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignHCenter
                                    text: "暂无轮询任务 · 点击「新增任务」创建，或从 CSV 寄存器表导入"
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
                                    text: "手动读写"
                                    color: Theme.textPrimary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }

                                GridLayout {
                                    Layout.fillWidth: true
                                    columns: 4
                                    columnSpacing: 14
                                    rowSpacing: 8

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: "从站"
                                        AppTextField { id: manualSlave; Layout.fillWidth: true; mono: true; text: "1" }
                                    }

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: "功能码"
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
                                        label: "起始地址"
                                        AppTextField { id: manualAddress; Layout.fillWidth: true; mono: true; text: "0" }
                                    }

                                    FormRow {
                                        Layout.fillWidth: true
                                        label: "数量 / 值"
                                        AppTextField { id: manualValues; Layout.fillWidth: true; mono: true; text: "1" }
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    AppButton {
                                        text: "读取"
                                        variant: "primary"
                                        accent: Theme.cyan
                                        enabled: root.isMaster
                                        onClicked: {
                                            const fc = [1, 2, 3, 4][manualFc.currentIndex]
                                            ModbusManager.manualRead(parseInt(manualSlave.text) || 1, fc,
                                                                     parseInt(manualAddress.text) || 0,
                                                                     parseInt(manualValues.text) || 1)
                                        }
                                    }

                                    AppButton {
                                        text: "写入"
                                        variant: "primary"
                                        enabled: root.isMaster && manualFc.currentIndex >= 4
                                        onClicked: {
                                            const fc = [5, 6, 15, 16][manualFc.currentIndex - 4]
                                            ModbusManager.manualWrite(parseInt(manualSlave.text) || 1, fc,
                                                                      parseInt(manualAddress.text) || 0,
                                                                      manualValues.text)
                                        }
                                    }

                                    Text {
                                        text: "地址按 0 基协议地址发送；写值支持十进制或 0x 十六进制，多个值用逗号分隔"
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
                                                    const result = ModbusManager.previewWrite(
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

                                Item { Layout.fillHeight: true }
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
                                        text: "寄存器表（最近一次读取值）"
                                        color: Theme.textPrimary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 14
                                        font.weight: Font.DemiBold
                                    }

                                    Item { Layout.fillWidth: true }

                                    Text {
                                        text: "显示地址 = 协议地址 + 1（4xxxx 体系）"
                                        color: Theme.textTertiary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 10
                                    }
                                }

                                ListView {
                                    id: registerList
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    spacing: 2
                                    model: ModbusManager.registerMap

                                    ScrollBar.vertical: ScrollBar { }

                                    delegate: Rectangle {
                                        id: regRow
                                        required property var modelData
                                        required property int index
                                        width: registerList.width
                                        implicitHeight: 32
                                        radius: Theme.radiusSmall
                                        color: index % 2 === 0 ? "transparent"
                                                               : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.03 : 0.018)

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.leftMargin: 10
                                            anchors.rightMargin: 10
                                            spacing: 10

                                            Text { Layout.preferredWidth: 96; text: regRow.modelData.task; color: Theme.textPrimary; font.family: Theme.fontFamily; font.pixelSize: 12; elide: Text.ElideRight }
                                            Text { Layout.preferredWidth: 44; text: "S" + regRow.modelData.slave; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 78; text: regRow.modelData.protocolAddress + " / " + regRow.modelData.displayAddress; color: Theme.textSecondary; font.family: "Consolas"; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 64; text: regRow.modelData.type; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 110; text: "0x" + regRow.modelData.rawHex; color: Theme.primary; font.family: "Consolas"; font.pixelSize: 12; font.weight: Font.DemiBold }
                                            Text { Layout.fillWidth: true; text: typeof regRow.modelData.value === "number" ? Number(regRow.modelData.value).toFixed(3) : String(regRow.modelData.value); color: Theme.success; font.family: "Consolas"; font.pixelSize: 13; font.weight: Font.DemiBold }
                                            Text { Layout.preferredWidth: 40; text: regRow.modelData.unit || ""; color: Theme.textTertiary; font.family: Theme.fontFamily; font.pixelSize: 11 }
                                            Text { Layout.preferredWidth: 86; text: regRow.modelData.updatedAt; color: Theme.textTertiary; font.family: "Consolas"; font.pixelSize: 10 }
                                        }
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        visible: ModbusManager.registerMap.length === 0
                                        text: "暂无数据 · 执行读取或轮询后，解码结果会显示在这里"
                                        color: Theme.textTertiary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 12
                                    }
                                }
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
                                    text: root.isRtu
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
                                        onClicked: ModbusManager.sendRawFrame(rawRequest.text,
                                                                              !root.isRtu,
                                                                              rawAutoCrc.checked)
                                    }
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
                                            text: "响应解析"
                                            color: Theme.textPrimary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 12
                                            font.weight: Font.DemiBold
                                        }

                                        Text {
                                            Layout.fillWidth: true
                                            visible: !ModbusManager.rawResult
                                                     || Object.keys(ModbusManager.rawResult).length === 0
                                            text: "尚未发送 Raw 请求"
                                            color: Theme.textTertiary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 11
                                        }

                                        Flow {
                                            Layout.fillWidth: true
                                            spacing: 16

                                            Text {
                                                visible: ModbusManager.rawResult.ok === true
                                                text: "从站 " + ModbusManager.rawResult.slave
                                                      + " · FC 0x" + Number(ModbusManager.rawResult.functionCode || 0).toString(16)
                                                      + " · " + (ModbusManager.rawResult.elapsedMs || 0) + " ms"
                                                color: Theme.textPrimary
                                                font.family: "Consolas"
                                                font.pixelSize: 12
                                            }

                                            Text {
                                                visible: ModbusManager.rawResult.words !== undefined
                                                text: "数据 [" + (ModbusManager.rawResult.words || []).join(" ") + "]"
                                                color: Theme.success
                                                font.family: "Consolas"
                                                font.pixelSize: 12
                                            }

                                            Text {
                                                visible: ModbusManager.rawResult.exception === true
                                                text: ModbusManager.rawResult.exceptionText || "异常响应"
                                                color: Theme.error
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                font.weight: Font.DemiBold
                                            }

                                            Text {
                                                visible: ModbusManager.rawResult.timeout === true
                                                text: "超时 · 2 s 无响应"
                                                color: Theme.error
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                font.weight: Font.DemiBold
                                            }

                                            Text {
                                                visible: ModbusManager.rawResult.crcOk === false
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
                    }
                }

                AppCard {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 220
                    padding: 0

                    TerminalView {
                        anchors.fill: parent
                        model: ModbusManager.traffic
                        emptyHint: "Modbus 事务流量（重建 ADU）会显示在这里"
                        onToastRequested: (text, ok) => root.toastRequested(text, ok)
                    }
                }
            }
        }
    }

    // 任务编辑弹窗（简化版风险确认骨架复用）
    Popup {
        id: taskEditor
        property int editIndex: -1

        parent: Overlay.overlay
        x: (parent ? parent.width - width : 0) / 2
        y: (parent ? parent.height - height : 0) / 2
        width: 640
        height: 460
        padding: 18
        closePolicy: Popup.CloseOnEscape

        background: Rectangle {
            color: Theme.surface
            radius: Theme.radiusLarge
            border.width: 1
            border.color: Theme.alpha(Theme.primary, 0.42)
        }

        function openFor(index) {
            editIndex = index
            if (index >= 0) {
                const t = ModbusManager.taskAt(index)
                nameField.text = t.name || ""
                slaveField.text = t.slave || 1
                fcCombo.currentIndex = [1, 2, 3, 4].indexOf(t.fc) >= 0 ? [1, 2, 3, 4].indexOf(t.fc) : 2
                addrField.text = t.address || 0
                countField.text = t.count || 1
                periodField.text = t.periodMs || 1000
                typeCombo.currentIndex = ["uint16", "int16", "uint32", "int32", "float32", "bool", "hex"]
                                          .indexOf(t.type) >= 0
                                          ? ["uint16", "int16", "uint32", "int32", "float32", "bool", "hex"].indexOf(t.type) : 0
                orderCombo.currentIndex = ["ABCD", "BADC", "CDAB", "DCBA"].indexOf(t.wordOrder) >= 0
                                          ? ["ABCD", "BADC", "CDAB", "DCBA"].indexOf(t.wordOrder) : 0
                scaleField.text = t.scale !== undefined ? t.scale : 1
                unitField.text = t.unit || ""
                enableCheck.checked = t.enabled !== false
            } else {
                nameField.text = ""
                slaveField.text = "1"
                fcCombo.currentIndex = 2
                addrField.text = "0"
                countField.text = "1"
                periodField.text = "1000"
                typeCombo.currentIndex = 0
                orderCombo.currentIndex = 0
                scaleField.text = "1"
                unitField.text = ""
                enableCheck.checked = true
            }
            open()
        }

        contentItem: ColumnLayout {
            spacing: 10

            Text {
                text: taskEditor.editIndex >= 0 ? "编辑轮询任务" : "新增轮询任务"
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.DemiBold
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                columnSpacing: 14
                rowSpacing: 7

                FormRow {
                    Layout.fillWidth: true
                    label: "名称"
                    AppTextField { id: nameField; Layout.fillWidth: true; placeholder: "如 DC Bus 电压" }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "从站"
                    AppTextField { id: slaveField; Layout.fillWidth: true; mono: true; text: "1" }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "功能码"
                    AppComboBox {
                        id: fcCombo
                        Layout.fillWidth: true
                        model: ["01 读线圈", "02 读离散输入", "03 读保持寄存器", "04 读输入寄存器"]
                    }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "起始地址"
                    AppTextField { id: addrField; Layout.fillWidth: true; mono: true; text: "0" }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "数量"
                    AppTextField { id: countField; Layout.fillWidth: true; mono: true; text: "1" }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "周期 ms"
                    AppTextField { id: periodField; Layout.fillWidth: true; mono: true; text: "1000" }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "类型"
                    AppComboBox {
                        id: typeCombo
                        Layout.fillWidth: true
                        model: ["uint16", "int16", "uint32", "int32", "float32", "bool", "hex"]
                    }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "字序"
                    AppComboBox {
                        id: orderCombo
                        Layout.fillWidth: true
                        model: ["ABCD", "BADC", "CDAB", "DCBA"]
                    }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "比例"
                    AppTextField { id: scaleField; Layout.fillWidth: true; mono: true; text: "1" }
                }

                FormRow {
                    Layout.fillWidth: true
                    label: "单位"
                    AppTextField { id: unitField; Layout.fillWidth: true; text: "" ; placeholder: "V / A / ℃" }
                }
            }

            AppCheckBox {
                id: enableCheck
                text: "启用该任务"
                checked: true
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                AppButton {
                    visible: taskEditor.editIndex >= 0
                    text: "删除"
                    variant: "danger"
                    onClicked: {
                        ModbusManager.removeTask(taskEditor.editIndex)
                        taskEditor.close()
                    }
                }

                Item { Layout.fillWidth: true }

                AppButton {
                    text: "取消"
                    variant: "outlined"
                    accent: Theme.textSecondary
                    onClicked: taskEditor.close()
                }

                AppButton {
                    text: "保存"
                    variant: "primary"
                    onClicked: {
                        const task = {
                            name: nameField.text || ("任务 " + (taskEditor.editIndex + 1)),
                            slave: parseInt(slaveField.text) || 1,
                            fc: [1, 2, 3, 4][fcCombo.currentIndex],
                            address: parseInt(addrField.text) || 0,
                            count: parseInt(countField.text) || 1,
                            periodMs: parseInt(periodField.text) || 1000,
                            type: typeCombo.model[typeCombo.currentIndex],
                            wordOrder: orderCombo.model[orderCombo.currentIndex],
                            scale: parseFloat(scaleField.text) || 1,
                            offset: 0,
                            unit: unitField.text,
                            enabled: enableCheck.checked
                        }
                        if (taskEditor.editIndex >= 0)
                            ModbusManager.updateTask(taskEditor.editIndex, task)
                        else
                            ModbusManager.addTask(task)
                        taskEditor.close()
                    }
                }
            }
        }
    }

    FileDialog {
        id: taskCsvDialog
        title: "导入寄存器表 CSV"
        nameFilters: ["CSV 文件 (*.csv)", "所有文件 (*)"]
        onAccepted: ModbusManager.importTasksFromCsv(selectedFile)
    }
}
