#pragma once

#include <QByteArray>
#include <QString>
#include <QVariantMap>

// 自研 Modbus 编解码（Raw Test Center / 原始流量显示用）：
//  - RTU ADU 组帧与 CRC；
//  - TCP MBAP 组帧；
//  - 响应帧解析（含异常码识别）；
//  - 32 位字序（ABCD/BADC/CDAB/DCBA）与常用数据类型解码。
// 标准请求仍由 QModbusClient 完成，本类承担"第二引擎"的角色。
class ModbusCodec
{
public:
    static constexpr quint8 FcReadCoils = 0x01;
    static constexpr quint8 FcReadDiscreteInputs = 0x02;
    static constexpr quint8 FcReadHoldingRegisters = 0x03;
    static constexpr quint8 FcReadInputRegisters = 0x04;
    static constexpr quint8 FcWriteSingleCoil = 0x05;
    static constexpr quint8 FcWriteSingleRegister = 0x06;
    static constexpr quint8 FcWriteMultipleCoils = 0x0F;
    static constexpr quint8 FcWriteMultipleRegisters = 0x10;

    // 组标准请求 PDU。写 06/05 时 value 为寄存器值或线圈状态（0/1）；
    // 写 0F/10 时 values 为寄存器值列表（线圈以 0/1 传入）。
    static QByteArray buildPdu(quint8 functionCode, quint16 address,
                               const QVector<quint16> &values = {});
    static QByteArray buildRtuAdu(quint8 slaveAddress, const QByteArray &pdu);
    static QByteArray buildAsciiAdu(quint8 slaveAddress, const QByteArray &pdu);
    static QByteArray buildTcpAdu(quint16 transactionId, quint8 unitId, const QByteArray &pdu);

    struct ParsedResponse {
        bool ok = false;
        bool crcOk = false;
        bool exception = false;
        quint8 slave = 0;
        quint8 functionCode = 0;
        quint8 exceptionCode = 0;
        QVector<quint16> words;   // 响应数据（寄存器或位展开为 0/1）
        quint16 transactionId = 0;
        quint16 protocolId = 0;
        QString error;
    };

    // 解析 RTU 或 TCP 响应 ADU（isTcp 为真时按 MBAP 解析）。
    static ParsedResponse parseResponse(const QByteArray &adu, bool isTcp);
    static ParsedResponse parseAsciiResponse(const QByteArray &frame);
    static QString exceptionText(quint8 code);

    // 32 位字序：ABCD（大端）/ BADC / CDAB / DCBA。
    static QByteArray wordsToBytes32(quint16 word0, quint16 word1, const QString &wordOrder);
    static float bytesToFloat32(const QByteArray &bytes, const QString &wordOrder);
    static qint32 bytesToInt32(const QByteArray &bytes, const QString &wordOrder);

    // 寄存器值 → 数据行。type: uint16/int16/uint32/int32/float32/hex/ascii
    // wordOrder 仅对 32 位有效。
    static QVariantMap decodeRegisters(const QVector<quint16> &registers, int offset,
                                       const QString &type, const QString &wordOrder,
                                       double scale, double offsetValue);
    // Point commands are engineering values, not a list of raw register words.
    // Reject unsupported/partial-register writes rather than corrupt adjacent bits.
    static bool encodePointValue(const QVariantMap &point, const QString &text,
                                 QVector<quint16> &words, QString &error);
};
