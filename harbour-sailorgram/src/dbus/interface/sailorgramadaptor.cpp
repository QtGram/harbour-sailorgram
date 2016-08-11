#include "sailorgramadaptor.h"

SailorgramAdaptor::SailorgramAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
{

}

void SailorgramAdaptor::wakeUp()
{
    QMetaObject::invokeMethod(parent(), "wakeUp");
}

void SailorgramAdaptor::openDialog(QString peerkey)
{
    QMetaObject::invokeMethod(parent(), "openDialog", Q_ARG(QString, peerkey));
}
