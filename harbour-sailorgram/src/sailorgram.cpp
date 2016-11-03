#include "sailorgram.h"
#include <config/telegramconfig.h>
#include <types/telegramhelper.h>

const QString SailorGram::DAEMON_FILE = "daemon";
const QString SailorGram::PUBLIC_KEY_FILE = "public.key";
const QString SailorGram::EMOJI_FOLDER = "emoji";
const QString SailorGram::APPLICATION_PRETTY_NAME = "SailorGram";

SailorGram::SailorGram(QObject *parent): QObject(parent), _telegram(NULL), _daemonized(false)
{
    this->_interface = new SailorgramInterface(this);
    this->_notifications = new TelegramNotifications(this);
    //this->_autostart = !SailorGram::hasDaemonFile();

    connect(qApp, &QGuiApplication::applicationStateChanged, this, &SailorGram::onApplicationStateChanged);
    connect(this->_notifications, &TelegramNotifications::newMessage, this, &SailorGram::notify);
    connect(this->_interface, &SailorgramInterface::wakeUpRequested, this, &SailorGram::onWakeUpRequested);
    connect(this->_interface, &SailorgramInterface::openDialogRequested, this, &SailorGram::openDialogRequested);
}

Telegram *SailorGram::telegram() const
{
    return this->_telegram;
}

bool SailorGram::autostart() const
{
    return this->_autostart;
}

bool SailorGram::keepRunning() const
{
    return !qApp->quitOnLastWindowClosed();
}

bool SailorGram::daemonized() const
{
    return qApp->applicationState() == Qt::ApplicationActive;
}

QString SailorGram::emojiPath() const
{
    return qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + SailorGram::EMOJI_FOLDER + QDir::separator();
}

QString SailorGram::publicKey() const
{
    return qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + SailorGram::PUBLIC_KEY_FILE;
}

QString SailorGram::homeFolder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

QString SailorGram::sdcardFolder() const
{
    QString sdcardfolder;

    if(QFile::exists("/media/sdcard"))
        sdcardfolder = "/media/sdcard";

    if(QFile::exists("/run/user/100000/media/sdcard"))
        sdcardfolder = "/run/user/100000/media/sdcard";

    if(sdcardfolder.isEmpty())
        return QString();

    QDir dir(sdcardfolder);
    QFileInfoList fileinfolist = dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::DirsFirst | QDir::Time);

    if(fileinfolist.isEmpty())
        return QString();

    return fileinfolist.first().filePath();
}

QString SailorGram::picturesFolder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}

QString SailorGram::androidStorage() const
{
    QDir homefolder(this->homeFolder());

    if(!homefolder.exists("android_storage"))
        return QString();

    return homefolder.absoluteFilePath("android_storage");
}

QString SailorGram::voiceRecordPath() const
{
    QDir cachefolder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    return cachefolder.absoluteFilePath("voice-rec.ogg");
}

TelegramNotifications *SailorGram::notifications() const
{
    return this->_notifications;
}

QList<QObject *> SailorGram::translations() const
{
    QList<QObject *> trlist;
    QFile jsonfile(":/translations/translations.json");
    if (jsonfile.open(QFile::ReadOnly))
    {
        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson(jsonfile.readAll(), &error);
        if (error.error != QJsonParseError::NoError)
            return trlist;
        foreach (QJsonValue jsonarrayvalue, json.array())
            trlist.append(new TranslationInfoItem(jsonarrayvalue.toObject()));
    }
    return trlist;
}

void SailorGram::setTelegram(Telegram *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    this->_notifications->setTelegram(telegram);
    emit telegramChanged();
}

void SailorGram::setKeepRunning(bool keep)
{
    if(keep == this->keepRunning())
        return;

    qApp->setQuitOnLastWindowClosed(!keep);
    emit keepRunningChanged();
}

void SailorGram::setAutostart(bool autostart)
{
    if(this->_autostart == autostart)
        return;

    this->_autostart = autostart;

    QDir dir(TelegramConfig_storagePath);

    if(autostart)
    {
        QFile file(dir.absoluteFilePath(SailorGram::DAEMON_FILE));
        file.open(QFile::WriteOnly);
        file.close();
    }
    else
        QFile::remove(dir.absoluteFilePath(SailorGram::DAEMON_FILE));

    emit autostartChanged();
}

bool SailorGram::hasDaemonFile()
{
    QDir dir(TelegramConfig_storagePath);
    return QFile::exists(dir.absoluteFilePath(SailorGram::DAEMON_FILE));
}

void SailorGram::beep() const
{
    Notification notification;
    notification.setCategory("harbour.sailorgram.notificationfg");
    notification.setAppName(SailorGram::APPLICATION_PRETTY_NAME);
    notification.setTimestamp(QDateTime::currentDateTime());
    notification.publish();
}

void SailorGram::notify(const NotificationObject *notificationobj)
{
    if(notificationobj->isCurrentDialog() && (qApp->applicationState() == Qt::ApplicationActive))
    {
        this->beep();
        return;
    }

    Notification* notification = NULL;

    if(!this->_notificationsmap.contains(notificationobj->dialogId()))
    {
        notification = new Notification(this);

        QVariantMap remoteaction;
        remoteaction["name"] = "default";
        remoteaction["service"] = "org.harbour.sailorgram";
        remoteaction["path"] = "/";
        remoteaction["iface"] = "org.harbour.sailorgram";
        remoteaction["method"] = "openDialog";
        remoteaction["arguments"] = (QVariantList() << notificationobj->dialogId());
        remoteaction["icon"] = "icon-m-notifications";

        notification->setRemoteAction(remoteaction);
        this->_notificationsmap[notificationobj->dialogId()] = notification;

        connect(notification, &Notification::closed, this, &SailorGram::onNotificationClosed);
    }
    else
        notification = this->_notificationsmap[notificationobj->dialogId()];

    notification->setCategory("harbour.sailorgram.notification");
    notification->setAppName(SailorGram::APPLICATION_PRETTY_NAME);
    notification->setTimestamp(QDateTime::currentDateTime());

    notification->setPreviewSummary(notificationobj->title());
    notification->setSummary(notificationobj->title());

    notification->setPreviewBody(notificationobj->message());
    notification->setBody(notificationobj->message());

    notification->publish();
}

void SailorGram::closeNotification(Dialog *dialog)
{
    TLInt dialogid = TelegramHelper::identifier(dialog);

    if(!this->_notificationsmap.contains(dialogid))
        return;

    Notification* notification = this->_notificationsmap.take(dialogid);
    notification->close();
    notification->deleteLater();
}

void SailorGram::onApplicationStateChanged(Qt::ApplicationState state)
{
    bool active = state == Qt::ApplicationActive;

    if((!this->_daemonized && active) || (this->_daemonized && !active))
        return;

    this->_daemonized = !active;
    emit daemonizedChanged();
}

void SailorGram::onNotificationClosed(uint)
{
    Notification* notification = qobject_cast<Notification*>(this->sender());
    TLInt dialogid = notification->remoteActions().first().toMap()["arguments"].toList().first().toInt();

    this->_notificationsmap.remove(dialogid);
    notification->deleteLater();
}

void SailorGram::onWakeUpRequested()
{
    if(this->_daemonized)
    {
        this->_daemonized = false;
        emit daemonizedChanged();
    }

    emit wakeUpRequested();
}
