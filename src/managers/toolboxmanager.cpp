#include "toolboxmanager.h"

#include "core/checksumengine.h"
#include "core/hexcodec.h"
#include "core/modbuscodec.h"

#include <cmath>
#include <cstring>

namespace {

QString formatDouble(double value)
{
    if (std::isnan(value))
        return QStringLiteral("NaN");
    if (std::isinf(value))
        return value > 0 ? QStringLiteral("+Inf") : QStringLiteral("-Inf");
    return QString::number(value, 'f', 6);
}

} // namespace

ToolboxManager::ToolboxManager(QObject *parent)
    : QObject(parent)
{
}

QVariantMap ToolboxManager::checksums(const QString &hexInput)
{
    QByteArray data;
    QString error;
    if (!HexCodec::parseHex(hexInput, data, &error)) {
        return QVariantMap{ { QStringLiteral("error"), error } };
    }
    if (data.isEmpty())
        return QVariantMap{ { QStringLiteral("error"), QStringLiteral("输入为空") } };
    return ChecksumEngine::computeAll(data);
}

QVariantMap ToolboxManager::analyzeHex(const QString &hexInput)
{
    QByteArray data;
    QString error;
    if (!HexCodec::parseHex(hexInput, data, &error)) {
        return QVariantMap{ { QStringLiteral("error"), error } };
    }
    if (data.isEmpty())
        return QVariantMap{ { QStringLiteral("error"), QStringLiteral("输入为空") } };

    QVariantMap result;
    result.insert(QStringLiteral("byteCount"), data.size());
    result.insert(QStringLiteral("hex"), HexCodec::toHex(data));

    auto readU16 = [&data](int offset) -> quint16 {
        if (offset + 1 >= data.size())
            return 0;
        return quint16(quint8(data.at(offset)) << 8 | quint8(data.at(offset + 1)));
    };
    auto readU32 = [&data](int offset) -> quint32 {
        quint32 value = 0;
        for (int i = 0; i < 4 && offset + i < data.size(); ++i)
            value = (value << 8) | quint8(data.at(offset + i));
        return value;
    };

    if (data.size() >= 2) {
        const quint16 be = readU16(0);
        quint16 le = 0;
        le = quint16(quint8(data.at(1)) << 8 | quint8(data.at(0)));
        result.insert(QStringLiteral("uint16be"), be);
        result.insert(QStringLiteral("uint16le"), le);
        result.insert(QStringLiteral("int16be"), short(be));
        result.insert(QStringLiteral("int16le"), short(le));
    }
    if (data.size() >= 4) {
        const quint32 be = readU32(0);
        quint32 le = 0;
        for (int i = 0; i < 4; ++i)
            le = (le << 8) | quint8(data.at(3 - i));
        result.insert(QStringLiteral("uint32be"), be);
        result.insert(QStringLiteral("uint32le"), le);
        result.insert(QStringLiteral("int32be"), int(be));
        result.insert(QStringLiteral("int32le"), int(le));

        float f = 0.0f;
        std::memcpy(&f, &be, sizeof(float));
        result.insert(QStringLiteral("float32be"), formatDouble(f));
        std::memcpy(&f, &le, sizeof(float));
        result.insert(QStringLiteral("float32le"), formatDouble(f));
    }
    result.insert(QStringLiteral("ascii"), QString::fromLatin1(data));
    result.insert(QStringLiteral("utf8"), QString::fromUtf8(data));
    result.insert(QStringLiteral("base64"), QString::fromLatin1(data.toBase64()));

    // Bit 视图：前 4 字节，每字节一行（高位在左）。
    QStringList bitRows;
    for (int i = 0; i < qMin(4, data.size()); ++i) {
        const quint8 byte = quint8(data.at(i));
        QString bits;
        for (int bit = 7; bit >= 0; --bit)
            bits += (byte & (1 << bit)) ? QLatin1Char('1') : QLatin1Char('0');
        bitRows << QStringLiteral("%1  %2").arg(byte, 2, 16, QLatin1Char('0')).toUpper().arg(bits);
    }
    result.insert(QStringLiteral("bits"), bitRows);
    return result;
}

QVariantMap ToolboxManager::encodeNumber(double value, const QString &type,
                                         const QString &endianness) const
{
    QVariantMap result;
    if (type == QLatin1String("uint16") || type == QLatin1String("int16")) {
        qint16 raw = 0;
        if (type == QLatin1String("uint16"))
            raw = qint16(qBound(0.0, value, 65535.0));
        else
            raw = qint16(qBound(-32768.0, value, 32767.0));
        const quint16 word = quint16(raw);
        if (endianness == QLatin1String("little"))
            result.insert(QStringLiteral("hex"),
                          ChecksumEngine::byteHex(quint8(word & 0xFF)) + QLatin1Char(' ')
                              + ChecksumEngine::byteHex(quint8(word >> 8)));
        else
            result.insert(QStringLiteral("hex"),
                          ChecksumEngine::byteHex(quint8(word >> 8)) + QLatin1Char(' ')
                              + ChecksumEngine::byteHex(quint8(word & 0xFF)));
        result.insert(QStringLiteral("raw"), raw);
        return result;
    }

    // 32 位类型
    if (type == QLatin1String("float32")) {
        const float f = float(value);
        quint32 raw = 0;
        std::memcpy(&raw, &f, sizeof(quint32));
        const auto b = [&raw](int shift) {
            return ChecksumEngine::byteHex(quint8((raw >> shift) & 0xFF));
        };
        if (endianness == QLatin1String("ABCD") || endianness == QLatin1String("big"))
            result.insert(QStringLiteral("hex"), b(24) + QLatin1Char(' ') + b(16) + QLatin1Char(' ')
                                              + b(8) + QLatin1Char(' ') + b(0));
        else if (endianness == QLatin1String("DCBA") || endianness == QLatin1String("little"))
            result.insert(QStringLiteral("hex"), b(0) + QLatin1Char(' ') + b(8) + QLatin1Char(' ')
                                              + b(16) + QLatin1Char(' ') + b(24));
        else if (endianness == QLatin1String("BADC"))
            result.insert(QStringLiteral("hex"), b(16) + QLatin1Char(' ') + b(24) + QLatin1Char(' ')
                                              + b(0) + QLatin1Char(' ') + b(8));
        else // CDAB
            result.insert(QStringLiteral("hex"), b(8) + QLatin1Char(' ') + b(24) + QLatin1Char(' ')
                                              + b(0) + QLatin1Char(' ') + b(16));
        result.insert(QStringLiteral("raw"), qRound64(value));
        result.insert(QStringLiteral("ieee754"), ChecksumEngine::dwordHex(raw));
        return result;
    }

    qint32 raw = 0;
    if (type == QLatin1String("uint32"))
        raw = int(qBound(0.0, value, 4294967295.0));
    else
        raw = int(qBound(-2147483648.0, value, 2147483647.0));
    const QByteArray bytes = ModbusCodec::wordsToBytes32(quint16((raw >> 16) & 0xFFFF),
                                                         quint16(raw & 0xFFFF),
                                                         endianness == QLatin1String("little")
                                                             ? QStringLiteral("DCBA")
                                                             : QStringLiteral("ABCD"));
    result.insert(QStringLiteral("hex"), HexCodec::toHex(bytes));
    result.insert(QStringLiteral("raw"), raw);
    return result;
}

QString ToolboxManager::textToHex(const QString &text) const
{
    return HexCodec::toHex(text.toUtf8());
}

QString ToolboxManager::hexToText(const QString &hexInput) const
{
    QByteArray data;
    QString error;
    if (!HexCodec::parseHex(hexInput, data, &error))
        return QStringLiteral("错误: %1").arg(error);
    return QString::fromUtf8(data);
}
