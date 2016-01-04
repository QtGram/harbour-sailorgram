#include "notificationmanager.h"

const QString NotificationManager::APPLICATION_PRETTY_NAME = "Sailorgram";

NotificationManager::NotificationManager(QObject *parent): QObject(parent), _telegram(NULL), _foregrounddialog(NULL)
{

}

TelegramQml *NotificationManager::telegram() const
{
    return this->_telegram;
}

DialogObject *NotificationManager::foregroundDialog() const
{
    return this->_foregrounddialog;
}

void NotificationManager::setTelegram(TelegramQml *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    emit telegramChanged();
}

void NotificationManager::setForegroundDialog(DialogObject *dialog)
{
    if(this->_foregrounddialog == dialog)
        return;

    this->_foregrounddialog = dialog;
    emit foregroundDialog();
}

QString NotificationManager::completeName(UserObject* user) const
{
    QString completename = user->firstName();

    if(!user->lastName().isEmpty())
        completename += " " + user->lastName();

    return completename;
}

void NotificationManager::send(const QString &summary, const QString &body, const QString &icon, bool feedback, bool temporary)
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

    args << NotificationManager::APPLICATION_PRETTY_NAME;
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

void NotificationManager::send(const QString &summary, const QString &body)
{
    this->send(summary, body, QString(), false, false);
}

void NotificationManager::beep()
{
    this->send(QString(), QString(), QString(), true, true);
}

void NotificationManager::notify(MessageObject *message)
{
    if(!this->_telegram || this->_telegram->globalMute() || message->out() || (message->classType() == Message::typeMessageService))
        return;

    UserData* userdata = this->_telegram->userData();
    DialogObject* dialog = this->_telegram->messageDialog(message->id());
    qint32 peerid = (dialog->peer()->classType() == Peer::typePeerChat) ? dialog->peer()->chatId() : dialog->peer()->userId();

    if(userdata->isMuted(peerid))
        return;

    bool active = qApp->applicationState() == Qt::ApplicationActive;

    if((dialog != this->_foregrounddialog) || !active)
    {
        UserObject* user = this->_telegram->user(message->fromId());
        this->send(this->completeName(user), message->message(), user->photo()->photoSmall()->download()->location(), true, active);
    }
    else
        this->beep();
}
