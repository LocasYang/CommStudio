import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0

ComboBox {
    id: control

    property bool showSelectionMark: true
    signal aboutToShowPopup()

    TextMetrics {
        id: itemMetrics
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }

    implicitHeight: 42
    implicitWidth: 220
    leftPadding: 14
    rightPadding: 42
    font.family: Theme.fontFamily
    font.pixelSize: 13

    contentItem: Text {
        leftPadding: 0
        rightPadding: 0
        text: control.displayText
        color: control.enabled ? Theme.textPrimary : Theme.textTertiary
        font: control.font
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    indicator: Item {
        x: control.width - width - 12
        y: (control.height - height) / 2
        width: 22
        height: 22

        Canvas {
            id: arrowCanvas
            anchors.fill: parent

            onPaint: {
                const ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                ctx.strokeStyle = control.popup.visible ? Theme.primary : Theme.textSecondary
                ctx.lineWidth = 1.8
                ctx.lineCap = "round"
                ctx.lineJoin = "round"
                ctx.beginPath()
                if (control.popup.visible) {
                    ctx.moveTo(6, 13)
                    ctx.lineTo(11, 8)
                    ctx.lineTo(16, 13)
                } else {
                    ctx.moveTo(6, 9)
                    ctx.lineTo(11, 14)
                    ctx.lineTo(16, 9)
                }
                ctx.stroke()
            }

            Connections {
                target: control.popup
                function onVisibleChanged() { arrowCanvas.requestPaint() }
            }

            Connections {
                target: Theme
                function onDarkChanged() { arrowCanvas.requestPaint() }
                function onPrimaryChanged() { arrowCanvas.requestPaint() }
            }
        }
    }

    background: Rectangle {
        color: Theme.inputBackground
        radius: Theme.radiusSmall
        border.width: control.activeFocus || control.popup.visible ? 2 : 1
        border.color: control.activeFocus || control.popup.visible ? Theme.primary : Theme.border

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }
        Behavior on border.color { ColorAnimation { duration: Theme.durationFast } }
    }

    delegate: ItemDelegate {
        id: option

        required property int index

        width: control.popup && control.popup.width > 0
               ? control.popup.width - 12 : control.width - 12
        height: 40
        leftPadding: 12
        rightPadding: 10
        highlighted: control.highlightedIndex === index

        contentItem: RowLayout {
            spacing: 9

            Rectangle {
                Layout.preferredWidth: 3
                Layout.preferredHeight: 20
                radius: 2
                color: Theme.primary
                visible: control.currentIndex === option.index
            }

            Item {
                visible: control.currentIndex !== option.index
                Layout.preferredWidth: 3
                Layout.preferredHeight: 20
            }

            Text {
                Layout.fillWidth: true
                text: control.textAt(option.index)
                color: control.currentIndex === option.index || option.highlighted
                       ? Theme.primary : Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 13
                font.weight: control.currentIndex === option.index ? Font.DemiBold : Font.Normal
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            Text {
                visible: control.showSelectionMark && control.currentIndex === option.index
                text: "✓"
                color: Theme.primary
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: Font.Bold
            }
        }

        background: Rectangle {
            radius: Theme.radiusSmall
            color: control.currentIndex === option.index
                   ? Theme.selectedFill
                   : option.highlighted ? Theme.hoverFill : "transparent"
            border.width: control.currentIndex === option.index ? 1 : 0
            border.color: Theme.alpha(Theme.primary, 0.18)

            Behavior on color { ColorAnimation { duration: Theme.durationFast } }
        }
    }

    popup: Popup {
        id: comboBoxPopup
        y: control.height + 4
        width: control.width
        // 打开时按"最长选项文字 + 边距"一次性加宽（命令式计算，无绑定副作用），
        // 保证下拉列表完整显示不截断。
        onAboutToShow: {
            control.aboutToShowPopup()
            let widest = control.width
            for (let i = 0; i < control.count; ++i) {
                itemMetrics.text = control.textAt(i)
                widest = Math.max(widest, itemMetrics.width + 64)
            }
            comboBoxPopup.width = Math.min(Math.round(widest), 520)
        }
        implicitHeight: Math.min(contentItem.implicitHeight + 12, 340)
        padding: 6
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            boundsBehavior: Flickable.StopAtBounds
            spacing: 2

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: Theme.surface
            radius: Theme.radius
            border.width: 1
            border.color: Theme.border

            Behavior on color { ColorAnimation { duration: Theme.durationNormal } }
        }
    }
}
