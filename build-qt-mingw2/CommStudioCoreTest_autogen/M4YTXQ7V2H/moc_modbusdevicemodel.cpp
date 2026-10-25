/****************************************************************************
** Meta object code from reading C++ file 'modbusdevicemodel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/models/modbusdevicemodel.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbusdevicemodel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS = QtMocHelpers::stringData(
    "ModbusDeviceModel",
    "definitionsChanged",
    "",
    "renameDevice",
    "index",
    "name",
    "Role",
    "NameRole",
    "UnitIdRole",
    "OnlineRole",
    "LabelRole",
    "IdRole"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS_t {
    uint offsetsAndSizes[24];
    char stringdata0[18];
    char stringdata1[19];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[6];
    char stringdata5[5];
    char stringdata6[5];
    char stringdata7[9];
    char stringdata8[11];
    char stringdata9[11];
    char stringdata10[10];
    char stringdata11[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS_t qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "ModbusDeviceModel"
        QT_MOC_LITERAL(18, 18),  // "definitionsChanged"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 12),  // "renameDevice"
        QT_MOC_LITERAL(51, 5),  // "index"
        QT_MOC_LITERAL(57, 4),  // "name"
        QT_MOC_LITERAL(62, 4),  // "Role"
        QT_MOC_LITERAL(67, 8),  // "NameRole"
        QT_MOC_LITERAL(76, 10),  // "UnitIdRole"
        QT_MOC_LITERAL(87, 10),  // "OnlineRole"
        QT_MOC_LITERAL(98, 9),  // "LabelRole"
        QT_MOC_LITERAL(108, 6)   // "IdRole"
    },
    "ModbusDeviceModel",
    "definitionsChanged",
    "",
    "renameDevice",
    "index",
    "name",
    "Role",
    "NameRole",
    "UnitIdRole",
    "OnlineRole",
    "LabelRole",
    "IdRole"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusDeviceModelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       1,   32, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x06,    2 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    2,   27,    2, 0x02,    3 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Bool, QMetaType::Int, QMetaType::QString,    4,    5,

 // enums: name, alias, flags, count, data
       6,    6, 0x0,    5,   37,

 // enum data: key, value
       7, uint(ModbusDeviceModel::NameRole),
       8, uint(ModbusDeviceModel::UnitIdRole),
       9, uint(ModbusDeviceModel::OnlineRole),
      10, uint(ModbusDeviceModel::LabelRole),
      11, uint(ModbusDeviceModel::IdRole),

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusDeviceModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusDeviceModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS_t,
        // enum 'Role'
        QtPrivate::TypeAndForceComplete<ModbusDeviceModel::Role, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusDeviceModel, std::true_type>,
        // method 'definitionsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'renameDevice'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ModbusDeviceModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusDeviceModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->definitionsChanged(); break;
        case 1: { bool _r = _t->renameDevice((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusDeviceModel::*)();
            if (_t _q_method = &ModbusDeviceModel::definitionsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *ModbusDeviceModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusDeviceModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusDeviceModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int ModbusDeviceModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ModbusDeviceModel::definitionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
