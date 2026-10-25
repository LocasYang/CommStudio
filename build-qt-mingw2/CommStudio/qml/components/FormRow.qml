import QtQuick
import QtQuick.Layouts
import CommStudio 1.0

// 表单行：左侧固定宽度标签 + 右侧内容（default 属性）。
// 内容槽必须是 Layout：子项的 Layout.fillWidth/implicitWidth 约束依赖它传导，
// 否则控件保持自身隐式宽度、溢出卡片右缘。
RowLayout {
    id: root

    property alias label: labelText.text
    property int labelWidth: 78
    default property alias contentData: contentSlot.data
    property alias contentItem: contentSlot

    spacing: 8

    Text {
        id: labelText
        Layout.preferredWidth: root.labelWidth
        Layout.alignment: Qt.AlignVCenter
        color: Theme.textSecondary
        font.family: Theme.fontFamily
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
    }

    ColumnLayout {
        id: contentSlot
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
        spacing: 2
    }
}
