/****************************************************************************
** Meta object code from reading C++ file 'MusicPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MusicPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MusicPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CdWidget_t {
    QByteArrayData data[3];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CdWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CdWidget_t qt_meta_stringdata_CdWidget = {
    {
QT_MOC_LITERAL(0, 0, 8), // "CdWidget"
QT_MOC_LITERAL(1, 9, 14), // "updateRotation"
QT_MOC_LITERAL(2, 24, 0) // ""

    },
    "CdWidget\0updateRotation\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CdWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CdWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CdWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateRotation(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CdWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_CdWidget.data,
    qt_meta_data_CdWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CdWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CdWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CdWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CdWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_MusicPage_t {
    QByteArrayData data[20];
    char stringdata0[235];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MusicPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MusicPage_t qt_meta_stringdata_MusicPage = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MusicPage"
QT_MOC_LITERAL(1, 10, 10), // "togglePlay"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 12), // "toggleDrawer"
QT_MOC_LITERAL(4, 35, 14), // "scanMediaFiles"
QT_MOC_LITERAL(5, 50, 14), // "onFileSelected"
QT_MOC_LITERAL(6, 65, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 82, 4), // "item"
QT_MOC_LITERAL(8, 87, 14), // "changeMediaDir"
QT_MOC_LITERAL(9, 102, 8), // "playNext"
QT_MOC_LITERAL(10, 111, 10), // "isAutoPlay"
QT_MOC_LITERAL(11, 122, 12), // "playPrevious"
QT_MOC_LITERAL(12, 135, 14), // "togglePlayMode"
QT_MOC_LITERAL(13, 150, 14), // "toggleFavorite"
QT_MOC_LITERAL(14, 165, 12), // "changeVolume"
QT_MOC_LITERAL(15, 178, 5), // "value"
QT_MOC_LITERAL(16, 184, 20), // "onMediaStatusChanged"
QT_MOC_LITERAL(17, 205, 6), // "status"
QT_MOC_LITERAL(18, 212, 13), // "onSliderMoved"
QT_MOC_LITERAL(19, 226, 8) // "position"

    },
    "MusicPage\0togglePlay\0\0toggleDrawer\0"
    "scanMediaFiles\0onFileSelected\0"
    "QListWidgetItem*\0item\0changeMediaDir\0"
    "playNext\0isAutoPlay\0playPrevious\0"
    "togglePlayMode\0toggleFavorite\0"
    "changeVolume\0value\0onMediaStatusChanged\0"
    "status\0onSliderMoved\0position"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MusicPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    1,   82,    2, 0x08 /* Private */,
       8,    0,   85,    2, 0x08 /* Private */,
       9,    1,   86,    2, 0x08 /* Private */,
       9,    0,   89,    2, 0x28 /* Private | MethodCloned */,
      11,    0,   90,    2, 0x08 /* Private */,
      12,    0,   91,    2, 0x08 /* Private */,
      13,    0,   92,    2, 0x08 /* Private */,
      14,    1,   93,    2, 0x08 /* Private */,
      16,    1,   96,    2, 0x08 /* Private */,
      18,    1,   99,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,   19,

       0        // eod
};

void MusicPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MusicPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->togglePlay(); break;
        case 1: _t->toggleDrawer(); break;
        case 2: _t->scanMediaFiles(); break;
        case 3: _t->onFileSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->changeMediaDir(); break;
        case 5: _t->playNext((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->playNext(); break;
        case 7: _t->playPrevious(); break;
        case 8: _t->togglePlayMode(); break;
        case 9: _t->toggleFavorite(); break;
        case 10: _t->changeVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->onMediaStatusChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->onSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MusicPage::staticMetaObject = { {
    &IAppModule::staticMetaObject,
    qt_meta_stringdata_MusicPage.data,
    qt_meta_data_MusicPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MusicPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MusicPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MusicPage.stringdata0))
        return static_cast<void*>(this);
    return IAppModule::qt_metacast(_clname);
}

int MusicPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = IAppModule::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
