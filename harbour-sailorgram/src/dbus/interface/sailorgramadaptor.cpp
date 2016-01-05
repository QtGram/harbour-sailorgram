#include "sailorgramadaptor.h"

SailorgramAdaptor::SailorgramAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
{

}

void SailorgramAdaptor::wakeUp()
{
    QMetaObject::invokeMethod(parent(), "wakeUp");
}

void SailorgramAdaptor::openDialog(qint32 peerid)
{
    QMetaObject::invokeMethod(parent(), "openDialog", Q_ARG(qint32, peerid));
}
