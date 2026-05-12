/****************************************************************************
** Meta object code from reading C++ file 'HardwareCenterPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "HardwareCenterPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HardwareCenterPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BasicIoWidget_t {
    QByteArrayData data[1];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BasicIoWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BasicIoWidget_t qt_meta_stringdata_BasicIoWidget = {
    {
QT_MOC_LITERAL(0, 0, 13) // "BasicIoWidget"

    },
    "BasicIoWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BasicIoWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void BasicIoWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject BasicIoWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_BasicIoWidget.data,
    qt_meta_data_BasicIoWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BasicIoWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BasicIoWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BasicIoWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BasicIoWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_BusTestWidget_t {
    QByteArrayData data[11];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BusTestWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BusTestWidget_t qt_meta_stringdata_BusTestWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "BusTestWidget"
QT_MOC_LITERAL(1, 14, 10), // "onUartOpen"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 11), // "onUartClose"
QT_MOC_LITERAL(4, 38, 10), // "onUartSend"
QT_MOC_LITERAL(5, 49, 9), // "onI2cRead"
QT_MOC_LITERAL(6, 59, 10), // "onI2cWrite"
QT_MOC_LITERAL(7, 70, 9), // "onSpiSend"
QT_MOC_LITERAL(8, 80, 7), // "onCanUp"
QT_MOC_LITERAL(9, 88, 9), // "onCanDown"
QT_MOC_LITERAL(10, 98, 9) // "onCanSend"

    },
    "BusTestWidget\0onUartOpen\0\0onUartClose\0"
    "onUartSend\0onI2cRead\0onI2cWrite\0"
    "onSpiSend\0onCanUp\0onCanDown\0onCanSend"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BusTestWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    0,   63,    2, 0x08 /* Private */,
       7,    0,   64,    2, 0x08 /* Private */,
       8,    0,   65,    2, 0x08 /* Private */,
       9,    0,   66,    2, 0x08 /* Private */,
      10,    0,   67,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void BusTestWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BusTestWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onUartOpen(); break;
        case 1: _t->onUartClose(); break;
        case 2: _t->onUartSend(); break;
        case 3: _t->onI2cRead(); break;
        case 4: _t->onI2cWrite(); break;
        case 5: _t->onSpiSend(); break;
        case 6: _t->onCanUp(); break;
        case 7: _t->onCanDown(); break;
        case 8: _t->onCanSend(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject BusTestWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_BusTestWidget.data,
    qt_meta_data_BusTestWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BusTestWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BusTestWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BusTestWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BusTestWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
struct qt_meta_stringdata_SysLoadWidget_t {
    QByteArrayData data[3];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SysLoadWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SysLoadWidget_t qt_meta_stringdata_SysLoadWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "SysLoadWidget"
QT_MOC_LITERAL(1, 14, 13), // "updateSysInfo"
QT_MOC_LITERAL(2, 28, 0) // ""

    },
    "SysLoadWidget\0updateSysInfo\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SysLoadWidget[] = {

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
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void SysLoadWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SysLoadWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateSysInfo(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject SysLoadWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_SysLoadWidget.data,
    qt_meta_data_SysLoadWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SysLoadWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SysLoadWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SysLoadWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SysLoadWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_AudioTestWidget_t {
    QByteArrayData data[5];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AudioTestWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AudioTestWidget_t qt_meta_stringdata_AudioTestWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "AudioTestWidget"
QT_MOC_LITERAL(1, 16, 14), // "startRecording"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 13), // "stopRecording"
QT_MOC_LITERAL(4, 46, 16) // "onPlayBtnClicked"

    },
    "AudioTestWidget\0startRecording\0\0"
    "stopRecording\0onPlayBtnClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioTestWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08 /* Private */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    0,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AudioTestWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AudioTestWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startRecording(); break;
        case 1: _t->stopRecording(); break;
        case 2: _t->onPlayBtnClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject AudioTestWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_AudioTestWidget.data,
    qt_meta_data_AudioTestWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AudioTestWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioTestWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AudioTestWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AudioTestWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_WaveformPlotWidget_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WaveformPlotWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WaveformPlotWidget_t qt_meta_stringdata_WaveformPlotWidget = {
    {
QT_MOC_LITERAL(0, 0, 18) // "WaveformPlotWidget"

    },
    "WaveformPlotWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WaveformPlotWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void WaveformPlotWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject WaveformPlotWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_WaveformPlotWidget.data,
    qt_meta_data_WaveformPlotWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WaveformPlotWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WaveformPlotWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WaveformPlotWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int WaveformPlotWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_LogicAnalyzerView_t {
    QByteArrayData data[13];
    char stringdata0[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LogicAnalyzerView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LogicAnalyzerView_t qt_meta_stringdata_LogicAnalyzerView = {
    {
QT_MOC_LITERAL(0, 0, 17), // "LogicAnalyzerView"
QT_MOC_LITERAL(1, 18, 14), // "onReadyReadUdp"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 13), // "onItemChanged"
QT_MOC_LITERAL(4, 48, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(5, 65, 4), // "item"
QT_MOC_LITERAL(6, 70, 6), // "column"
QT_MOC_LITERAL(7, 77, 18), // "onBtnActionClicked"
QT_MOC_LITERAL(8, 96, 22), // "onTreeSelectionChanged"
QT_MOC_LITERAL(9, 119, 17), // "onProcessFinished"
QT_MOC_LITERAL(10, 137, 8), // "exitCode"
QT_MOC_LITERAL(11, 146, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(12, 167, 10) // "exitStatus"

    },
    "LogicAnalyzerView\0onReadyReadUdp\0\0"
    "onItemChanged\0QTreeWidgetItem*\0item\0"
    "column\0onBtnActionClicked\0"
    "onTreeSelectionChanged\0onProcessFinished\0"
    "exitCode\0QProcess::ExitStatus\0exitStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LogicAnalyzerView[] = {

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
       1,    0,   39,    2, 0x0a /* Public */,
       3,    2,   40,    2, 0x0a /* Public */,
       7,    0,   45,    2, 0x0a /* Public */,
       8,    0,   46,    2, 0x0a /* Public */,
       9,    2,   47,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,   10,   12,

       0        // eod
};

void LogicAnalyzerView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LogicAnalyzerView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onReadyReadUdp(); break;
        case 1: _t->onItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->onBtnActionClicked(); break;
        case 3: _t->onTreeSelectionChanged(); break;
        case 4: _t->onProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LogicAnalyzerView::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_LogicAnalyzerView.data,
    qt_meta_data_LogicAnalyzerView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LogicAnalyzerView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LogicAnalyzerView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LogicAnalyzerView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LogicAnalyzerView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_HardwareCenterPage_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HardwareCenterPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HardwareCenterPage_t qt_meta_stringdata_HardwareCenterPage = {
    {
QT_MOC_LITERAL(0, 0, 18) // "HardwareCenterPage"

    },
    "HardwareCenterPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HardwareCenterPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void HardwareCenterPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject HardwareCenterPage::staticMetaObject = { {
    &IAppModule::staticMetaObject,
    qt_meta_stringdata_HardwareCenterPage.data,
    qt_meta_data_HardwareCenterPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HardwareCenterPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HardwareCenterPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HardwareCenterPage.stringdata0))
        return static_cast<void*>(this);
    return IAppModule::qt_metacast(_clname);
}

int HardwareCenterPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = IAppModule::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
