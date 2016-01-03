#include "sailorgraminterface.h"

const QString SailorgramInterface::INTERFACE_NAME = "org.harbour.sailorgram";

SailorgramInterface::SailorgramInterface(QObject *parent) : QObject(parent)
{
    new SailorgramAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();

    if(!connection.isConnected())
    {
        qWarning("Cannot connect to the D-Bus session bus.");
        return;
    }

    if(!connection.registerService(SailorgramInterface::INTERFACE_NAME))
    {
        qWarning() << connection.lastError().message();
        return;
    }

    if(!connection.registerObject("/", this))
        qWarning() << connection.lastError().message();
}

void SailorgramInterface::sendWakeUp()
{
    QDBusMessage message = QDBusMessage::createMethodCall(SailorgramInterface::INTERFACE_NAME, "/", SailorgramInterface::INTERFACE_NAME, "wakeUp");
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.send(message);
}

void SailorgramInterface::wakeUp()
{
    emit wakeUpRequested();
}

