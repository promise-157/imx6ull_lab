/****************************************************************************
** Meta object code from reading C++ file 'VideoService.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "VideoService.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VideoService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VideoService_t {
    QByteArrayData data[10];
    char stringdata0[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoService_t qt_meta_stringdata_VideoService = {
    {
QT_MOC_LITERAL(0, 0, 12), // "VideoService"
QT_MOC_LITERAL(1, 13, 11), // "requestScan"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 7), // "dirPath"
QT_MOC_LITERAL(4, 34, 14), // "onHalVideoList"
QT_MOC_LITERAL(5, 49, 4), // "list"
QT_MOC_LITERAL(6, 54, 17), // "handlePlayCommand"
QT_MOC_LITERAL(7, 72, 5), // "index"
QT_MOC_LITERAL(8, 78, 17), // "handleNextCommand"
QT_MOC_LITERAL(9, 96, 17) // "handlePrevCommand"

    },
    "VideoService\0requestScan\0\0dirPath\0"
    "onHalVideoList\0list\0handlePlayCommand\0"
    "index\0handleNextCommand\0handlePrevCommand"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       4,    1,   42,    2, 0x0a /* Public */,
       6,    1,   45,    2, 0x0a /* Public */,
       8,    0,   48,    2, 0x0a /* Public */,
       9,    0,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void VideoService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VideoService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestScan((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->onHalVideoList((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 2: _t->handlePlayCommand((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->handleNextCommand(); break;
        case 4: _t->handlePrevCommand(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject VideoService::staticMetaObject = { {
    &ILifecycleModule::staticMetaObject,
    qt_meta_stringdata_VideoService.data,
    qt_meta_data_VideoService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VideoService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VideoService.stringdata0))
        return static_cast<void*>(this);
    return ILifecycleModule::qt_metacast(_clname);
}

int VideoService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ILifecycleModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
