/****************************************************************************
** Meta object code from reading C++ file 'VideoPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "VideoPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VideoPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VideoPage_t {
    QByteArrayData data[18];
    char stringdata0[211];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoPage_t qt_meta_stringdata_VideoPage = {
    {
QT_MOC_LITERAL(0, 0, 9), // "VideoPage"
QT_MOC_LITERAL(1, 10, 10), // "togglePlay"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 12), // "toggleDrawer"
QT_MOC_LITERAL(4, 35, 14), // "scanVideoFiles"
QT_MOC_LITERAL(5, 50, 14), // "onFileSelected"
QT_MOC_LITERAL(6, 65, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 82, 4), // "item"
QT_MOC_LITERAL(8, 87, 14), // "changeVideoDir"
QT_MOC_LITERAL(9, 102, 8), // "playNext"
QT_MOC_LITERAL(10, 111, 12), // "playPrevious"
QT_MOC_LITERAL(11, 124, 12), // "changeVolume"
QT_MOC_LITERAL(12, 137, 5), // "value"
QT_MOC_LITERAL(13, 143, 17), // "onPositionChanged"
QT_MOC_LITERAL(14, 161, 8), // "position"
QT_MOC_LITERAL(15, 170, 17), // "onDurationChanged"
QT_MOC_LITERAL(16, 188, 8), // "duration"
QT_MOC_LITERAL(17, 197, 13) // "onSliderMoved"

    },
    "VideoPage\0togglePlay\0\0toggleDrawer\0"
    "scanVideoFiles\0onFileSelected\0"
    "QListWidgetItem*\0item\0changeVideoDir\0"
    "playNext\0playPrevious\0changeVolume\0"
    "value\0onPositionChanged\0position\0"
    "onDurationChanged\0duration\0onSliderMoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    1,   72,    2, 0x08 /* Private */,
       8,    0,   75,    2, 0x08 /* Private */,
       9,    0,   76,    2, 0x08 /* Private */,
      10,    0,   77,    2, 0x08 /* Private */,
      11,    1,   78,    2, 0x08 /* Private */,
      13,    1,   81,    2, 0x08 /* Private */,
      15,    1,   84,    2, 0x08 /* Private */,
      17,    1,   87,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::LongLong,   14,
    QMetaType::Void, QMetaType::LongLong,   16,
    QMetaType::Void, QMetaType::Int,   14,

       0        // eod
};

void VideoPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VideoPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->togglePlay(); break;
        case 1: _t->toggleDrawer(); break;
        case 2: _t->scanVideoFiles(); break;
        case 3: _t->onFileSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->changeVideoDir(); break;
        case 5: _t->playNext(); break;
        case 6: _t->playPrevious(); break;
        case 7: _t->changeVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->onPositionChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 9: _t->onDurationChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 10: _t->onSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject VideoPage::staticMetaObject = { {
    &IAppModule::staticMetaObject,
    qt_meta_stringdata_VideoPage.data,
    qt_meta_data_VideoPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VideoPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VideoPage.stringdata0))
        return static_cast<void*>(this);
    return IAppModule::qt_metacast(_clname);
}

int VideoPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = IAppModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
