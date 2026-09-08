#pragma once

#include <QByteArray>
#include <QString>
#include <QVariantMap>

// 校验引擎：Sum8/Sum16/XOR/CRC8/CRC16-Modbus/CRC16-CCITT/CRC32。
// 全部输出为十六进制字符串，16/32 位同时给出大端与小端两种字节序，
// 方便"校验计算器"与发送区的自动校验附加直接取用。
class ChecksumEngine
{
public:
    static quint8 sum8(const QByteArray &data);
    static quint16 sum16(const QByteArray &data);
    static quint8 xor8(const QByteArray &data);
    static quint8 crc8(const QByteArray &data);
    static quint16 crc16Modbus(const QByteArray &data);
    static quint16 crc16Ccitt(const QByteArray &data, quint16 init = 0xFFFF);
    static quint32 crc32(const QByteArray &data);

    static QString byteHex(quint8 value);
    static QString wordHex(quint16 value);              // 大端显示 AB CD
    static QString wordHexLe(quint16 value);            // 小端显示 CD AB（Modbus 追加顺序）
    static QString dwordHex(quint32 value);             // 大端显示
    static QString dwordHexLe(quint32 value);           // 小端显示

    // 汇总计算，键：sum8 / sum16be / sum16le / xor / crc8 / crc16modbus /
    // crc16modbusle / crc16modbusbe / crc16ccitt / crc16ccittbe / crc32 / crc32le
    static QVariantMap computeAll(const QByteArray &data);
};
