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
    "toastRequested",
    "text",
    "ok",
    "refreshPorts",
    "open",
    "close",
    "manualRead",
    "slave",
    "functionCode",
    "address",
    "count",
    "manualWrite",
    "valuesText",
    "addTask",
    "task",
    "updateTask",
    "index",
    "removeTask",
    "taskAt",
    "startPolling",
    "stopPolling",
    "readTaskOnce",
    "importTasksFromCsv",
    "path",
    "previewWrite",
    "value",
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
    "mode",
    "portName",
    "baudRate",
    "parity",
    "host",
    "port",
    "unitId",
    "slaveAddress",
    "timeoutMs",
    "retries",
    "portInfos",
    "tasks",
    "polling",
    "registerMap",
    "simulatorRunning",
    "simulatorRegisters",
    "rawResult",
    "rawBusy",
    "txCount",
    "rxCount",
    "errorCount",
    "lastResponseMs"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusManagerENDCLASS_t {
    uint offsetsAndSizes[170];
    char stringdata0[14];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[16];
    char stringdata5[16];
    char stringdata6[14];
    char stringdata7[12];
    char stringdata8[12];
    char stringdata9[14];
    char stringdata10[20];
    char stringdata11[17];
    char stringdata12[15];
    char stringdata13[17];
    char stringdata14[13];
    char stringdata15[15];
    char stringdata16[19];
    char stringdata17[17];
    char stringdata18[17];
    char stringdata19[13];
    char stringdata20[15];
    char stringdata21[5];
    char stringdata22[3];
    char stringdata23[13];
    char stringdata24[5];
    char stringdata25[6];
    char stringdata26[11];
    char stringdata27[6];
    char stringdata28[13];
    char stringdata29[8];
    char stringdata30[6];
    char stringdata31[12];
    char stringdata32[11];
    char stringdata33[8];
    char stringdata34[5];
    char stringdata35[11];
    char stringdata36[6];
    char stringdata37[11];
    char stringdata38[7];
    char stringdata39[13];
    char stringdata40[12];
    char stringdata41[13];
    char stringdata42[19];
    char stringdata43[5];
    char stringdata44[13];
    char stringdata45[6];
    char stringdata46[6];
    char stringdata47[12];
    char stringdata48[5];
    char stringdata49[10];
    char stringdata50[13];
    char stringdata51[12];
    char stringdata52[6];
    char stringdata53[8];
    char stringdata54[13];
    char stringdata55[23];
    char stringdata56[5];
    char stringdata57[7];
    char stringdata58[10];
    char stringdata59[10];
    char stringdata60[23];
    char stringdata61[8];
    char stringdata62[14];
    char stringdata63[5];
    char stringdata64[9];
    char stringdata65[9];
    char stringdata66[7];
    char stringdata67[5];
    char stringdata68[5];
    char stringdata69[7];
    char stringdata70[13];
    char stringdata71[10];
    char stringdata72[8];
    char stringdata73[10];
    char stringdata74[6];
    char stringdata75[8];
    char stringdata76[12];
    char stringdata77[17];
    char stringdata78[19];
    char stringdata79[10];
    char stringdata80[8];
    char stringdata81[8];
    char stringdata82[8];
    char stringdata83[11];
    char stringdata84[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusManagerENDCLASS_t qt_meta_stringdata_CLASSModbusManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13),  // "ModbusManager"
        QT_MOC_LITERAL(14, 11),  // "modeChanged"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 11),  // "openChanged"
        QT_MOC_LITERAL(39, 15),  // "portNameChanged"
        QT_MOC_LITERAL(55, 15),  // "baudRateChanged"
        QT_MOC_LITERAL(71, 13),  // "parityChanged"
        QT_MOC_LITERAL(85, 11),  // "hostChanged"
        QT_MOC_LITERAL(97, 11),  // "portChanged"
        QT_MOC_LITERAL(109, 13),  // "unitIdChanged"
        QT_MOC_LITERAL(123, 19),  // "slaveAddressChanged"
        QT_MOC_LITERAL(143, 16),  // "timeoutMsChanged"
        QT_MOC_LITERAL(160, 14),  // "retriesChanged"
        QT_MOC_LITERAL(175, 16),  // "portInfosChanged"
        QT_MOC_LITERAL(192, 12),  // "tasksChanged"
        QT_MOC_LITERAL(205, 14),  // "pollingChanged"
        QT_MOC_LITERAL(220, 18),  // "registerMapChanged"
        QT_MOC_LITERAL(239, 16),  // "simulatorChanged"
        QT_MOC_LITERAL(256, 16),  // "rawResultChanged"
        QT_MOC_LITERAL(273, 12),  // "statsChanged"
        QT_MOC_LITERAL(286, 14),  // "toastRequested"
        QT_MOC_LITERAL(301, 4),  // "text"
        QT_MOC_LITERAL(306, 2),  // "ok"
        QT_MOC_LITERAL(309, 12),  // "refreshPorts"
        QT_MOC_LITERAL(322, 4),  // "open"
        QT_MOC_LITERAL(327, 5),  // "close"
        QT_MOC_LITERAL(333, 10),  // "manualRead"
        QT_MOC_LITERAL(344, 5),  // "slave"
        QT_MOC_LITERAL(350, 12),  // "functionCode"
        QT_MOC_LITERAL(363, 7),  // "address"
        QT_MOC_LITERAL(371, 5),  // "count"
        QT_MOC_LITERAL(377, 11),  // "manualWrite"
        QT_MOC_LITERAL(389, 10),  // "valuesText"
        QT_MOC_LITERAL(400, 7),  // "addTask"
        QT_MOC_LITERAL(408, 4),  // "task"
        QT_MOC_LITERAL(413, 10),  // "updateTask"
        QT_MOC_LITERAL(424, 5),  // "index"
        QT_MOC_LITERAL(430, 10),  // "removeTask"
        QT_MOC_LITERAL(441, 6),  // "taskAt"
        QT_MOC_LITERAL(448, 12),  // "startPolling"
        QT_MOC_LITERAL(461, 11),  // "stopPolling"
        QT_MOC_LITERAL(473, 12),  // "readTaskOnce"
        QT_MOC_LITERAL(486, 18),  // "importTasksFromCsv"
        QT_MOC_LITERAL(505, 4),  // "path"
        QT_MOC_LITERAL(510, 12),  // "previewWrite"
        QT_MOC_LITERAL(523, 5),  // "value"
        QT_MOC_LITERAL(529, 5),  // "scale"
        QT_MOC_LITERAL(535, 11),  // "offsetValue"
        QT_MOC_LITERAL(547, 4),  // "type"
        QT_MOC_LITERAL(552, 9),  // "wordOrder"
        QT_MOC_LITERAL(562, 12),  // "sendRawFrame"
        QT_MOC_LITERAL(575, 11),  // "requestText"
        QT_MOC_LITERAL(587, 5),  // "isTcp"
        QT_MOC_LITERAL(593, 7),  // "autoCrc"
        QT_MOC_LITERAL(601, 12),  // "closeRawLink"
        QT_MOC_LITERAL(614, 22),  // "configureSimulatorArea"
        QT_MOC_LITERAL(637, 4),  // "area"
        QT_MOC_LITERAL(642, 6),  // "source"
        QT_MOC_LITERAL(649, 9),  // "baseValue"
        QT_MOC_LITERAL(659, 9),  // "amplitude"
        QT_MOC_LITERAL(669, 22),  // "writeSimulatorRegister"
        QT_MOC_LITERAL(692, 7),  // "traffic"
        QT_MOC_LITERAL(700, 13),  // "TrafficModel*"
        QT_MOC_LITERAL(714, 4),  // "mode"
        QT_MOC_LITERAL(719, 8),  // "portName"
        QT_MOC_LITERAL(728, 8),  // "baudRate"
        QT_MOC_LITERAL(737, 6),  // "parity"
        QT_MOC_LITERAL(744, 4),  // "host"
        QT_MOC_LITERAL(749, 4),  // "port"
        QT_MOC_LITERAL(754, 6),  // "unitId"
        QT_MOC_LITERAL(761, 12),  // "slaveAddress"
        QT_MOC_LITERAL(774, 9),  // "timeoutMs"
        QT_MOC_LITERAL(784, 7),  // "retries"
        QT_MOC_LITERAL(792, 9),  // "portInfos"
        QT_MOC_LITERAL(802, 5),  // "tasks"
        QT_MOC_LITERAL(808, 7),  // "polling"
        QT_MOC_LITERAL(816, 11),  // "registerMap"
        QT_MOC_LITERAL(828, 16),  // "simulatorRunning"
        QT_MOC_LITERAL(845, 18),  // "simulatorRegisters"
        QT_MOC_LITERAL(864, 9),  // "rawResult"
        QT_MOC_LITERAL(874, 7),  // "rawBusy"
        QT_MOC_LITERAL(882, 7),  // "txCount"
        QT_MOC_LITERAL(890, 7),  // "rxCount"
        QT_MOC_LITERAL(898, 10),  // "errorCount"
        QT_MOC_LITERAL(909, 14)   // "lastResponseMs"
    },
    "ModbusManager",
    "modeChanged",
    "",
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
    "toastRequested",
    "text",
    "ok",
    "refreshPorts",
    "open",
    "close",
    "manualRead",
    "slave",
    "functionCode",
    "address",
    "count",
    "manualWrite",
    "valuesText",
    "addTask",
    "task",
    "updateTask",
    "index",
    "removeTask",
    "taskAt",
    "startPolling",
    "stopPolling",
    "readTaskOnce",
    "importTasksFromCsv",
    "path",
    "previewWrite",
    "value",
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
    "mode",
    "portName",
    "baudRate",
    "parity",
    "host",
    "port",
    "unitId",
    "slaveAddress",
    "timeoutMs",
    "retries",
    "portInfos",
    "tasks",
    "polling",
    "registerMap",
    "simulatorRunning",
    "simulatorRegisters",
    "rawResult",
    "rawBusy",
    "txCount",
    "rxCount",
    "errorCount",
    "lastResponseMs"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      37,   14, // methods
      24,  339, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  236,    2, 0x06,   25 /* Public */,
       3,    0,  237,    2, 0x06,   26 /* Public */,
       4,    0,  238,    2, 0x06,   27 /* Public */,
       5,    0,  239,    2, 0x06,   28 /* Public */,
       6,    0,  240,    2, 0x06,   29 /* Public */,
       7,    0,  241,    2, 0x06,   30 /* Public */,
       8,    0,  242,    2, 0x06,   31 /* Public */,
       9,    0,  243,    2, 0x06,   32 /* Public */,
      10,    0,  244,    2, 0x06,   33 /* Public */,
      11,    0,  245,    2, 0x06,   34 /* Public */,
      12,    0,  246,    2, 0x06,   35 /* Public */,
      13,    0,  247,    2, 0x06,   36 /* Public */,
      14,    0,  248,    2, 0x06,   37 /* Public */,
      15,    0,  249,    2, 0x06,   38 /* Public */,
      16,    0,  250,    2, 0x06,   39 /* Public */,
      17,    0,  251,    2, 0x06,   40 /* Public */,
      18,    0,  252,    2, 0x06,   41 /* Public */,
      19,    0,  253,    2, 0x06,   42 /* Public */,
      20,    2,  254,    2, 0x06,   43 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      23,    0,  259,    2, 0x02,   46 /* Public */,
      24,    0,  260,    2, 0x02,   47 /* Public */,
      25,    0,  261,    2, 0x02,   48 /* Public */,
      26,    4,  262,    2, 0x02,   49 /* Public */,
      31,    4,  271,    2, 0x02,   54 /* Public */,
      33,    1,  280,    2, 0x02,   59 /* Public */,
      35,    2,  283,    2, 0x02,   61 /* Public */,
      37,    1,  288,    2, 0x02,   64 /* Public */,
      38,    1,  291,    2, 0x102,   66 /* Public | MethodIsConst  */,
      39,    0,  294,    2, 0x02,   68 /* Public */,
      40,    0,  295,    2, 0x02,   69 /* Public */,
      41,    1,  296,    2, 0x02,   70 /* Public */,
      42,    1,  299,    2, 0x02,   72 /* Public */,
      44,    5,  302,    2, 0x102,   74 /* Public | MethodIsConst  */,
      50,    3,  313,    2, 0x02,   80 /* Public */,
      54,    0,  320,    2, 0x02,   84 /* Public */,
      55,    5,  321,    2, 0x02,   85 /* Public */,
      60,    3,  332,    2, 0x02,   91 /* Public */,

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
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   21,   22,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   27,   28,   29,   30,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString,   27,   28,   29,   32,
    QMetaType::Void, QMetaType::QVariantMap,   34,
    QMetaType::Void, QMetaType::Int, QMetaType::QVariantMap,   36,   34,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::QVariantMap, QMetaType::Int,   36,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Bool, QMetaType::QUrl,   43,
    QMetaType::QVariantMap, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::QString, QMetaType::QString,   45,   46,   47,   48,   49,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   51,   52,   53,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::Double, QMetaType::Double,   56,   30,   57,   58,   59,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   56,   29,   45,

 // properties: name, type, flags
      61, 0x80000000 | 62, 0x00015409, uint(-1), 0,
      63, QMetaType::QString, 0x00015103, uint(0), 0,
      24, QMetaType::Bool, 0x00015001, uint(1), 0,
      64, QMetaType::QString, 0x00015103, uint(2), 0,
      65, QMetaType::QString, 0x00015103, uint(3), 0,
      66, QMetaType::QString, 0x00015103, uint(4), 0,
      67, QMetaType::QString, 0x00015103, uint(5), 0,
      68, QMetaType::Int, 0x00015103, uint(6), 0,
      69, QMetaType::Int, 0x00015103, uint(7), 0,
      70, QMetaType::Int, 0x00015103, uint(8), 0,
      71, QMetaType::Int, 0x00015103, uint(9), 0,
      72, QMetaType::Int, 0x00015103, uint(10), 0,
      73, QMetaType::QVariantList, 0x00015001, uint(11), 0,
      74, QMetaType::QVariantList, 0x00015001, uint(12), 0,
      75, QMetaType::Bool, 0x00015001, uint(13), 0,
      76, QMetaType::QVariantList, 0x00015001, uint(14), 0,
      77, QMetaType::Bool, 0x00015001, uint(15), 0,
      78, QMetaType::QVariantList, 0x00015001, uint(15), 0,
      79, QMetaType::QVariantMap, 0x00015001, uint(16), 0,
      80, QMetaType::Bool, 0x00015001, uint(16), 0,
      81, QMetaType::LongLong, 0x00015001, uint(17), 0,
      82, QMetaType::LongLong, 0x00015001, uint(17), 0,
      83, QMetaType::LongLong, 0x00015001, uint(17), 0,
      84, QMetaType::Int, 0x00015001, uint(17), 0,

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
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusManager, std::true_type>,
        // method 'modeChanged'
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
        // method 'toastRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'refreshPorts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open'
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
        case 1: _t->openChanged(); break;
        case 2: _t->portNameChanged(); break;
        case 3: _t->baudRateChanged(); break;
        case 4: _t->parityChanged(); break;
        case 5: _t->hostChanged(); break;
        case 6: _t->portChanged(); break;
        case 7: _t->unitIdChanged(); break;
        case 8: _t->slaveAddressChanged(); break;
        case 9: _t->timeoutMsChanged(); break;
        case 10: _t->retriesChanged(); break;
        case 11: _t->portInfosChanged(); break;
        case 12: _t->tasksChanged(); break;
        case 13: _t->pollingChanged(); break;
        case 14: _t->registerMapChanged(); break;
        case 15: _t->simulatorChanged(); break;
        case 16: _t->rawResultChanged(); break;
        case 17: _t->statsChanged(); break;
        case 18: _t->toastRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 19: _t->refreshPorts(); break;
        case 20: _t->open(); break;
        case 21: _t->close(); break;
        case 22: _t->manualRead((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 23: _t->manualWrite((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 24: _t->addTask((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 25: _t->updateTask((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 26: _t->removeTask((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 27: { QVariantMap _r = _t->taskAt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 28: _t->startPolling(); break;
        case 29: _t->stopPolling(); break;
        case 30: _t->readTaskOnce((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 31: { bool _r = _t->importTasksFromCsv((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 32: { QVariantMap _r = _t->previewWrite((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 33: _t->sendRawFrame((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 34: _t->closeRawLink(); break;
        case 35: _t->configureSimulatorArea((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[5]))); break;
        case 36: _t->writeSimulatorRegister((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
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
            if (_t _q_method = &ModbusManager::openChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::portNameChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::baudRateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::parityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::hostChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::portChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::unitIdChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::slaveAddressChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::timeoutMsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::retriesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::portInfosChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::tasksChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::pollingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::registerMapChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::simulatorChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::rawResultChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)();
            if (_t _q_method = &ModbusManager::statsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(const QString & , bool );
            if (_t _q_method = &ModbusManager::toastRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TrafficModel* >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TrafficModel**>(_v) = _t->traffic(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->mode(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isOpen(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->portName(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->baudRate(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->parity(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->host(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->port(); break;
        case 8: *reinterpret_cast< int*>(_v) = _t->unitId(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->slaveAddress(); break;
        case 10: *reinterpret_cast< int*>(_v) = _t->timeoutMs(); break;
        case 11: *reinterpret_cast< int*>(_v) = _t->retries(); break;
        case 12: *reinterpret_cast< QVariantList*>(_v) = _t->portInfos(); break;
        case 13: *reinterpret_cast< QVariantList*>(_v) = _t->tasks(); break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->polling(); break;
        case 15: *reinterpret_cast< QVariantList*>(_v) = _t->registerMap(); break;
        case 16: *reinterpret_cast< bool*>(_v) = _t->simulatorRunning(); break;
        case 17: *reinterpret_cast< QVariantList*>(_v) = _t->simulatorRegisters(); break;
        case 18: *reinterpret_cast< QVariantMap*>(_v) = _t->rawResult(); break;
        case 19: *reinterpret_cast< bool*>(_v) = _t->rawBusy(); break;
        case 20: *reinterpret_cast< qint64*>(_v) = _t->txCount(); break;
        case 21: *reinterpret_cast< qint64*>(_v) = _t->rxCount(); break;
        case 22: *reinterpret_cast< qint64*>(_v) = _t->errorCount(); break;
        case 23: *reinterpret_cast< int*>(_v) = _t->lastResponseMs(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setMode(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setPortName(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setBaudRate(*reinterpret_cast< QString*>(_v)); break;
        case 5: _t->setParity(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setHost(*reinterpret_cast< QString*>(_v)); break;
        case 7: _t->setPort(*reinterpret_cast< int*>(_v)); break;
        case 8: _t->setUnitId(*reinterpret_cast< int*>(_v)); break;
        case 9: _t->setSlaveAddress(*reinterpret_cast< int*>(_v)); break;
        case 10: _t->setTimeoutMs(*reinterpret_cast< int*>(_v)); break;
        case 11: _t->setRetries(*reinterpret_cast< int*>(_v)); break;
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
        if (_id < 37)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 37;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 37)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 37;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}

// SIGNAL 0
void ModbusManager::modeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModbusManager::openChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusManager::portNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ModbusManager::baudRateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ModbusManager::parityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ModbusManager::hostChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ModbusManager::portChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ModbusManager::unitIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ModbusManager::slaveAddressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ModbusManager::timeoutMsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ModbusManager::retriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void ModbusManager::portInfosChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void ModbusManager::tasksChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void ModbusManager::pollingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ModbusManager::registerMapChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void ModbusManager::simulatorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void ModbusManager::rawResultChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void ModbusManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void ModbusManager::toastRequested(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_WARNING_POP
