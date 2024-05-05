/****************************************************************************
** Meta object code from reading C++ file 'CgQtGLRenderWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "CgQtViewer/CgQtGLRenderWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CgQtGLRenderWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CgQtGLRenderWidget_t {
    QByteArrayData data[16];
    char stringdata0[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CgQtGLRenderWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CgQtGLRenderWidget_t qt_meta_stringdata_CgQtGLRenderWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CgQtGLRenderWidget"
QT_MOC_LITERAL(1, 19, 10), // "mouseEvent"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "QMouseEvent*"
QT_MOC_LITERAL(4, 44, 5), // "event"
QT_MOC_LITERAL(5, 50, 15), // "viewportChanged"
QT_MOC_LITERAL(6, 66, 1), // "w"
QT_MOC_LITERAL(7, 68, 1), // "h"
QT_MOC_LITERAL(8, 70, 16), // "trackballChanged"
QT_MOC_LITERAL(9, 87, 19), // "slotBackfaceCulling"
QT_MOC_LITERAL(10, 107, 12), // "slotLighting"
QT_MOC_LITERAL(11, 120, 13), // "slotUseSplats"
QT_MOC_LITERAL(12, 134, 16), // "slotRenderPoints"
QT_MOC_LITERAL(13, 151, 15), // "slotRenderLines"
QT_MOC_LITERAL(14, 167, 16), // "slotRenderFilled"
QT_MOC_LITERAL(15, 184, 7) // "cleanup"

    },
    "CgQtGLRenderWidget\0mouseEvent\0\0"
    "QMouseEvent*\0event\0viewportChanged\0w\0"
    "h\0trackballChanged\0slotBackfaceCulling\0"
    "slotLighting\0slotUseSplats\0slotRenderPoints\0"
    "slotRenderLines\0slotRenderFilled\0"
    "cleanup"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CgQtGLRenderWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       5,    2,   67,    2, 0x06 /* Public */,
       8,    0,   72,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   73,    2, 0x0a /* Public */,
      10,    0,   74,    2, 0x0a /* Public */,
      11,    0,   75,    2, 0x0a /* Public */,
      12,    0,   76,    2, 0x0a /* Public */,
      13,    0,   77,    2, 0x0a /* Public */,
      14,    0,   78,    2, 0x0a /* Public */,
      15,    0,   79,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CgQtGLRenderWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CgQtGLRenderWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->mouseEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 1: _t->viewportChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->trackballChanged(); break;
        case 3: _t->slotBackfaceCulling(); break;
        case 4: _t->slotLighting(); break;
        case 5: _t->slotUseSplats(); break;
        case 6: _t->slotRenderPoints(); break;
        case 7: _t->slotRenderLines(); break;
        case 8: _t->slotRenderFilled(); break;
        case 9: _t->cleanup(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CgQtGLRenderWidget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CgQtGLRenderWidget::mouseEvent)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CgQtGLRenderWidget::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CgQtGLRenderWidget::viewportChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CgQtGLRenderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CgQtGLRenderWidget::trackballChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CgQtGLRenderWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_CgQtGLRenderWidget.data,
    qt_meta_data_CgQtGLRenderWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CgQtGLRenderWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CgQtGLRenderWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CgQtGLRenderWidget.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CgBaseRenderer"))
        return static_cast< CgBaseRenderer*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions"))
        return static_cast< QOpenGLFunctions*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int CgQtGLRenderWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void CgQtGLRenderWidget::mouseEvent(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CgQtGLRenderWidget::viewportChanged(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CgQtGLRenderWidget::trackballChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
