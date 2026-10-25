/****************************************************************************
** Meta object code from reading C++ file 'modbusworkspace.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/managers/modbusworkspace.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbusworkspace.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS = QtMocHelpers::stringData(
    "ModbusWorkspace",
    "currentChanged",
    "",
    "channelsChanged",
    "toastRequested",
    "message",
    "ok",
    "select",
    "index",
    "add",
    "name",
    "remove",
    "rename",
    "closeAll",
    "current",
    "ModbusManager*",
    "currentIndex",
    "channels"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS_t {
    uint offsetsAndSizes[36];
    char stringdata0[16];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[15];
    char stringdata5[8];
    char stringdata6[3];
    char stringdata7[7];
    char stringdata8[6];
    char stringdata9[4];
    char stringdata10[5];
    char stringdata11[7];
    char stringdata12[7];
    char stringdata13[9];
    char stringdata14[8];
    char stringdata15[15];
    char stringdata16[13];
    char stringdata17[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS_t qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS = {
    {
        QT_MOC_LITERAL(0, 15),  // "ModbusWorkspace"
        QT_MOC_LITERAL(16, 14),  // "currentChanged"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 15),  // "channelsChanged"
        QT_MOC_LITERAL(48, 14),  // "toastRequested"
        QT_MOC_LITERAL(63, 7),  // "message"
        QT_MOC_LITERAL(71, 2),  // "ok"
        QT_MOC_LITERAL(74, 6),  // "select"
        QT_MOC_LITERAL(81, 5),  // "index"
        QT_MOC_LITERAL(87, 3),  // "add"
        QT_MOC_LITERAL(91, 4),  // "name"
        QT_MOC_LITERAL(96, 6),  // "remove"
        QT_MOC_LITERAL(103, 6),  // "rename"
        QT_MOC_LITERAL(110, 8),  // "closeAll"
        QT_MOC_LITERAL(119, 7),  // "current"
        QT_MOC_LITERAL(127, 14),  // "ModbusManager*"
        QT_MOC_LITERAL(142, 12),  // "currentIndex"
        QT_MOC_LITERAL(155, 8)   // "channels"
    },
    "ModbusWorkspace",
    "currentChanged",
    "",
    "channelsChanged",
    "toastRequested",
    "message",
    "ok",
    "select",
    "index",
    "add",
    "name",
    "remove",
    "rename",
    "closeAll",
    "current",
    "ModbusManager*",
    "currentIndex",
    "channels"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusWorkspaceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       3,   84, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x06,    4 /* Public */,
       3,    0,   63,    2, 0x06,    5 /* Public */,
       4,    2,   64,    2, 0x06,    6 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,   69,    2, 0x02,    9 /* Public */,
       9,    1,   72,    2, 0x02,   11 /* Public */,
      11,    1,   75,    2, 0x02,   13 /* Public */,
      12,    2,   78,    2, 0x02,   15 /* Public */,
      13,    0,   83,    2, 0x02,   18 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    5,    6,

 // methods: parameters
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    8,   10,
    QMetaType::Void,

 // properties: name, type, flags
      14, 0x80000000 | 15, 0x00015009, uint(0), 0,
      16, QMetaType::Int, 0x00015001, uint(0), 0,
      17, QMetaType::QVariantList, 0x00015001, uint(1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusWorkspace::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusWorkspaceENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS_t,
        // property 'current'
        QtPrivate::TypeAndForceComplete<ModbusManager*, std::true_type>,
        // property 'currentIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'channels'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusWorkspace, std::true_type>,
        // method 'currentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'channelsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toastRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'select'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'add'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'remove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'rename'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'closeAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ModbusWorkspace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusWorkspace *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentChanged(); break;
        case 1: _t->channelsChanged(); break;
        case 2: _t->toastRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 3: _t->select((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->add((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->remove((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->rename((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->closeAll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusWorkspace::*)();
            if (_t _q_method = &ModbusWorkspace::currentChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusWorkspace::*)();
            if (_t _q_method = &ModbusWorkspace::channelsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusWorkspace::*)(const QString & , bool );
            if (_t _q_method = &ModbusWorkspace::toastRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ModbusManager* >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ModbusWorkspace *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ModbusManager**>(_v) = _t->current(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->currentIndex(); break;
        case 2: *reinterpret_cast< QVariantList*>(_v) = _t->channels(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *ModbusWorkspace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusWorkspace::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusWorkspaceENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusWorkspace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ModbusWorkspace::currentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModbusWorkspace::channelsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusWorkspace::toastRequested(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
