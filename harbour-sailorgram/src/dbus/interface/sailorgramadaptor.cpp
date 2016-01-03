#include "sailorgramadaptor.h"

SailorgramAdaptor::SailorgramAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
{

}

void SailorgramAdaptor::wakeUp()
{
    QMetaObject::invokeMethod(parent(), "wakeUp");
}
