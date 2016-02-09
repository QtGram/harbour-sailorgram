#include "connectivitychecker.h"

ConnectivityChecker::ConnectivityChecker(TelegramQml *telegram, QObject *parent) : QObject(parent), _telegram(telegram), _connected(telegram->connected())
{
    QDBusConnection connection = QDBusConnection::systemBus();
    connection.connect("net.connman", "/", "net.connman.Manager", "PropertyChanged", this, SLOT(onConnmanPropertyChanged(QString,QDBusVariant)));

    connect(this->_telegram, SIGNAL(connectedChanged()), this, SLOT(onTelegramConnectedChanged()));
}

bool ConnectivityChecker::connected() const
{
    return this->_connected;
}

void ConnectivityChecker::onTelegramConnectedChanged()
{
    this->_connected = this->_telegram->connected();
    emit connectedChanged();
}

void ConnectivityChecker::onConnmanPropertyChanged(QString key, QDBusVariant value)
{
    if(!this->_telegram || (key != "State"))
        return;

    QString val = value.variant().toString();

    if((val == "idle") || (val == "offline"))
    {
        this->_telegram->sleep();
        this->_connected = false;
        emit connectedChanged();
    }
    else if(val == "online")
        this->_telegram->reconnect();
}

