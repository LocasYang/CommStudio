/****************************************************************************
** Meta object code from reading C++ file 'serialmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/managers/serialmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialmanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSSerialManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSerialManagerENDCLASS = QtMocHelpers::stringData(
    "SerialManager",
    "portInfosChanged",
    "",
    "portNameChanged",
    "baudRateChanged",
    "dataBitsChanged",
    "parityChanged",
    "stopBitsChanged",
    "flowControlChanged",
    "openChanged",
    "reconnectingChanged",
    "configChanged",
    "autoReconnectChanged",
    "hexSendChanged",
    "escapeEnabledChanged",
    "lineEndingChanged",
    "pinsChanged",
    "statsChanged",
    "timerChanged",
    "fileChanged",
    "toastRequested",
    "text",
    "ok",
    "refreshPorts",
    "openConnection",
    "close",
    "send",
    "startTimerSend",
    "intervalMs",
    "repeatCount",
    "stopTimerSend",
    "sendFile",
    "path",
    "chunkSize",
    "cancelFileSend",
    "sendBreak",
    "durationMs",
    "traffic",
    "TrafficModel*",
    "portInfos",
    "portName",
    "baudRate",
    "dataBits",
    "parity",
    "stopBits",
    "flowControl",
    "open",
    "connecting",
    "configLocked",
    "reconnecting",
    "configSummary",
    "autoReconnect",
    "hexSend",
    "escapeEnabled",
    "lineEnding",
    "dtr",
    "rts",
    "pinCts",
    "pinDsr",
    "pinDcd",
    "pinRi",
    "rxBytes",
    "txBytes",
    "rxRate",
    "peakRate",
    "timerActive",
    "timerRemaining",
    "fileActive",
    "fileProgress",
    "fileRate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSSerialManagerENDCLASS_t {
    uint offsetsAndSizes[140];
    char stringdata0[14];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[16];
    char stringdata5[16];
    char stringdata6[14];
    char stringdata7[16];
    char stringdata8[19];
    char stringdata9[12];
    char stringdata10[20];
    char stringdata11[14];
    char stringdata12[21];
    char stringdata13[15];
    char stringdata14[21];
    char stringdata15[18];
    char stringdata16[12];
    char stringdata17[13];
    char stringdata18[13];
    char stringdata19[12];
    char stringdata20[15];
    char stringdata21[5];
    char stringdata22[3];
    char stringdata23[13];
    char stringdata24[15];
    char stringdata25[6];
    char stringdata26[5];
    char stringdata27[15];
    char stringdata28[11];
    char stringdata29[12];
    char stringdata30[14];
    char stringdata31[9];
    char stringdata32[5];
    char stringdata33[10];
    char stringdata34[15];
    char stringdata35[10];
    char stringdata36[11];
    char stringdata37[8];
    char stringdata38[14];
    char stringdata39[10];
    char stringdata40[9];
    char stringdata41[9];
    char stringdata42[9];
    char stringdata43[7];
    char stringdata44[9];
    char stringdata45[12];
    char stringdata46[5];
    char stringdata47[11];
    char stringdata48[13];
    char stringdata49[13];
    char stringdata50[14];
    char stringdata51[14];
    char stringdata52[8];
    char stringdata53[14];
    char stringdata54[11];
    char stringdata55[4];
    char stringdata56[4];
    char stringdata57[7];
    char stringdata58[7];
    char stringdata59[7];
    char stringdata60[6];
    char stringdata61[8];
    char stringdata62[8];
    char stringdata63[7];
    char stringdata64[9];
    char stringdata65[12];
    char stringdata66[15];
    char stringdata67[11];
    char stringdata68[13];
    char stringdata69[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSSerialManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSSerialManagerENDCLASS_t qt_meta_stringdata_CLASSSerialManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13),  // "SerialManager"
        QT_MOC_LITERAL(14, 16),  // "portInfosChanged"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 15),  // "portNameChanged"
        QT_MOC_LITERAL(48, 15),  // "baudRateChanged"
        QT_MOC_LITERAL(64, 15),  // "dataBitsChanged"
        QT_MOC_LITERAL(80, 13),  // "parityChanged"
        QT_MOC_LITERAL(94, 15),  // "stopBitsChanged"
        QT_MOC_LITERAL(110, 18),  // "flowControlChanged"
        QT_MOC_LITERAL(129, 11),  // "openChanged"
        QT_MOC_LITERAL(141, 19),  // "reconnectingChanged"
        QT_MOC_LITERAL(161, 13),  // "configChanged"
        QT_MOC_LITERAL(175, 20),  // "autoReconnectChanged"
        QT_MOC_LITERAL(196, 14),  // "hexSendChanged"
        QT_MOC_LITERAL(211, 20),  // "escapeEnabledChanged"
        QT_MOC_LITERAL(232, 17),  // "lineEndingChanged"
        QT_MOC_LITERAL(250, 11),  // "pinsChanged"
        QT_MOC_LITERAL(262, 12),  // "statsChanged"
        QT_MOC_LITERAL(275, 12),  // "timerChanged"
        QT_MOC_LITERAL(288, 11),  // "fileChanged"
        QT_MOC_LITERAL(300, 14),  // "toastRequested"
        QT_MOC_LITERAL(315, 4),  // "text"
        QT_MOC_LITERAL(320, 2),  // "ok"
        QT_MOC_LITERAL(323, 12),  // "refreshPorts"
        QT_MOC_LITERAL(336, 14),  // "openConnection"
        QT_MOC_LITERAL(351, 5),  // "close"
        QT_MOC_LITERAL(357, 4),  // "send"
        QT_MOC_LITERAL(362, 14),  // "startTimerSend"
        QT_MOC_LITERAL(377, 10),  // "intervalMs"
        QT_MOC_LITERAL(388, 11),  // "repeatCount"
        QT_MOC_LITERAL(400, 13),  // "stopTimerSend"
        QT_MOC_LITERAL(414, 8),  // "sendFile"
        QT_MOC_LITERAL(423, 4),  // "path"
        QT_MOC_LITERAL(428, 9),  // "chunkSize"
        QT_MOC_LITERAL(438, 14),  // "cancelFileSend"
        QT_MOC_LITERAL(453, 9),  // "sendBreak"
        QT_MOC_LITERAL(463, 10),  // "durationMs"
        QT_MOC_LITERAL(474, 7),  // "traffic"
        QT_MOC_LITERAL(482, 13),  // "TrafficModel*"
        QT_MOC_LITERAL(496, 9),  // "portInfos"
        QT_MOC_LITERAL(506, 8),  // "portName"
        QT_MOC_LITERAL(515, 8),  // "baudRate"
        QT_MOC_LITERAL(524, 8),  // "dataBits"
        QT_MOC_LITERAL(533, 6),  // "parity"
        QT_MOC_LITERAL(540, 8),  // "stopBits"
        QT_MOC_LITERAL(549, 11),  // "flowControl"
        QT_MOC_LITERAL(561, 4),  // "open"
        QT_MOC_LITERAL(566, 10),  // "connecting"
        QT_MOC_LITERAL(577, 12),  // "configLocked"
        QT_MOC_LITERAL(590, 12),  // "reconnecting"
        QT_MOC_LITERAL(603, 13),  // "configSummary"
        QT_MOC_LITERAL(617, 13),  // "autoReconnect"
        QT_MOC_LITERAL(631, 7),  // "hexSend"
        QT_MOC_LITERAL(639, 13),  // "escapeEnabled"
        QT_MOC_LITERAL(653, 10),  // "lineEnding"
        QT_MOC_LITERAL(664, 3),  // "dtr"
        QT_MOC_LITERAL(668, 3),  // "rts"
        QT_MOC_LITERAL(672, 6),  // "pinCts"
        QT_MOC_LITERAL(679, 6),  // "pinDsr"
        QT_MOC_LITERAL(686, 6),  // "pinDcd"
        QT_MOC_LITERAL(693, 5),  // "pinRi"
        QT_MOC_LITERAL(699, 7),  // "rxBytes"
        QT_MOC_LITERAL(707, 7),  // "txBytes"
        QT_MOC_LITERAL(715, 6),  // "rxRate"
        QT_MOC_LITERAL(722, 8),  // "peakRate"
        QT_MOC_LITERAL(731, 11),  // "timerActive"
        QT_MOC_LITERAL(743, 14),  // "timerRemaining"
        QT_MOC_LITERAL(758, 10),  // "fileActive"
        QT_MOC_LITERAL(769, 12),  // "fileProgress"
        QT_MOC_LITERAL(782, 8)   // "fileRate"
    },
    "SerialManager",
    "portInfosChanged",
    "",
    "portNameChanged",
    "baudRateChanged",
    "dataBitsChanged",
    "parityChanged",
    "stopBitsChanged",
    "flowControlChanged",
    "openChanged",
    "reconnectingChanged",
    "configChanged",
    "autoReconnectChanged",
    "hexSendChanged",
    "escapeEnabledChanged",
    "lineEndingChanged",
    "pinsChanged",
    "statsChanged",
    "timerChanged",
    "fileChanged",
    "toastRequested",
    "text",
    "ok",
    "refreshPorts",
    "openConnection",
    "close",
    "send",
    "startTimerSend",
    "intervalMs",
    "repeatCount",
    "stopTimerSend",
    "sendFile",
    "path",
    "chunkSize",
    "cancelFileSend",
    "sendBreak",
    "durationMs",
    "traffic",
    "TrafficModel*",
    "portInfos",
    "portName",
    "baudRate",
    "dataBits",
    "parity",
    "stopBits",
    "flowControl",
    "open",
    "connecting",
    "configLocked",
    "reconnecting",
    "configSummary",
    "autoReconnect",
    "hexSend",
    "escapeEnabled",
    "lineEnding",
    "dtr",
    "rts",
    "pinCts",
    "pinDsr",
    "pinDcd",
    "pinRi",
    "rxBytes",
    "txBytes",
    "rxRate",
    "peakRate",
    "timerActive",
    "timerRemaining",
    "fileActive",
    "fileProgress",
    "fileRate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSerialManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
      32,  237, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  188,    2, 0x06,   33 /* Public */,
       3,    0,  189,    2, 0x06,   34 /* Public */,
       4,    0,  190,    2, 0x06,   35 /* Public */,
       5,    0,  191,    2, 0x06,   36 /* Public */,
       6,    0,  192,    2, 0x06,   37 /* Public */,
       7,    0,  193,    2, 0x06,   38 /* Public */,
       8,    0,  194,    2, 0x06,   39 /* Public */,
       9,    0,  195,    2, 0x06,   40 /* Public */,
      10,    0,  196,    2, 0x06,   41 /* Public */,
      11,    0,  197,    2, 0x06,   42 /* Public */,
      12,    0,  198,    2, 0x06,   43 /* Public */,
      13,    0,  199,    2, 0x06,   44 /* Public */,
      14,    0,  200,    2, 0x06,   45 /* Public */,
      15,    0,  201,    2, 0x06,   46 /* Public */,
      16,    0,  202,    2, 0x06,   47 /* Public */,
      17,    0,  203,    2, 0x06,   48 /* Public */,
      18,    0,  204,    2, 0x06,   49 /* Public */,
      19,    0,  205,    2, 0x06,   50 /* Public */,
      20,    2,  206,    2, 0x06,   51 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      23,    0,  211,    2, 0x02,   54 /* Public */,
      24,    0,  212,    2, 0x02,   55 /* Public */,
      25,    0,  213,    2, 0x02,   56 /* Public */,
      26,    1,  214,    2, 0x02,   57 /* Public */,
      27,    3,  217,    2, 0x02,   59 /* Public */,
      30,    0,  224,    2, 0x02,   63 /* Public */,
      31,    3,  225,    2, 0x02,   64 /* Public */,
      34,    0,  232,    2, 0x02,   68 /* Public */,
      35,    1,  233,    2, 0x02,   69 /* Public */,
      35,    0,  236,    2, 0x22,   71 /* Public | MethodCloned */,

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
    QMetaType::QString, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   21,   28,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl, QMetaType::Int, QMetaType::Int,   32,   33,   28,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void,

 // properties: name, type, flags
      37, 0x80000000 | 38, 0x00015409, uint(-1), 0,
      39, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      40, QMetaType::QString, 0x00015103, uint(1), 0,
      41, QMetaType::QString, 0x00015103, uint(2), 0,
      42, QMetaType::Int, 0x00015103, uint(3), 0,
      43, QMetaType::QString, 0x00015103, uint(4), 0,
      44, QMetaType::Double, 0x00015103, uint(5), 0,
      45, QMetaType::QString, 0x00015103, uint(6), 0,
      46, QMetaType::Bool, 0x00015001, uint(7), 0,
      47, QMetaType::Bool, 0x00015001, uint(7), 0,
      48, QMetaType::Bool, 0x00015001, uint(7), 0,
      49, QMetaType::Bool, 0x00015001, uint(8), 0,
      50, QMetaType::QString, 0x00015001, uint(9), 0,
      51, QMetaType::Bool, 0x00015103, uint(10), 0,
      52, QMetaType::Bool, 0x00015103, uint(11), 0,
      53, QMetaType::Bool, 0x00015103, uint(12), 0,
      54, QMetaType::QString, 0x00015103, uint(13), 0,
      55, QMetaType::Bool, 0x00015103, uint(14), 0,
      56, QMetaType::Bool, 0x00015103, uint(14), 0,
      57, QMetaType::Bool, 0x00015001, uint(14), 0,
      58, QMetaType::Bool, 0x00015001, uint(14), 0,
      59, QMetaType::Bool, 0x00015001, uint(14), 0,
      60, QMetaType::Bool, 0x00015001, uint(14), 0,
      61, QMetaType::LongLong, 0x00015001, uint(15), 0,
      62, QMetaType::LongLong, 0x00015001, uint(15), 0,
      63, QMetaType::LongLong, 0x00015001, uint(15), 0,
      64, QMetaType::LongLong, 0x00015001, uint(15), 0,
      65, QMetaType::Bool, 0x00015001, uint(16), 0,
      66, QMetaType::Int, 0x00015001, uint(16), 0,
      67, QMetaType::Bool, 0x00015001, uint(17), 0,
      68, QMetaType::Int, 0x00015001, uint(17), 0,
      69, QMetaType::QString, 0x00015001, uint(17), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject SerialManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSSerialManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSerialManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSerialManagerENDCLASS_t,
        // property 'traffic'
        QtPrivate::TypeAndForceComplete<TrafficModel*, std::true_type>,
        // property 'portInfos'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'portName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'baudRate'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'dataBits'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'parity'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'stopBits'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'flowControl'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'open'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'connecting'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'configLocked'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'reconnecting'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'configSummary'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'autoReconnect'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'hexSend'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'escapeEnabled'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'lineEnding'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'dtr'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'rts'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'pinCts'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'pinDsr'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'pinDcd'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'pinRi'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'rxBytes'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'txBytes'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'rxRate'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'peakRate'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'timerActive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'timerRemaining'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'fileActive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'fileProgress'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'fileRate'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SerialManager, std::true_type>,
        // method 'portInfosChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'portNameChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'baudRateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dataBitsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'parityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopBitsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'flowControlChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reconnectingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'configChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'autoReconnectChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'hexSendChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'escapeEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lineEndingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pinsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'timerChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fileChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toastRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'refreshPorts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'close'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'startTimerSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'stopTimerSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'cancelFileSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendBreak'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sendBreak'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SerialManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->portInfosChanged(); break;
        case 1: _t->portNameChanged(); break;
        case 2: _t->baudRateChanged(); break;
        case 3: _t->dataBitsChanged(); break;
        case 4: _t->parityChanged(); break;
        case 5: _t->stopBitsChanged(); break;
        case 6: _t->flowControlChanged(); break;
        case 7: _t->openChanged(); break;
        case 8: _t->reconnectingChanged(); break;
        case 9: _t->configChanged(); break;
        case 10: _t->autoReconnectChanged(); break;
        case 11: _t->hexSendChanged(); break;
        case 12: _t->escapeEnabledChanged(); break;
        case 13: _t->lineEndingChanged(); break;
        case 14: _t->pinsChanged(); break;
        case 15: _t->statsChanged(); break;
        case 16: _t->timerChanged(); break;
        case 17: _t->fileChanged(); break;
        case 18: _t->toastRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 19: _t->refreshPorts(); break;
        case 20: _t->openConnection(); break;
        case 21: _t->close(); break;
        case 22: { QString _r = _t->send((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 23: _t->startTimerSend((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 24: _t->stopTimerSend(); break;
        case 25: _t->sendFile((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 26: _t->cancelFileSend(); break;
        case 27: _t->sendBreak((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->sendBreak(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::portInfosChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::portNameChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::baudRateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::dataBitsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::parityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::stopBitsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::flowControlChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::openChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::reconnectingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::configChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::autoReconnectChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::hexSendChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::escapeEnabledChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::lineEndingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::pinsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::statsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::timerChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (_t _q_method = &SerialManager::fileChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QString & , bool );
            if (_t _q_method = &SerialManager::toastRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
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
        auto *_t = static_cast<SerialManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TrafficModel**>(_v) = _t->traffic(); break;
        case 1: *reinterpret_cast< QVariantList*>(_v) = _t->portInfos(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->portName(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->baudRate(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->dataBits(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->parity(); break;
        case 6: *reinterpret_cast< double*>(_v) = _t->stopBits(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->flowControl(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->isOpen(); break;
        case 9: *reinterpret_cast< bool*>(_v) = _t->isConnecting(); break;
        case 10: *reinterpret_cast< bool*>(_v) = _t->configLocked(); break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->reconnecting(); break;
        case 12: *reinterpret_cast< QString*>(_v) = _t->configSummary(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->autoReconnect(); break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->hexSend(); break;
        case 15: *reinterpret_cast< bool*>(_v) = _t->escapeEnabled(); break;
        case 16: *reinterpret_cast< QString*>(_v) = _t->lineEnding(); break;
        case 17: *reinterpret_cast< bool*>(_v) = _t->dtr(); break;
        case 18: *reinterpret_cast< bool*>(_v) = _t->rts(); break;
        case 19: *reinterpret_cast< bool*>(_v) = _t->pinCts(); break;
        case 20: *reinterpret_cast< bool*>(_v) = _t->pinDsr(); break;
        case 21: *reinterpret_cast< bool*>(_v) = _t->pinDcd(); break;
        case 22: *reinterpret_cast< bool*>(_v) = _t->pinRi(); break;
        case 23: *reinterpret_cast< qint64*>(_v) = _t->rxBytes(); break;
        case 24: *reinterpret_cast< qint64*>(_v) = _t->txBytes(); break;
        case 25: *reinterpret_cast< qint64*>(_v) = _t->rxRate(); break;
        case 26: *reinterpret_cast< qint64*>(_v) = _t->peakRate(); break;
        case 27: *reinterpret_cast< bool*>(_v) = _t->timerActive(); break;
        case 28: *reinterpret_cast< int*>(_v) = _t->timerRemaining(); break;
        case 29: *reinterpret_cast< bool*>(_v) = _t->fileActive(); break;
        case 30: *reinterpret_cast< int*>(_v) = _t->fileProgress(); break;
        case 31: *reinterpret_cast< QString*>(_v) = _t->fileRate(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<SerialManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 2: _t->setPortName(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setBaudRate(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setDataBits(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setParity(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setStopBits(*reinterpret_cast< double*>(_v)); break;
        case 7: _t->setFlowControl(*reinterpret_cast< QString*>(_v)); break;
        case 13: _t->setAutoReconnect(*reinterpret_cast< bool*>(_v)); break;
        case 14: _t->setHexSend(*reinterpret_cast< bool*>(_v)); break;
        case 15: _t->setEscapeEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 16: _t->setLineEnding(*reinterpret_cast< QString*>(_v)); break;
        case 17: _t->setDtr(*reinterpret_cast< bool*>(_v)); break;
        case 18: _t->setRts(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *SerialManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSerialManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 29;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void SerialManager::portInfosChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SerialManager::portNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SerialManager::baudRateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SerialManager::dataBitsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SerialManager::parityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SerialManager::stopBitsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SerialManager::flowControlChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void SerialManager::openChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void SerialManager::reconnectingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void SerialManager::configChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void SerialManager::autoReconnectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void SerialManager::hexSendChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void SerialManager::escapeEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void SerialManager::lineEndingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void SerialManager::pinsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void SerialManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void SerialManager::timerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void SerialManager::fileChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void SerialManager::toastRequested(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_WARNING_POP
