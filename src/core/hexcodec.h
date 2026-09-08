#pragma once

#include <QByteArray>
#include <QString>

// HEX / 文本编解码：
//  - 宽松解析用户输入的 HEX（空格 / 逗号 / 换行 / 0x 前缀均可）；
//  - 发送文本的转义序列（\r \n \t \0 \xNN \\）；
//  - 行结尾追加（无 / CR / LF / CRLF）。
class HexCodec
{
public:
    // 宽松 HEX 解析，解析失败返回 false 并给出错误说明。
    static bool parseHex(const QString &text, QByteArray &out, QString *error = nullptr);
    static QString toHex(const QByteArray &data);
    static QString toHexCompact(const QByteArray &data);
    static QByteArray fromHexCompact(const QString &hex);

    // 处理转义序列；仅当 escapeEnabled 为真时展开，否则原样 UTF-8 输出。
    static QByteArray encodeText(const QString &text, bool escapeEnabled);

    // 组装最终发送载荷：文本或 HEX + 可选转义 + 行结尾后缀。
    // lineEnding: none / cr / lf / crlf
    static QByteArray buildPayload(const QString &text, bool hexMode, bool escapeEnabled,
                                   const QString &lineEnding, QString *error = nullptr);

    static QByteArray lineEndingBytes(const QString &lineEnding);
};
