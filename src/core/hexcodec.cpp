#include "hexcodec.h"

#include <QChar>

namespace {

int hexDigitValue(QChar c, bool &ok)
{
    ok = true;
    if (c >= QLatin1Char('0') && c <= QLatin1Char('9'))
        return c.toLatin1() - '0';
    if (c >= QLatin1Char('a') && c <= QLatin1Char('f'))
        return c.toLatin1() - 'a' + 10;
    if (c >= QLatin1Char('A') && c <= QLatin1Char('F'))
        return c.toLatin1() - 'A' + 10;
    ok = false;
    return 0;
}

bool isHexDigit(QChar c)
{
    bool ok = false;
    hexDigitValue(c, ok);
    return ok;
}

} // namespace

bool HexCodec::parseHex(const QString &text, QByteArray &out, QString *error)
{
    out.clear();
    QString compact;
    compact.reserve(text.size());
    for (const QChar &c : text) {
        if (c.isSpace() || c == QLatin1Char(',') || c == QLatin1Char(';')
            || c == QLatin1Char('-') || c == QLatin1Char(':'))
            continue;
        compact.append(c);
    }
    // 允许 0x 前缀。
    compact.remove(QStringLiteral("0x"), Qt::CaseInsensitive);

    if (compact.size() % 2 != 0) {
        if (error)
            *error = QStringLiteral("HEX 长度必须为偶数（当前 %1 个字符）").arg(compact.size());
        return false;
    }
    for (int i = 0; i < compact.size(); i += 2) {
        bool hiOk = false;
        bool loOk = false;
        const int hi = hexDigitValue(compact.at(i), hiOk);
        const int lo = hexDigitValue(compact.at(i + 1), loOk);
        if (!hiOk || !loOk) {
            if (error)
                *error = QStringLiteral("存在非法 HEX 字符: %1").arg(compact.mid(i, 2));
            return false;
        }
        out.append(static_cast<char>((hi << 4) | lo));
    }
    return true;
}

QString HexCodec::toHex(const QByteArray &data)
{
    QString text;
    text.reserve(data.size() * 3);
    static const char digits[] = "0123456789ABCDEF";
    for (int i = 0; i < data.size(); ++i) {
        if (i > 0)
            text += QLatin1Char(' ');
        const quint8 byte = static_cast<quint8>(data.at(i));
        text += QLatin1Char(digits[byte >> 4]);
        text += QLatin1Char(digits[byte & 0x0F]);
    }
    return text;
}

QString HexCodec::toHexCompact(const QByteArray &data)
{
    return QString::fromLatin1(data.toHex()).toUpper();
}

QByteArray HexCodec::fromHexCompact(const QString &hex)
{
    return QByteArray::fromHex(hex.toLatin1());
}

QByteArray HexCodec::encodeText(const QString &text, bool escapeEnabled)
{
    if (!escapeEnabled)
        return text.toUtf8();

    QByteArray out;
    out.reserve(text.size());
    for (int i = 0; i < text.size(); ++i) {
        const QChar c = text.at(i);
        if (c != QLatin1Char('\\')) {
            out += text.mid(i, 1).toUtf8();
            continue;
        }
        if (i + 1 >= text.size()) {
            out += '\\';
            break;
        }
        const QChar next = text.at(++i);
        switch (next.toLatin1()) {
        case 'r': out += '\r'; break;
        case 'n': out += '\n'; break;
        case 't': out += '\t'; break;
        case '0': out += '\0'; break;
        case '\\': out += '\\'; break;
        case 'x': case 'X': {
            bool valid = i + 2 < text.size()
                         && isHexDigit(text.at(i + 1)) && isHexDigit(text.at(i + 2));
            if (!valid) {
                out += '\\';
                out += next.toLatin1();
                break;
            }
            bool hiOk = false;
            bool loOk = false;
            const int value = (hexDigitValue(text.at(i + 1), hiOk) << 4)
                              | hexDigitValue(text.at(i + 2), loOk);
            out += static_cast<char>(value);
            i += 2;
            break;
        }
        default:
            out += '\\';
            out += text.mid(i, 1).toUtf8();
            break;
        }
    }
    return out;
}

QByteArray HexCodec::lineEndingBytes(const QString &lineEnding)
{
    if (lineEnding == QLatin1String("cr"))
        return QByteArrayLiteral("\r");
    if (lineEnding == QLatin1String("lf"))
        return QByteArrayLiteral("\n");
    if (lineEnding == QLatin1String("crlf"))
        return QByteArrayLiteral("\r\n");
    if (lineEnding == QLatin1String("lfcr"))
        return QByteArrayLiteral("\n\r");
    return QByteArray();
}

QByteArray HexCodec::buildPayload(const QString &text, bool hexMode, bool escapeEnabled,
                                  const QString &lineEnding, QString *error)
{
    QByteArray payload;
    if (hexMode) {
        if (!parseHex(text, payload, error))
            return QByteArray();
    } else {
        payload = encodeText(text, escapeEnabled);
    }
    payload += lineEndingBytes(lineEnding);
    return payload;
}
