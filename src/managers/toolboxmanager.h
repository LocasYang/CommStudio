#pragma once

#include <QObject>
#include <QVariantMap>

// 工具箱后端：校验计算器（Sum/XOR/CRC 系列）、HEX ↔ 数值（多类型多字节序）、
// HEX ↔ 文本（UTF-8）、Base64、Bit 视图。
class ToolboxManager : public QObject
{
    Q_OBJECT

public:
    explicit ToolboxManager(QObject *parent = nullptr);

    // 输入 HEX 字符串 → 各校验值（键与 ChecksumEngine::computeAll 一致）。
    Q_INVOKABLE QVariantMap checksums(const QString &hexInput);

    // 将输入按 HEX 解析后给出全部数值/文本解释。
    Q_INVOKABLE QVariantMap analyzeHex(const QString &hexInput);

    // 数值编码：value + 类型（uint16/int16/uint32/int32/float32）+ 字节序
    // （little/big 或 ABCD/BADC/CDAB/DCBA）→ HEX。
    Q_INVOKABLE QVariantMap encodeNumber(double value, const QString &type,
                                         const QString &endianness) const;

    // 文本 ↔ HEX（encoding: utf8）。
    Q_INVOKABLE QString textToHex(const QString &text) const;
    Q_INVOKABLE QString hexToText(const QString &hexInput) const;
};
