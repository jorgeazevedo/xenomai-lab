/****************************************************************************
** Meta object code from reading C++ file 'blockbase.h'
**
** Created: Fri Nov 25 22:07:50 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/blockbase.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blockbase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BlockBase[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      23,   10,   10,   10, 0x08,
      54,   10,   10,   10, 0x08,
      63,   10,   10,   10, 0x08,
      82,   10,   10,   10, 0x08,
     110,   10,   10,   10, 0x08,
     134,   10,   10,   10, 0x08,
     163,   10,  158,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BlockBase[] = {
    "BlockBase\0\0pressedOk()\0"
    "pressedApply(QAbstractButton*)\0cancel()\0"
    "spinBoxChange(int)\0doubleSpinBoxChange(double)\0"
    "comboBoxChange(QString)\0lineEditChange(QString)\0"
    "bool\0permissionToQuit()\0"
};

const QMetaObject BlockBase::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_BlockBase,
      qt_meta_data_BlockBase, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BlockBase::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BlockBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BlockBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BlockBase))
        return static_cast<void*>(const_cast< BlockBase*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int BlockBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: pressedOk(); break;
        case 1: pressedApply((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 2: cancel(); break;
        case 3: spinBoxChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: doubleSpinBoxChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: comboBoxChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: lineEditChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: { bool _r = permissionToQuit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
