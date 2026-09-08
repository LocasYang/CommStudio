import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    ScrollView {
        id: settingsScroll
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            width: settingsScroll.contentWidth
            spacing: 12

            Text {
                text: "设置"
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.weight: Font.DemiBold
            }

            // 外观
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: appearanceColumn.implicitHeight + 36

                ColumnLayout {
                    id: appearanceColumn
                    anchors.fill: parent
                    spacing: 12

                    Text {
                        text: "外观"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppButton {
                            text: "浅色主题"
                            variant: !Theme.dark ? "primary" : "default"
                            onClicked: Theme.dark = false
                        }

                        AppButton {
                            text: "深色主题"
                            variant: Theme.dark ? "primary" : "default"
                            onClicked: Theme.dark = true
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: "固定终端配色 · RX / TX 语义色"
                            color: Theme.textSecondary
                            font.family: "Consolas"
                            font.pixelSize: 11
                        }
                    }
                }
            }

            // 终端说明
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: terminalColumn.implicitHeight + 36

                ColumnLayout {
                    id: terminalColumn
                    anchors.fill: parent
                    spacing: 10

                    Text {
                        text: "收发终端"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    Repeater {
                        model: [
                            "接收区以 30 ms 批量刷新，高速数据流下 UI 不冻结。",
                            "终端只保留最近 3000 行（环形缓冲），「导出」可保存完整会话文本。",
                            "「暂停」只冻结显示，后台继续接收并统计字节。",
                            "发送区支持转义序列 \\r \\n \\t \\0 \\xNN，可追加 CR/LF/CRLF 行尾。",
                            "上行颜色为蓝色 (TX)，下行颜色为绿色 (RX)，信息行为灰色，异常为红色。"
                        ]

                        delegate: Text {
                            required property var modelData
                            Layout.fillWidth: true
                            text: "·  " + modelData
                            color: Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                            lineHeight: 1.3
                        }
                    }
                }
            }

            // 关于
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: aboutColumn.implicitHeight + 36

                ColumnLayout {
                    id: aboutColumn
                    anchors.fill: parent
                    spacing: 10

                    Text {
                        text: "关于"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 24
                        rowSpacing: 8

                        Text {
                            text: "应用"
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }

                        Text {
                            text: "CommStudio v0.1.0"
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Medium
                        }

                        Text {
                            text: "运行环境"
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Qt 6.6 / Qt Quick · 原生 C++（SerialPort / SerialBus / Bluetooth / Network）"
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Medium
                        }

                        Text {
                            text: "界面语言"
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }

                        Text {
                            text: "设计令牌与 KeilPlugin Studio 保持一致"
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Medium
                        }

                        Text {
                            text: "能力覆盖"
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }

                        Text {
                            text: "串口 / Modbus RTU-TCP / BLE GATT / TCP-UDP · 收发 · 解码 · 轮询 · 模拟 · Raw"
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Medium
                            wrapMode: Text.WordWrap
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "四类助手共享统一收发终端、编码与校验体系；连接方式只是入口，"
                              + "协议、流量、自动化与记录在同一底座上运行。"
                        color: Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        lineHeight: 1.3
                    }
                }
            }

            Item { Layout.preferredHeight: 4 }
        }
    }
}
