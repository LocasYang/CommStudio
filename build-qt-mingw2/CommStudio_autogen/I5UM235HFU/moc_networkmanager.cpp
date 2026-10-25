/****************************************************************************
** Meta object code from reading C++ file 'networkmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/managers/networkmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'networkmanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSNetworkManagerENDCLASS = QtMocHelpers::stringData(
    "NetworkManager",
    "modeChanged",
    "",
    "openChanged",
    "bindAddressChanged",
    "remoteHostChanged",
    "remotePortChanged",
    "listenPortChanged",
    "autoReconnectChanged",
    "reuseAddressChanged",
    "broadcastChanged",
    "multicastGroupChanged",
    "clientsChanged",
    "framingChanged",
    "hexSendChanged",
    "escapeEnabledChanged",
    "lineEndingChanged",
    "replyRulesChanged",
    "statsChanged",
    "timerChanged",
    "toastRequested",
    "text",
    "ok",
    "openConnection",
    "close",
    "send",
    "targetClientId",
    "sendTo",
    "host",
    "port",
    "disconnectClient",
    "clientId",
    "joinMulticast",
    "group",
    "leaveMulticast",
    "startTimerSend",
    "intervalMs",
    "repeatCount",
    "stopTimerSend",
    "traffic",
    "TrafficModel*",
    "mode",
    "open",
    "interfaces",
    "bindAddress",
    "remoteHost",
    "remotePort",
    "listenPort",
    "autoReconnect",
    "reuseAddress",
    "broadcast",
    "multicastGroup",
    "multicastJoined",
    "clients",
    "framing",
    "hexSend",
    "escapeEnabled",
    "lineEnding",
    "replyRules",
    "rxBytes",
    "txBytes",
    "rxRate",
    "timerActive",
    "timerRemaining",
    "stateText"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t {
    uint offsetsAndSizes[130];
    char stringdata0[15];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[19];
    char stringdata5[18];
    char stringdata6[18];
    char stringdata7[18];
    char stringdata8[21];
    char stringdata9[20];
    char stringdata10[17];
    char stringdata11[22];
    char stringdata12[15];
    char stringdata13[15];
    char stringdata14[15];
    char stringdata15[21];
    char stringdata16[18];
    char stringdata17[18];
    char stringdata18[13];
    char stringdata19[13];
    char stringdata20[15];
    char stringdata21[5];
    char stringdata22[3];
    char stringdata23[15];
    char stringdata24[6];
    char stringdata25[5];
    char stringdata26[15];
    char stringdata27[7];
    char stringdata28[5];
    char stringdata29[5];
    char stringdata30[17];
    char stringdata31[9];
    char stringdata32[14];
    char stringdata33[6];
    char stringdata34[15];
    char stringdata35[15];
    char stringdata36[11];
    char stringdata37[12];
    char stringdata38[14];
    char stringdata39[8];
    char stringdata40[14];
    char stringdata41[5];
    char stringdata42[5];
    char stringdata43[11];
    char stringdata44[12];
    char stringdata45[11];
    char stringdata46[11];
    char stringdata47[11];
    char stringdata48[14];
    char stringdata49[13];
    char stringdata50[10];
    char stringdata51[15];
    char stringdata52[16];
    char stringdata53[8];
    char stringdata54[8];
    char stringdata55[8];
    char stringdata56[14];
    char stringdata57[11];
    char stringdata58[11];
    char stringdata59[8];
    char stringdata60[8];
    char stringdata61[7];
    char stringdata62[12];
    char stringdata63[15];
    char stringdata64[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t qt_meta_stringdata_CLASSNetworkManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "NetworkManager"
        QT_MOC_LITERAL(15, 11),  // "modeChanged"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 11),  // "openChanged"
        QT_MOC_LITERAL(40, 18),  // "bindAddressChanged"
        QT_MOC_LITERAL(59, 17),  // "remoteHostChanged"
        QT_MOC_LITERAL(77, 17),  // "remotePortChanged"
        QT_MOC_LITERAL(95, 17),  // "listenPortChanged"
        QT_MOC_LITERAL(113, 20),  // "autoReconnectChanged"
        QT_MOC_LITERAL(134, 19),  // "reuseAddressChanged"
        QT_MOC_LITERAL(154, 16),  // "broadcastChanged"
        QT_MOC_LITERAL(171, 21),  // "multicastGroupChanged"
        QT_MOC_LITERAL(193, 14),  // "clientsChanged"
        QT_MOC_LITERAL(208, 14),  // "framingChanged"
        QT_MOC_LITERAL(223, 14),  // "hexSendChanged"
        QT_MOC_LITERAL(238, 20),  // "escapeEnabledChanged"
        QT_MOC_LITERAL(259, 17),  // "lineEndingChanged"
        QT_MOC_LITERAL(277, 17),  // "replyRulesChanged"
        QT_MOC_LITERAL(295, 12),  // "statsChanged"
        QT_MOC_LITERAL(308, 12),  // "timerChanged"
        QT_MOC_LITERAL(321, 14),  // "toastRequested"
        QT_MOC_LITERAL(336, 4),  // "text"
        QT_MOC_LITERAL(341, 2),  // "ok"
        QT_MOC_LITERAL(344, 14),  // "openConnection"
        QT_MOC_LITERAL(359, 5),  // "close"
        QT_MOC_LITERAL(365, 4),  // "send"
        QT_MOC_LITERAL(370, 14),  // "targetClientId"
        QT_MOC_LITERAL(385, 6),  // "sendTo"
        QT_MOC_LITERAL(392, 4),  // "host"
        QT_MOC_LITERAL(397, 4),  // "port"
        QT_MOC_LITERAL(402, 16),  // "disconnectClient"
        QT_MOC_LITERAL(419, 8),  // "clientId"
        QT_MOC_LITERAL(428, 13),  // "joinMulticast"
        QT_MOC_LITERAL(442, 5),  // "group"
        QT_MOC_LITERAL(448, 14),  // "leaveMulticast"
        QT_MOC_LITERAL(463, 14),  // "startTimerSend"
        QT_MOC_LITERAL(478, 10),  // "intervalMs"
        QT_MOC_LITERAL(489, 11),  // "repeatCount"
        QT_MOC_LITERAL(501, 13),  // "stopTimerSend"
        QT_MOC_LITERAL(515, 7),  // "traffic"
        QT_MOC_LITERAL(523, 13),  // "TrafficModel*"
        QT_MOC_LITERAL(537, 4),  // "mode"
        QT_MOC_LITERAL(542, 4),  // "open"
        QT_MOC_LITERAL(547, 10),  // "interfaces"
        QT_MOC_LITERAL(558, 11),  // "bindAddress"
        QT_MOC_LITERAL(570, 10),  // "remoteHost"
        QT_MOC_LITERAL(581, 10),  // "remotePort"
        QT_MOC_LITERAL(592, 10),  // "listenPort"
        QT_MOC_LITERAL(603, 13),  // "autoReconnect"
        QT_MOC_LITERAL(617, 12),  // "reuseAddress"
        QT_MOC_LITERAL(630, 9),  // "broadcast"
        QT_MOC_LITERAL(640, 14),  // "multicastGroup"
        QT_MOC_LITERAL(655, 15),  // "multicastJoined"
        QT_MOC_LITERAL(671, 7),  // "clients"
        QT_MOC_LITERAL(679, 7),  // "framing"
        QT_MOC_LITERAL(687, 7),  // "hexSend"
        QT_MOC_LITERAL(695, 13),  // "escapeEnabled"
        QT_MOC_LITERAL(709, 10),  // "lineEnding"
        QT_MOC_LITERAL(720, 10),  // "replyRules"
        QT_MOC_LITERAL(731, 7),  // "rxBytes"
        QT_MOC_LITERAL(739, 7),  // "txBytes"
        QT_MOC_LITERAL(747, 6),  // "rxRate"
        QT_MOC_LITERAL(754, 11),  // "timerActive"
        QT_MOC_LITERAL(766, 14),  // "timerRemaining"
        QT_MOC_LITERAL(781, 9)   // "stateText"
    },
    "NetworkManager",
    "modeChanged",
    "",
    "openChanged",
    "bindAddressChanged",
    "remoteHostChanged",
    "remotePortChanged",
    "listenPortChanged",
    "autoReconnectChanged",
    "reuseAddressChanged",
    "broadcastChanged",
    "multicastGroupChanged",
    "clientsChanged",
    "framingChanged",
    "hexSendChanged",
    "escapeEnabledChanged",
    "lineEndingChanged",
    "replyRulesChanged",
    "statsChanged",
    "timerChanged",
    "toastRequested",
    "text",
    "ok",
    "openConnection",
    "close",
    "send",
    "targetClientId",
    "sendTo",
    "host",
    "port",
    "disconnectClient",
    "clientId",
    "joinMulticast",
    "group",
    "leaveMulticast",
    "startTimerSend",
    "intervalMs",
    "repeatCount",
    "stopTimerSend",
    "traffic",
    "TrafficModel*",
    "mode",
    "open",
    "interfaces",
    "bindAddress",
    "remoteHost",
    "remotePort",
    "listenPort",
    "autoReconnect",
    "reuseAddress",
    "broadcast",
    "multicastGroup",
    "multicastJoined",
    "clients",
    "framing",
    "hexSend",
    "escapeEnabled",
    "lineEnding",
    "replyRules",
    "rxBytes",
    "txBytes",
    "rxRate",
    "timerActive",
    "timerRemaining",
    "stateText"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSNetworkManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
      25,  243, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  188,    2, 0x06,   26 /* Public */,
       3,    0,  189,    2, 0x06,   27 /* Public */,
       4,    0,  190,    2, 0x06,   28 /* Public */,
       5,    0,  191,    2, 0x06,   29 /* Public */,
       6,    0,  192,    2, 0x06,   30 /* Public */,
       7,    0,  193,    2, 0x06,   31 /* Public */,
       8,    0,  194,    2, 0x06,   32 /* Public */,
       9,    0,  195,    2, 0x06,   33 /* Public */,
      10,    0,  196,    2, 0x06,   34 /* Public */,
      11,    0,  197,    2, 0x06,   35 /* Public */,
      12,    0,  198,    2, 0x06,   36 /* Public */,
      13,    0,  199,    2, 0x06,   37 /* Public */,
      14,    0,  200,    2, 0x06,   38 /* Public */,
      15,    0,  201,    2, 0x06,   39 /* Public */,
      16,    0,  202,    2, 0x06,   40 /* Public */,
      17,    0,  203,    2, 0x06,   41 /* Public */,
      18,    0,  204,    2, 0x06,   42 /* Public */,
      19,    0,  205,    2, 0x06,   43 /* Public */,
      20,    2,  206,    2, 0x06,   44 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      23,    0,  211,    2, 0x02,   47 /* Public */,
      24,    0,  212,    2, 0x02,   48 /* Public */,
      25,    2,  213,    2, 0x02,   49 /* Public */,
      25,    1,  218,    2, 0x22,   52 /* Public | MethodCloned */,
      27,    3,  221,    2, 0x02,   54 /* Public */,
      30,    1,  228,    2, 0x02,   58 /* Public */,
      32,    1,  231,    2, 0x02,   60 /* Public */,
      34,    0,  234,    2, 0x02,   62 /* Public */,
      35,    3,  235,    2, 0x02,   63 /* Public */,
      38,    0,  242,    2, 0x02,   67 /* Public */,

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
    QMetaType::QString, QMetaType::QString, QMetaType::LongLong,   21,   26,
    QMetaType::QString, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,   28,   29,   21,
    QMetaType::Void, QMetaType::LongLong,   31,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   21,   36,   37,
    QMetaType::Void,

 // properties: name, type, flags
      39, 0x80000000 | 40, 0x00015409, uint(-1), 0,
      41, QMetaType::QString, 0x00015103, uint(0), 0,
      42, QMetaType::Bool, 0x00015001, uint(1), 0,
      43, QMetaType::QVariantList, 0x00015401, uint(-1), 0,
      44, QMetaType::QString, 0x00015103, uint(2), 0,
      45, QMetaType::QString, 0x00015103, uint(3), 0,
      46, QMetaType::Int, 0x00015103, uint(4), 0,
      47, QMetaType::Int, 0x00015103, uint(5), 0,
      48, QMetaType::Bool, 0x00015103, uint(6), 0,
      49, QMetaType::Bool, 0x00015103, uint(7), 0,
      50, QMetaType::Bool, 0x00015103, uint(8), 0,
      51, QMetaType::QString, 0x00015103, uint(9), 0,
      52, QMetaType::Bool, 0x00015001, uint(1), 0,
      53, QMetaType::QVariantList, 0x00015001, uint(10), 0,
      54, QMetaType::QString, 0x00015103, uint(11), 0,
      55, QMetaType::Bool, 0x00015103, uint(12), 0,
      56, QMetaType::Bool, 0x00015103, uint(13), 0,
      57, QMetaType::QString, 0x00015103, uint(14), 0,
      58, QMetaType::QVariantList, 0x00015103, uint(15), 0,
      59, QMetaType::LongLong, 0x00015001, uint(16), 0,
      60, QMetaType::LongLong, 0x00015001, uint(16), 0,
      61, QMetaType::LongLong, 0x00015001, uint(16), 0,
      62, QMetaType::Bool, 0x00015001, uint(17), 0,
      63, QMetaType::Int, 0x00015001, uint(17), 0,
      64, QMetaType::QString, 0x00015001, uint(1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject NetworkManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSNetworkManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSNetworkManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t,
        // property 'traffic'
        QtPrivate::TypeAndForceComplete<TrafficModel*, std::true_type>,
        // property 'mode'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'open'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'interfaces'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'bindAddress'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'remoteHost'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'remotePort'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'listenPort'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'autoReconnect'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'reuseAddress'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'broadcast'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'multicastGroup'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'multicastJoined'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'clients'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'framing'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'hexSend'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'escapeEnabled'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'lineEnding'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'replyRules'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'rxBytes'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'txBytes'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'rxRate'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'timerActive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'timerRemaining'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'stateText'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<NetworkManager, std::true_type>,
        // method 'modeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'bindAddressChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remoteHostChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remotePortChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'listenPortChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'autoReconnectChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reuseAddressChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'broadcastChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'multicastGroupChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clientsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'framingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'hexSendChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'escapeEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lineEndingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'replyRulesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'timerChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toastRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'openConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'close'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'send'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'sendTo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'disconnectClient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'joinMulticast'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'leaveMulticast'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startTimerSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'stopTimerSend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void NetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetworkManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->modeChanged(); break;
        case 1: _t->openChanged(); break;
        case 2: _t->bindAddressChanged(); break;
        case 3: _t->remoteHostChanged(); break;
        case 4: _t->remotePortChanged(); break;
        case 5: _t->listenPortChanged(); break;
        case 6: _t->autoReconnectChanged(); break;
        case 7: _t->reuseAddressChanged(); break;
        case 8: _t->broadcastChanged(); break;
        case 9: _t->multicastGroupChanged(); break;
        case 10: _t->clientsChanged(); break;
        case 11: _t->framingChanged(); break;
        case 12: _t->hexSendChanged(); break;
        case 13: _t->escapeEnabledChanged(); break;
        case 14: _t->lineEndingChanged(); break;
        case 15: _t->replyRulesChanged(); break;
        case 16: _t->statsChanged(); break;
        case 17: _t->timerChanged(); break;
        case 18: _t->toastRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 19: _t->openConnection(); break;
        case 20: _t->close(); break;
        case 21: { QString _r = _t->send((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 22: { QString _r = _t->send((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 23: _t->sendTo((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 24: _t->disconnectClient((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 25: _t->joinMulticast((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->leaveMulticast(); break;
        case 27: _t->startTimerSend((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 28: _t->stopTimerSend(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::modeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::openChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::bindAddressChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::remoteHostChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::remotePortChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::listenPortChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::autoReconnectChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::reuseAddressChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::broadcastChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::multicastGroupChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::clientsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::framingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::hexSendChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::escapeEnabledChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::lineEndingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::replyRulesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::statsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::timerChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const QString & , bool );
            if (_t _q_method = &NetworkManager::toastRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
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
        auto *_t = static_cast<NetworkManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TrafficModel**>(_v) = _t->traffic(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->mode(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isOpen(); break;
        case 3: *reinterpret_cast< QVariantList*>(_v) = _t->interfaces(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->bindAddress(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->remoteHost(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->remotePort(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->listenPort(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->autoReconnect(); break;
        case 9: *reinterpret_cast< bool*>(_v) = _t->reuseAddress(); break;
        case 10: *reinterpret_cast< bool*>(_v) = _t->broadcast(); break;
        case 11: *reinterpret_cast< QString*>(_v) = _t->multicastGroup(); break;
        case 12: *reinterpret_cast< bool*>(_v) = _t->multicastJoined(); break;
        case 13: *reinterpret_cast< QVariantList*>(_v) = _t->clients(); break;
        case 14: *reinterpret_cast< QString*>(_v) = _t->framing(); break;
        case 15: *reinterpret_cast< bool*>(_v) = _t->hexSend(); break;
        case 16: *reinterpret_cast< bool*>(_v) = _t->escapeEnabled(); break;
        case 17: *reinterpret_cast< QString*>(_v) = _t->lineEnding(); break;
        case 18: *reinterpret_cast< QVariantList*>(_v) = _t->replyRules(); break;
        case 19: *reinterpret_cast< qint64*>(_v) = _t->rxBytes(); break;
        case 20: *reinterpret_cast< qint64*>(_v) = _t->txBytes(); break;
        case 21: *reinterpret_cast< qint64*>(_v) = _t->rxRate(); break;
        case 22: *reinterpret_cast< bool*>(_v) = _t->timerActive(); break;
        case 23: *reinterpret_cast< int*>(_v) = _t->timerRemaining(); break;
        case 24: *reinterpret_cast< QString*>(_v) = _t->stateText(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<NetworkManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setMode(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setBindAddress(*reinterpret_cast< QString*>(_v)); break;
        case 5: _t->setRemoteHost(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setRemotePort(*reinterpret_cast< int*>(_v)); break;
        case 7: _t->setListenPort(*reinterpret_cast< int*>(_v)); break;
        case 8: _t->setAutoReconnect(*reinterpret_cast< bool*>(_v)); break;
        case 9: _t->setReuseAddress(*reinterpret_cast< bool*>(_v)); break;
        case 10: _t->setBroadcast(*reinterpret_cast< bool*>(_v)); break;
        case 11: _t->setMulticastGroup(*reinterpret_cast< QString*>(_v)); break;
        case 14: _t->setFraming(*reinterpret_cast< QString*>(_v)); break;
        case 15: _t->setHexSend(*reinterpret_cast< bool*>(_v)); break;
        case 16: _t->setEscapeEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 17: _t->setLineEnding(*reinterpret_cast< QString*>(_v)); break;
        case 18: _t->setReplyRules(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *NetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSNetworkManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void NetworkManager::modeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NetworkManager::openChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkManager::bindAddressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void NetworkManager::remoteHostChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void NetworkManager::remotePortChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void NetworkManager::listenPortChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void NetworkManager::autoReconnectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void NetworkManager::reuseAddressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void NetworkManager::broadcastChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void NetworkManager::multicastGroupChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void NetworkManager::clientsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void NetworkManager::framingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void NetworkManager::hexSendChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void NetworkManager::escapeEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void NetworkManager::lineEndingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void NetworkManager::replyRulesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void NetworkManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void NetworkManager::timerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void NetworkManager::toastRequested(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_WARNING_POP
