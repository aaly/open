/****************************************************************************
** Meta object code from reading C++ file 'embedder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "embedder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'embedder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EmbedWidgetContainer[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_EmbedWidgetContainer[] = {
    "EmbedWidgetContainer\0"
};

void EmbedWidgetContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EmbedWidgetContainer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EmbedWidgetContainer::staticMetaObject = {
    { &QX11EmbedContainer::staticMetaObject, qt_meta_stringdata_EmbedWidgetContainer,
      qt_meta_data_EmbedWidgetContainer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EmbedWidgetContainer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EmbedWidgetContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EmbedWidgetContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EmbedWidgetContainer))
        return static_cast<void*>(const_cast< EmbedWidgetContainer*>(this));
    return QX11EmbedContainer::qt_metacast(_clname);
}

int EmbedWidgetContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QX11EmbedContainer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Embedder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x08,
      18,    9,    9,    9, 0x08,
      28,    9,    9,    9, 0x08,
      45,    9,    9,    9, 0x08,
      66,   60,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Embedder[] = {
    "Embedder\0\0embed()\0discard()\0"
    "clientEmbedded()\0clientClosed()\0error\0"
    "embedError(QX11EmbedContainer::Error)\0"
};

void Embedder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Embedder *_t = static_cast<Embedder *>(_o);
        switch (_id) {
        case 0: _t->embed(); break;
        case 1: _t->discard(); break;
        case 2: _t->clientEmbedded(); break;
        case 3: _t->clientClosed(); break;
        case 4: _t->embedError((*reinterpret_cast< QX11EmbedContainer::Error(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Embedder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Embedder::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Embedder,
      qt_meta_data_Embedder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Embedder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Embedder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Embedder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Embedder))
        return static_cast<void*>(const_cast< Embedder*>(this));
    return QWidget::qt_metacast(_clname);
}

int Embedder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
