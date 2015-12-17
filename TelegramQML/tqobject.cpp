#include "tqobject.h"

#include <QSet>

QSet<TqObject*> tq_object_instances;

TqObject::TqObject(QObject *parent) :
    QObject(parent)
{
    tq_object_instances.insert(this);
}

bool TqObject::isValid(TqObject *obj)
{
    return tq_object_instances.contains(obj);
}

TqObject::~TqObject()
{
    tq_object_instances.remove(this);
}

