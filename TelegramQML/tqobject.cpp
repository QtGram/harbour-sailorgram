#include "tqobject.h"

#include <QSet>

QSet<TqObject*> tq_object_instances;

TqObject::TqObject(QObject *parent) :
    QObject(parent),
    mErrorCode(0)
{
    tq_object_instances.insert(this);
}

QQmlListProperty<QObject> TqObject::items()
{
    return QQmlListProperty<QObject>(this, &_items, QQmlListProperty<QObject>::AppendFunction(append),
                                                      QQmlListProperty<QObject>::CountFunction(count),
                                                      QQmlListProperty<QObject>::AtFunction(at),
                                     QQmlListProperty<QObject>::ClearFunction(clear) );
}

void TqObject::append(QQmlListProperty<QObject> *p, QObject *v)
{
    TqObject *aobj = static_cast<TqObject*>(p->object);
    aobj->_items.append(v);
    Q_EMIT aobj->itemsChanged();
}

int TqObject::count(QQmlListProperty<QObject> *p)
{
    TqObject *aobj = static_cast<TqObject*>(p->object);
    return aobj->_items.count();
}

QObject *TqObject::at(QQmlListProperty<QObject> *p, int idx)
{
    TqObject *aobj = static_cast<TqObject*>(p->object);
    return aobj->_items.at(idx);
}

void TqObject::clear(QQmlListProperty<QObject> *p)
{
    TqObject *aobj = static_cast<TqObject*>(p->object);
    aobj->_items.clear();
    Q_EMIT aobj->itemsChanged();
}

bool TqObject::isValid(TqObject *obj)
{
    return tq_object_instances.contains(obj);
}

TqObject::~TqObject()
{
    tq_object_instances.remove(this);
}

