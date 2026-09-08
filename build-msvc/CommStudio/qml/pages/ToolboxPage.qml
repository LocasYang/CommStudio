import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CommStudio 1.0
import CommStudio.Backend 1.0

Item {
    id: root

    ScrollView {
        id: toolScroll
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            width: toolScroll.contentWidth
            spacing: 12

            Text {
                text: "工具箱"
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.weight: Font.DemiBold
            }

            // -------- 校验计算器 --------
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: checksumColumn.implicitHeight + 36

                ColumnLayout {
                    id: checksumColumn
                    anchors.fill: parent
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "校验计算器"
                            color: Theme.textPrimary
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.DemiBold
                        }

                        Text {
                            text: "Sum8 / Sum16 / XOR / CRC8 / CRC16-Modbus / CRC16-CCITT / CRC32"
                            color: Theme.textTertiary
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                        }

                        Item { Layout.fillWidth: true }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppTextField {
                            id: checksumInput
                            Layout.fillWidth: true
                            mono: true
                            placeholder: "输入 HEX 字节序列，如 01 03 00 00 00 02"
                            onAccepted: checksumOutput.text = root.computeChecksums()
                        }

                        AppButton {
                            text: "计算"
                            variant: "primary"
                            onClicked: checksumOutput.text = root.computeChecksums()
                        }
                    }

                    Text {
                        id: checksumOutput
                        Layout.fillWidth: true
                        text: "输入字节序列后点击「计算」"
                        color: Theme.textSecondary
                        font.family: "Consolas"
                        font.pixelSize: 12
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 1.45
                    }
                }
            }

            // -------- HEX 全解释 --------
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: analyzeColumn.implicitHeight + 36

                ColumnLayout {
                    id: analyzeColumn
                    anchors.fill: parent
                    spacing: 10

                    Text {
                        text: "HEX 解析（多类型 / 多字节序 / 编码）"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppTextField {
                            id: analyzeInput
                            Layout.fillWidth: true
                            mono: true
                            placeholder: "输入 HEX，如 41 45 70 A4"
                            onAccepted: analyzeOutput.text = root.formatAnalyze(ToolboxManager.analyzeHex(analyzeInput.text))
                        }

                        AppButton {
                            text: "解析"
                            variant: "primary"
                            accent: Theme.cyan
                            onClicked: analyzeOutput.text = root.formatAnalyze(ToolboxManager.analyzeHex(analyzeInput.text))
                        }
                    }

                    Text {
                        id: analyzeOutput
                        Layout.fillWidth: true
                        text: "输入 HEX 后点击「解析」"
                        color: Theme.textSecondary
                        font.family: "Consolas"
                        font.pixelSize: 12
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 1.45
                    }
                }
            }

            // -------- 数值编码 --------
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: encodeColumn.implicitHeight + 36

                ColumnLayout {
                    id: encodeColumn
                    anchors.fill: parent
                    spacing: 10

                    Text {
                        text: "数值编码（工程值 → 寄存器 HEX）"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: "数值"
                            color: Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                        }

                        AppTextField {
                            id: encodeValue
                            Layout.preferredWidth: 120
                            mono: true
                            text: "12.34"
                        }

                        AppComboBox {
                            id: encodeType
                            implicitWidth: 110
                            model: ["uint16", "int16", "uint32", "int32", "float32"]
                        }

                        AppComboBox {
                            id: encodeOrder
                            implicitWidth: 100
                            model: encodeType.currentIndex === 4
                                   ? ["ABCD", "BADC", "CDAB", "DCBA"]
                                   : ["big", "little"]
                            Connections {
                                target: encodeType
                                function onCurrentIndexChanged() { encodeOrder.currentIndex = 0 }
                            }
                        }

                        AppButton {
                            text: "编码"
                            variant: "primary"
                            accent: Theme.purple
                            onClicked: {
                                const result = ToolboxManager.encodeNumber(
                                    parseFloat(encodeValue.text) || 0,
                                    encodeType.model[encodeType.currentIndex],
                                    encodeOrder.model[encodeOrder.currentIndex])
                                encodeOutput.text = result.hex
                            }
                        }

                        Text {
                            id: encodeOutput
                            Layout.fillWidth: true
                            text: "—"
                            color: Theme.success
                            font.family: "Consolas"
                            font.pixelSize: 13
                            font.weight: Font.DemiBold
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            // -------- 文本 ↔ HEX --------
            AppCard {
                Layout.fillWidth: true
                padding: 18
                implicitHeight: textColumn.implicitHeight + 36

                ColumnLayout {
                    id: textColumn
                    anchors.fill: parent
                    spacing: 10

                    Text {
                        text: "文本 ↔ HEX（UTF-8）"
                        color: Theme.textPrimary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppTextField {
                            id: textToHexInput
                            Layout.fillWidth: true
                            placeholder: "输入文本"
                        }

                        AppButton {
                            compact: true
                            text: "→ HEX"
                            onClicked: textConvertOutput.text = ToolboxManager.textToHex(textToHexInput.text)
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        AppTextField {
                            id: hexToTextInput
                            Layout.fillWidth: true
                            mono: true
                            placeholder: "输入 HEX，如 E4 BD A0 E5 A5 BD"
                        }

                        AppButton {
                            compact: true
                            text: "→ 文本"
                            accent: Theme.cyan
                            variant: "outlined"
                            onClicked: textConvertOutput.text = ToolboxManager.hexToText(hexToTextInput.text)
                        }
                    }

                    Text {
                        id: textConvertOutput
                        Layout.fillWidth: true
                        text: "转换结果显示在这里"
                        color: Theme.textSecondary
                        font.family: "Consolas"
                        font.pixelSize: 12
                        wrapMode: Text.WrapAnywhere
                    }
                }
            }

            Item { Layout.preferredHeight: 4 }
        }
    }

    function computeChecksums() {
        const result = ToolboxManager.checksums(checksumInput.text)
        if (result.error)
            return "错误: " + result.error
        return "Sum8      = 0x" + result.sum8
             + "\nSum16     = 0x" + result.sum16be + " (大端) / 0x" + result.sum16le + " (小端)"
             + "\nXOR       = 0x" + result.xor
             + "\nCRC8      = 0x" + result.crc8
             + "\nCRC16/Modbus = 0x" + result.crc16modbusbe + " (追加顺序: " + result.crc16modbusle + ")"
             + "\nCRC16/CCITT  = 0x" + result.crc16ccittbe + " (追加顺序: " + result.crc16ccittle + ")"
             + "\nCRC32     = 0x" + result.crc32be + " (大端) / 0x" + result.crc32le + " (小端)"
    }

    function formatAnalyze(result) {
        if (result.error)
            return "错误: " + result.error
        let text = "字节数 " + result.byteCount + " · " + result.hex
        if (result.uint16be !== undefined)
            text += "\nu16: BE " + result.uint16be + " / LE " + result.uint16le
                  + "   ·   s16: BE " + result.int16be + " / LE " + result.int16le
        if (result.uint32be !== undefined)
            text += "\nu32: BE " + result.uint32be + " / LE " + result.uint32le
                  + "   ·   s32: BE " + result.int32be + " / LE " + result.int32le
                  + "   ·   f32: BE " + result.float32be + " / LE " + result.float32le
        text += "\nASCII: " + JSON.stringify(result.ascii)
              + "   ·   UTF-8: " + JSON.stringify(result.utf8)
              + "   ·   Base64: " + result.base64
        if (result.bits && result.bits.length > 0)
            text += "\nBit 视图:\n  " + result.bits.join("\n  ")
        return text
    }
}
