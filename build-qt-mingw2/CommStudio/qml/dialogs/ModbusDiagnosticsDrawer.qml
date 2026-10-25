import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Drawer {
    id: root
    property var backend: ModbusWorkspace.current
    parent: Overlay.overlay
    edge: Qt.RightEdge
    width: Math.min(460, parent.width)
    height: parent.height
    padding: 18
    modal: true
    closePolicy: Popup.CloseOnEscape
    property bool scanner: true
    function statLabel(key) {
        const labels = {
            txFrames: I18n.t("TX Frames", "TX 帧数"), rxFrames: I18n.t("RX Frames", "RX 帧数"),
            txBytes: I18n.t("TX Bytes", "TX 字节数"), rxBytes: I18n.t("RX Bytes", "RX 字节数"),
            errors: I18n.t("Errors", "错误"), timeout: I18n.t("Timeout", "超时"),
            exception: I18n.t("Exception", "异常响应"), crcError: I18n.t("CRC Error", "CRC 错误"),
            frameError: I18n.t("Frame Error", "帧格式错误"), success: I18n.t("Success", "成功率"),
            rttCurrent: I18n.t("RTT Current", "当前 RTT"), rttAverage: I18n.t("RTT Average", "平均 RTT"),
            rttMin: I18n.t("RTT Min", "最小 RTT"), rttMax: I18n.t("RTT Max", "最大 RTT")
        }
        return labels[key] || key
    }
    background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.color: Theme.border }
    contentItem: ColumnLayout {
        spacing: 12
        Text { text: root.scanner ? I18n.t("DEVICE SCANNER", "设备扫描") : I18n.t("SESSION STATISTICS", "会话统计"); color: Theme.cyan; font.pixelSize: 18 }
        ColumnLayout {
            visible: root.scanner
            Layout.fillWidth: true
            FormRow { label: I18n.t("First ID", "起始站号"); AppTextField { id: first; text: "1"; Layout.fillWidth: true; validator: IntValidator { bottom: 1; top: 247 } } }
            FormRow { label: I18n.t("Last ID", "结束站号"); AppTextField { id: last; text: "247"; Layout.fillWidth: true; validator: IntValidator { bottom: 1; top: 247 } } }
            FormRow { label: "FC"; AppComboBox { id: fc; model: ["01", "02", "03", "04"]; currentIndex: 2; Layout.fillWidth: true } }
            FormRow { label: I18n.t("Address", "地址"); AppTextField { id: address; text: "0"; Layout.fillWidth: true; validator: IntValidator { bottom: 0; top: 65535 } } }
            FormRow { label: "Timeout ms"; AppTextField { id: timeout; text: "100"; Layout.fillWidth: true; validator: IntValidator { bottom: 20; top: 10000 } } }
            RowLayout {
                AppButton { text: I18n.t("Start", "开始"); enabled: root.backend.open && !root.backend.scanning && first.acceptableInput && last.acceptableInput && Number(first.text) <= Number(last.text) && address.acceptableInput && timeout.acceptableInput; onClicked: root.backend.startScan(Number(first.text), Number(last.text), fc.currentIndex + 1, Number(address.text), Number(timeout.text)) }
                AppButton { text: I18n.t("Stop", "停止"); enabled: root.backend.scanning; onClicked: root.backend.stopScan() }
            }
            Text { text: I18n.t("Completed: ", "已完成：") + root.backend.scanResults.length; color: Theme.textSecondary }
        }
        ListView {
            visible: root.scanner
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.backend.scanResults
            delegate: Text {
                required property var modelData
                width: ListView.view.width
                height: 32
                text: "ID " + modelData.unitId + "    " + modelData.rtt + " ms    " + modelData.status
                color: modelData.online ? Theme.success : Theme.textTertiary
                font.family: "Consolas"
                font.pixelSize: 12
            }
            ScrollBar.vertical: ScrollBar {}
        }
        ScrollView {
            visible: !root.scanner
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                Repeater {
                    model: ["txFrames", "rxFrames", "txBytes", "rxBytes", "errors", "timeout", "exception", "success", "rttCurrent", "rttAverage", "rttMin", "rttMax"]
                    delegate: RowLayout {
                        required property string modelData
                        Layout.fillWidth: true
                        Text { text: root.statLabel(modelData); color: Theme.textSecondary; Layout.fillWidth: true }
                        Text { text: Number(root.backend.statistics[modelData]).toFixed(2); color: Theme.cyan; font.family: "Consolas" }
                    }
                }
                Text { Layout.fillWidth: true; wrapMode: Text.WordWrap; text: I18n.t("Standard and Raw requests; transport retries and scan traffic are excluded.", "统计标准及 Raw 请求；不含底层重试和扫描流量。"); color: Theme.textTertiary; font.pixelSize: 11 }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            AppButton { visible: root.scanner; text: I18n.t("Add discovered", "添加发现设备"); enabled: !root.backend.scanning; onClicked: root.backend.addDiscoveredDevices() }
            AppButton { visible: !root.scanner; text: I18n.t("Reset", "重置"); onClicked: root.backend.resetStatistics() }
            Item { Layout.fillWidth: true }
            AppButton { text: I18n.t("Close", "关闭"); onClicked: root.close() }
        }
    }
}
