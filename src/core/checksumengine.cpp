#include "checksumengine.h"

#include <QVariantMap>

quint8 ChecksumEngine::sum8(const QByteArray &data)
{
    quint32 sum = 0;
    for (char byte : data)
        sum += static_cast<quint8>(byte);
    return static_cast<quint8>(sum & 0xFF);
}

quint16 ChecksumEngine::sum16(const QByteArray &data)
{
    quint32 sum = 0;
    for (char byte : data)
        sum += static_cast<quint8>(byte);
    return static_cast<quint16>(sum & 0xFFFF);
}

quint8 ChecksumEngine::xor8(const QByteArray &data)
{
    quint8 value = 0;
    for (char byte : data)
        value ^= static_cast<quint8>(byte);
    return value;
}

quint8 ChecksumEngine::crc8(const QByteArray &data)
{
    // CRC-8 (poly 0x07, init 0x00), 常见 ATM/传感器校验。
    quint8 crc = 0x00;
    for (char byte : data) {
        crc ^= static_cast<quint8>(byte);
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc & 0x80) ? static_cast<quint8>((crc << 1) ^ 0x07)
                               : static_cast<quint8>(crc << 1);
    }
    return crc;
}

quint16 ChecksumEngine::crc16Modbus(const QByteArray &data)
{
    // CRC-16/MODBUS (poly 0xA001 反射, init 0xFFFF), 低字节在前追加。
    quint16 crc = 0xFFFF;
    for (char byte : data) {
        crc ^= static_cast<quint8>(byte);
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x0001)
                crc = static_cast<quint16>((crc >> 1) ^ 0xA001);
            else
                crc = static_cast<quint16>(crc >> 1);
        }
    }
    return crc;
}

quint16 ChecksumEngine::crc16Ccitt(const QByteArray &data, quint16 init)
{
    // CRC-16/CCITT-FALSE (poly 0x1021, init 默认 0xFFFF)。
    quint16 crc = init;
    for (char byte : data) {
        crc ^= static_cast<quint16>(static_cast<quint8>(byte) << 8);
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc & 0x8000) ? static_cast<quint16>((crc << 1) ^ 0x1021)
                                 : static_cast<quint16>(crc << 1);
    }
    return crc;
}

quint32 ChecksumEngine::crc32(const QByteArray &data)
{
    // CRC-32/ISO-HDLC (poly 0xEDB88320 反射, init 0xFFFFFFFF, 终异或 0xFFFFFFFF)。
    static quint32 table[256];
    static bool tableReady = false;
    if (!tableReady) {
        for (quint32 i = 0; i < 256; ++i) {
            quint32 value = i;
            for (int bit = 0; bit < 8; ++bit)
                value = (value & 1) ? (0xEDB88320u ^ (value >> 1)) : (value >> 1);
            table[i] = value;
        }
        tableReady = true;
    }
    quint32 crc = 0xFFFFFFFFu;
    for (char byte : data)
        crc = table[(crc ^ static_cast<quint8>(byte)) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}

QString ChecksumEngine::byteHex(quint8 value)
{
    return QStringLiteral("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
}

QString ChecksumEngine::wordHex(quint16 value)
{
    return QStringLiteral("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}

QString ChecksumEngine::wordHexLe(quint16 value)
{
    return byteHex(static_cast<quint8>(value >> 8)) + QLatin1Char(' ')
           + byteHex(static_cast<quint8>(value & 0xFF));
}

QString ChecksumEngine::dwordHex(quint32 value)
{
    return QStringLiteral("%1").arg(value, 8, 16, QLatin1Char('0')).toUpper();
}

QString ChecksumEngine::dwordHexLe(quint32 value)
{
    return wordHex(static_cast<quint16>(value & 0xFFFF)) + QLatin1Char(' ')
           + wordHex(static_cast<quint16>(value >> 16));
}

QVariantMap ChecksumEngine::computeAll(const QByteArray &data)
{
    QVariantMap result;
    if (data.isEmpty())
        return result;
    const quint16 sum = sum16(data);
    const quint16 crcModbus = crc16Modbus(data);
    const quint16 crcCcitt = crc16Ccitt(data);
    const quint32 crcWide = crc32(data);
    result.insert(QStringLiteral("sum8"), byteHex(sum8(data)));
    result.insert(QStringLiteral("sum16be"), wordHex(sum));
    result.insert(QStringLiteral("sum16le"), wordHexLe(sum));
    result.insert(QStringLiteral("xor"), byteHex(xor8(data)));
    result.insert(QStringLiteral("crc8"), byteHex(crc8(data)));
    result.insert(QStringLiteral("crc16modbusle"), wordHexLe(crcModbus));
    result.insert(QStringLiteral("crc16modbusbe"), wordHex(crcModbus));
    result.insert(QStringLiteral("crc16ccittle"), wordHexLe(crcCcitt));
    result.insert(QStringLiteral("crc16ccittbe"), wordHex(crcCcitt));
    result.insert(QStringLiteral("crc32be"), dwordHex(crcWide));
    result.insert(QStringLiteral("crc32le"), dwordHexLe(crcWide));
    return result;
}
