import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CommStudio 1.0
import CommStudio.Backend 1.0

Window {
    id: root
    property var backend: ModbusWorkspace.current
    width: 1100
    height: 680
    minimumWidth: 900
    minimumHeight: 540
    title: I18n.t("Modbus trends", "Modbus 趋势")
    color: Theme.background
    property var series: []
    property var legend: []
    property var rightAxis: ({})
    property bool paused: false
    property double endTime: Date.now()
    readonly property int windowMs: [10000, 60000, 300000, 3600000][span.currentIndex]
    readonly property var colors: [Theme.cyan, Theme.success, Theme.primary, Theme.warning, Theme.purple, Theme.error]
    function refresh() {
        if (!visible || paused) return
        endTime = Date.now()
        series = root.backend.points.trendSeries(windowMs)
        const nextLegend = series.map(function(line) { return {id: line.id, name: line.name, unit: line.unit} })
        if (JSON.stringify(nextLegend) !== JSON.stringify(legend)) legend = nextLegend
        chart.requestPaint()
    }
    function limits(right) {
        let low = Infinity, high = -Infinity
        for (const line of series) {
            if (!!rightAxis[line.id] !== right || !line.count) continue
            low = Math.min(low, line.min); high = Math.max(high, line.max)
        }
        if (!isFinite(low)) return [0, 1]
        const padding = Math.max((high - low) * 0.08, Math.abs(high) * 0.01, 0.01)
        return [low - padding, high + padding]
    }
    onVisibleChanged: refresh()
    Timer { interval: 100; repeat: true; running: root.visible && !root.paused; onTriggered: root.refresh() }
    FileDialog {
        id: csv
        fileMode: FileDialog.SaveFile
        defaultSuffix: "csv"
        nameFilters: ["CSV (*.csv)"]
        onAccepted: status.text = root.backend.points.exportHistory(selectedFile) ? I18n.t("Exported", "已导出") : I18n.t("Export failed", "导出失败")
    }
    FileDialog {
        id: png
        fileMode: FileDialog.SaveFile
        defaultSuffix: "png"
        nameFilters: ["PNG (*.png)"]
        onAccepted: chart.grabToImage(function(result) { status.text = result.saveToFile(decodeURIComponent(png.selectedFile.toString().replace(/^file:\/\//, "").replace(/^\/([A-Za-z]:)/, "$1"))) ? I18n.t("Exported", "已导出") : I18n.t("Export failed", "导出失败") })
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12
        RowLayout {
            Layout.fillWidth: true
            Text { text: I18n.t("POINT TRENDS", "数据点趋势"); color: Theme.cyan; font.pixelSize: 18 }
            Item { Layout.fillWidth: true }
            AppComboBox { id: span; model: ["10 s", "60 s", "5 min", "1 h"]; currentIndex: 1; onCurrentIndexChanged: root.refresh() }
            AppButton { text: root.paused ? I18n.t("Resume", "继续") : I18n.t("Pause", "暂停"); onClicked: { root.paused = !root.paused; root.refresh() } }
            AppButton { text: "PNG"; onClicked: png.open() }
            AppButton { text: "CSV"; onClicked: csv.open() }
        }
        RowLayout {
            Text { text: I18n.t("Reference / warning (left axis)", "参考线 / 警戒线（左轴）"); color: Theme.textSecondary }
            AppTextField { id: reference; placeholder: I18n.t("Reference", "参考值"); Layout.preferredWidth: 100; onTextChanged: chart.requestPaint() }
            AppTextField { id: warning; placeholder: I18n.t("Warning", "警戒值"); Layout.preferredWidth: 100; onTextChanged: chart.requestPaint() }
            Text { id: status; color: Theme.textTertiary; Layout.fillWidth: true; elide: Text.ElideRight }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ListView {
                Layout.preferredWidth: 250
                Layout.fillHeight: true
                clip: true
                model: root.legend
                delegate: ColumnLayout {
                    required property var modelData
                    required property int index
                    property var sampleStats: root.series[index] || {min: 0, max: 0, avg: 0, count: 0}
                    width: ListView.view.width
                    Text { Layout.fillWidth: true; text: modelData.name + "  " + modelData.unit; color: root.colors[index % root.colors.length]; elide: Text.ElideRight }
                    Text { text: "Min " + Number(sampleStats.min).toPrecision(5) + " / Max " + Number(sampleStats.max).toPrecision(5); color: Theme.textSecondary; font.pixelSize: 10 }
                    Text { text: "Avg " + Number(sampleStats.avg).toPrecision(5) + " · " + sampleStats.count; color: Theme.textTertiary; font.pixelSize: 10 }
                    AppCheckBox {
                        text: I18n.t("Right Y axis", "右 Y 轴")
                        checked: !!root.rightAxis[modelData.id]
                        onToggled: { const axes = Object.assign({}, root.rightAxis); axes[modelData.id] = checked; root.rightAxis = axes; chart.requestPaint() }
                    }
                }
                ScrollBar.vertical: ScrollBar {}
            }
            Canvas {
                id: chart
                Layout.fillWidth: true
                Layout.fillHeight: true
                property real cursorX: -1
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
                onPaint: {
                    const ctx = getContext("2d")
                    ctx.reset()
                    ctx.fillStyle = Theme.surface; ctx.fillRect(0, 0, width, height)
                    const left = 60, top = 20, w = width - 120, h = height - 60
                    if (w <= 0 || h <= 0) return
                    const limitsLeft = root.limits(false), limitsRight = root.limits(true)
                    ctx.font = "10px Consolas"
                    for (let i = 0; i <= 5; ++i) {
                        const y = top + h * i / 5
                        ctx.strokeStyle = Theme.divider; ctx.beginPath(); ctx.moveTo(left, y); ctx.lineTo(left + w, y); ctx.stroke()
                        ctx.fillStyle = Theme.textSecondary
                        ctx.fillText((limitsLeft[1] - (limitsLeft[1] - limitsLeft[0]) * i / 5).toPrecision(4), 2, y + 3)
                        ctx.fillText((limitsRight[1] - (limitsRight[1] - limitsRight[0]) * i / 5).toPrecision(4), left + w + 5, y + 3)
                    }
                    for (let i = 0; i <= 4; ++i) ctx.fillText("-" + ((4 - i) * root.windowMs / 4000).toFixed(0) + "s", left + i * w / 4, top + h + 22)
                    ctx.save(); ctx.beginPath(); ctx.rect(left, top, w, h); ctx.clip()
                    for (let i = 0; i < root.series.length; ++i) {
                        const line = root.series[i], bounds = root.rightAxis[line.id] ? limitsRight : limitsLeft
                        ctx.strokeStyle = root.colors[i % root.colors.length]; ctx.lineWidth = 1.5; ctx.beginPath()
                        let first = true
                        for (const sample of line.samples) {
                            const x = left + (sample.time - root.endTime + root.windowMs) * w / root.windowMs
                            const y = top + h - (sample.value - bounds[0]) * h / (bounds[1] - bounds[0])
                            if (first) { ctx.moveTo(x, y); first = false } else ctx.lineTo(x, y)
                        }
                        ctx.stroke()
                    }
                    for (const marker of [{field: reference, color: Theme.warning}, {field: warning, color: Theme.error}]) {
                        if (!marker.field.text.length || !isFinite(Number(marker.field.text))) continue
                        const y = top + h - (Number(marker.field.text) - limitsLeft[0]) * h / (limitsLeft[1] - limitsLeft[0])
                        ctx.strokeStyle = marker.color; ctx.beginPath(); ctx.moveTo(left, y); ctx.lineTo(left + w, y); ctx.stroke()
                    }
                    if (cursorX >= left && cursorX <= left + w) {
                        ctx.strokeStyle = Theme.textSecondary; ctx.beginPath(); ctx.moveTo(cursorX, top); ctx.lineTo(cursorX, top + h); ctx.stroke()
                        ctx.fillStyle = Theme.textPrimary; ctx.fillText(new Date(root.endTime - root.windowMs + (cursorX - left) / w * root.windowMs).toLocaleTimeString(), cursorX + 5, top + 12)
                    }
                    ctx.restore()
                }
                MouseArea { anchors.fill: parent; hoverEnabled: true; onPositionChanged: function(mouse) { chart.cursorX = mouse.x; chart.requestPaint() }; onExited: { chart.cursorX = -1; chart.requestPaint() } }
                Connections { target: Theme; function onDarkChanged() { chart.requestPaint() } }
            }
        }
        Text { text: I18n.t("Selected points · Up to 50 curves · Latest 10,000 samples per point retained", "选中数据点 · 最多 50 条曲线 · 每点保留最近 10,000 个采样"); color: Theme.textTertiary; font.pixelSize: 11 }
    }
}
