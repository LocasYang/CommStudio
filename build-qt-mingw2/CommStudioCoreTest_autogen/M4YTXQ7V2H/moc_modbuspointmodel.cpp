/****************************************************************************
** Meta object code from reading C++ file 'modbuspointmodel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/models/modbuspointmodel.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbuspointmodel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusPointModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusPointModelENDCLASS = QtMocHelpers::stringData(
    "ModbusPointModel",
    "deviceIdChanged",
    "",
    "definitionsChanged",
    "validationError",
    "message",
    "filterChanged",
    "groupsChanged",
    "pointAt",
    "row",
    "savePoint",
    "point",
    "removePoint",
    "copyPoint",
    "exportFile",
    "url",
    "importFile",
    "pollingPlans",
    "strategy",
    "maxGap",
    "maxRegisters",
    "pasteClipboard",
    "selectPoint",
    "additive",
    "range",
    "updateSelected",
    "changes",
    "setWriteValue",
    "value",
    "deleteSelected",
    "copySelectedToDevice",
    "deviceId",
    "movePoint",
    "from",
    "to",
    "sortByAddress",
    "trendSeries",
    "windowMs",
    "exportHistory",
    "filtered",
    "QAbstractItemModel*",
    "search",
    "area",
    "groupFilter",
    "groups"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusPointModelENDCLASS_t {
    uint offsetsAndSizes[90];
    char stringdata0[17];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[16];
    char stringdata5[8];
    char stringdata6[14];
    char stringdata7[14];
    char stringdata8[8];
    char stringdata9[4];
    char stringdata10[10];
    char stringdata11[6];
    char stringdata12[12];
    char stringdata13[10];
    char stringdata14[11];
    char stringdata15[4];
    char stringdata16[11];
    char stringdata17[13];
    char stringdata18[9];
    char stringdata19[7];
    char stringdata20[13];
    char stringdata21[15];
    char stringdata22[12];
    char stringdata23[9];
    char stringdata24[6];
    char stringdata25[15];
    char stringdata26[8];
    char stringdata27[14];
    char stringdata28[6];
    char stringdata29[15];
    char stringdata30[21];
    char stringdata31[9];
    char stringdata32[10];
    char stringdata33[5];
    char stringdata34[3];
    char stringdata35[14];
    char stringdata36[12];
    char stringdata37[9];
    char stringdata38[14];
    char stringdata39[9];
    char stringdata40[20];
    char stringdata41[7];
    char stringdata42[5];
    char stringdata43[12];
    char stringdata44[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusPointModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusPointModelENDCLASS_t qt_meta_stringdata_CLASSModbusPointModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "ModbusPointModel"
        QT_MOC_LITERAL(17, 15),  // "deviceIdChanged"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 18),  // "definitionsChanged"
        QT_MOC_LITERAL(53, 15),  // "validationError"
        QT_MOC_LITERAL(69, 7),  // "message"
        QT_MOC_LITERAL(77, 13),  // "filterChanged"
        QT_MOC_LITERAL(91, 13),  // "groupsChanged"
        QT_MOC_LITERAL(105, 7),  // "pointAt"
        QT_MOC_LITERAL(113, 3),  // "row"
        QT_MOC_LITERAL(117, 9),  // "savePoint"
        QT_MOC_LITERAL(127, 5),  // "point"
        QT_MOC_LITERAL(133, 11),  // "removePoint"
        QT_MOC_LITERAL(145, 9),  // "copyPoint"
        QT_MOC_LITERAL(155, 10),  // "exportFile"
        QT_MOC_LITERAL(166, 3),  // "url"
        QT_MOC_LITERAL(170, 10),  // "importFile"
        QT_MOC_LITERAL(181, 12),  // "pollingPlans"
        QT_MOC_LITERAL(194, 8),  // "strategy"
        QT_MOC_LITERAL(203, 6),  // "maxGap"
        QT_MOC_LITERAL(210, 12),  // "maxRegisters"
        QT_MOC_LITERAL(223, 14),  // "pasteClipboard"
        QT_MOC_LITERAL(238, 11),  // "selectPoint"
        QT_MOC_LITERAL(250, 8),  // "additive"
        QT_MOC_LITERAL(259, 5),  // "range"
        QT_MOC_LITERAL(265, 14),  // "updateSelected"
        QT_MOC_LITERAL(280, 7),  // "changes"
        QT_MOC_LITERAL(288, 13),  // "setWriteValue"
        QT_MOC_LITERAL(302, 5),  // "value"
        QT_MOC_LITERAL(308, 14),  // "deleteSelected"
        QT_MOC_LITERAL(323, 20),  // "copySelectedToDevice"
        QT_MOC_LITERAL(344, 8),  // "deviceId"
        QT_MOC_LITERAL(353, 9),  // "movePoint"
        QT_MOC_LITERAL(363, 4),  // "from"
        QT_MOC_LITERAL(368, 2),  // "to"
        QT_MOC_LITERAL(371, 13),  // "sortByAddress"
        QT_MOC_LITERAL(385, 11),  // "trendSeries"
        QT_MOC_LITERAL(397, 8),  // "windowMs"
        QT_MOC_LITERAL(406, 13),  // "exportHistory"
        QT_MOC_LITERAL(420, 8),  // "filtered"
        QT_MOC_LITERAL(429, 19),  // "QAbstractItemModel*"
        QT_MOC_LITERAL(449, 6),  // "search"
        QT_MOC_LITERAL(456, 4),  // "area"
        QT_MOC_LITERAL(461, 11),  // "groupFilter"
        QT_MOC_LITERAL(473, 6)   // "groups"
    },
    "ModbusPointModel",
    "deviceIdChanged",
    "",
    "definitionsChanged",
    "validationError",
    "message",
    "filterChanged",
    "groupsChanged",
    "pointAt",
    "row",
    "savePoint",
    "point",
    "removePoint",
    "copyPoint",
    "exportFile",
    "url",
    "importFile",
    "pollingPlans",
    "strategy",
    "maxGap",
    "maxRegisters",
    "pasteClipboard",
    "selectPoint",
    "additive",
    "range",
    "updateSelected",
    "changes",
    "setWriteValue",
    "value",
    "deleteSelected",
    "copySelectedToDevice",
    "deviceId",
    "movePoint",
    "from",
    "to",
    "sortByAddress",
    "trendSeries",
    "windowMs",
    "exportHistory",
    "filtered",
    "QAbstractItemModel*",
    "search",
    "area",
    "groupFilter",
    "groups"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusPointModelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       6,  246, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  170,    2, 0x06,    7 /* Public */,
       3,    0,  171,    2, 0x06,    8 /* Public */,
       4,    1,  172,    2, 0x06,    9 /* Public */,
       6,    0,  175,    2, 0x06,   11 /* Public */,
       7,    0,  176,    2, 0x06,   12 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       8,    1,  177,    2, 0x102,   13 /* Public | MethodIsConst  */,
      10,    2,  180,    2, 0x02,   15 /* Public */,
      12,    1,  185,    2, 0x02,   18 /* Public */,
      13,    1,  188,    2, 0x02,   20 /* Public */,
      14,    1,  191,    2, 0x102,   22 /* Public | MethodIsConst  */,
      16,    1,  194,    2, 0x02,   24 /* Public */,
      17,    3,  197,    2, 0x102,   26 /* Public | MethodIsConst  */,
      17,    2,  204,    2, 0x122,   30 /* Public | MethodCloned | MethodIsConst  */,
      17,    1,  209,    2, 0x122,   33 /* Public | MethodCloned | MethodIsConst  */,
      17,    0,  212,    2, 0x122,   35 /* Public | MethodCloned | MethodIsConst  */,
      21,    0,  213,    2, 0x02,   36 /* Public */,
      22,    3,  214,    2, 0x02,   37 /* Public */,
      25,    1,  221,    2, 0x02,   41 /* Public */,
      27,    2,  224,    2, 0x02,   43 /* Public */,
      29,    0,  229,    2, 0x02,   46 /* Public */,
      30,    1,  230,    2, 0x02,   47 /* Public */,
      32,    2,  233,    2, 0x02,   49 /* Public */,
      35,    0,  238,    2, 0x02,   52 /* Public */,
      36,    1,  239,    2, 0x102,   53 /* Public | MethodIsConst  */,
      36,    0,  242,    2, 0x122,   55 /* Public | MethodCloned | MethodIsConst  */,
      38,    1,  243,    2, 0x102,   56 /* Public | MethodIsConst  */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::QVariantMap, QMetaType::Int,    9,
    QMetaType::Bool, QMetaType::Int, QMetaType::QVariantMap,    9,   11,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Bool, QMetaType::QUrl,   15,
    QMetaType::Bool, QMetaType::QUrl,   15,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Int,   18,   19,
    QMetaType::QVariantList, QMetaType::Int,   18,
    QMetaType::QVariantList,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    9,   23,   24,
    QMetaType::Bool, QMetaType::QVariantMap,   26,
    QMetaType::Bool, QMetaType::Int, QMetaType::QString,    9,   28,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   31,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   33,   34,
    QMetaType::Void,
    QMetaType::QVariantList, QMetaType::Int,   37,
    QMetaType::QVariantList,
    QMetaType::Bool, QMetaType::QUrl,   15,

 // properties: name, type, flags
      31, QMetaType::QString, 0x00015103, uint(0), 0,
      39, 0x80000000 | 40, 0x00015409, uint(-1), 0,
      41, QMetaType::QString, 0x00015103, uint(3), 0,
      42, QMetaType::Int, 0x00015103, uint(3), 0,
      43, QMetaType::QString, 0x00015103, uint(3), 0,
      44, QMetaType::QVariantList, 0x00015001, uint(4), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusPointModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractTableModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusPointModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusPointModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusPointModelENDCLASS_t,
        // property 'deviceId'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'filtered'
        QtPrivate::TypeAndForceComplete<QAbstractItemModel*, std::true_type>,
        // property 'search'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'area'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'groupFilter'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'groups'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusPointModel, std::true_type>,
        // method 'deviceIdChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'definitionsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'validationError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'filterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'groupsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pointAt'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'savePoint'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'removePoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'copyPoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'exportFile'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'importFile'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'pollingPlans'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'pollingPlans'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'pollingPlans'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'pollingPlans'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        // method 'pasteClipboard'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'selectPoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'updateSelected'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>,
        // method 'setWriteValue'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'deleteSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copySelectedToDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'movePoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sortByAddress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'trendSeries'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'trendSeries'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        // method 'exportHistory'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>
    >,
    nullptr
} };

void ModbusPointModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusPointModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->deviceIdChanged(); break;
        case 1: _t->definitionsChanged(); break;
        case 2: _t->validationError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->filterChanged(); break;
        case 4: _t->groupsChanged(); break;
        case 5: { QVariantMap _r = _t->pointAt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->savePoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->removePoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->copyPoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: { bool _r = _t->exportFile((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->importFile((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { QVariantList _r = _t->pollingPlans((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 12: { QVariantList _r = _t->pollingPlans((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 13: { QVariantList _r = _t->pollingPlans((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 14: { QVariantList _r = _t->pollingPlans();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->pasteClipboard();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->selectPoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 17: { bool _r = _t->updateSelected((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { bool _r = _t->setWriteValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 19: _t->deleteSelected(); break;
        case 20: _t->copySelectedToDevice((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->movePoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 22: _t->sortByAddress(); break;
        case 23: { QVariantList _r = _t->trendSeries((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 24: { QVariantList _r = _t->trendSeries();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 25: { bool _r = _t->exportHistory((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusPointModel::*)();
            if (_t _q_method = &ModbusPointModel::deviceIdChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusPointModel::*)();
            if (_t _q_method = &ModbusPointModel::definitionsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusPointModel::*)(const QString & );
            if (_t _q_method = &ModbusPointModel::validationError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusPointModel::*)();
            if (_t _q_method = &ModbusPointModel::filterChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ModbusPointModel::*)();
            if (_t _q_method = &ModbusPointModel::groupsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractItemModel* >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ModbusPointModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->deviceId(); break;
        case 1: *reinterpret_cast< QAbstractItemModel**>(_v) = _t->filtered(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->search(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->area(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->groupFilter(); break;
        case 5: *reinterpret_cast< QVariantList*>(_v) = _t->groups(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ModbusPointModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDeviceId(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setSearch(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setArea(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setGroupFilter(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *ModbusPointModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusPointModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusPointModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int ModbusPointModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 26;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ModbusPointModel::deviceIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModbusPointModel::definitionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusPointModel::validationError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModbusPointModel::filterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ModbusPointModel::groupsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
