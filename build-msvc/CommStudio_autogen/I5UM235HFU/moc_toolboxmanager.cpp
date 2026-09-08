/****************************************************************************
** Meta object code from reading C++ file 'toolboxmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/managers/toolboxmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toolboxmanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSToolboxManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSToolboxManagerENDCLASS = QtMocHelpers::stringData(
    "ToolboxManager",
    "checksums",
    "",
    "hexInput",
    "analyzeHex",
    "encodeNumber",
    "value",
    "type",
    "endianness",
    "textToHex",
    "text",
    "hexToText"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSToolboxManagerENDCLASS_t {
    uint offsetsAndSizes[24];
    char stringdata0[15];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[11];
    char stringdata5[13];
    char stringdata6[6];
    char stringdata7[5];
    char stringdata8[11];
    char stringdata9[10];
    char stringdata10[5];
    char stringdata11[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSToolboxManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSToolboxManagerENDCLASS_t qt_meta_stringdata_CLASSToolboxManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "ToolboxManager"
        QT_MOC_LITERAL(15, 9),  // "checksums"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 8),  // "hexInput"
        QT_MOC_LITERAL(35, 10),  // "analyzeHex"
        QT_MOC_LITERAL(46, 12),  // "encodeNumber"
        QT_MOC_LITERAL(59, 5),  // "value"
        QT_MOC_LITERAL(65, 4),  // "type"
        QT_MOC_LITERAL(70, 10),  // "endianness"
        QT_MOC_LITERAL(81, 9),  // "textToHex"
        QT_MOC_LITERAL(91, 4),  // "text"
        QT_MOC_LITERAL(96, 9)   // "hexToText"
    },
    "ToolboxManager",
    "checksums",
    "",
    "hexInput",
    "analyzeHex",
    "encodeNumber",
    "value",
    "type",
    "endianness",
    "textToHex",
    "text",
    "hexToText"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSToolboxManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x02,    1 /* Public */,
       4,    1,   47,    2, 0x02,    3 /* Public */,
       5,    3,   50,    2, 0x102,    5 /* Public | MethodIsConst  */,
       9,    1,   57,    2, 0x102,    9 /* Public | MethodIsConst  */,
      11,    1,   60,    2, 0x102,   11 /* Public | MethodIsConst  */,

 // methods: parameters
    QMetaType::QVariantMap, QMetaType::QString,    3,
    QMetaType::QVariantMap, QMetaType::QString,    3,
    QMetaType::QVariantMap, QMetaType::Double, QMetaType::QString, QMetaType::QString,    6,    7,    8,
    QMetaType::QString, QMetaType::QString,   10,
    QMetaType::QString, QMetaType::QString,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject ToolboxManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSToolboxManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSToolboxManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSToolboxManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ToolboxManager, std::true_type>,
        // method 'checksums'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'analyzeHex'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'encodeNumber'
        QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'textToHex'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'hexToText'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ToolboxManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToolboxManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { QVariantMap _r = _t->checksums((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 1: { QVariantMap _r = _t->analyzeHex((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 2: { QVariantMap _r = _t->encodeNumber((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->textToHex((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QString _r = _t->hexToText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *ToolboxManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToolboxManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSToolboxManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ToolboxManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
