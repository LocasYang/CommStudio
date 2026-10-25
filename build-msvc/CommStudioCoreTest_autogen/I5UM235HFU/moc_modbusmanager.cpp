/****************************************************************************
** Meta object code from reading C++ file 'modbusmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/managers/modbusmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbusmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSModbusManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusManagerENDCLASS = QtMocHelpers::stringData(
    "ModbusManager",
    "modeChanged",
    "",
    "selectedDeviceChanged",
    "openChanged",
    "portNameChanged",
    "baudRateChanged",
    "parityChanged",
    "hostChanged",
    "portChanged",
    "unitIdChanged",
    "slaveAddressChanged",
    "timeoutMsChanged",
    "retriesChanged",
    "portInfosChanged",
    "tasksChanged",
    "pollingChanged",
    "registerMapChanged",
    "simulatorChanged",
    "rawResultChanged",
    "statsChanged",
    "scanChanged",
    "rawLoopChanged",
    "rawTemplatesChanged",
    "channelOptionsChanged",
    "toastRequested",
    "text",
    "ok",
    "readPoint",
    "row",
    "createPointTasks",
    "strategy",
    "maxGap",
    "maxRegisters",
    "resetStatistics",
    "startScan",
    "first",
    "last",
    "fc",
    "address",
    "timeoutMs",
    "stopScan",
    "addDiscoveredDevices",
    "startRawLoop",
    "request",
    "tcp",
    "crc",
    "count",
    "intervalMs",
    "jitterMs",
    "stopRawLoop",
    "saveRawTemplate",
    "name",
    "removeRawTemplate",
    "index",
    "setChannelOption",
    "key",
    "value",
    "refreshPorts",
    "selectDevice",
    "addDevice",
    "unitId",
    "removeDevice",
    "openConnection",
    "close",
    "manualRead",
    "slave",
    "functionCode",
    "manualWrite",
    "valuesText",
    "writePoint",
    "addTask",
    "task",
    "updateTask",
    "removeTask",
    "taskAt",
    "startPolling",
    "stopPolling",
    "readTaskOnce",
    "importTasksFromCsv",
    "path",
    "previewWrite",
    "scale",
    "offsetValue",
    "type",
    "wordOrder",
    "sendRawFrame",
    "requestText",
    "isTcp",
    "autoCrc",
    "closeRawLink",
    "configureSimulatorArea",
    "area",
    "source",
    "baseValue",
    "amplitude",
    "writeSimulatorRegister",
    "traffic",
    "TrafficModel*",
    "devices",
    "ModbusDeviceModel*",
    "points",
    "ModbusPointModel*",
    "selectedDeviceIndex",
    "mode",
    "open",
    "portName",
    "baudRate",
    "parity",
    "host",
    "port",
    "slaveAddress",
    "retries",
    "portInfos",
    "tasks",
    "polling",
    "registerMap",
    "simulatorRunning",
    "simulatorAreas",
    "simulatorRegisters",
    "rawResult",
    "rawBusy",
    "txCount",
    "rxCount",
    "errorCount",
    "lastResponseMs",
    "statistics",
    "scanning",
    "scanResults",
    "rawLoopActive",
    "rawLoopSent",
    "rawTemplates",
    "channelOptions",
    "connectionState"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusManagerENDCLASS_t {
    uint offsetsAndSizes[268];
    char stringdata0[14];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[12];
    char stringdata5[16];
    char stringdata6[16];
    char stringdata7[14];
    char stringdata8[12];
    char stringdata9[12];
    char stringdata10[14];
    char stringdata11[20];
    char stringdata12[17];
    char stringdata13[15];
    char stringdata14[17];
    char stringdata15[13];
    char stringdata16[15];
    char stringdata17[19];
    char stringdata18[17];
    char stringdata19[17];
    char stringdata20[13];
    char stringdata21[12];
    char stringdata22[15];
    char stringdata23[20];
    char stringdata24[22];
    char stringdata25[15];
    char stringdata26[5];
    char stringdata27[3];
    char stringdata28[10];
    char stringdata29[4];
    char stringdata30[17];
    char stringdata31[9];
    char stringdata32[7];
    char stringdata33[13];
    char stringdata34[16];
    char stringdata35[10];
    char stringdata36[6];
    char stringdata37[5];
    char stringdata38[3];
    char stringdata39[8];
    char stringdata40[10];
    char stringdata41[9];
    char stringdata42[21];
    char stringdata43[13];
    char stringdata44[8];
    char stringdata45[4];
    char stringdata46[4];
    char stringdata47[6];
    char stringdata48[11];
    char stringdata49[9];
    char stringdata50[12];
    char stringdata51[16];
    char stringdata52[5];
    char stringdata53[18];
    char stringdata54[6];
    char stringdata55[17];
    char stringdata56[4];
    char stringdata57[6];
    char stringdata58[13];
    char stringdata59[13];
    char stringdata60[10];
    char stringdata61[7];
    char stringdata62[13];
    char stringdata63[15];
    char stringdata64[6];
    char stringdata65[11];
    char stringdata66[6];
    char stringdata67[13];
    char stringdata68[12];
    char stringdata69[11];
    char stringdata70[11];
    char stringdata71[8];
    char stringdata72[5];
    char stringdata73[11];
    char stringdata74[11];
    char stringdata75[7];
    char stringdata76[13];
    char stringdata77[12];
    char stringdata78[13];
    char stringdata79[19];
    char stringdata80[5];
    char stringdata81[13];
    char stringdata82[6];
    char stringdata83[12];
    char stringdata84[5];
    char stringdata85[10];
    char stringdata86[13];
    char stringdata87[12];
    char stringdata88[6];
    char stringdata89[8];
    char stringdata90[13];
    char stringdata91[23];
    char stringdata92[5];
    char stringdata93[7];
    char stringdata94[10];
    char stringdata95[10];
    char stringdata96[23];
    char stringdata97[8];
    char stringdata98[14];
    char stringdata99[8];
    char stringdata100[19];
    char stringdata101[7];
    char stringdata102[18];
    char stringdata103[20];
    char stringdata104[5];
    char stringdata105[5];
    char stringdata106[9];
    char stringdata107[9];
    char stringdata108[7];
    char stringdata109[5];
    char stringdata110[5];
    char stringdata111[13];
    char stringdata112[8];
    char stringdata113[10];
    char stringdata114[6];
    char stringdata115[8];
    char stringdata116[12];
    char stringdata117[17];
    char stringdata118[15];
    char stringdata119[19];
    char stringdata120[10];
    char stringdata121[8];
    char stringdata122[8];
    char stringdata123[8];
    char stringdata124[11];
    char stringdata125[15];
    char stringdata126[11];
    char stringdata127[9];
    char stringdata128[12];
    char stringdata129[14];
    char stringdata130[12];
    char stringdata131[13];
    char stringdata132[15];
    char stringdata133[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusManagerENDCLASS_t qt_meta_stringdata_CLASSModbusManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13),  // "ModbusManager"
        QT_MOC_LITERAL(14, 11),  // "modeChanged"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 21),  // "selectedDeviceChanged"
        QT_MOC_LITERAL(49, 11),  // "openChanged"
        QT_MOC_LITERAL(61, 15),  // "portNameChanged"
        QT_MOC_LITERAL(77, 15),  // "baudRateChanged"
        QT_MOC_LITERAL(93, 13),  // "parityChanged"
        QT_MOC_LITERAL(107, 11),  // "hostChanged"
        QT_MOC_LITERAL(119, 11),  // "portChanged"
        QT_MOC_LITERAL(131, 13),  // "unitIdChanged"
        QT_MOC_LITERAL(145, 19),  // "slaveAddressChanged"
        QT_MOC_LITERAL(165, 16),  // "timeoutMsChanged"
        QT_MOC_LITERAL(182, 14),  // "retriesChanged"
        QT_MOC_LITERAL(197, 16),  // "portInfosChanged"
        QT_MOC_LITERAL(214, 12),  // "tasksChanged"
        QT_MOC_LITERAL(227, 14),  // "pollingChanged"
        QT_MOC_LITERAL(242, 18),  // "registerMapChanged"
        QT_MOC_LITERAL(261, 16),  // "simulatorChanged"
        QT_MOC_LITERAL(278, 16),  // "rawResultChanged"
        QT_MOC_LITERAL(295, 12),  // "statsChanged"
        QT_MOC_LITERAL(308, 11),  // "scanChanged"
        QT_MOC_LITERAL(320, 14),  // "rawLoopChanged"
        QT_MOC_LITERAL(335, 19),  // "rawTemplatesChanged"
        QT_MOC_LITERAL(355, 21),  // "channelOptionsChanged"
        QT_MOC_LITERAL(377, 14),  // "toastRequested"
        QT_MOC_LITERAL(392, 4),  // "text"
        QT_MOC_LITERAL(397, 2),  // "ok"
        QT_MOC_LITERAL(400, 9),  // "readPoint"
        QT_MOC_LITERAL(410, 3),  // "row"
        QT_MOC_LITERAL(414, 16),  // "createPointTasks"
        QT_MOC_LITERAL(431, 8),  // "strategy"
        QT_MOC_LITERAL(440, 6),  // "maxGap"
        QT_MOC_LITERAL(447, 12),  // "maxRegisters"
        QT_MOC_LITERAL(460, 15),  // "resetStatistics"
        QT_MOC_LITERAL(476, 9),  // "startScan"
        QT_MOC_LITERAL(486, 5),  // "first"
        QT_MOC_LITERAL(492, 4),  // "last"
        QT_MOC_LITERAL(497, 2),  // "fc"
        QT_MOC_LITERAL(500, 7),  // "address"
        QT_MOC_LITERAL(508, 9),  // "timeoutMs"
        QT_MOC_LITERAL(518, 8),  // "stopScan"
        QT_MOC_LITERAL(527, 20),  // "addDiscoveredDevices"
        QT_MOC_LITERAL(548, 12),  // "startRawLoop"
        QT_MOC_LITERAL(561, 7),  // "request"
        QT_MOC_LITERAL(569, 3),  // "tcp"
        QT_MOC_LITERAL(573, 3),  // "crc"
        QT_MOC_LITERAL(577, 5),  // "count"
        QT_MOC_LITERAL(583, 10),  // "intervalMs"
        QT_MOC_LITERAL(594, 8),  // "jitterMs"
        QT_MOC_LITERAL(603, 11),  // "stopRawLoop"
        QT_MOC_LITERAL(615, 15),  // "saveRawTemplate"
        QT_MOC_LITERAL(631, 4),  // "name"
        QT_MOC_LITERAL(636, 17),  // "removeRawTemplate"
        QT_MOC_LITERAL(654, 5),  // "index"
        QT_MOC_LITERAL(660, 16),  // "setChannelOption"
        QT_MOC_LITERAL(677, 3),  // "key"
        QT_MOC_LITERAL(681, 5),  // "value"
        QT_MOC_LITERAL(687, 12),  // "refreshPorts"
        QT_MOC_LITERAL(700, 12),  // "selectDevice"
        QT_MOC_LITERAL(713, 9),  // "addDevice"
        QT_MOC_LITERAL(723, 6),  // "unitId"
        QT_MOC_LITERAL(730, 12),  // "removeDevice"
        QT_MOC_LITERAL(743, 14),  // "openConnection"
        QT_MOC_LITERAL(758, 5),  // "close"
        QT_MOC_LITERAL(764, 10),  // "manualRead"
        QT_MOC_LITERAL(775, 5),  // "slave"
        QT_MOC_LITERAL(781, 12),  // "functionCode"
        QT_MOC_LITERAL(794, 11),  // "manualWrite"
        QT_MOC_LITERAL(806, 10),  // "valuesText"
        QT_MOC_LITERAL(817, 10),  // "writePoint"
        QT_MOC_LITERAL(828, 7),  // "addTask"
        QT_MOC_LITERAL(836, 4),  // "task"
        QT_MOC_LITERAL(841, 10),  // "updateTask"
        QT_MOC_LITERAL(852, 10),  // "removeTask"
        QT_MOC_LITERAL(863, 6),  // "taskAt"
        QT_MOC_LITERAL(870, 12),  // "startPolling"
        QT_MOC_LITERAL(883, 11),  // "stopPolling"
        QT_MOC_LITERAL(895, 12),  // "readTaskOnce"
        QT_MOC_LITERAL(908, 18),  // "importTasksFromCsv"
        QT_MOC_LITERAL(927, 4),  // "path"
        QT_MOC_LITERAL(932, 12),  // "previewWrite"
        QT_MOC_LITERAL(945, 5),  // "scale"
        QT_MOC_LITERAL(951, 11),  // "offsetValue"
        QT_MOC_LITERAL(963, 4),  // "type"
        QT_MOC_LITERAL(968, 9),  // "wordOrder"
        QT_MOC_LITERAL(978, 12),  // "sendRawFrame"
        QT_MOC_LITERAL(991, 11),  // "requestText"
        QT_MOC_LITERAL(1003, 5),  // "isTcp"
        QT_MOC_LITERAL(1009, 7),  // "autoCrc"
        QT_MOC_LITERAL(1017, 12),  // "closeRawLink"
        QT_MOC_LITERAL(1030, 22),  // "configureSimulatorArea"
        QT_MOC_LITERAL(1053, 4),  // "area"
        QT_MOC_LITERAL(1058, 6),  // "source"
        QT_MOC_LITERAL(1065, 9),  // "baseValue"
        QT_MOC_LITERAL(1075, 9),  // "amplitude"
        QT_MOC_LITERAL(1085, 22),  // "writeSimulatorRegister"
        QT_MOC_LITERAL(1108, 7),  // "traffic"
        QT_MOC_LITERAL(1116, 13),  // "TrafficModel*"
        QT_MOC_LITERAL(1130, 7),  // "devices"
        QT_MOC_LITERAL(1138, 18),  // "ModbusDeviceModel*"
        QT_MOC_LITERAL(1157, 6),  // "points"
        QT_MOC_LITERAL(1164, 17),  // "ModbusPointModel*"
        QT_MOC_LITERAL(1182, 19),  // "selectedDeviceIndex"
        QT_MOC_LITERAL(1202, 4),  // "mode"
        QT_MOC_LITERAL(1207, 4),  // "open"
        QT_MOC_LITERAL(1212, 8),  // "portName"
        QT_MOC_LITERAL(1221, 8),  // "baudRate"
        QT_MOC_LITERAL(1230, 6),  // "parity"
        QT_MOC_LITERAL(1237, 4),  // "host"
        QT_MOC_LITERAL(1242, 4),  // "port"
        QT_MOC_LITERAL(1247, 12),  // "slaveAddress"
        QT_MOC_LITERAL(1260, 7),  // "retries"
        QT_MOC_LITERAL(1268, 9),  // "portInfos"
        QT_MOC_LITERAL(1278, 5),  // "tasks"
        QT_MOC_LITERAL(1284, 7),  // "polling"
        QT_MOC_LITERAL(1292, 11),  // "registerMap"
        QT_MOC_LITERAL(1304, 16),  // "simulatorRunning"
        QT_MOC_LITERAL(1321, 14),  // "simulatorAreas"
        QT_MOC_LITERAL(1336, 18),  // "simulatorRegisters"
        QT_MOC_LITERAL(1355, 9),  // "rawResult"
        QT_MOC_LITERAL(1365, 7),  // "rawBusy"
        QT_MOC_LITERAL(1373, 7),  // "txCount"
        QT_MOC_LITERAL(1381, 7),  // "rxCount"
        QT_MOC_LITERAL(1389, 10),  // "errorCount"
        QT_MOC_LITERAL(1400, 14),  // "lastResponseMs"
        QT_MOC_LITERAL(1415, 10),  // "statistics"
        QT_MOC_LITERAL(1426, 8),  // "scanning"
        QT_MOC_LITERAL(1435, 11),  // "scanResults"
        QT_MOC_LITERAL(1447, 13),  // "rawLoopActive"
        QT_MOC_LITERAL(1461, 11),  // "rawLoopSent"
        QT_MOC_LITERAL(1473, 12),  // "rawTemplates"
        QT_MOC_LITERAL(1486, 14),  // "channelOptions"
        QT_MOC_LITERAL(1501, 15)   // "connectionState"
    },
    "ModbusManager",
    "modeChanged",
    "",
    "selectedDeviceChanged",
    "openChanged",
    "portNameChanged",
    "baudRateChanged",
    "parityChanged",
    "hostChanged",
    "portChanged",
    "unitIdChanged",
    "slaveAddressChanged",
    "timeoutMsChanged",
    "retriesChanged",
    "portInfosChanged",
    "tasksChanged",
    "pollingChanged",
    "registerMapChanged",
    "simulatorChanged",
    "rawResultChanged",
    "statsChanged",
    "scanChanged",
    "rawLoopChanged",
    "rawTemplatesChanged",
    "channelOptionsChanged",
    "toastRequested",
    "text",
    "ok",
    "readPoint",
    "row",
    "createPointTasks",
    "strategy",
    "maxGap",
    "maxRegisters",
    "resetStatistics",
    "startScan",
    "first",
    "last",
    "fc",
    "address",
    "timeoutMs",
    "stopScan",
    "addDiscoveredDevices",
    "startRawLoop",
    "request",
    "tcp",
    "crc",
    "count",
    "intervalMs",
    "jitterMs",
    "stopRawLoop",
    "saveRawTemplate",
    "name",
    "removeRawTemplate",
    "index",
    "setChannelOption",
    "key",
    "value",
    "refreshPorts",
    "selectDevice",
    "addDevice",
    "unitId",
    "removeDevice",
    "openConnection",
    "close",
    "manualRead",
    "slave",
    "functionCode",
    "manualWrite",
    "valuesText",
    "writePoint",
    "addTask",
    "task",
    "updateTask",
    "removeTask",
    "taskAt",
    "startPolling",
    "stopPolling",
    "readTaskOnce",
    "importTasksFromCsv",
    "path",
    "previewWrite",
    "scale",
    "offsetValue",
    "type",
    "wordOrder",
    "sendRawFrame",
    "requestText",
    "isTcp",
    "autoCrc",
    "closeRawLink",
    "configureSimulatorArea",
    "area",
    "source",
    "baseValue",
    "amplitude",
    "writeSimulatorRegister",
    "traffic",
    "TrafficModel*",
    "devices",
    "ModbusDeviceModel*",
    "points",
    "ModbusPointModel*",
    "selectedDeviceIndex",
    "mode",
    "open",
    "portName",
    "baudRate",
    "parity",
    "host",
    "port",
    "slaveAddress",
    "retries",
    "portInfos",
    "tasks",
    "polling",
    "registerMap",
    "simulatorRunning",
    "simulatorAreas",
    "simulatorRegisters",
    "rawResult",
    "rawBusy",
    "txCount",
    "rxCount",
    "errorCount",
    "lastResponseMs",
    "statistics",
    "scanning",
    "scanResults",
    "rawLoopActive",
    "rawLoopSent",
    "rawTemplates",
    "channelOptions",
    "connectionState"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      61,   14, // methods
      36,  579, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      24,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  380,    2, 0x06,   37 /* Public */,
       3,    0,  381,    2, 0x06,   38 /* Public */,
       4,    0,  382,    2, 0x06,   39 /* Public */,
       5,    0,  383,    2, 0x06,   40 /* Public */,
       6,    0,  384,    2, 0x06,   41 /* Public */,
       7,    0,  385,    2, 0x06,   42 /* Public */,
       8,    0,  386,    2, 0x06,   43 /* Public */,
       9,    0,  387,    2, 0x06,   44 /* Public */,
      10,    0,  388,    2, 0x06,   45 /* Public */,
      11,    0,  389,    2, 0x06,   46 /* Public */,
      12,    0,  390,    2, 0x06,   47 /* Public */,
      13,    0,  391,    2, 0x06,   48 /* Public */,
      14,    0,  392,    2, 0x06,   49 /* Public */,
      15,    0,  393,    2, 0x06,   50 /* Public */,
      16,    0,  394,    2, 0x06,   51 /* Public */,
      17,    0,  395,    2, 0x06,   52 /* Public */,
      18,    0,  396,    2, 0x06,   53 /* Public */,
      19,    0,  397,    2, 0x06,   54 /* Public */,
      20,    0,  398,    2, 0x06,   55 /* Public */,
      21,    0,  399,    2, 0x06,   56 /* Public */,
      22,    0,  400,    2, 0x06,   57 /* Public */,
      23,    0,  401,    2, 0x06,   58 /* Public */,
      24,    0,  402,    2, 0x06,   59 /* Public */,
      25,    2,  403,    2, 0x06,   60 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      28,    1,  408,    2, 0x02,   63 /* Public */,
      30,    3,  411,    2, 0x02,   65 /* Public */,
      30,    2,  418,    2, 0x22,   69 /* Public | MethodCloned */,
      30,    1,  423,    2, 0x22,   72 /* Public | MethodCloned */,
      30,    0,  426,    2, 0x22,   74 /* Public | MethodCloned */,
      34,    0,  427,    2, 0x02,   75 /* Public */,
      35,    5,  428,    2, 0x02,   76 /* Public */,
      41,    0,  439,    2, 0x02,   82 /* Public */,
      42,    0,  440,    2, 0x02,   83 /* Public */,
      43,    6,  441,    2, 0x02,   84 /* Public */,
      43,    5,  454,    2, 0x22,   91 /* Public | MethodCloned */,
      50,    0,  465,    2, 0x02,   97 /* Public */,
      51,    4,  466,    2, 0x02,   98 /* Public */,
      53,    1,  475,    2, 0x02,  103 /* Public */,
      55,    2,  478,    2, 0x02,  105 /* Public */,
      58,    0,  483,    2, 0x02,  108 /* Public */,
      59,    1,  484,    2, 0x02,  109 /* Public */,
      60,    2,  487,    2, 0x02,  111 /* Public */,
      62,    1,  492,    2, 0x02,  114 /* Public */,
      63,    0,  495,    2, 0x02,  116 /* Public */,
      64,    0,  496,    2, 0x02,  117 /* Public */,
      65,    4,  497,    2, 0x02,  118 /* Public */,
      68,    4,  506,    2, 0x02,  123 /* Public */,
      70,    2,  515,    2, 0x02,  128 /* Public */,
      71,    1,  520,    2, 0x02,  131 /* Public */,
      73,    2,  523,    2, 0x02,  133 /* Public */,
      74,    1,  528,    2, 0x02,  136 /* Public */,
      75,    1,  531,    2, 0x102,  138 /* Public | MethodIsConst  */,
      76,    0,  534,    2, 0x02,  140 /* Public */,
      77,    0,  535,    2, 0x02,  141 /* Public */,
      78,    1,  536,    2, 0x02,  142 /* Public */,
      79,    1,  539,    2, 0x02,  144 /* Public */,
      81,    5,  542,    2, 0x102,  146 /* Public | MethodIsConst  */,
      86,    3,  553,    2, 0x02,  152 /* Public */,
      90,    0,  560,    2, 0x02,  156 /* Public */,
      91,    5,  561,    2, 0x02,  157 /* Public */,
      96,    3,  572,    2, 0x02,  163 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   26,   27,

 // methods: parameters
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   31,   32,   33,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   31,   32,
    QMetaType::Void, QMetaType::Int,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   36,   37,   38,   39,   40,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool, QMetaType::Int, QMetaType::Int, QMetaType::Int,   44,   45,   46,   47,   48,   49,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool, QMetaType::Int, QMetaType::Int,   44,   45,   46,   47,   48,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   52,   44,   45,   46,
    QMetaType::Void, QMetaType::Int,   54,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   56,   57,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   54,
    QMetaType::Int, QMetaType::QString, QMetaType::Int,   52,   61,
    QMetaType::Void, QMetaType::Int,   54,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   66,   67,   39,   47,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString,   66,   67,   39,   69,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   29,   69,
    QMetaType::Void, QMetaType::QVariantMap,   72,
    QMetaType::Void, QMetaType::Int, QMetaType::QVariantMap,   54,   72,
    QMetaType::Void, QMetaType::Int,   54,
    QMetaType::QVariantMap, QMetaType::Int,   54,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   54,
    QMetaType::Bool, QMetaType::QUrl,   80,
    QMetaType::QVariantMap, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::QString, QMetaType::QString,   57,   82,   83,   84,   85,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   87,   88,   89,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::Double, QMetaType::Double,   92,   47,   93,   94,   95,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   92,   39,   57,

 // properties: name, type, flags
      97, 0x80000000 | 98, 0x00015409, uint(-1), 0,
      99, 0x80000000 | 100, 0x00015409, uint(-1), 0,
     101, 0x80000000 | 102, 0x00015409, uint(-1), 0,
     103, QMetaType::Int, 0x00015001, uint(1), 0,
     104, QMetaType::QString, 0x00015103, uint(0), 0,
     105, QMetaType::Bool, 0x00015001, uint(2), 0,
     106, QMetaType::QString, 0x00015103, uint(3), 0,
     107, QMetaType::QString, 0x00015103, uint(4), 0,
     108, QMetaType::QString, 0x00015103, uint(5), 0,
     109, QMetaType::QString, 0x00015103, uint(6), 0,
     110, QMetaType::Int, 0x00015103, uint(7), 0,
      61, QMetaType::Int, 0x00015103, uint(8), 0,
     111, QMetaType::Int, 0x00015103, uint(9), 0,
      40, QMetaType::Int, 0x00015103, uint(10), 0,
     112, QMetaType::Int, 0x00015103, uint(11), 0,
     113, QMetaType::QVariantList, 0x00015001, uint(12), 0,
     114, QMetaType::QVariantList, 0x00015001, uint(13), 0,
     115, QMetaType::Bool, 0x00015001, uint(14), 0,
     116, QMetaType::QVariantList, 0x00015001, uint(15), 0,
     117, QMetaType::Bool, 0x00015001, uint(16), 0,
     118, QMetaType::QVariantMap, 0x00015001, uint(16), 0,
     119, QMetaType::QVariantList, 0x00015001, uint(16), 0,
     120, QMetaType::QVariantMap, 0x00015001, uint(17), 0,
     121, QMetaType::Bool, 0x00015001, uint(17), 0,
     122, QMetaType::LongLong, 0x00015001, uint(18), 0,
     123, QMetaType::LongLong, 0x00015001, uint(18), 0,
     124, QMetaType::LongLong, 0x00015001, uint(18), 0,
     125, QMetaType::Int, 0x00015001, uint(18), 0,
     126, QMetaType::QVariantMap, 0x00015001, uint(18), 0,
     127, QMetaType::Bool, 0x00015001, uint(19), 0,
     128, QMetaType::QVariantList, 0x00015001, uint(19), 0,
     129, QMetaType::Bool, 0x00015001, uint(20), 0,
     130, QMetaType::Int, 0x00015001, uint(20), 0,
     131, QMetaType::QVariantList, 0x00015001, uint(21), 0,
     132, QMetaType::QVariantMap, 0x00015001, uint(22), 0,
     133, QMetaType::Int, 0x00015001, uint(2), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusManagerENDCLASS_t,
        // property 'traffic'
        QtPrivate::TypeAndForceComplete<TrafficModel*, std::true_type>,
        // property 'devices'
        QtPrivate::TypeAndForceComplete<ModbusDeviceModel*, std::true_type>,
        // property 'points'
        QtPrivate::TypeAndForceComplete<ModbusPointModel*, std::true_type>,
        // property 'selectedDeviceIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'mode'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'open'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'portName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'baudRate'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'parity'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'host'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'port'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'unitId'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'slaveAddress'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'timeoutMs'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'retries'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'portInfos'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'tasks'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'polling'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'registerMap'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'simulatorRunning'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'simulatorAreas'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>,
        // property 'simulatorRegisters'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'rawResult'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>,
        // property 'rawBusy'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'txCount'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'rxCount'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'errorCount'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'lastResponseMs'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'statistics'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>,
        // property 'scanning'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'scanResults'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'rawLoopActive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'rawLoopSent'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'rawTemplates'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'channelOptions'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>,
        // property 'connectionState'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusManager, std::true_type>,
        // method 'modeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectedDeviceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'portNameChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'baudRateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'parityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'hostChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'portChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unitIdChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slaveAddressChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'timeoutMsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'retriesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'portInfosChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'tasksChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pollingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'registerMapChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'simulatorChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rawResultChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'scanChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rawLoopChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rawTemplatesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'channelOptionsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toastRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'readPoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'createPointTasks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'createPointTasks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'createPointTasks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'createPointTasks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetStatistics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startScan'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'stopScan'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addDiscoveredDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startRawLoop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'startRawLoop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'stopRawLoop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveRawTemplate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'removeRawTemplate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setChannelOption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'refreshPorts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'addDevice'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'removeDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'openConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'close'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'manualRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'manualWrite'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'writePoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'addTask'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'updateTask'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'removeTask'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'taskAt'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'startPolling'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopPolling'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readTaskOnce'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'importTasksFromCsv'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'previewWrite'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'sendRawFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'closeRawLink'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'configureSimulatorArea'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'writeSimulatorRegister'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void ModbusManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->modeChanged(); break;
        case 1: _t->selectedDeviceChanged(); break;
        case 2: _t->openChanged(); break;
        case 3: _t->portNameChanged(); break;
        case 4: _t->baudRateChanged(); break;
        case 5: _t->parityChanged(); break;
        case 6: _t->hostChanged(); break;
        case 7: _t->portChanged(); break;
        case 8: _t->unitIdChanged(); break;
        case 9: _t->slaveAddressChanged(); break;
        case 10: _t->timeoutMsChanged(); break;
        case 11: _t->retriesChanged(); break;
        case 12: _t->portInfosChanged(); break;
        case 13: _t->tasksChanged(); break;
        case 14: _t->pollingChanged(); break;
        case 15: _t->registerMapChanged(); break;
        case 16: _t->simulatorChanged(); break;
        case 17: _t->rawResultChanged(); break;
        case 18: _t->statsChanged(); break;
        case 19: _t->scanChanged(); break;
        case 20: _t->rawLoopChanged(); break;
        case 21: _t->rawTemplatesChanged(); break;
        case 22: _t->channelOptionsChanged(); break;
        case 23: _t->toastRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 24: _t->readPoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 25: _t->createPointTasks((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 26: _t->createPointTasks((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 27: _t->createPointTasks((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->createPointTasks(); break;
        case 29: _t->resetStatistics(); break;
        case 30: _t->startScan((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5]))); break;
        case 31: _t->stopScan(); break;
        case 32: _t->addDiscoveredDevices(); break;
        case 33: _t->startRawLoop((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6]))); break;
        case 34: _t->startRawLoop((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5]))); break;
        case 35: _t->stopRawLoop(); break;
        case 36: _t->saveRawTemplate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[4]))); break;
        case 37: _t->removeRawTemplate((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 38: _t->setChannelOption((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 39: _t->refreshPorts(); break;
        case 40: _t->selectDevice((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 41: { int _r = _t->addDevice((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 42: _t->removeDevice((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 43: _t->openConnection(); break;
        case 44: _t->close(); break;
        case 45: _t->manualRead((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 46: _t->manualWrite((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 47: _t->writePoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 48: _t->addTask((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 49: _t->updateTask((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 50: _t->removeTask((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 51: { QVariantMap _r = _t->taskAt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 52: _t->startPolling(); break;
        case 53: _t->stopPolling(); break;
        case 54: _t->readTaskOnce((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 55: { bool _r = _t->importTasksFromCsv((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 56: { QVariantMap _r = _t->previewWrite((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 57: _t->sendRawFrame((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 58: _t->closeRawLink(); break;
        case 59: _t->configureSimulatorArea((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[5]))); break;
        case 60: _t->writeSimulatorRegister((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::modeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::selectedDeviceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::openChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::portNameChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::baudRateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::parityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::hostChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::portChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::unitIdChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::slaveAddressChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::timeoutMsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::retriesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::portInfosChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::tasksChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::pollingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::registerMapChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::simulatorChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::rawResultChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::statsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::scanChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::rawLoopChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::rawTemplatesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 21;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::channelOptionsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 22;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(const QString & , bool );
            if (_t _q_method = &ModbusManager::toastRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 23;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ModbusDeviceModel* >(); break;
        case 2:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ModbusPointModel* >(); break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TrafficModel* >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TrafficModel**>(_v) = _t->traffic(); break;
        case 1: *reinterpret_cast< ModbusDeviceModel**>(_v) = _t->devices(); break;
        case 2: *reinterpret_cast< ModbusPointModel**>(_v) = _t->points(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->selectedDeviceIndex(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->mode(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->isOpen(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->portName(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->baudRate(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->parity(); break;
        case 9: *reinterpret_cast< QString*>(_v) = _t->host(); break;
        case 10: *reinterpret_cast< int*>(_v) = _t->port(); break;
        case 11: *reinterpret_cast< int*>(_v) = _t->unitId(); break;
        case 12: *reinterpret_cast< int*>(_v) = _t->slaveAddress(); break;
        case 13: *reinterpret_cast< int*>(_v) = _t->timeoutMs(); break;
        case 14: *reinterpret_cast< int*>(_v) = _t->retries(); break;
        case 15: *reinterpret_cast< QVariantList*>(_v) = _t->portInfos(); break;
        case 16: *reinterpret_cast< QVariantList*>(_v) = _t->tasks(); break;
        case 17: *reinterpret_cast< bool*>(_v) = _t->polling(); break;
        case 18: *reinterpret_cast< QVariantList*>(_v) = _t->registerMap(); break;
        case 19: *reinterpret_cast< bool*>(_v) = _t->simulatorRunning(); break;
        case 20: *reinterpret_cast< QVariantMap*>(_v) = _t->simulatorAreas(); break;
        case 21: *reinterpret_cast< QVariantList*>(_v) = _t->simulatorRegisters(); break;
        case 22: *reinterpret_cast< QVariantMap*>(_v) = _t->rawResult(); break;
        case 23: *reinterpret_cast< bool*>(_v) = _t->rawBusy(); break;
        case 24: *reinterpret_cast< qint64*>(_v) = _t->txCount(); break;
        case 25: *reinterpret_cast< qint64*>(_v) = _t->rxCount(); break;
        case 26: *reinterpret_cast< qint64*>(_v) = _t->errorCount(); break;
        case 27: *reinterpret_cast< int*>(_v) = _t->lastResponseMs(); break;
        case 28: *reinterpret_cast< QVariantMap*>(_v) = _t->statistics(); break;
        case 29: *reinterpret_cast< bool*>(_v) = _t->scanning(); break;
        case 30: *reinterpret_cast< QVariantList*>(_v) = _t->scanResults(); break;
        case 31: *reinterpret_cast< bool*>(_v) = _t->rawLoopActive(); break;
        case 32: *reinterpret_cast< int*>(_v) = _t->rawLoopSent(); break;
        case 33: *reinterpret_cast< QVariantList*>(_v) = _t->rawTemplates(); break;
        case 34: *reinterpret_cast< QVariantMap*>(_v) = _t->channelOptions(); break;
        case 35: *reinterpret_cast< int*>(_v) = _t->connectionState(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 4: _t->setMode(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setPortName(*reinterpret_cast< QString*>(_v)); break;
        case 7: _t->setBaudRate(*reinterpret_cast< QString*>(_v)); break;
        case 8: _t->setParity(*reinterpret_cast< QString*>(_v)); break;
        case 9: _t->setHost(*reinterpret_cast< QString*>(_v)); break;
        case 10: _t->setPort(*reinterpret_cast< int*>(_v)); break;
        case 11: _t->setUnitId(*reinterpret_cast< int*>(_v)); break;
        case 12: _t->setSlaveAddress(*reinterpret_cast< int*>(_v)); break;
        case 13: _t->setTimeoutMs(*reinterpret_cast< int*>(_v)); break;
        case 14: _t->setRetries(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *ModbusManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 61)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 61;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    return _id;
}

// SIGNAL 0
void ModbusManager::modeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModbusManager::selectedDeviceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusManager::openChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ModbusManager::portNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ModbusManager::baudRateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ModbusManager::parityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ModbusManager::hostChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ModbusManager::portChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ModbusManager::unitIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ModbusManager::slaveAddressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ModbusManager::timeoutMsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void ModbusManager::retriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void ModbusManager::portInfosChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void ModbusManager::tasksChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ModbusManager::pollingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void ModbusManager::registerMapChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void ModbusManager::simulatorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void ModbusManager::rawResultChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void ModbusManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void ModbusManager::scanChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void ModbusManager::rawLoopChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 20, nullptr);
}

// SIGNAL 21
void ModbusManager::rawTemplatesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void ModbusManager::channelOptionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 22, nullptr);
}

// SIGNAL 23
void ModbusManager::toastRequested(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}
QT_WARNING_POP
