#include "sailorgramadaptor.h"

SailorgramAdaptor::SailorgramAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
{

}

void SailorgramAdaptor::wakeUp()
{
    QMetaObject::invokeMethod(parent(), "wakeUp");
}

void SailorgramAdaptor::openDialog(QByteArray peerkey)
{
    QMetaObject::invokeMethod(parent(), "openDialog", Q_ARG(QByteArray, peerkey));
}
