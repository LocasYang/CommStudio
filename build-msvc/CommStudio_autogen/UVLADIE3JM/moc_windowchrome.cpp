/****************************************************************************
** Meta object code from reading C++ file 'windowchrome.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/windowchrome.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'windowchrome.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSWindowChromeENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSWindowChromeENDCLASS = QtMocHelpers::stringData(
    "WindowChrome",
    "apply",
    "",
    "windowObject",
    "dark",
    "captionColor",
    "textColor",
    "borderColor"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSWindowChromeENDCLASS_t {
    uint offsetsAndSizes[16];
    char stringdata0[13];
    char stringdata1[6];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[5];
    char stringdata5[13];
    char stringdata6[10];
    char stringdata7[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSWindowChromeENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSWindowChromeENDCLASS_t qt_meta_stringdata_CLASSWindowChromeENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "WindowChrome"
        QT_MOC_LITERAL(13, 5),  // "apply"
        QT_MOC_LITERAL(19, 0),  // ""
        QT_MOC_LITERAL(20, 12),  // "windowObject"
        QT_MOC_LITERAL(33, 4),  // "dark"
        QT_MOC_LITERAL(38, 12),  // "captionColor"
        QT_MOC_LITERAL(51, 9),  // "textColor"
        QT_MOC_LITERAL(61, 11)   // "borderColor"
    },
    "WindowChrome",
    "apply",
    "",
    "windowObject",
    "dark",
    "captionColor",
    "textColor",
    "borderColor"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWindowChromeENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       1,    5,   20,    2, 0x02,    1 /* Public */,

 // methods: parameters
    QMetaType::Bool, QMetaType::QObjectStar, QMetaType::Bool, QMetaType::QColor, QMetaType::QColor, QMetaType::QColor,    3,    4,    5,    6,    7,

       0        // eod
};

Q_CONSTINIT const QMetaObject WindowChrome::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSWindowChromeENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWindowChromeENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWindowChromeENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WindowChrome, std::true_type>,
        // method 'apply'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QObject *, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QColor &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QColor &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QColor &, std::false_type>
    >,
    nullptr
} };

void WindowChrome::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WindowChrome *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { bool _r = _t->apply((*reinterpret_cast< std::add_pointer_t<QObject*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QColor>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QColor>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QColor>>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *WindowChrome::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WindowChrome::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWindowChromeENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WindowChrome::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
