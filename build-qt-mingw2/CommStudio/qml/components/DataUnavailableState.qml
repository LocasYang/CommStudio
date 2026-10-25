import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0

// Generic empty/unavailable state (no project loaded, no .map file, ...).
Item {
    id: root

    property string symbol: "×"
    property color accent: Theme.textTertiary
    property string title: "暂无数据"
    property string hint: ""

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 10

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            radius: 24
            color: Theme.alpha(root.accent, Theme.dark ? 0.14 : 0.09)
            border.width: 1
            border.color: Theme.alpha(root.accent, 0.24)

            Text {
                anchors.centerIn: parent
                text: root.symbol
                color: root.accent
                font.family: Theme.fontFamily
                font.pixelSize: 21
                font.weight: Font.DemiBold
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.title
            color: Theme.textPrimary
            font.family: Theme.fontFamily
            font.pixelSize: 16
            font.weight: Font.DemiBold
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            visible: root.hint.length > 0
            text: root.hint
            color: Theme.textTertiary
            font.family: Theme.fontFamily
            font.pixelSize: 12
        }
    }
}
