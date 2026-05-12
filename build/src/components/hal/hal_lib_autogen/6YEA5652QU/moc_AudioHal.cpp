/****************************************************************************
** Meta object code from reading C++ file 'AudioHal.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "AudioHal.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioHal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AudioHal_t {
    QByteArrayData data[11];
    char stringdata0[75];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AudioHal_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AudioHal_t qt_meta_stringdata_AudioHal = {
    {
QT_MOC_LITERAL(0, 0, 8), // "AudioHal"
QT_MOC_LITERAL(1, 9, 6), // "doPlay"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 6), // "urlStr"
QT_MOC_LITERAL(4, 24, 8), // "doResume"
QT_MOC_LITERAL(5, 33, 7), // "doPause"
QT_MOC_LITERAL(6, 41, 6), // "doStop"
QT_MOC_LITERAL(7, 48, 6), // "doSeek"
QT_MOC_LITERAL(8, 55, 3), // "pos"
QT_MOC_LITERAL(9, 59, 11), // "doSetVolume"
QT_MOC_LITERAL(10, 71, 3) // "vol"

    },
    "AudioHal\0doPlay\0\0urlStr\0doResume\0"
    "doPause\0doStop\0doSeek\0pos\0doSetVolume\0"
    "vol"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioHal[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       4,    0,   47,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    1,   50,    2, 0x0a /* Public */,
       9,    1,   53,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    8,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

void AudioHal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AudioHal *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doPlay((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->doResume(); break;
        case 2: _t->doPause(); break;
        case 3: _t->doStop(); break;
        case 4: _t->doSeek((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 5: _t->doSetVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AudioHal::staticMetaObject = { {
    &ILifecycleModule::staticMetaObject,
    qt_meta_stringdata_AudioHal.data,
    qt_meta_data_AudioHal,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AudioHal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioHal::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AudioHal.stringdata0))
        return static_cast<void*>(this);
    return ILifecycleModule::qt_metacast(_clname);
}

int AudioHal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ILifecycleModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
