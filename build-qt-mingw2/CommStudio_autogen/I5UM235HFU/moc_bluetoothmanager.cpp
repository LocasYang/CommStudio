/****************************************************************************
** Meta object code from reading C++ file 'bluetoothmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/managers/bluetoothmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bluetoothmanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSBluetoothManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBluetoothManagerENDCLASS = QtMocHelpers::stringData(
    "BluetoothManager",
    "adaptersChanged",
    "",
    "scanningChanged",
    "devicesChanged",
    "nameFilterChanged",
    "leOnlyChanged",
    "leStateChanged",
    "servicesChanged",
    "selectedServiceChanged",
    "rfcommChanged",
    "hexSendChanged",
    "escapeEnabledChanged",
    "lineEndingChanged",
    "statsChanged",
    "toastRequested",
    "text",
    "ok",
    "refreshAdapters",
    "startScan",
    "stopScan",
    "clearDevices",
    "connectDevice",
    "index",
    "disconnectDevice",
    "readCharacteristic",
    "serviceIndex",
    "charIndex",
    "writeCharacteristic",
    "hexMode",
    "withResponse",
    "setNotify",
    "enable",
    "indicate",
    "enableUartMode",
    "serviceUuid",
    "txUuid",
    "rxUuid",
    "disableUartMode",
    "send",
    "startRfcommDiscovery",
    "connectRfcomm",
    "disconnectRfcomm",
    "traffic",
    "TrafficModel*",
    "adapters",
    "backendSupported",
    "scanning",
    "devices",
    "nameFilter",
    "leOnly",
    "leState",
    "connectedDeviceName",
    "mtu",
    "rssi",
    "services",
    "selectedService",
    "rfcommConnected",
    "rfcommDiscovering",
    "rfcommServices",
    "hexSend",
    "escapeEnabled",
    "lineEnding",
    "rxBytes",
    "txBytes",
    "notifyActive"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSBluetoothManagerENDCLASS_t {
    uint offsetsAndSizes[132];
    char stringdata0[17];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[15];
    char stringdata5[18];
    char stringdata6[14];
    char stringdata7[15];
    char stringdata8[16];
    char stringdata9[23];
    char stringdata10[14];
    char stringdata11[15];
    char stringdata12[21];
    char stringdata13[18];
    char stringdata14[13];
    char stringdata15[15];
    char stringdata16[5];
    char stringdata17[3];
    char stringdata18[16];
    char stringdata19[10];
    char stringdata20[9];
    char stringdata21[13];
    char stringdata22[14];
    char stringdata23[6];
    char stringdata24[17];
    char stringdata25[19];
    char stringdata26[13];
    char stringdata27[10];
    char stringdata28[20];
    char stringdata29[8];
    char stringdata30[13];
    char stringdata31[10];
    char stringdata32[7];
    char stringdata33[9];
    char stringdata34[15];
    char stringdata35[12];
    char stringdata36[7];
    char stringdata37[7];
    char stringdata38[16];
    char stringdata39[5];
    char stringdata40[21];
    char stringdata41[14];
    char stringdata42[17];
    char stringdata43[8];
    char stringdata44[14];
    char stringdata45[9];
    char stringdata46[17];
    char stringdata47[9];
    char stringdata48[8];
    char stringdata49[11];
    char stringdata50[7];
    char stringdata51[8];
    char stringdata52[20];
    char stringdata53[4];
    char stringdata54[5];
    char stringdata55[9];
    char stringdata56[16];
    char stringdata57[16];
    char stringdata58[18];
    char stringdata59[15];
    char stringdata60[8];
    char stringdata61[14];
    char stringdata62[11];
    char stringdata63[8];
    char stringdata64[8];
    char stringdata65[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSBluetoothManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSBluetoothManagerENDCLASS_t qt_meta_stringdata_CLASSBluetoothManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "BluetoothManager"
        QT_MOC_LITERAL(17, 15),  // "adaptersChanged"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 15),  // "scanningChanged"
        QT_MOC_LITERAL(50, 14),  // "devicesChanged"
        QT_MOC_LITERAL(65, 17),  // "nameFilterChanged"
        QT_MOC_LITERAL(83, 13),  // "leOnlyChanged"
        QT_MOC_LITERAL(97, 14),  // "leStateChanged"
        QT_MOC_LITERAL(112, 15),  // "servicesChanged"
        QT_MOC_LITERAL(128, 22),  // "selectedServiceChanged"
        QT_MOC_LITERAL(151, 13),  // "rfcommChanged"
        QT_MOC_LITERAL(165, 14),  // "hexSendChanged"
        QT_MOC_LITERAL(180, 20),  // "escapeEnabledChanged"
        QT_MOC_LITERAL(201, 17),  // "lineEndingChanged"
        QT_MOC_LITERAL(219, 12),  // "statsChanged"
        QT_MOC_LITERAL(232, 14),  // "toastRequested"
        QT_MOC_LITERAL(247, 4),  // "text"
        QT_MOC_LITERAL(252, 2),  // "ok"
        QT_MOC_LITERAL(255, 15),  // "refreshAdapters"
        QT_MOC_LITERAL(271, 9),  // "startScan"
        QT_MOC_LITERAL(281, 8),  // "stopScan"
        QT_MOC_LITERAL(290, 12),  // "clearDevices"
        QT_MOC_LITERAL(303, 13),  // "connectDevice"
        QT_MOC_LITERAL(317, 5),  // "index"
        QT_MOC_LITERAL(323, 16),  // "disconnectDevice"
        QT_MOC_LITERAL(340, 18),  // "readCharacteristic"
        QT_MOC_LITERAL(359, 12),  // "serviceIndex"
        QT_MOC_LITERAL(372, 9),  // "charIndex"
        QT_MOC_LITERAL(382, 19),  // "writeCharacteristic"
        QT_MOC_LITERAL(402, 7),  // "hexMode"
        QT_MOC_LITERAL(410, 12),  // "withResponse"
        QT_MOC_LITERAL(423, 9),  // "setNotify"
        QT_MOC_LITERAL(433, 6),  // "enable"
        QT_MOC_LITERAL(440, 8),  // "indicate"
        QT_MOC_LITERAL(449, 14),  // "enableUartMode"
        QT_MOC_LITERAL(464, 11),  // "serviceUuid"
        QT_MOC_LITERAL(476, 6),  // "txUuid"
        QT_MOC_LITERAL(483, 6),  // "rxUuid"
        QT_MOC_LITERAL(490, 15),  // "disableUartMode"
        QT_MOC_LITERAL(506, 4),  // "send"
        QT_MOC_LITERAL(511, 20),  // "startRfcommDiscovery"
        QT_MOC_LITERAL(532, 13),  // "connectRfcomm"
        QT_MOC_LITERAL(546, 16),  // "disconnectRfcomm"
        QT_MOC_LITERAL(563, 7),  // "traffic"
        QT_MOC_LITERAL(571, 13),  // "TrafficModel*"
        QT_MOC_LITERAL(585, 8),  // "adapters"
        QT_MOC_LITERAL(594, 16),  // "backendSupported"
        QT_MOC_LITERAL(611, 8),  // "scanning"
        QT_MOC_LITERAL(620, 7),  // "devices"
        QT_MOC_LITERAL(628, 10),  // "nameFilter"
        QT_MOC_LITERAL(639, 6),  // "leOnly"
        QT_MOC_LITERAL(646, 7),  // "leState"
        QT_MOC_LITERAL(654, 19),  // "connectedDeviceName"
        QT_MOC_LITERAL(674, 3),  // "mtu"
        QT_MOC_LITERAL(678, 4),  // "rssi"
        QT_MOC_LITERAL(683, 8),  // "services"
        QT_MOC_LITERAL(692, 15),  // "selectedService"
        QT_MOC_LITERAL(708, 15),  // "rfcommConnected"
        QT_MOC_LITERAL(724, 17),  // "rfcommDiscovering"
        QT_MOC_LITERAL(742, 14),  // "rfcommServices"
        QT_MOC_LITERAL(757, 7),  // "hexSend"
        QT_MOC_LITERAL(765, 13),  // "escapeEnabled"
        QT_MOC_LITERAL(779, 10),  // "lineEnding"
        QT_MOC_LITERAL(790, 7),  // "rxBytes"
        QT_MOC_LITERAL(798, 7),  // "txBytes"
        QT_MOC_LITERAL(806, 12)   // "notifyActive"
    },
    "BluetoothManager",
    "adaptersChanged",
    "",
    "scanningChanged",
    "devicesChanged",
    "nameFilterChanged",
    "leOnlyChanged",
    "leStateChanged",
    "servicesChanged",
    "selectedServiceChanged",
    "rfcommChanged",
    "hexSendChanged",
    "escapeEnabledChanged",
    "lineEndingChanged",
    "statsChanged",
    "toastRequested",
    "text",
    "ok",
    "refreshAdapters",
    "startScan",
    "stopScan",
    "clearDevices",
    "connectDevice",
    "index",
    "disconnectDevice",
    "readCharacteristic",
    "serviceIndex",
    "charIndex",
    "writeCharacteristic",
    "hexMode",
    "withResponse",
    "setNotify",
    "enable",
    "indicate",
    "enableUartMode",
    "serviceUuid",
    "txUuid",
    "rxUuid",
    "disableUartMode",
    "send",
    "startRfcommDiscovery",
    "connectRfcomm",
    "disconnectRfcomm",
    "traffic",
    "TrafficModel*",
    "adapters",
    "backendSupported",
    "scanning",
    "devices",
    "nameFilter",
    "leOnly",
    "leState",
    "connectedDeviceName",
    "mtu",
    "rssi",
    "services",
    "selectedService",
    "rfcommConnected",
    "rfcommDiscovering",
    "rfcommServices",
    "hexSend",
    "escapeEnabled",
    "lineEnding",
    "rxBytes",
    "txBytes",
    "notifyActive"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBluetoothManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
      22,  255, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  188,    2, 0x06,   23 /* Public */,
       3,    0,  189,    2, 0x06,   24 /* Public */,
       4,    0,  190,    2, 0x06,   25 /* Public */,
       5,    0,  191,    2, 0x06,   26 /* Public */,
       6,    0,  192,    2, 0x06,   27 /* Public */,
       7,    0,  193,    2, 0x06,   28 /* Public */,
       8,    0,  194,    2, 0x06,   29 /* Public */,
       9,    0,  195,    2, 0x06,   30 /* Public */,
      10,    0,  196,    2, 0x06,   31 /* Public */,
      11,    0,  197,    2, 0x06,   32 /* Public */,
      12,    0,  198,    2, 0x06,   33 /* Public */,
      13,    0,  199,    2, 0x06,   34 /* Public */,
      14,    0,  200,    2, 0x06,   35 /* Public */,
      15,    2,  201,    2, 0x06,   36 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      18,    0,  206,    2, 0x02,   39 /* Public */,
      19,    0,  207,    2, 0x02,   40 /* Public */,
      20,    0,  208,    2, 0x02,   41 /* Public */,
      21,    0,  209,    2, 0x02,   42 /* Public */,
      22,    1,  210,    2, 0x02,   43 /* Public */,
      24,    0,  213,    2, 0x02,   45 /* Public */,
      25,    2,  214,    2, 0x02,   46 /* Public */,
      28,    5,  219,    2, 0x02,   49 /* Public */,
      31,    4,  230,    2, 0x02,   55 /* Public */,
      34,    3,  239,    2, 0x02,   60 /* Public */,
      38,    0,  246,    2, 0x02,   64 /* Public */,
      39,    1,  247,    2, 0x02,   65 /* Public */,
      40,    0,  250,    2, 0x02,   67 /* Public */,
      41,    1,  251,    2, 0x02,   68 /* Public */,
      42,    0,  254,    2, 0x02,   70 /* Public */,

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
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   16,   17,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   26,   27,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   26,   27,   16,   29,   30,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,   26,   27,   32,   33,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   35,   36,   37,
    QMetaType::Void,
    QMetaType::QString, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void,

 // properties: name, type, flags
      43, 0x80000000 | 44, 0x00015409, uint(-1), 0,
      45, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      46, QMetaType::Bool, 0x00015401, uint(-1), 0,
      47, QMetaType::Bool, 0x00015001, uint(1), 0,
      48, QMetaType::QVariantList, 0x00015001, uint(2), 0,
      49, QMetaType::QString, 0x00015103, uint(3), 0,
      50, QMetaType::Bool, 0x00015103, uint(4), 0,
      51, QMetaType::QString, 0x00015001, uint(5), 0,
      52, QMetaType::QString, 0x00015001, uint(5), 0,
      53, QMetaType::Int, 0x00015001, uint(5), 0,
      54, QMetaType::Int, 0x00015001, uint(5), 0,
      55, QMetaType::QVariantList, 0x00015001, uint(6), 0,
      56, QMetaType::Int, 0x00015103, uint(7), 0,
      57, QMetaType::Bool, 0x00015001, uint(8), 0,
      58, QMetaType::Bool, 0x00015001, uint(8), 0,
      59, QMetaType::QVariantList, 0x00015001, uint(8), 0,
      60, QMetaType::Bool, 0x00015103, uint(9), 0,
      61, QMetaType::Bool, 0x00015103, uint(10), 0,
      62, QMetaType::QString, 0x00015103, uint(11), 0,
      63, QMetaType::LongLong, 0x00015001, uint(12), 0,
      64, QMetaType::LongLong, 0x00015001, uint(12), 0,
      65, QMetaType::Bool, 0x00015001, uint(5), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject BluetoothManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSBluetoothManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBluetoothManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBluetoothManagerENDCLASS_t,
        // property 'traffic'
        QtPrivate::TypeAndForceComplete<TrafficModel*, std::true_type>,
        // property 'adapters'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'backendSupported'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'scanning'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'devices'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'nameFilter'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'leOnly'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'leState'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'connectedDeviceName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'mtu'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'rssi'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'services'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'selectedService'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'rfcommConnected'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'rfcommDiscovering'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'rfcommServices'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'hexSend'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'escapeEnabled'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'lineEnding'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'rxBytes'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'txBytes'
        QtPrivate::TypeAndForceComplete<qint64, std::true_type>,
        // property 'notifyActive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BluetoothManager, std::true_type>,
        // method 'adaptersChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'scanningChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'devicesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'nameFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'leOnlyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'leStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'servicesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectedServiceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rfcommChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'hexSendChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'escapeEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lineEndingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toastRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'refreshAdapters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startScan'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopScan'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disconnectDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readCharacteristic'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'writeCharacteristic'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setNotify'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'enableUartMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'disableUartMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'startRfcommDiscovery'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectRfcomm'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disconnectRfcomm'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void BluetoothManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BluetoothManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->adaptersChanged(); break;
        case 1: _t->scanningChanged(); break;
        case 2: _t->devicesChanged(); break;
        case 3: _t->nameFilterChanged(); break;
        case 4: _t->leOnlyChanged(); break;
        case 5: _t->leStateChanged(); break;
        case 6: _t->servicesChanged(); break;
        case 7: _t->selectedServiceChanged(); break;
        case 8: _t->rfcommChanged(); break;
        case 9: _t->hexSendChanged(); break;
        case 10: _t->escapeEnabledChanged(); break;
        case 11: _t->lineEndingChanged(); break;
        case 12: _t->statsChanged(); break;
        case 13: _t->toastRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 14: _t->refreshAdapters(); break;
        case 15: _t->startScan(); break;
        case 16: _t->stopScan(); break;
        case 17: _t->clearDevices(); break;
        case 18: _t->connectDevice((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->disconnectDevice(); break;
        case 20: _t->readCharacteristic((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 21: _t->writeCharacteristic((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 22: _t->setNotify((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[4]))); break;
        case 23: _t->enableUartMode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 24: _t->disableUartMode(); break;
        case 25: { QString _r = _t->send((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 26: _t->startRfcommDiscovery(); break;
        case 27: _t->connectRfcomm((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->disconnectRfcomm(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::adaptersChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::scanningChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::devicesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::nameFilterChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::leOnlyChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::leStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::servicesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::selectedServiceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::rfcommChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::hexSendChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::escapeEnabledChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::lineEndingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)();
            if (_t _q_method = &BluetoothManager::statsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (BluetoothManager::*)(const QString & , bool );
            if (_t _q_method = &BluetoothManager::toastRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
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
        auto *_t = static_cast<BluetoothManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TrafficModel**>(_v) = _t->traffic(); break;
        case 1: *reinterpret_cast< QVariantList*>(_v) = _t->adapters(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->backendSupported(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->scanning(); break;
        case 4: *reinterpret_cast< QVariantList*>(_v) = _t->devices(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->nameFilter(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->leOnly(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->leState(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->connectedDeviceName(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->mtu(); break;
        case 10: *reinterpret_cast< int*>(_v) = _t->rssi(); break;
        case 11: *reinterpret_cast< QVariantList*>(_v) = _t->services(); break;
        case 12: *reinterpret_cast< int*>(_v) = _t->selectedService(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->rfcommConnected(); break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->rfcommDiscovering(); break;
        case 15: *reinterpret_cast< QVariantList*>(_v) = _t->rfcommServices(); break;
        case 16: *reinterpret_cast< bool*>(_v) = _t->hexSend(); break;
        case 17: *reinterpret_cast< bool*>(_v) = _t->escapeEnabled(); break;
        case 18: *reinterpret_cast< QString*>(_v) = _t->lineEnding(); break;
        case 19: *reinterpret_cast< qint64*>(_v) = _t->rxBytes(); break;
        case 20: *reinterpret_cast< qint64*>(_v) = _t->txBytes(); break;
        case 21: *reinterpret_cast< bool*>(_v) = _t->notifyActive(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<BluetoothManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 5: _t->setNameFilter(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setLeOnly(*reinterpret_cast< bool*>(_v)); break;
        case 12: _t->setSelectedService(*reinterpret_cast< int*>(_v)); break;
        case 16: _t->setHexSend(*reinterpret_cast< bool*>(_v)); break;
        case 17: _t->setEscapeEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 18: _t->setLineEnding(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *BluetoothManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BluetoothManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBluetoothManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BluetoothManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void BluetoothManager::adaptersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void BluetoothManager::scanningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void BluetoothManager::devicesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void BluetoothManager::nameFilterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void BluetoothManager::leOnlyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void BluetoothManager::leStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void BluetoothManager::servicesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void BluetoothManager::selectedServiceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void BluetoothManager::rfcommChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void BluetoothManager::hexSendChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void BluetoothManager::escapeEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void BluetoothManager::lineEndingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void BluetoothManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void BluetoothManager::toastRequested(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}
QT_WARNING_POP
