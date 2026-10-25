#include "modbuscodec.h"

#include "checksumengine.h"

#include <cstring>
#include <cmath>
#include <algorithm>
#include <limits>

namespace {

void appendWord(QByteArray &out, quint16 value)
{
    out.append(static_cast<char>(value >> 8));
    out.append(static_cast<char>(value & 0xFF));
}

void appendByte(QByteArray &out, quint8 value)
{
    out.append(static_cast<char>(value));
}

quint16 readWord(const QByteArray &data, int offset)
{
    return static_cast<quint8>(data.at(offset)) << 8
           | static_cast<quint8>(data.at(offset + 1));
}

} // namespace

QByteArray ModbusCodec::buildPdu(quint8 functionCode, quint16 address,
                                 const QVector<quint16> &values)
{
    QByteArray pdu;
    appendByte(pdu, functionCode);
    appendWord(pdu, address);

    switch (functionCode) {
    case FcReadCoils:
    case FcReadDiscreteInputs:
    case FcReadHoldingRegisters:
    case FcReadInputRegisters: {
        const quint16 count = values.isEmpty() ? 0 : values.first();
        appendWord(pdu, count);
        break;
    }
    case FcWriteSingleCoil: {
        appendWord(pdu, values.isEmpty() ? 0 : (values.first() ? 0xFF00 : 0x0000));
        break;
    }
    case FcWriteSingleRegister: {
        appendWord(pdu, values.isEmpty() ? 0 : values.first());
        break;
    }
    case FcWriteMultipleCoils: {
        const quint16 count = values.size();
        appendWord(pdu, count);
        appendByte(pdu, static_cast<quint8>((count + 7) / 8));
        quint8 byte = 0;
        int bitIndex = 0;
        for (quint16 value : values) {
            if (value)
                byte |= static_cast<quint8>(1 << bitIndex);
            if (++bitIndex == 8) {
                appendByte(pdu, byte);
                byte = 0;
                bitIndex = 0;
            }
        }
        if (bitIndex > 0)
            appendByte(pdu, byte);
        break;
    }
    case FcWriteMultipleRegisters: {
        appendWord(pdu, static_cast<quint16>(values.size()));
        appendByte(pdu, static_cast<quint8>(values.size() * 2));
        for (quint16 value : values)
            appendWord(pdu, value);
        break;
    }
    default:
        break; // 其余功能码由用户在 Raw 模式手工构造 PDU。
    }
    return pdu;
}

QByteArray ModbusCodec::buildRtuAdu(quint8 slaveAddress, const QByteArray &pdu)
{
    QByteArray adu;
    appendByte(adu, slaveAddress);
    adu += pdu;
    const quint16 crc = ChecksumEngine::crc16Modbus(adu);
    appendByte(adu, static_cast<quint8>(crc & 0xFF));
    appendByte(adu, static_cast<quint8>(crc >> 8));
    return adu;
}

QByteArray ModbusCodec::buildAsciiAdu(quint8 slaveAddress, const QByteArray &pdu)
{
    QByteArray binary;
    appendByte(binary, slaveAddress);
    binary += pdu;
    quint8 sum = 0;
    for (const char byte : binary) sum = quint8(sum + quint8(byte));
    const quint8 lrc = quint8(0u - sum);
    const QByteArray hex = binary.toHex().toUpper() + QByteArray::number(lrc, 16).rightJustified(2, '0').toUpper();
    return QByteArray(":") + hex + QByteArray("\r\n");
}

QByteArray ModbusCodec::buildTcpAdu(quint16 transactionId, quint8 unitId, const QByteArray &pdu)
{
    QByteArray adu;
    appendWord(adu, transactionId);
    appendWord(adu, 0); // 协议标识符
    appendWord(adu, static_cast<quint16>(pdu.size() + 1));
    appendByte(adu, unitId);
    adu += pdu;
    return adu;
}

ModbusCodec::ParsedResponse ModbusCodec::parseResponse(const QByteArray &adu, bool isTcp)
{
    ParsedResponse parsed;
    if (isTcp) {
        if (adu.size() < 8) {
            parsed.error = QStringLiteral("TCP ADU 长度不足（MBAP 至少 8 字节）");
            return parsed;
        }
        parsed.transactionId = readWord(adu, 0);
        parsed.protocolId = readWord(adu, 2);
        const quint16 length = readWord(adu, 4);
        parsed.slave = static_cast<quint8>(adu.at(6));
        parsed.functionCode = static_cast<quint8>(adu.at(7));
        const int pduLength = qMin<int>(length - 1, adu.size() - 7);
        const QByteArray pdu = adu.mid(7, pduLength);
        parsed.crcOk = true;
        if (parsed.functionCode & 0x80) {
            parsed.exception = true;
            parsed.exceptionCode = pdu.size() > 1 ? static_cast<quint8>(pdu.at(1)) : 0;
        } else if (pdu.size() > 2
                   && (parsed.functionCode <= FcReadInputRegisters)) {
            const quint8 byteCount = static_cast<quint8>(pdu.at(1));
            for (int i = 0; i + 1 < byteCount && 2 + i + 1 < pdu.size(); i += 2)
                parsed.words.append(readWord(pdu, 2 + i));
        }
        parsed.ok = true;
        return parsed;
    }

    if (adu.size() < 4) {
        parsed.error = QStringLiteral("RTU ADU 长度不足（至少 4 字节）");
        return parsed;
    }
    parsed.slave = static_cast<quint8>(adu.at(0));
    parsed.functionCode = static_cast<quint8>(adu.at(1));
    const QByteArray payload = adu.left(adu.size() - 2);
    if (adu.size() >= 2) {
        const quint16 crcExpected = static_cast<quint8>(adu.at(adu.size() - 2))
                                    | (static_cast<quint8>(adu.at(adu.size() - 1)) << 8);
        parsed.crcOk = ChecksumEngine::crc16Modbus(payload) == crcExpected;
    }
    if (parsed.functionCode & 0x80) {
        parsed.exception = true;
        if (payload.size() >= 3)
            parsed.exceptionCode = static_cast<quint8>(payload.at(2));
    } else if (parsed.functionCode <= FcReadInputRegisters && payload.size() >= 3) {
        const quint8 byteCount = static_cast<quint8>(payload.at(2));
        for (int i = 0; i + 1 < byteCount && 3 + i + 1 < payload.size(); i += 2)
            parsed.words.append(readWord(payload, 3 + i));
    }
    parsed.ok = true;
    return parsed;
}

ModbusCodec::ParsedResponse ModbusCodec::parseAsciiResponse(const QByteArray &frame)
{
    ParsedResponse parsed;
    QByteArray text = frame.trimmed();
    if (text.size() < 7 || text.at(0) != ':') {
        parsed.error = QStringLiteral("ASCII 帧格式无效");
        return parsed;
    }
    text.remove(0, 1);
    if (text.size() % 2 != 0) {
        parsed.error = QStringLiteral("ASCII 十六进制长度必须为偶数");
        return parsed;
    }
    QByteArray binary = QByteArray::fromHex(text);
    if (binary.size() < 3) {
        parsed.error = QStringLiteral("ASCII 帧长度不足");
        return parsed;
    }
    quint8 sum = 0;
    for (const char byte : binary) sum = quint8(sum + quint8(byte));
    parsed.crcOk = sum == 0;
    parsed.slave = quint8(binary.at(0));
    parsed.functionCode = quint8(binary.at(1));
    const QByteArray payload = binary.left(binary.size() - 1);
    if (parsed.functionCode & 0x80) {
        parsed.exception = true;
        parsed.exceptionCode = payload.size() > 2 ? quint8(payload.at(2)) : 0;
    } else if (parsed.functionCode <= FcReadInputRegisters && payload.size() >= 3) {
        const quint8 byteCount = quint8(payload.at(2));
        for (int i = 0; i + 1 < byteCount && 3 + i + 1 < payload.size(); i += 2)
            parsed.words.append(readWord(payload, 3 + i));
    }
    parsed.ok = parsed.crcOk;
    if (!parsed.crcOk) parsed.error = QStringLiteral("ASCII LRC 校验失败");
    return parsed;
}

QString ModbusCodec::exceptionText(quint8 code)
{
    switch (code) {
    case 0x01: return QStringLiteral("01 非法功能 (Illegal Function)");
    case 0x02: return QStringLiteral("02 非法数据地址 (Illegal Data Address)");
    case 0x03: return QStringLiteral("03 非法数据值 (Illegal Data Value)");
    case 0x04: return QStringLiteral("04 从站设备故障 (Server Device Failure)");
    case 0x05: return QStringLiteral("05 已接收 (Acknowledge)");
    case 0x06: return QStringLiteral("06 从站设备忙 (Server Device Busy)");
    case 0x08: return QStringLiteral("08 存储奇偶校验错误 (Memory Parity Error)");
    case 0x0A: return QStringLiteral("0A 网关路径不可用 (Gateway Path Unavailable)");
    case 0x0B: return QStringLiteral("0B 网关目标无响应 (Gateway Target Failed to Respond)");
    default: return QStringLiteral("%1 未知异常码").arg(code, 2, 16, QLatin1Char('0')).toUpper();
    }
}

QByteArray ModbusCodec::wordsToBytes32(quint16 word0, quint16 word1, const QString &wordOrder)
{
    // word0 是低地址寄存器。AB/CD 分别为两个寄存器的高低字节。
    const quint8 a = static_cast<quint8>(word0 >> 8);
    const quint8 b = static_cast<quint8>(word0 & 0xFF);
    const quint8 c = static_cast<quint8>(word1 >> 8);
    const quint8 d = static_cast<quint8>(word1 & 0xFF);
    QByteArray bytes(4, Qt::Uninitialized);
    if (wordOrder == QLatin1String("BADC")) {        // B A D C
        bytes[0] = static_cast<char>(b);
        bytes[1] = static_cast<char>(a);
        bytes[2] = static_cast<char>(d);
        bytes[3] = static_cast<char>(c);
    } else if (wordOrder == QLatin1String("CDAB")) { // C D A B
        bytes[0] = static_cast<char>(c);
        bytes[1] = static_cast<char>(d);
        bytes[2] = static_cast<char>(a);
        bytes[3] = static_cast<char>(b);
    } else if (wordOrder == QLatin1String("DCBA")) { // D C B A
        bytes[0] = static_cast<char>(d);
        bytes[1] = static_cast<char>(c);
        bytes[2] = static_cast<char>(b);
        bytes[3] = static_cast<char>(a);
    } else {                                          // ABCD（大端）
        bytes[0] = static_cast<char>(a);
        bytes[1] = static_cast<char>(b);
        bytes[2] = static_cast<char>(c);
        bytes[3] = static_cast<char>(d);
    }
    return bytes;
}

float ModbusCodec::bytesToFloat32(const QByteArray &bytes, const QString &wordOrder)
{
    const QByteArray ordered = wordsToBytes32(
        static_cast<quint8>(bytes.at(0)) << 8 | static_cast<quint8>(bytes.at(1)),
        static_cast<quint8>(bytes.at(2)) << 8 | static_cast<quint8>(bytes.at(3)),
        wordOrder);
    // wordsToBytes32 输出为大端（ABCD 规范序）字节，手工按位组装避免端序歧义。
    const quint32 bits = (static_cast<quint32>(static_cast<quint8>(ordered.at(0))) << 24)
                         | (static_cast<quint32>(static_cast<quint8>(ordered.at(1))) << 16)
                         | (static_cast<quint32>(static_cast<quint8>(ordered.at(2))) << 8)
                         | static_cast<quint32>(static_cast<quint8>(ordered.at(3)));
    float value = 0.0f;
    std::memcpy(&value, &bits, sizeof(float));
    return value;
}

qint32 ModbusCodec::bytesToInt32(const QByteArray &bytes, const QString &wordOrder)
{
    const QByteArray ordered = wordOrder == QLatin1String("ABCD")
                                   ? bytes : wordsToBytes32(
                                         static_cast<quint8>(bytes.at(0)) << 8 | static_cast<quint8>(bytes.at(1)),
                                         static_cast<quint8>(bytes.at(2)) << 8 | static_cast<quint8>(bytes.at(3)),
                                         wordOrder);
    return (static_cast<qint32>(static_cast<quint8>(ordered.at(0))) << 24)
           | (static_cast<qint32>(static_cast<quint8>(ordered.at(1))) << 16)
           | (static_cast<qint32>(static_cast<quint8>(ordered.at(2))) << 8)
           | static_cast<qint32>(static_cast<quint8>(ordered.at(3)));
}

bool ModbusCodec::encodePointValue(const QVariantMap &point, const QString &text,
                                   QVector<quint16> &words, QString &error)
{
    words.clear(); error.clear();
    const auto fail = [&error](const QString &message) { error = message; return false; };
    const QString type = point.value("type", "uint16").toString();
    const int count = point.value("count", 1).toInt();
    const int fc = point.value("fc", 3).toInt();
    const QString order = point.value("wordOrder", "ABCD").toString();
    if ((fc != 1 && fc != 3) || count < 1 || count > (fc == 1 ? 1 : 123)
        || !QStringList{"ABCD", "BADC", "CDAB", "DCBA"}.contains(order))
        return fail(QStringLiteral("数据区、数量或字节序不支持写入"));
    const int bitCount = point.value("bitCount", 0).toInt();
    if (fc == 3 && (point.value("bitOffset", 0).toInt() != 0
                    || (bitCount != 0 && bitCount != count * 16)))
        return fail(QStringLiteral("暂不支持部分寄存器位写入，已阻止覆盖其他位"));

    QByteArray bytes;
    if (type == "string" || type == "bytes") {
        if (fc != 3) return fail(QStringLiteral("线圈仅支持 0 或 1"));
        if (type == "string") bytes = text.toUtf8();
        else {
            QString hex = text.simplified().remove(' ');
            if (hex.startsWith("0x", Qt::CaseInsensitive)) hex.remove(0, 2);
            if (hex.isEmpty() || hex.size() % 2 != 0
                || std::any_of(hex.cbegin(), hex.cend(), [](QChar c) {
                    return !QStringLiteral("0123456789abcdefABCDEF").contains(c);
                })) return fail(QStringLiteral("字节数据需要完整的十六进制字节"));
            bytes = QByteArray::fromHex(hex.toLatin1());
        }
        if (bytes.size() > count * 2 || (type == "bytes" && bytes.size() != count * 2))
            return fail(QStringLiteral("输入长度与数据点数量不匹配"));
        bytes.append(QByteArray(count * 2 - bytes.size(), '\0'));
    } else {
        const int width = type.endsWith("64") ? 4 : type.endsWith("32") ? 2 : 1;
        const bool bcd = type == "bcd";
        const bool floating = type == "float32" || type == "float64";
        const bool signedType = type.startsWith("int");
        if (!QStringList{"bit", "uint16", "int16", "uint32", "int32", "uint64", "int64", "float32", "float64", "bcd"}.contains(type)
            || (bcd ? count > 4 : count != width))
            return fail(QStringLiteral("数据类型与数量不匹配，不能安全编码写入值"));
        const double scale = point.value("scale", 1).toDouble();
        const double bias = point.value("offset", 0).toDouble();
        if (!std::isfinite(scale) || scale == 0 || !std::isfinite(bias))
            return fail(QStringLiteral("比例系数不能为 0，系数和偏移必须为有限数值"));
        const QString input = text.trimmed();
        bool ok = false;
        quint64 raw = 0;
        // Preserve exact 64-bit integers instead of passing them through double.
        if (!floating && scale == 1 && bias == 0) {
            const int base = input.startsWith("0x", Qt::CaseInsensitive) ? 16 : 10;
            if (signedType) raw = quint64(input.toLongLong(&ok, base));
            else if (!input.startsWith('-')) raw = input.toULongLong(&ok, base);
            if (!ok) return fail(QStringLiteral("请输入该数据类型可表示的整数"));
        } else {
            const double value = input.toDouble(&ok);
            const double unscaled = (value - bias) / scale;
            if (!ok || !std::isfinite(value) || !std::isfinite(unscaled))
                return fail(QStringLiteral("写入值无效或超出数值范围"));
            if (type == "float32") {
                const float f = float(unscaled);
                if (!std::isfinite(f)) return fail(QStringLiteral("写入值超出 FLOAT32 范围"));
                quint32 bits = 0; std::memcpy(&bits, &f, sizeof(f)); raw = bits;
            } else if (type == "float64") std::memcpy(&raw, &unscaled, sizeof(raw));
            else {
                const double rounded = std::round(unscaled);
                if (signedType) {
                    if (rounded < -std::ldexp(1.0, 63) || rounded >= std::ldexp(1.0, 63))
                        return fail(QStringLiteral("写入值超出有符号整数范围"));
                    raw = quint64(qint64(rounded));
                } else {
                    if (rounded < 0 || rounded >= std::ldexp(1.0, 64))
                        return fail(QStringLiteral("写入值超出无符号整数范围"));
                    raw = quint64(rounded);
                }
            }
        }
        if (fc == 1 && (type != "bit" || raw > 1))
            return fail(QStringLiteral("线圈仅支持 0 或 1"));
        if (!floating && !bcd && width < 4) {
            const int bits = width * 16;
            const qint64 signedRaw = qint64(raw);
            if (signedType ? (signedRaw < -(qint64(1) << (bits - 1)) || signedRaw >= (qint64(1) << (bits - 1)))
                           : raw >= (quint64(1) << bits))
                return fail(QStringLiteral("写入值超出数据类型范围"));
        }
        if (bcd) {
            const QString digits = QString::number(raw);
            if (digits.size() > count * 4) return fail(QStringLiteral("写入值超出 BCD 容量"));
            raw = digits.toULongLong(&ok, 16);
            if (!ok) return fail(QStringLiteral("BCD 编码失败"));
        }
        if (fc == 1) { words.append(quint16(raw)); return true; }
        for (int i = count * 2 - 1; i >= 0; --i) bytes.append(char(raw >> (i * 8)));
    }
    QVector<quint16> encoded;
    for (int i = 0; i < bytes.size(); i += 2) {
        const quint8 a = quint8(bytes[i]), b = quint8(bytes[i + 1]);
        encoded.append(order == "BADC" || order == "DCBA" ? quint16(b << 8 | a) : quint16(a << 8 | b));
    }
    if (order == "CDAB" || order == "DCBA") std::reverse(encoded.begin(), encoded.end());
    words = encoded;
    return true;
}

QVariantMap ModbusCodec::decodeRegisters(const QVector<quint16> &registers, int offset,
                                         const QString &type, const QString &wordOrder,
                                         double scale, double offsetValue)
{
    QVariantMap row;
    if (offset < 0 || offset >= registers.size())
        return row;

    if (type == QLatin1String("uint16") || type == QLatin1String("int16")
        || type == QLatin1String("hex") || type == QLatin1String("ascii")
        || type == QLatin1String("bool")) {
        const quint16 raw = registers.at(offset);
        row.insert(QStringLiteral("rawHex"), ChecksumEngine::wordHex(raw));
        if (type == QLatin1String("uint16")) {
            row.insert(QStringLiteral("value"), raw * scale + offsetValue);
        } else if (type == QLatin1String("int16")) {
            row.insert(QStringLiteral("value"),
                       static_cast<int>(static_cast<qint16>(raw)) * scale + offsetValue);
        } else if (type == QLatin1String("bool")) {
            row.insert(QStringLiteral("value"), raw != 0);
        } else if (type == QLatin1String("ascii")) {
            const QChar ch(raw & 0xFF);
            row.insert(QStringLiteral("value"), QString(ch));
            row.insert(QStringLiteral("rawHex"), QString::fromLatin1(
                           QByteArray(reinterpret_cast<const char *>(&raw), 2).toHex().toUpper()));
        } else {
            row.insert(QStringLiteral("value"), ChecksumEngine::wordHex(raw));
        }
        return row;
    }

    // 32 位类型：需要两个连续寄存器。
    if (offset + 1 >= registers.size())
        return row;
    const QByteArray bytes = wordsToBytes32(registers.at(offset), registers.at(offset + 1),
                                            wordOrder);
    quint32 rawHex = 0;
    for (int i = 0; i < 4; ++i)
        rawHex = (rawHex << 8) | static_cast<quint8>(bytes.at(i));
    row.insert(QStringLiteral("rawHex"),
               QStringLiteral("%1").arg(rawHex, 8, 16, QLatin1Char('0')).toUpper());
    if (type == QLatin1String("float32")) {
        float value = 0.0f;
        // rawHex is the numeric IEEE-754 bit pattern, independent of host byte order.
        std::memcpy(&value, &rawHex, sizeof(float));
        row.insert(QStringLiteral("value"), static_cast<double>(value) * scale + offsetValue);
    } else if (type == QLatin1String("int32")) {
        row.insert(QStringLiteral("value"),
                   static_cast<double>(bytesToInt32(bytes, QStringLiteral("ABCD"))) * scale
                       + offsetValue);
    } else {
        quint32 raw = 0;
        for (int i = 0; i < 4; ++i)
            raw = (raw << 8) | static_cast<quint8>(bytes.at(i));
        row.insert(QStringLiteral("value"),
                   static_cast<double>(raw) * scale + offsetValue);
    }
    return row;
}
