import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CommStudio 1.0

// Dense traffic view inspired by TuiSerial's stable timestamp/direction/size
// columns. The model keeps raw bytes; this component only changes presentation.
Item {
    id: root

    property var model
    property string emptyHint: I18n.t("Waiting for data · received and transmitted traffic appears here after connecting", "等待数据 · 连接后收发的数据会显示在这里")
    property int displayMode: 2 // 0 TEXT / 1 HEX / 2 HEX+ASCII
    property bool decodeModbus: false
    property bool allowHideSent: false
    property bool showSentMessages: true
    readonly property bool compactToolbar: width < 720
    readonly property var displayModeLabels: [I18n.t("Text", "文本"), I18n.t("HEX", "十六进制"), I18n.t("HEX+ASCII", "十六进制+ASCII")]
    property bool autoScroll: true
    property bool followTail: true
    property int unseenCount: 0
    property bool tailScrollQueued: false
    property string traceFilterText: ""
    property int traceFilterDirection: -1
    property int selectedTraceIndex: -1
    property int selectedTraceDirection: -1
    onShowSentMessagesChanged: {
        if (!showSentMessages && selectedTraceDirection === 1)
            selectedTraceIndex = -1
    }
    property string selectedTraceTitle: ""
    property string selectedTraceDetail: ""
    property bool selectedTracePlain: false
    property string selectedTraceText: ""
    property string selectedTraceHex: ""
    property string selectedTraceAscii: ""
    readonly property string selectedTracePayload: selectedTracePlain || displayMode === 0 ? selectedTraceText
        : displayMode === 1 ? selectedTraceHex : selectedTraceHex + "\n\nASCII: " + selectedTraceAscii

    function matchesFilter(dir, dataHex, dataText, peer) {
        if (allowHideSent && !showSentMessages && dir === 1)
            return false
        if (traceFilterDirection >= 0 && dir !== traceFilterDirection)
            return false
        const query = traceFilterText.trim().toLowerCase()
        if (query.length === 0)
            return true
        return (dataHex + " " + dataText + " " + peer).toLowerCase().indexOf(query) >= 0
    }

    function compactTextPreview(text) {
        // A record already has its own row boundary. Its final line ending
        // must not allocate another empty visual line; intentional blank lines
        // inside the response and the unmodified payload remain available.
        return text.endsWith("\n") ? text.slice(0, -1) : text
    }

    function parseHexBytes(text) {
        const tokens = text.trim().split(/\s+/)
        const bytes = []
        for (let i = 0; i < tokens.length; ++i) {
            if (!tokens[i]) continue
            const value = parseInt(tokens[i], 16)
            if (!isNaN(value)) bytes.push(value & 0xff)
        }
        return bytes
    }

    function protocolSummary(hex, dir, size, peer) {
        if (!decodeModbus)
            return dirLabel(dir) + " · " + size + I18n.t(" bytes", " 字节")
                    + (peer.length > 0 ? " · " + peer : "")
        const bytes = parseHexBytes(hex)
        if (bytes.length === 0)
            return I18n.t("No binary payload", "无二进制载荷")
        let offset = 0
        let transport = "RTU"
        let address = bytes[0]
        let functionCode = bytes.length > 1 ? bytes[1] : -1
        if (bytes.length >= 8 && bytes[2] === 0 && bytes[3] === 0
                && bytes[4] === 0 && bytes[5] === 0 && bytes[6] === bytes.length - 6) {
            transport = "TCP"
            offset = 6
            address = bytes[6]
            functionCode = bytes[7]
        } else if (bytes[0] === 0x3a) {
            transport = "ASCII"
            address = bytes.length > 1 ? bytes[1] : -1
            functionCode = bytes.length > 2 ? bytes[2] : -1
        }
        const fcName = {
            1: "Read Coils", 2: "Read Discrete Inputs", 3: "Read Holding Registers",
            4: "Read Input Registers", 5: "Write Single Coil", 6: "Write Single Register",
            15: "Write Multiple Coils", 16: "Write Multiple Registers"
        }[functionCode & 0x7f] || I18n.t("Custom / Unknown", "自定义 / 未知")
        const exception = (functionCode & 0x80) !== 0
        const direction = dir === 0 ? "RX" : dir === 1 ? "TX" : dir === 3 ? "ERR" : "INFO"
        const idText = address >= 0 ? address.toString(16).padStart(2, "0").toUpperCase() : "--"
        const fcText = functionCode >= 0 ? functionCode.toString(16).padStart(2, "0").toUpperCase() : "--"
        const exceptionText = exception ? I18n.t(" · EXCEPTION", " · 异常") : ""
        return transport + " · " + direction + " · ID " + idText + " · FC 0x" + fcText
                + " (" + fcName + ")" + exceptionText + " · " + size + " B · "
                + (peer.length > 0 ? peer : I18n.t("local", "本地"))
    }

    // rowsInserted 时 ListView 的 contentHeight 可能尚未完成重算；延迟到下一轮
    // 布局后再定位，才能稳定贴住最新一行而不是停留在旧的滚动位置。
    function scheduleTailScroll() {
        if (tailScrollQueued || !followTail || !autoScroll)
            return
        tailScrollQueued = true
        Qt.callLater(function() {
            tailScrollQueued = false
            if (!followTail || !autoScroll)
                return
            trafficList.forceLayout()
            trafficList.positionViewAtEnd()
        })
    }

    function dirAccent(dir) {
        if (dir === 0) return Theme.rx
        if (dir === 1) return Theme.tx
        if (dir === 3) return Theme.error
        return Theme.warning
    }

    function dirLabel(dir) {
        if (dir === 0) return "◄ RX"
        if (dir === 1) return "► TX"
        if (dir === 3) return "! ERR"
        return "· INF"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 10
            Layout.topMargin: 8
            Layout.bottomMargin: 7
            spacing: root.compactToolbar ? 4 : 8

            Text {
                text: I18n.t("MESSAGE", "消息")
                color: Theme.textPrimary
                font.family: "Consolas"
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }

            Text {
                visible: !root.compactToolbar
                text: root.displayModeLabels[root.displayMode]
                color: Theme.cyan
                font.family: "Consolas"
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }

            StatusPill {
                visible: root.width >= 620
                text: root.model ? I18n.count(root.model.count, "items", " 条") : I18n.t("0 items", "0 条")
                accent: Theme.primary
                dot: false
                scaleFactor: 0.82
            }

            Text {
                visible: !root.compactToolbar && root.model && root.model.paused
                text: I18n.t("PAUSED · RECEIVING IN BACKGROUND", "已暂停 · 后台仍在接收")
                color: Theme.warning
                font.family: "Consolas"
                font.pixelSize: 10
            }

            Item { Layout.fillWidth: true }

            AppComboBox {
                implicitWidth: I18n.chinese ? 142 : 122
                implicitHeight: 30
                font.family: "Consolas"
                font.pixelSize: 11
                model: root.displayModeLabels
                currentIndex: root.displayMode
                onActivated: (index) => root.displayMode = index
            }

            AppButton {
                compact: true
                Layout.minimumWidth: I18n.chinese ? 48 : 56
                Layout.preferredWidth: Layout.minimumWidth
                textPixelSize: 11
                text: root.model && root.model.paused ? I18n.t("Resume", "继续") : I18n.t("Pause", "暂停")
                accent: root.model && root.model.paused ? Theme.warning : Theme.primary
                variant: "text"
                onClicked: if (root.model) root.model.paused = !root.model.paused
            }

            AppButton {
                compact: true
                Layout.minimumWidth: I18n.chinese ? 48 : 56
                Layout.preferredWidth: Layout.minimumWidth
                textPixelSize: 11
                text: I18n.t("Clear", "清屏")
                variant: "text"
                onClicked: if (root.model) root.model.clear()
            }

            AppButton {
                compact: true
                Layout.minimumWidth: I18n.chinese ? 48 : 56
                Layout.preferredWidth: Layout.minimumWidth
                textPixelSize: 11
                text: I18n.t("Export", "导出")
                variant: "text"
                onClicked: exportDialog.open()
            }

            AppButton {
                compact: true
                Layout.minimumWidth: I18n.chinese ? 76 : 104
                Layout.preferredWidth: Layout.minimumWidth
                textPixelSize: 11
                text: root.followTail ? I18n.t("Follow tail", "自动跟随") : I18n.t("Manual browse", "手动浏览")
                accent: root.followTail ? Theme.success : Theme.warning
                variant: "text"
                onClicked: {
                    root.followTail = !root.followTail
                    if (root.followTail) {
                        root.unseenCount = 0
                        root.scheduleTailScroll()
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

        Text {
            visible: root.compactToolbar && root.model && root.model.paused
            Layout.fillWidth: true
            Layout.leftMargin: 12
            text: I18n.t("Paused · receiving in background", "已暂停显示 · 后台仍在接收")
            color: Theme.warning
            font.family: Theme.fontFamily
            font.pixelSize: 10
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 10
            Layout.topMargin: 6
            Layout.bottomMargin: 6
            spacing: 8

            Text {
                text: I18n.t("FILTER", "筛选")
                color: Theme.textTertiary
                font.family: "Consolas"
                font.pixelSize: 10
                font.weight: Font.DemiBold
            }

            AppComboBox {
                implicitWidth: 92
                implicitHeight: 28
                font.family: "Consolas"
                font.pixelSize: 10
                model: [I18n.t("ALL", "全部"), I18n.t("RX", "接收"), I18n.t("TX", "发送"), I18n.t("INFO", "信息"), I18n.t("ERR", "错误")]
                currentIndex: root.traceFilterDirection < 0 ? 0 : root.traceFilterDirection + 1
                onActivated: (index) => {
                    root.traceFilterDirection = index === 0 ? -1 : index - 1
                    if (root.traceFilterDirection === 1)
                        root.showSentMessages = true
                }
            }

            AppCheckBox {
                visible: root.allowHideSent
                Layout.alignment: Qt.AlignVCenter
                text: I18n.t("Show sent", "显示发送")
                font.pixelSize: 11
                checked: root.showSentMessages
                onToggled: {
                    root.showSentMessages = checked
                    if (!checked && root.traceFilterDirection === 1)
                        root.traceFilterDirection = -1
                    root.scheduleTailScroll()
                }
                ToolTip.visible: hovered
                ToolTip.text: I18n.t("Show TX records alongside received data; hidden records are still kept and exported.",
                                    "同时显示发送记录；隐藏只影响显示，原始记录仍保留并可导出。")
            }

            AppTextField {
                Layout.fillWidth: true
                implicitHeight: 28
                mono: true
                placeholder: root.decodeModbus ? I18n.t("Channel / device / FC / payload", "通道 / 设备 / FC / 内容")
                                               : I18n.t("Port / text / hex bytes", "端口 / 文本 / 十六进制字节")
                text: root.traceFilterText
                onTextChanged: root.traceFilterText = text
            }

            AppButton {
                compact: true
                textPixelSize: 10
                text: I18n.t("Reset", "重置")
                variant: "text"
                visible: root.traceFilterText.length > 0 || root.traceFilterDirection >= 0
                onClicked: {
                    root.traceFilterText = ""
                    root.traceFilterDirection = -1
                }
            }
        }

        ListView {
            id: trafficList
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 5
            clip: true
            spacing: 0
            model: root.model

            ScrollBar.vertical: ScrollBar { }

            onCountChanged: {
                if (root.selectedTraceIndex >= count)
                    root.selectedTraceIndex = -1
                if (root.followTail && root.autoScroll)
                    root.scheduleTailScroll()
                else if (!atYEnd)
                    root.unseenCount = Math.min(999, root.unseenCount + 1)
            }
            onContentHeightChanged: {
                if (count > 0)
                    root.scheduleTailScroll()
            }
            onMovementStarted: if (!atYEnd) root.followTail = false
            onMovementEnded: if (atYEnd) { root.followTail = true; root.unseenCount = 0 }

            delegate: Rectangle {
                id: row
                required property int index
                required property string time
                required property int dir
                required property string dataHex
                required property string dataText
                required property string dataAscii
                required property int omittedBytes
                required property string peer
                required property int size
                required property bool plain
                property bool matchesFilter: root.matchesFilter(row.dir, row.dataHex, row.dataText, row.peer)
                width: trafficList.width
                visible: matchesFilter
                height: matchesFilter ? Math.max(25, dataTextItem.implicitHeight + 8) : 0
                color: root.selectedTraceIndex === index ? Theme.selectedFill
                       : index % 2 === 0 ? "transparent" : Theme.alpha(Theme.textPrimary, 0.025)

                Rectangle {
                    width: 2
                    height: parent.height
                    color: root.dirAccent(row.dir)
                    opacity: row.dir === 2 ? 0.55 : 0.95
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 9
                    anchors.rightMargin: 8
                    spacing: 9

                    Text {
                        Layout.preferredWidth: 82
                        Layout.alignment: Qt.AlignTop
                        Layout.topMargin: 4
                        text: row.time
                        color: Theme.timestamp
                        font.family: "Consolas"
                        font.pixelSize: 11
                    }

                    Text {
                        Layout.preferredWidth: 48
                        Layout.alignment: Qt.AlignTop
                        Layout.topMargin: 4
                        text: root.dirLabel(row.dir)
                        color: root.dirAccent(row.dir)
                        font.family: "Consolas"
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                    }

                    Text {
                        Layout.preferredWidth: 52
                        Layout.alignment: Qt.AlignTop
                        Layout.topMargin: 4
                        text: row.size + " B"
                        color: Theme.warning
                        font.family: "Consolas"
                        font.pixelSize: 10
                    }

                    Text {
                        id: dataTextItem
                        Layout.fillWidth: true
                        text: {
                            const body = row.plain ? row.dataText
                                         : root.displayMode === 0 ? (row.omittedBytes === 0 ? root.compactTextPreview(row.dataText) : row.dataText)
                                         : root.displayMode === 1 ? row.dataHex
                                         : row.dataHex + "  |  " + row.dataAscii
                            return body + (row.omittedBytes > 0
                                   ? "\n" + I18n.t("… preview truncated; click to view all " + row.size + " bytes",
                                                    "… 预览已截短，点击查看完整 " + row.size + " 字节") : "")
                        }
                        color: row.plain && row.dir === 3 ? Theme.error
                               : row.plain ? Theme.textSecondary : Theme.payload
                        font.family: "Consolas"
                        font.pixelSize: 12
                        wrapMode: Text.WrapAnywhere
                        textFormat: Text.PlainText
                    }

                    Text {
                        Layout.alignment: Qt.AlignTop
                        Layout.topMargin: 4
                        visible: row.peer.length > 0
                        text: row.peer
                        color: Theme.textTertiary
                        font.family: "Consolas"
                        font.pixelSize: 10
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    z: 1
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        root.selectedTraceIndex = row.index
                        root.selectedTraceDirection = row.dir
                        root.selectedTraceTitle = root.protocolSummary(row.dataHex, row.dir, row.size, row.peer)
                        root.selectedTracePlain = row.plain
                        root.selectedTraceText = root.model.payloadText(row.index)
                        root.selectedTraceHex = root.model.payloadHex(row.index)
                        root.selectedTraceAscii = root.model.payloadAscii(row.index)
                        root.selectedTraceDetail = row.plain
                                ? row.dataText
                                : I18n.t("Click another frame to inspect it · raw payload preserved",
                                         "点击其他报文可继续查看 · 原始载荷已完整保留")
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: root.model && root.model.count === 0
                text: root.emptyHint + "\n\n" + I18n.t("Ctrl+Enter send · ↑/↓ history · F1 shortcuts", "Ctrl+Enter 发送 · ↑/↓ 浏览历史 · F1 查看快捷键")
                color: Theme.textTertiary
                font.family: "Consolas"
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                anchors.centerIn: parent
                visible: root.model && root.model.count > 0
                         && (root.traceFilterText.length > 0 || root.traceFilterDirection >= 0
                             || (root.allowHideSent && !root.showSentMessages))
                         && trafficList.contentHeight < 2
                text: I18n.t("No frames match the current filter", "没有报文符合当前筛选条件")
                color: Theme.textTertiary
                font.family: "Consolas"
                font.pixelSize: 12
            }

            Rectangle {
                visible: root.unseenCount > 0
                anchors.right: parent.right
                anchors.rightMargin: 14
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 12
                width: newMessagesText.implicitWidth + 22
                height: 28
                color: Theme.selectedFill
                border.width: 1
                border.color: Theme.primary

                Text {
                    id: newMessagesText
                    anchors.centerIn: parent
                    text: "↓ " + (root.unseenCount >= 999 ? "999+" : root.unseenCount)
                          + (root.allowHideSent && !root.showSentMessages
                             ? I18n.t(" new records (includes hidden)", " 条新记录（含隐藏项）")
                             : I18n.t(" new messages", " 条新消息"))
                    color: Theme.textPrimary
                    font.family: "Consolas"
                    font.pixelSize: 11
                    font.weight: Font.DemiBold
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.followTail = true
                        root.unseenCount = 0
                        trafficList.positionViewAtEnd()
                    }
                }
            }
        }

        Rectangle {
            visible: root.selectedTraceIndex >= 0
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? Math.min(170, root.height * 0.45) : 0
            color: Theme.alpha(Theme.surface, 0.92)
            border.width: visible ? 1 : 0
            border.color: Theme.primary
            radius: Theme.radiusSmall

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 9
                spacing: 4
                RowLayout {
                    Layout.fillWidth: true
                    Text { text: root.decodeModbus ? I18n.t("FRAME INSPECTOR", "报文解析") : I18n.t("MESSAGE DETAILS", "数据详情"); color: Theme.cyan; font.family: "Consolas"; font.pixelSize: 10; font.weight: Font.DemiBold }
                    Item { Layout.fillWidth: true }
                    AppButton { compact: true; textPixelSize: 10; text: I18n.t("Close", "关闭"); variant: "text"; onClicked: root.selectedTraceIndex = -1 }
                }
                Text {
                    Layout.fillWidth: true
                    text: root.selectedTraceTitle
                    color: Theme.textPrimary
                    font.family: "Consolas"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    contentWidth: availableWidth
                    TextArea {
                        text: root.selectedTracePayload
                        readOnly: true
                        selectByMouse: true
                        wrapMode: TextEdit.WrapAnywhere
                        textFormat: TextEdit.PlainText
                        color: Theme.payload
                        selectionColor: Theme.selectedFill
                        font.family: "Consolas"
                        font.pixelSize: 12
                        background: null
                    }
                }
            }
        }
    }

    FileDialog {
        id: exportDialog
        title: I18n.t("Export traffic log", "导出收发记录")
        nameFilters: [I18n.t("Text files (*.txt *.log *.csv)", "文本文件 (*.txt *.log *.csv)"),
                      I18n.t("All files (*)", "所有文件 (*)")]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            if (root.model && root.model.exportToFile(selectedFile))
                root.toastRequested(I18n.t("Traffic log exported", "收发记录已导出"), true)
        }
    }

    signal toastRequested(string text, bool ok)
}
