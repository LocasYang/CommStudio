#pragma once

#include <QVariantMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRegularExpression>

// MThings device templates contain one DEVICE, not a complete project/port list.
namespace MThingsPointTemplate {
inline bool decode(const QByteArray &bytes, QVariantList &points, QString &error)
{
    QXmlStreamReader xml(bytes);
    if (!xml.readNextStartElement() || xml.name() != u"MThings") {
        error = QStringLiteral("不是 MThings 设备模板"); return false;
    }
    int devices = 0;
    while (xml.readNextStartElement()) {
        if (xml.name() != u"DEVICE" || ++devices != 1) {
            xml.raiseError(QStringLiteral("请选择单设备 .dtmthings 模板，不是完整工程")); break;
        }
        while (xml.readNextStartElement()) {
            if (xml.name() != u"DATA_LIST") { xml.skipCurrentElement(); continue; }
            while (xml.readNextStartElement()) {
                if (xml.name() != u"DATA") { xml.skipCurrentElement(); continue; }
                const auto a = xml.attributes();
                auto number = [&](const char *key, int fallback) {
                    if (!a.hasAttribute(QLatin1String(key))) return fallback;
                    bool ok = false;
                    const int result = a.value(QLatin1String(key)).toInt(&ok);
                    if (!ok) xml.raiseError(QStringLiteral("模板参数 %1 不是整数").arg(QLatin1String(key)));
                    return result;
                };
                const int block = number("BLOCK", -1);
                const int size = number("sz", number("Size", 1) * 2);
                const int protocolType = number("PrtcTYPE", 1);
                const int display = number("ShowType", 2);
                if (block < 0 || block > 3 || size < 2 || size % 2 != 0
                    || protocolType < 0 || protocolType > 5 || display < 0 || display > 7) {
                    xml.raiseError(QStringLiteral("模板包含当前不支持的协议、数据长度或类型")); break;
                }
                QString type;
                if (block < 2) type = "bit";
                else if (protocolType <= 1 && (size == 2 || size == 4 || size == 8))
                    type = QString(protocolType == 0 ? "int" : "uint") + QString::number(size * 8);
                else if (protocolType == 2 && (size == 4 || size == 8)) type = "float" + QString::number(size * 8);
                else if (protocolType == 3) type = display == 5 ? "string" : "bytes";
                else if (protocolType == 4) type = "bit";
                else if (protocolType == 5) type = "bcd";
                if (type.isEmpty()) { xml.raiseError(QStringLiteral("模板中的传输类型与长度不匹配")); break; }
                const int byteOrder = number("ByteOder", 0), wordOrder = number("WordOder", 0);
                if (byteOrder < 0 || byteOrder > 1 || wordOrder < 0 || wordOrder > 1) {
                    xml.raiseError(QStringLiteral("模板字节序无效")); break;
                }
                const QStringList orders{"ABCD", "BADC", "CDAB", "DCBA"};
                const QStringList displays{"FLOAT", "BIN", "DEC", "HEX", "HEX", "STRING", "TIMESTAMP", "ENUM"};
                QVariantMap p{{"name", a.value(u"Name").toString()}, {"fc", block + 1},
                              {"address", number("Addr", -1)}, {"count", block < 2 ? 1 : size / 2},
                              {"type", type}, {"displayType", displays[display]},
                              {"wordOrder", orders[byteOrder + wordOrder * 2]},
                              {"decimals", number("PointNum", 1)}, {"bitOffset", number("BitOffset", 0)},
                              {"bitCount", number("BitNum", block < 2 ? 1 : qMin(64, size * 8))},
                              {"unit", a.value(u"Unit").toString()}, {"writeValue", a.value(u"CmdValue").toString()},
                              {"poll", number("IsBRead", 1) != 0}, {"batchWrite", number("IsBWrite", 0) != 0},
                              {"periodMs", number("IntervalTime", 0)},
                              {"enumMap", QVariantMap{}}};
                p["scale"] = a.hasAttribute(u"Gain") ? a.value(u"Gain").toString() : QStringLiteral("1");
                p["offset"] = a.hasAttribute(u"Offset") ? a.value(u"Offset").toString() : QStringLiteral("0");
                const QString range = a.value(u"Range").toString().trimmed();
                if (!range.isEmpty() && range != "--") {
                    static const QRegularExpression rangePattern(
                        R"(^\s*([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)\s*[-~,]\s*([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)\s*$)");
                    const auto match = rangePattern.match(range);
                    if (!match.hasMatch()) { xml.raiseError(QStringLiteral("模板范围不是数值区间：%1").arg(range)); break; }
                    p["minValue"] = match.captured(1); p["maxValue"] = match.captured(2);
                }
                while (xml.readNextStartElement()) {
                    if (xml.name() == u"TIMEOUT") {
                        const auto timeout = xml.attributes();
                        bool timeOk = false, retryOk = false;
                        p["timeoutMs"] = timeout.value(u"Time").toInt(&timeOk);
                        p["retries"] = timeout.value(u"ResendCount").toInt(&retryOk);
                        if (!timeOk || !retryOk) xml.raiseError(QStringLiteral("模板超时或重试参数无效"));
                        xml.skipCurrentElement();
                    } else if (xml.name() == u"ENUMS") {
                        QVariantMap enums;
                        while (xml.readNextStartElement()) {
                            if (xml.name() == u"enum") enums[xml.attributes().value(u"Value").toString()] = xml.attributes().value(u"Name").toString();
                            xml.skipCurrentElement();
                        }
                        p["enumMap"] = enums;
                    } else if (xml.name() == u"GROUPS") {
                        QStringList groups;
                        while (xml.readNextStartElement()) {
                            if (xml.name() == u"group") groups.append(xml.attributes().value(u"Name").toString());
                            xml.skipCurrentElement();
                        }
                        if (groups.size() > 1) { xml.raiseError(QStringLiteral("当前仅支持每个数据点一个分组")); break; }
                        if (!groups.isEmpty()) p["group"] = groups.first();
                    } else xml.skipCurrentElement();
                }
                points.append(p);
            }
        }
    }
    // Consume the document end as well, so trailing malformed XML cannot pass.
    while (!xml.atEnd()) xml.readNext();
    if (xml.hasError() || devices != 1) {
        error = xml.hasError() ? xml.errorString() : QStringLiteral("模板缺少设备");
        points.clear(); return false;
    }
    return true;
}

inline QByteArray encode(const QVariantList &points, const QVariantMap &metadata)
{
    QByteArray bytes;
    QXmlStreamWriter xml(&bytes);
    xml.setAutoFormatting(true);
    xml.writeStartDocument(); xml.writeStartElement("MThings"); xml.writeStartElement("DEVICE");
    auto attr = [&](const QString &key, const QVariant &value) { xml.writeAttribute(key, value.toString()); };
    attr("ID", 1); attr("Name", metadata.value("deviceName")); attr("DeviceType", metadata.value("deviceType", 1));
    xml.writeEmptyElement("PORTS"); xml.writeEmptyElement("GROUPS"); xml.writeEmptyElement("SELF_LIST");
    xml.writeEmptyElement("ATTACH");
    for (const auto &pair : {qMakePair("Factory", "manufacturer"), qMakePair("DeviceName", "deviceName"),
                             qMakePair("DeviceModel", "model"), qMakePair("CfgVersion", "version"), qMakePair("Remarks", "notes")})
        attr(QLatin1String(pair.first), metadata.value(QLatin1String(pair.second)));
    xml.writeStartElement("DATA_LIST");
    int id = 0;
    for (const auto &entry : points) {
        const auto p = entry.toMap(); const auto type = p.value("type").toString();
        const int count = p.value("count", 1).toInt();
        const auto order = p.value("wordOrder", "ABCD").toString();
        const int protocol = type == "bit" ? 4 : type == "bcd" ? 5 : type.startsWith("uint") ? 1
                             : type.startsWith("int") ? 0 : type.startsWith("float") ? 2 : 3;
        const QHash<QString, int> displayTypes{{"FLOAT", 0}, {"BIN", 1}, {"BIT FIELD", 1}, {"DEC", 2},
                                               {"HEX", 3}, {"STRING", 5}, {"TIMESTAMP", 6}, {"ENUM", 7}};
        xml.writeStartElement("DATA");
        attr("ID", ++id); attr("Name", p.value("name")); attr("BLOCK", p.value("fc").toInt() - 1);
        attr("Addr", p.value("address")); attr("PrtcTYPE", protocol); attr("ShowType", displayTypes.value(p.value("displayType").toString(), 2));
        attr("sz", count * 2); attr("PointNum", p.value("decimals", 1));
        attr("Gain", p.value("scale", 1)); attr("Offset", p.value("offset", 0));
        attr("BitOffset", p.value("bitOffset", 0));
        attr("BitNum", p.value("bitCount").toInt() > 0 ? p.value("bitCount").toInt() : qMin(64, count * 16));
        attr("Unit", p.value("unit")); attr("CmdValue", p.value("writeValue"));
        attr("ByteOder", order == "BADC" || order == "DCBA" ? 1 : 0);
        attr("WordOder", order == "CDAB" || order == "DCBA" ? 1 : 0);
        attr("IntervalTime", p.value("periodMs", 0)); attr("IsBRead", p.value("poll", true).toBool() ? 1 : 0);
        attr("IsBWrite", p.value("batchWrite", false).toBool() ? 1 : 0);
        attr("Range", p.contains("minValue") && p.contains("maxValue")
                      ? p.value("minValue").toString() + "-" + p.value("maxValue").toString() : "--");
        xml.writeEmptyElement("TIMEOUT");
        attr("Time", p.value("timeoutMs", -1).toInt() < 0 ? metadata.value("timeoutMs", 2000) : p.value("timeoutMs"));
        attr("ResendCount", p.value("retries", -1).toInt() < 0 ? metadata.value("retries", 0) : p.value("retries"));
        xml.writeStartElement("ENUMS");
        const auto enums = p.value("enumMap").toMap();
        for (auto it = enums.begin(); it != enums.end(); ++it) {
            xml.writeEmptyElement("enum"); attr("Value", it.key()); attr("Name", it.value());
        }
        xml.writeEndElement();
        xml.writeStartElement("GROUPS");
        if (!p.value("group").toString().isEmpty()) { xml.writeEmptyElement("group"); attr("Name", p.value("group")); }
        xml.writeEndElement(); xml.writeEndElement();
    }
    xml.writeEndElement(); xml.writeEndElement(); xml.writeEndElement(); xml.writeEndDocument();
    return bytes;
}
}
