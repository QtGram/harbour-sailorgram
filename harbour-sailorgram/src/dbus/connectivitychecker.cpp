#include "connectivitychecker.h"

ConnectivityChecker::ConnectivityChecker(TelegramEngine *engine, QObject *parent) : QObject(parent), _engine(engine), _connected(false)
{
    QDBusConnection connection = QDBusConnection::systemBus();
    connection.connect("net.connman", "/", "net.connman.Manager", "PropertyChanged", this, SLOT(onConnmanPropertyChanged(QString,QDBusVariant)));

    connect(engine, &TelegramEngine::stateChanged, this, &ConnectivityChecker::onEngineStateChanged);
}

bool ConnectivityChecker::connected() const
{
    return this->_connected;
}

void ConnectivityChecker::onEngineStateChanged()
{
    this->_connected = (this->_engine->state() == TelegramEngine::AuthLoggedIn);
    emit connectedChanged();
}

void ConnectivityChecker::onConnmanPropertyChanged(QString key, QDBusVariant value)
{
    /*
    if(!this->_engine || (key != "State"))
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
        */
}

