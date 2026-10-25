import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    signal toastRequested(string text, bool ok)




    property int scanRemainingSec: 0

    Timer {
        id: scanCountdown
        interval: 1000
        repeat: true
        running: BluetoothManager.scanning
        onRunningChanged: if (running) root.scanRemainingSec = 12
        onTriggered: root.scanRemainingSec = Math.max(0, root.scanRemainingSec - 1)
    }


    readonly property var currentService:
        BluetoothManager.services.length > 0
            ? BluetoothManager.services[Math.min(BluetoothManager.selectedService,
                                                 BluetoothManager.services.length - 1)]
            : null

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: I18n.t("Bluetooth", "蓝牙")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.weight: Font.DemiBold
            }

            StatusPill {
                visible: !BluetoothManager.backendSupported
                text: I18n.t("Bluetooth backend unavailable", "蓝牙后端不可用")
                accent: Theme.error
                dot: false
                scaleFactor: 0.9
            }

            StatusPill {
                text: BluetoothManager.leState
                accent: ["已连接", "服务已发现"].indexOf(BluetoothManager.leState) >= 0
                        ? Theme.success
                        : BluetoothManager.leState === "未连接" ? Theme.textTertiary : Theme.warning
            }

            StatusPill {
                visible: BluetoothManager.rfcommConnected
                text: I18n.t("RFCOMM connected", "RFCOMM 已连接")
                accent: Theme.cyan
                dot: false
                scaleFactor: 0.9
            }

            StatusPill {
                visible: BluetoothManager.notifyActive
                text: I18n.t("Notify subscribed", "Notify 订阅中")
                accent: Theme.purple
                dot: false
                scaleFactor: 0.9
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "MTU " + BluetoothManager.mtu
                      + " · RX " + BluetoothManager.rxBytes + " B · TX " + BluetoothManager.txBytes + " B"
                color: Theme.textSecondary
                font.family: "Consolas"
                font.pixelSize: 11
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            // -------- 左：适配器 / 扫描 / 设备列表 --------
            AppCard {
                Layout.preferredWidth: 336
                Layout.maximumWidth: 336
                Layout.fillHeight: true
                padding: 16

                ScrollView {
                    id: leftScroll
                    anchors.fill: parent
                    clip: true
                    contentWidth: availableWidth
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    ColumnLayout {
                        width: leftScroll.availableWidth
                        spacing: 9

                    Text {
                        text: I18n.t("Local adapters", "本地适配器")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    Repeater {
                        model: BluetoothManager.adapters

                        delegate: RowLayout {
                            required property var modelData
                            Layout.fillWidth: true
                            spacing: 8

                            Text {
                                text: modelData.name
                                color: Theme.textPrimary
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                                font.weight: Font.Medium
                                elide: Text.ElideRight
                            }

                            Item { Layout.fillWidth: true }

                            StatusPill {
                                text: modelData.valid ? modelData.mode : "不可用"
                                accent: modelData.valid && modelData.mode !== "已关闭"
                                        ? Theme.success : Theme.error
                                dot: false
                                scaleFactor: 0.82
                            }
                        }
                    }

                    Text {
                        visible: BluetoothManager.adapters.length === 0
                        text: I18n.t("No local Bluetooth adapter detected (or system Bluetooth is off)", "未检测到本地蓝牙适配器（或系统蓝牙未开启）")
                        color: Theme.textTertiary
                        font.family: Theme.fontFamily
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: I18n.t("Device scan", "设备扫描")
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                        }

                        StatusPill {
                            visible: BluetoothManager.scanning
                            text: I18n.t("Scanning · remaining ", "扫描中 · 剩余 ") + root.scanRemainingSec + " s"
                            accent: Theme.warning
                            dot: true
                            scaleFactor: 0.85
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            visible: BluetoothManager.devices.length > 0
                            text: I18n.t("Discovered ", "已发现 ") + BluetoothManager.devices.length
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        AppTextField {
                            id: nameFilterField
                            Layout.fillWidth: true
                            placeholder: "按名称/地址过滤"
                            onEditingFinished: BluetoothManager.nameFilter = text
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 12
                            text: BluetoothManager.scanning ? I18n.t("Stop", "停止") : I18n.t("Scan", "扫描")
                            variant: BluetoothManager.scanning ? "danger" : "primary"
                            enabled: BluetoothManager.backendSupported
                            onClicked: BluetoothManager.scanning
                                       ? BluetoothManager.stopScan() : BluetoothManager.startScan()
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppCheckBox {
                            text: I18n.t("Low Energy only (BLE)", "仅低功耗 (BLE)")
                            checked: BluetoothManager.leOnly
                            onToggled: BluetoothManager.leOnly = checked
                        }

                        Item { Layout.fillWidth: true }

                        AppButton {
                            compact: true
                            textPixelSize: 12
                            text: I18n.t("Clear", "清空")
                            variant: "text"
                            onClicked: BluetoothManager.clearDevices()
                        }
                    }

                    ListView {
                        id: deviceList
                        Layout.fillWidth: true
                        Layout.preferredHeight: 210
                        clip: true
                        spacing: 3
                        model: BluetoothManager.devices

                        ScrollBar.vertical: ScrollBar { }

                        property int selectedIndex: -1

                        delegate: Rectangle {
                            id: deviceRow
                            required property var modelData
                            required property int index
                            width: deviceList.width
                            implicitHeight: 48
                            radius: Theme.radiusSmall
                            color: deviceList.selectedIndex === index
                                   ? Theme.selectedFill
                                   : index % 2 === 0 ? "transparent"
                                                     : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.03 : 0.018)

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: deviceList.selectedIndex = deviceRow.index
                                onDoubleClicked: BluetoothManager.connectDevice(deviceRow.index)
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                spacing: 8

                                Rectangle {
                                    Layout.preferredWidth: 30
                                    Layout.preferredHeight: 30
                                    radius: 8
                                    color: Theme.alpha(Theme.primary, Theme.dark ? 0.2 : 0.1)
                                    border.width: 1
                                    border.color: Theme.alpha(Theme.primary, 0.28)

                                    Text {
                                        anchors.centerIn: parent
                                        text: deviceRow.modelData.type === "BLE" ? "◈" : "◉"
                                        color: Theme.primary
                                        font.pixelSize: 14
                                    }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 2

                                    Text {
                                        Layout.fillWidth: true
                                        text: deviceRow.modelData.name
                                        color: Theme.textPrimary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 12
                                        font.weight: Font.Medium
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: deviceRow.modelData.key
                                              + (deviceRow.modelData.services ? " · " + deviceRow.modelData.services : "")
                                        color: Theme.textTertiary
                                        font.family: "Consolas"
                                        font.pixelSize: 9
                                        elide: Text.ElideMiddle
                                    }
                                }

                                ColumnLayout {
                                    spacing: 2

                                    StatusPill {
                                        text: deviceRow.modelData.type
                                        accent: Theme.cyan
                                        dot: false
                                        scaleFactor: 0.78
                                    }

                                    Text {
                                        text: deviceRow.modelData.hasRssi
                                              ? deviceRow.modelData.rssi + " dBm" : "—"
                                        color: deviceRow.modelData.hasRssi && deviceRow.modelData.rssi > -70
                                               ? Theme.success : Theme.textTertiary
                                        font.family: "Consolas"
                                        font.pixelSize: 10
                                    }
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            visible: BluetoothManager.devices.length === 0
                            text: BluetoothManager.scanning ? I18n.t("Scanning nearby devices…", "正在扫描附近设备…")
                                                            : "暂无设备 · 点击「扫描」开始"
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: I18n.t("Connect selected device", "连接选中设备")
                        variant: "primary"
                        enabled: deviceList.selectedIndex >= 0
                                 && deviceList.selectedIndex < BluetoothManager.devices.length
                                 && BluetoothManager.leState === "未连接"
                        onClicked: BluetoothManager.connectDevice(deviceList.selectedIndex)
                    }

                    AppButton {
                        Layout.fillWidth: true
                        text: I18n.t("Disconnect BLE", "断开 BLE")
                        variant: "danger"
                        enabled: BluetoothManager.leState !== "未连接"
                        onClicked: BluetoothManager.disconnectDevice()
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    Text {
                        text: I18n.t("BLE UART mode", "BLE UART 模式")
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        AppButton {
                            compact: true
                            textPixelSize: 11
                            text: "Nordic UART 预置"
                            variant: "outlined"
                            accent: Theme.purple
                            onClicked: {
                                uartServiceField.text = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
                                uartTxField.text = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
                                uartRxField.text = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
                            }
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 11
                            text: I18n.t("Enable UART", "启用 UART")
                            variant: "primary"
                            accent: Theme.purple
                            enabled: BluetoothManager.leState === "服务已发现"
                            onClicked: BluetoothManager.enableUartMode(
                                           uartServiceField.text, uartTxField.text, uartRxField.text)
                        }
                    }

                    AppTextField {
                        id: uartServiceField
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        mono: true
                        font.pixelSize: 9
                        text: "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
                        placeholder: "Service UUID"
                    }

                    AppTextField {
                        id: uartTxField
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        mono: true
                        font.pixelSize: 9
                        text: "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
                        placeholder: "写特征 UUID"
                    }

                    AppTextField {
                        id: uartRxField
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        mono: true
                        font.pixelSize: 9
                        text: "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
                        placeholder: "Notify 特征 UUID"
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                    Text {
                        text: "Classic RFCOMM (SPP)"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        AppButton {
                            compact: true
                            textPixelSize: 11
                            text: BluetoothManager.rfcommDiscovering ? I18n.t("Discovering…", "发现中…") : I18n.t("Discover services", "服务发现")
                            enabled: BluetoothManager.backendSupported
                            onClicked: BluetoothManager.startRfcommDiscovery()
                        }

                        AppButton {
                            compact: true
                            textPixelSize: 11
                            text: I18n.t("Connect", "连接")
                            variant: "outlined"
                            accent: Theme.cyan
                            onClicked: BluetoothManager.connectRfcomm(rfcommList.currentIndex)
                        }
                    }

                    ListView {
                        id: rfcommList
                        Layout.fillWidth: true
                        Layout.preferredHeight: Math.min(96, contentHeight)
                        clip: true
                        spacing: 2
                        model: BluetoothManager.rfcommServices

                        delegate: Rectangle {
                            id: rfcommRow
                            required property var modelData
                            required property int index
                            width: rfcommList.width
                            implicitHeight: 30
                            radius: Theme.radiusSmall
                            color: rfcommList.currentIndex === index
                                   ? Theme.selectedFill : "transparent"

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: rfcommList.currentIndex = rfcommRow.index
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8

                                Text {
                                    Layout.fillWidth: true
                                    text: rfcommRow.modelData.device + " · " + rfcommRow.modelData.service
                                    color: Theme.textSecondary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }
                }
            }

            // -------- 右：GATT 浏览 + 终端 --------
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12

                AppCard {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    padding: 0

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 12
                            Layout.rightMargin: 12
                            Layout.topMargin: 10
                            Layout.bottomMargin: 8
                            spacing: 8

                            Text {
                                text: I18n.t("GATT browser", "GATT 浏览")
                                color: Theme.textPrimary
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                font.weight: Font.DemiBold
                            }

                            AppComboBox {
                                implicitWidth: 250
                                implicitHeight: 32
                                font.pixelSize: 12
                                model: BluetoothManager.services
                                textRole: "name"
                                currentIndex: BluetoothManager.selectedService
                                onActivated: (index) => BluetoothManager.selectedService = index
                            }

                            Item { Layout.fillWidth: true }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                        ListView {
                            id: gattList
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.margins: 8
                            clip: true
                            spacing: 4
                            model: root.currentService
                                   ? root.currentService.characteristics : []

                            ScrollBar.vertical: ScrollBar { }

                            property int selectedChar: -1

                            delegate: Rectangle {
                                id: charRow
                                required property var modelData
                                required property int index
                                width: gattList.width
                                implicitHeight: Math.max(56, charColumn.implicitHeight + 14)
                                radius: Theme.radiusSmall
                                color: gattList.selectedChar === index
                                       ? Theme.selectedFill
                                       : index % 2 === 0 ? "transparent"
                                                         : Theme.alpha(Theme.textPrimary, Theme.dark ? 0.03 : 0.018)

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: gattList.selectedChar = charRow.index
                                }

                                ColumnLayout {
                                    id: charColumn
                                    anchors.fill: parent
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 10
                                    anchors.topMargin: 7
                                    anchors.bottomMargin: 7
                                    spacing: 4

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8

                                        Text {
                                            text: charRow.modelData.name
                                            color: Theme.textPrimary
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 12
                                            font.weight: Font.DemiBold
                                        }

                                        Text {
                                            Layout.fillWidth: true
                                            text: charRow.modelData.uuid
                                            color: Theme.textTertiary
                                            font.family: "Consolas"
                                            font.pixelSize: 10
                                            elide: Text.ElideMiddle
                                        }

                                        StatusPill {
                                            text: charRow.modelData.properties
                                            accent: Theme.primary
                                            dot: false
                                            scaleFactor: 0.78
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8

                                        Text {
                                            Layout.fillWidth: true
                                            text: "值: 0x" + charRow.modelData.valueHex
                                                  + (charRow.modelData.valueText
                                                     && charRow.modelData.valueText.length > 0
                                                     ? "  \"" + charRow.modelData.valueText + "\"" : "")
                                                  + " (" + charRow.modelData.valueSize + " B)"
                                            color: Theme.success
                                            font.family: "Consolas"
                                            font.pixelSize: 11
                                            elide: Text.ElideMiddle
                                        }

                                        AppButton {
                                            compact: true
                                            textPixelSize: 11
                                            text: I18n.t("Read", "读")
                                            enabled: charRow.modelData.canRead
                                                     && gattList.selectedChar === index
                                            onClicked: BluetoothManager.readCharacteristic(
                                                           BluetoothManager.selectedService, index)
                                        }

                                        AppButton {
                                            compact: true
                                            textPixelSize: 11
                                            text: I18n.t("Write (response)", "写(应答)")
                                            enabled: charRow.modelData.canWrite
                                                     && gattList.selectedChar === index
                                            onClicked: charEditor.open(
                                                           BluetoothManager.selectedService, index, true)
                                        }

                                        AppButton {
                                            compact: true
                                            textPixelSize: 11
                                            text: I18n.t("Write (no response)", "写(无应答)")
                                            enabled: charRow.modelData.canWriteNoResponse
                                                     && gattList.selectedChar === index
                                            onClicked: charEditor.open(
                                                           BluetoothManager.selectedService, index, false)
                                        }

                                        AppButton {
                                            compact: true
                                            textPixelSize: 11
                                            text: "Notify"
                                            accent: Theme.success
                                            variant: "outlined"
                                            enabled: charRow.modelData.canNotify
                                                     && gattList.selectedChar === index
                                            onClicked: BluetoothManager.setNotify(
                                                           BluetoothManager.selectedService, index,
                                                           true, false)
                                        }

                                        AppButton {
                                            compact: true
                                            textPixelSize: 11
                                            text: "Indicate"
                                            accent: Theme.warning
                                            variant: "outlined"
                                            enabled: charRow.modelData.canIndicate
                                                     && gattList.selectedChar === index
                                            onClicked: BluetoothManager.setNotify(
                                                           BluetoothManager.selectedService, index,
                                                           true, true)
                                        }

                                        AppButton {
                                            compact: true
                                            textPixelSize: 11
                                            text: I18n.t("Unsubscribe", "停订阅")
                                            variant: "text"
                                            enabled: gattList.selectedChar === index
                                            onClicked: BluetoothManager.setNotify(
                                                           BluetoothManager.selectedService, index,
                                                           false, false)
                                        }
                                    }
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                visible: !root.currentService
                                         || root.currentService.characteristics.length === 0
                                text: BluetoothManager.leState === "未连接"
                                      ? "连接 BLE 设备后，服务与特征会显示在这里"
                                      : "该服务暂无特征"
                                color: Theme.textTertiary
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                            }
                        }
                    }
                }

                AppCard {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 396
                    padding: 0

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        TerminalView {
                            id: bleTerminal
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            model: BluetoothManager.traffic
                            emptyHint: I18n.t("BLE Notify / Read / Write events appear here", "BLE Notify / Read / Write 事件会显示在这里")
                            onToastRequested: (text, ok) => root.toastRequested(text, ok)
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                        SendPanel {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 152
                            manager: BluetoothManager
                            showFile: false
                            sendLabel: I18n.t("Send", "发送")
                            hint: I18n.t("BLE UART mode fragments automatically by MTU", "BLE UART 模式按 MTU 自动分片")
                            onToastRequested: (text, ok) => root.toastRequested(text, ok)
                        }
                    }
                }
            }
        }
    }

    // 特征写值弹窗
    Popup {
        id: charEditor
        property int serviceIndex: 0
        property int charIndex: 0
        property bool withResponse: true

        parent: Overlay.overlay
        x: (parent ? parent.width - width : 0) / 2
        y: (parent ? parent.height - height : 0) / 2
        width: 520
        height: 240
        padding: 18
        closePolicy: Popup.CloseOnEscape

        background: Rectangle {
            color: Theme.surface
            radius: Theme.radiusLarge
            border.width: 1
            border.color: Theme.alpha(Theme.primary, 0.42)
        }

        function openFor(serviceIndex, charIndex, withResponse) {
            charEditor.serviceIndex = serviceIndex
            charEditor.charIndex = charIndex
            charEditor.withResponse = withResponse
            charValueField.text = ""
            charEditor.open()
        }

        contentItem: ColumnLayout {
            spacing: 10

            Text {
                text: charEditor.withResponse ? I18n.t("Write characteristic (with response)", "写入特征（带应答）") : I18n.t("Write characteristic (without response)", "写入特征（无应答）")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.DemiBold
            }

            FormRow {
                label: I18n.t("Format", "格式")
                AppComboBox {
                    id: charFormatCombo
                    implicitWidth: 120
                    model: ["HEX", "文本"]
                }
            }

            FormRow {
                label: I18n.t("Value", "值")
                AppTextField {
                    id: charValueField
                    Layout.fillWidth: true
                    mono: true
                    placeholder: "AA 55 01 02"
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Item { Layout.fillWidth: true }

                AppButton {
                    text: I18n.t("Cancel", "取消")
                    variant: "outlined"
                    accent: Theme.textSecondary
                    onClicked: charEditor.close()
                }

                AppButton {
                    text: I18n.t("Write", "写入")
                    variant: "primary"
                    onClicked: {
                        BluetoothManager.writeCharacteristic(
                            charEditor.serviceIndex, charEditor.charIndex,
                            charValueField.text,
                            charFormatCombo.currentIndex === 0,
                            charEditor.withResponse)
                        charEditor.close()
                    }
                }
            }
        }
    }
}
