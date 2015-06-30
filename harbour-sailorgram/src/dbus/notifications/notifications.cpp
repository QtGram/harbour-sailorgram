#include "notifications.h"

Notifications::Notifications(QObject *parent): QObject(parent)
{

}

void Notifications::send(const QString &summary, const QString &body, const QString &icon, bool feedback, bool temporary)
{
    QList<QVariant> args;
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");

    QVariantMap hints;

    if(!icon.isEmpty())
        hints["x-nemo-icon"] = icon;
    else
        hints["x-nemo-icon"] = "icon-m-notifications";

    if(feedback)
        hints["x-nemo-feedback"] = "chat";

    hints["x-nemo-preview-icon"] = "icon-m-notifications";

    if(!summary.isEmpty())
        hints["x-nemo-preview-summary"] = summary;

    if(!body.isEmpty())
        hints["x-nemo-preview-body"] = body;

    args << qApp->applicationName();
    args << 0u;
    args << QString();

    if(!temporary)
    {
        args << summary;
        args << body;
    }
    else
    {
        args << QString();
        args << QString();
    }

    args << QStringList();
    args << hints;
    args << -1;

    QDBusConnection connection(QDBusConnection::sessionBus());
    message.setArguments(args);
    connection.send(message);
}

void Notifications::send(const QString &summary, const QString &body)
{
    this->send(summary, body, QString(), false, false);
}

void Notifications::beep()
{
    this->send(QString(), QString(), QString(), true, true);
}
