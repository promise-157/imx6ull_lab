/****************************************************************************
** Meta object code from reading C++ file 'LocalAsrService.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LocalAsrService.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LocalAsrService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LocalAsrService_t {
    QByteArrayData data[14];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LocalAsrService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LocalAsrService_t qt_meta_stringdata_LocalAsrService = {
    {
QT_MOC_LITERAL(0, 0, 15), // "LocalAsrService"
QT_MOC_LITERAL(1, 16, 18), // "partialResultReady"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 4), // "text"
QT_MOC_LITERAL(4, 41, 16), // "finalResultReady"
QT_MOC_LITERAL(5, 58, 9), // "initModel"
QT_MOC_LITERAL(6, 68, 8), // "modelDir"
QT_MOC_LITERAL(7, 77, 13), // "getResultText"
QT_MOC_LITERAL(8, 91, 5), // "reset"
QT_MOC_LITERAL(9, 97, 12), // "recognizeWav"
QT_MOC_LITERAL(10, 110, 8), // "filePath"
QT_MOC_LITERAL(11, 119, 9), // "feedAudio"
QT_MOC_LITERAL(12, 129, 4), // "data"
QT_MOC_LITERAL(13, 134, 11) // "stopExiting"

    },
    "LocalAsrService\0partialResultReady\0\0"
    "text\0finalResultReady\0initModel\0"
    "modelDir\0getResultText\0reset\0recognizeWav\0"
    "filePath\0feedAudio\0data\0stopExiting"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LocalAsrService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   60,    2, 0x0a /* Public */,
       7,    0,   63,    2, 0x0a /* Public */,
       8,    0,   64,    2, 0x0a /* Public */,
       9,    1,   65,    2, 0x0a /* Public */,
      11,    1,   68,    2, 0x0a /* Public */,
      13,    0,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,    6,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QByteArray,   12,
    QMetaType::Void,

       0        // eod
};

void LocalAsrService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LocalAsrService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->partialResultReady((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->finalResultReady((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: { bool _r = _t->initModel((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->getResultText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->reset(); break;
        case 5: _t->recognizeWav((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->feedAudio((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 7: _t->stopExiting(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LocalAsrService::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LocalAsrService::partialResultReady)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LocalAsrService::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LocalAsrService::finalResultReady)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LocalAsrService::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_LocalAsrService.data,
    qt_meta_data_LocalAsrService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LocalAsrService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LocalAsrService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LocalAsrService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LocalAsrService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void LocalAsrService::partialResultReady(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LocalAsrService::finalResultReady(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
