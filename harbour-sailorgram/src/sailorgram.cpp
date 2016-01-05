#include "sailorgram.h"

const QString SailorGram::CONFIG_FOLDER = "telegram";
const QString SailorGram::PUBLIC_KEY_FILE = "server.pub";
const QString SailorGram::EMOJI_FOLDER = "emoji";
const QString SailorGram::APPLICATION_PRETTY_NAME = "SailorGram";

SailorGram::SailorGram(QObject *parent): QObject(parent), _telegram(NULL), _foregrounddialog(NULL), _daemonized(false)
{
    QDir cfgdir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    cfgdir.mkpath(qApp->applicationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + SailorGram::CONFIG_FOLDER);

    this->_heartbeat = new HeartBeat(this);
    this->_interface = new SailorgramInterface(this);

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
    connect(this->_interface, SIGNAL(wakeUpRequested()), this, SLOT(onWakeUpRequested()));
    connect(this->_interface, SIGNAL(openDialogRequested(qint32)), this, SIGNAL(openDialogRequested(qint32)));
    connect(this->_heartbeat, SIGNAL(connectedChanged()), this, SLOT(wakeSleep()), Qt::QueuedConnection);
    connect(this->_heartbeat, SIGNAL(connectedChanged()), this, SIGNAL(connectedChanged()), Qt::QueuedConnection);
    connect(this->_heartbeat, SIGNAL(intervalChanged()), this, SIGNAL(intervalChanged()));
}

bool SailorGram::keepRunning() const
{
    return !qApp->quitOnLastWindowClosed();
}

bool SailorGram::daemonized() const
{
    return qApp->applicationState() == Qt::ApplicationActive;
}

bool SailorGram::connected() const
{
    return this->_heartbeat->connected();
}

int SailorGram::interval() const
{
    return this->_heartbeat->interval();
}

void SailorGram::setInterval(int interval)
{
    this->_heartbeat->setInterval(interval);
}

QString SailorGram::emojiPath() const
{
    return qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + SailorGram::EMOJI_FOLDER + QDir::separator();
}

QString SailorGram::configPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QDir::separator() + SailorGram::CONFIG_FOLDER;
}

QString SailorGram::publicKey() const
{
    return qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + SailorGram::PUBLIC_KEY_FILE;
}

TelegramQml *SailorGram::telegram() const
{
    return this->_telegram;
}

DialogObject *SailorGram::foregroundDialog() const
{
    return this->_foregrounddialog;
}

void SailorGram::setTelegram(TelegramQml *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    this->_heartbeat->setTelegram(telegram);

    if(this->_telegram->connected())
        this->_heartbeat->start();
    else
        connect(this->_telegram, SIGNAL(connectedChanged()), this, SLOT(startHeartBeat()));

    emit telegramChanged();
}

void SailorGram::setForegroundDialog(DialogObject *dialog)
{
    if(this->_foregrounddialog == dialog)
        return;

    this->_foregrounddialog = dialog;
    emit foregroundDialog();
}

void SailorGram::setKeepRunning(bool keep)
{
    if(keep == this->keepRunning())
        return;

    qApp->setQuitOnLastWindowClosed(!keep);
    emit keepRunningChanged();
}

void SailorGram::moveMediaTo(FileLocationObject *locationobj, const QString &destination)
{
    if(!locationobj)
        return;

    QString filename = locationobj->fileName();
    QUrl location(locationobj->download()->location());

    if(filename.isEmpty())
        filename = location.fileName();

    QString destpath = destination + QDir::separator() + filename;

    if(QFile::exists(destpath)) // Don't overwrite existing files
        return;

    QFile::copy(location.path(), destpath);
}

void SailorGram::notify(const QString &summary, const QString &body, const QString &icon, qint32 peerid)
{
    Notification* notification = NULL;

    if(peerid)
    {
        if(!this->_notifications.contains(peerid))
        {
            notification = new Notification(this);

            QVariantMap remoteaction;
            remoteaction["name"] = "default";
            remoteaction["service"] = "org.harbour.sailorgram";
            remoteaction["path"] = "/";
            remoteaction["iface"] = "org.harbour.sailorgram";
            remoteaction["method"] = "openDialog";
            remoteaction["arguments"] = (QVariantList() << peerid);
            remoteaction["icon"] = icon.isEmpty() ? "icon-m-notifications" : icon;

            notification->setRemoteAction(remoteaction);
            this->_notifications[peerid] = notification;

            connect(notification, SIGNAL(closed(uint)), this, SLOT(onNotificationClosed(uint)));
        }
        else
            notification = this->_notifications[peerid];
    }
    else
        notification = new Notification(this);

    notification->setCategory("harbour.sailorgram.notification");
    notification->setAppName(SailorGram::APPLICATION_PRETTY_NAME);
    notification->setTimestamp(QDateTime::currentDateTime());

    if(!summary.isEmpty())
    {
        notification->setPreviewSummary(summary);
        notification->setSummary(summary);
    }

    if(!body.isEmpty())
    {
        notification->setPreviewBody(body);
        notification->setBody(body);
    }

    notification->publish();

    if(!peerid) // Temporary notification
        notification->deleteLater();
}

void SailorGram::beep()
{
    this->notify(QString(), QString(), QString(), 0);
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
    qint32 peerid = notification->remoteActions().first().toMap()["arguments"].toList().first().toInt();

    this->_notifications.remove(peerid);
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

bool SailorGram::fileIsPhoto(const QString &filepath)
{
    if(filepath.isEmpty())
        return false;

    QUrl url(filepath);
    QMimeType mime = this->_mimedb.mimeTypeForFile(url.path());
    return mime.isValid() && (mime.name().split("/")[0] == "image");
}

bool SailorGram::fileIsVideo(const QString &filepath)
{
    if(filepath.isEmpty())
        return false;

    QUrl url(filepath);
    QMimeType mime = this->_mimedb.mimeTypeForFile(url.path());
    return mime.isValid() && (mime.name().split("/")[0] == "video");
}

QString SailorGram::fileName(const QString &filepath)
{
    QUrl url(filepath);
    return url.fileName();
}

FileLocationObject* SailorGram::mediaLocation(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return NULL;

    FileLocationObject* locationobj = NULL;

    switch(messagemediaobject->classType())
    {
        case MessageMedia::typeMessageMediaAudio:
            locationobj = this->_telegram->locationOfAudio(messagemediaobject->audio());
            break;

        case MessageMedia::typeMessageMediaDocument:
            locationobj = this->_telegram->locationOfDocument(messagemediaobject->document());
            break;

        case MessageMedia::typeMessageMediaVideo:
            locationobj = this->_telegram->locationOfVideo(messagemediaobject->video());
            break;

        case MessageMedia::typeMessageMediaPhoto:
            locationobj = messagemediaobject->photo()->sizes()->last()->location();
            break;

        default:
            break;
    }

    return locationobj;
}

void SailorGram::moveMediaToDownloads(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return;

    FileLocationObject* locationobj = this->mediaLocation(messagemediaobject);
    this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

void SailorGram::moveMediaToGallery(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return;

    QString type;
    FileLocationObject* locationobj = this->mediaLocation(messagemediaobject);

    if(messagemediaobject->classType() == MessageMedia::typeMessageMediaDocument)
    {
        QString mime = messagemediaobject->document()->mimeType();
        type = mime.split("/")[0];
    }

    if((messagemediaobject->classType() == MessageMedia::typeMessageMediaVideo) || (type == "video"))
        this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    else if((messagemediaobject->classType() == MessageMedia::typeMessageMediaAudio) || (type == "audio"))
        this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    else if((messagemediaobject->classType() == MessageMedia::typeMessageMediaPhoto) || (type == "image"))
        this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    else
        this->moveMediaToDownloads(messagemediaobject); // Fallback to Downloads folder
}

void SailorGram::notify(MessageObject *message, const QString &name, const QString &elaboratedbody)
{
    if(!this->_telegram || this->_telegram->globalMute() || message->out() || (message->classType() == Message::typeMessageService))
        return;

    UserData* userdata = this->_telegram->userData();
    DialogObject* dialog = this->_telegram->messageDialog(message->id());
    qint32 peerid = (dialog->peer()->classType() == Peer::typePeerChat) ? dialog->peer()->chatId() : dialog->peer()->userId();

    if(userdata->isMuted(peerid) || (dialog->notifySettings()->muteUntil() > 0))
        return;

    bool active = qApp->applicationState() == Qt::ApplicationActive;

    if((dialog != this->_foregrounddialog) || !active)
    {
        UserObject* user = this->_telegram->user(message->fromId());
        QString photolocation = user->photo()->photoSmall()->download()->location();
        this->notify(name, elaboratedbody, photolocation, peerid);
    }
    else
        this->beep();
}

void SailorGram::startHeartBeat()
{
    if(!this->_telegram->connected())
        return;

    this->_heartbeat->start();
}

void SailorGram::wakeSleep()
{
    this->_heartbeat->connected() ? this->_telegram->wake() : this->_telegram->sleep();
}
