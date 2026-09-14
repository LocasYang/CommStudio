#include <QJsonDocument>
#include <QObject>
#include <QTest>

#include "core/checksumengine.h"
#include "core/hexcodec.h"
#include "core/modbuscodec.h"
#include "core/trafficmodel.h"

class CommStudioCoreTest : public QObject
{
    Q_OBJECT

private slots:
    void crc16ModbusVectors();
    void crcOtherVectors();
    void hexCodecParsing();
    void escapeSequences();
    void lineEndings();
    void pausedTrafficFlushesOnResume();
    void modbusRequestFrames();
    void modbusResponseParsing();
    void wordOrder32();
    void trafficModelRingBuffer();

    static QByteArray hex(const char *text) { return QByteArray::fromHex(QByteArray(text)); }
};

void CommStudioCoreTest::crc16ModbusVectors()
{
    // 经典校验向量：01 03 00 00 00 02 → C4 0B（低字节在前）。
    QCOMPARE(ChecksumEngine::crc16Modbus(hex("010300000002")), quint16(0x0BC4));
    // 01 04 02 FF FF 响应体 CRC（B8 80 低字节在前追加）。
    const QByteArray frame = hex("010402FFFF");
    const quint16 crc = ChecksumEngine::crc16Modbus(frame);
    QCOMPARE(crc, quint16(0x80B8));
}

void CommStudioCoreTest::crcOtherVectors()
{
    // CRC-16/CCITT-FALSE("123456789") = 0x29B1。
    QCOMPARE(ChecksumEngine::crc16Ccitt(QByteArray("123456789")), quint16(0x29B1));
    // CRC-32("123456789") = 0xCBF43926。
    QCOMPARE(ChecksumEngine::crc32(QByteArray("123456789")), quint32(0xCBF43926));
    QCOMPARE(ChecksumEngine::sum8(hex("0102")), quint8(0x03));
    QCOMPARE(ChecksumEngine::xor8(hex("0103")), quint8(0x02));
}

void CommStudioCoreTest::hexCodecParsing()
{
    QByteArray out;
    QString error;
    QVERIFY(HexCodec::parseHex("AA 55 0x1B,2c", out, &error));
    QCOMPARE(out, hex("AA551B2C"));
    QVERIFY(!HexCodec::parseHex("AA 5", out, &error));
    QVERIFY(!HexCodec::parseHex("AG", out, &error));
}

void CommStudioCoreTest::escapeSequences()
{
    QCOMPARE(HexCodec::encodeText(QStringLiteral("A\\r\\n\\x41"), true),
             hex("410D0A41"));
    QCOMPARE(HexCodec::encodeText(QStringLiteral("\\x00\\t\\\\"), true),
             hex("00095C"));
    // 未启用转义时按 UTF-8 原样输出。
    QCOMPARE(HexCodec::encodeText(QStringLiteral("\\r"), false), QByteArrayLiteral("\\r"));
}

void CommStudioCoreTest::lineEndings()
{
    QString error;
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("01"), true, false,
                                    QStringLiteral("crlf"), &error),
             hex("010D0A"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false,
                                    QStringLiteral("lf"), &error),
             QByteArrayLiteral("AT\n"));
    QCOMPARE(HexCodec::buildPayload(QStringLiteral("AT"), false, false,
                                    QStringLiteral("lfcr"), &error),
             QByteArrayLiteral("AT\n\r"));
}

void CommStudioCoreTest::pausedTrafficFlushesOnResume()
{
    TrafficModel model;
    model.setMaxRows(10);
    model.setPaused(true);
    model.append(TrafficModel::Rx, QByteArrayLiteral("rx"));
    model.flushPending();
    QCOMPARE(model.count(), 0);
    model.setPaused(false);
    QCOMPARE(model.count(), 1);
}

void CommStudioCoreTest::modbusRequestFrames()
{
    // 读保持寄存器 0x0000 x2：01 03 00 00 00 02 C4 0B。
    const QByteArray pdu = ModbusCodec::buildPdu(3, 0, QVector<quint16>{ 2 });
    QCOMPARE(pdu, hex("0300000002"));
    const QByteArray adu = ModbusCodec::buildRtuAdu(1, pdu);
    QCOMPARE(adu, hex("010300000002C40B"));
}

void CommStudioCoreTest::modbusResponseParsing()
{
    // 正常响应：01 03 04 41 20 00 00 ...（数据 0x4120 0x0000）。
    const QByteArray response = hex("01030441200000EFC5");
    const ModbusCodec::ParsedResponse parsed = ModbusCodec::parseResponse(response, false);
    QVERIFY(parsed.ok);
    QVERIFY(parsed.crcOk);
    QVERIFY(!parsed.exception);
    QCOMPARE(parsed.slave, quint8(1));
    QCOMPARE(parsed.functionCode, quint8(3));
    QCOMPARE(parsed.words.size(), 2);
    QCOMPARE(parsed.words.at(0), quint16(0x4120));
    QCOMPARE(parsed.words.at(1), quint16(0x0000));

    // 异常响应：01 83 02 C0 F1 → 非法数据地址。
    const ModbusCodec::ParsedResponse exc = ModbusCodec::parseResponse(hex("018302C0F1"), false);
    QVERIFY(exc.ok);
    QVERIFY(exc.exception);
    QCOMPARE(exc.exceptionCode, quint8(0x02));
    QVERIFY(ModbusCodec::exceptionText(0x02).contains(QStringLiteral("非法数据地址")));

    // CRC 错误检测。
    const ModbusCodec::ParsedResponse badCrc = ModbusCodec::parseResponse(hex("010300000002C40C"), false);
    QVERIFY(badCrc.ok);
    QVERIFY(!badCrc.crcOk);

    // TCP 响应（MBAP）解析。
    const QByteArray tcpAdu = hex("00010000000601030441200000");
    const ModbusCodec::ParsedResponse tcp = ModbusCodec::parseResponse(tcpAdu, true);
    QVERIFY(tcp.ok);
    QCOMPARE(tcp.transactionId, quint16(1));
    QCOMPARE(tcp.words.at(0), quint16(0x4120));
}

void CommStudioCoreTest::wordOrder32()
{
    // Float32 12.34 = 0x414570A4：ABCD → 41 45 70 A4；CDAB → 70 A4 41 45。
    const QByteArray bytes = ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("ABCD"));
    QCOMPARE(bytes, hex("414570A4"));
    QCOMPARE(ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("CDAB")), hex("70A44145"));
    QCOMPARE(ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("BADC")), hex("4541A470"));
    QCOMPARE(ModbusCodec::wordsToBytes32(0x4145, 0x70A4, QStringLiteral("DCBA")), hex("A4704541"));

    // 解码往返：float 12.34。
    const float value = ModbusCodec::bytesToFloat32(hex("414570A4"), QStringLiteral("ABCD"));
    QVERIFY(qAbs(value - 12.34f) < 0.001f);
}

void CommStudioCoreTest::trafficModelRingBuffer()
{
    TrafficModel model;
    model.setMaxRows(10);
    for (int i = 0; i < 25; ++i)
        // 连续 RX 片段会按短时间窗口合帧；用 TX 行验证环形缓冲的逐行裁剪。
        model.append(TrafficModel::Tx, QByteArrayLiteral("\x01\x02"), QStringLiteral("TEST"));
    model.flushPending();
    QCOMPARE(model.count(), 10);
    QCOMPARE(model.data(model.index(0, 0), TrafficModel::SizeRole), QVariant(2));

    model.appendInfo(QStringLiteral("信息行"));
    model.flushPending();
    QCOMPARE(model.count(), 10);

    model.setPaused(true);
    model.append(TrafficModel::Tx, QByteArrayLiteral("xx"));
    model.flushPending();
    QCOMPARE(model.count(), 10);

    model.clear();
    model.flushPending();
    QCOMPARE(model.count(), 0);
}

QTEST_MAIN(CommStudioCoreTest)
#include "comstudiotests.moc"
