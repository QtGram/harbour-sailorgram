#include "screenblank.h"

ScreenBlank::ScreenBlank(QObject *parent): QObject(parent), _enabled(true)
{
}

bool ScreenBlank::enabled() const
{
    return this->_enabled;
}

void ScreenBlank::setEnabled(bool e)
{
    if(this->_enabled && e) // We don't need to ping if it is already enabled
        return;

    this->_enabled = e;

    QDBusMessage message = QDBusMessage::createMethodCall("com.nokia.mce",
                                                          "/com/nokia/mce/request",
                                                          "com.nokia.mce.request",
                                                          e ? "req_display_cancel_blanking_pause" : "req_display_blanking_pause");

    QDBusConnection connection(QDBusConnection::systemBus());
    connection.send(message);
    emit enabledChanged();
}
