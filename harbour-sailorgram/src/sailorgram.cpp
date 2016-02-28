#include "sailorgram.h"

const QString SailorGram::NO_DAEMON_FILE = "no_daemon";
const QString SailorGram::CONFIG_FOLDER = "telegram";
const QString SailorGram::PUBLIC_KEY_FILE = "server.pub";
const QString SailorGram::EMOJI_FOLDER = "emoji";
const QString SailorGram::APPLICATION_PRETTY_NAME = "SailorGram";

SailorGram::SailorGram(QObject *parent): QObject(parent), _telegram(NULL), _connectivitychecker(NULL), _foregrounddialog(NULL), _connected(-1), _daemonized(false)
{
    QDir cfgdir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    cfgdir.mkpath(qApp->applicationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + SailorGram::CONFIG_FOLDER);

    QDir cachedir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    cachedir.mkpath(qApp->applicationName() + QDir::separator() + qApp->applicationName());

    this->_interface = new SailorgramInterface(this);
    this->_autostart = !SailorGram::hasNoDaemonFile();

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
    connect(this, SIGNAL(daemonizedChanged()), this, SLOT(updateLogLevel()));
    connect(this, SIGNAL(daemonizedChanged()), this, SLOT(updateOnlineState()));
    connect(this->_interface, SIGNAL(wakeUpRequested()), this, SLOT(onWakeUpRequested()));
    connect(this->_interface, SIGNAL(openDialogRequested(qint32)), this, SIGNAL(openDialogRequested(qint32)));
}

bool SailorGram::autostart()
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

bool SailorGram::connected() const
{
    if(!this->_connectivitychecker)
        return false;

    return this->_connectivitychecker->connected();
}

bool SailorGram::hasContact(const qint64 &id) const
{
    return _telegram->contacts().contains(id);
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

TelegramQml *SailorGram::telegram() const
{
    return this->_telegram;
}

DialogObject *SailorGram::foregroundDialog() const
{
    return this->_foregrounddialog;
}

QList<QObject *> SailorGram::translations()
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

void SailorGram::setTelegram(TelegramQml *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;

    if(this->_connectivitychecker)
        this->_connectivitychecker->deleteLater();

    this->_connectivitychecker = new ConnectivityChecker(telegram, this);
    connect(this->_connectivitychecker, SIGNAL(connectedChanged()), this, SIGNAL(connectedChanged()));

    emit telegramChanged();
    emit connectedChanged();
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

void SailorGram::setAutostart(bool autostart)
{
    if(this->_autostart == autostart)
        return;

    this->_autostart = autostart;

    QDir dir(this->configPath());

    if(autostart)
        QFile::remove(dir.absoluteFilePath(SailorGram::NO_DAEMON_FILE));
    else
    {
        QFile file(dir.absoluteFilePath(SailorGram::NO_DAEMON_FILE));
        file.open(QFile::WriteOnly);
        file.close();
    }

    emit autostartChanged();
}

bool SailorGram::hasNoDaemonFile()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QDir::separator() + SailorGram::CONFIG_FOLDER);
    return QFile::exists(dir.absoluteFilePath(SailorGram::NO_DAEMON_FILE));
}

void SailorGram::moveMediaTo(const QString& mediafile, const QString &destination)
{
    QFileInfo mediafileinfo(QUrl(mediafile).path());
    QString destpath = QDir(destination).absoluteFilePath(mediafileinfo.fileName());

    if (QFile(destpath).exists())
    {
        QString basename = mediafileinfo.baseName();
        QString suffix   = mediafileinfo.completeSuffix();
        quint32 copy = 0;
        while (QFile(destpath).exists())
            destpath = QDir(destination).absoluteFilePath(basename + QString(" (%1).").arg(++copy) + suffix);
    }

    QFile::copy(mediafileinfo.absoluteFilePath(), destpath);
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

void SailorGram::closeNotification(DialogObject *dialog)
{
    if(!dialog)
        return;

    qint32 peerid = (dialog->peer()->classType() == Peer::typePeerChat) ? dialog->peer()->chatId() : dialog->peer()->userId();

    if(!this->_notifications.contains(peerid))
        return;

    Notification* notification = this->_notifications[peerid];
    notification->close();
    notification->deleteLater();
    this->_notifications.remove(peerid);
}

void SailorGram::updateLogLevel()
{
    if(!this->_telegram)
        return;

    this->_telegram->setLogLevel(this->_daemonized ? TelegramQml::LogLevelClean : TelegramQml::LogLevelFull);
}

void SailorGram::updateOnlineState()
{
    if(!this->_telegram)
        return;

    this->_telegram->setOnline(!this->_daemonized);
}

void SailorGram::beep()
{
    Notification notification;
    notification.setCategory("harbour.sailorgram.notificationfg");
    notification.setAppName(SailorGram::APPLICATION_PRETTY_NAME);
    notification.setTimestamp(QDateTime::currentDateTime());
    notification.publish();
}

void SailorGram::delayedCloseNotification()
{
    QTimer* timer = qobject_cast<QTimer*>(this->sender());

    if(!this->_deletednotifications.contains(timer->timerId()))
    {
        timer->deleteLater();
        return;
    }

    MessageObject* message = this->_deletednotifications[timer->timerId()];
    DialogObject* dialog = this->_telegram->messageDialog(message->id());

    if(dialog == this->_telegram->nullDialog())
        return;

    this->closeNotification(dialog);
    disconnect(message, SIGNAL(unreadChanged()), this, 0);

    this->_deletednotifications.remove(timer->timerId());
    timer->deleteLater();
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

    if(this->_topmessages.contains(peerid))
        this->_topmessages.remove(peerid);

    notification->deleteLater();
}

void SailorGram::onMessageUnreadedChanged()
{
    MessageObject* message = qobject_cast<MessageObject*>(this->sender());

    if(message->unread())
        return;

    QTimer* timer = new QTimer(this);
    timer->setTimerType(Qt::VeryCoarseTimer);
    timer->setSingleShot(true);

    connect(timer, SIGNAL(timeout()), this, SLOT(delayedCloseNotification()));

    this->_deletednotifications[timer->timerId()] = message;
    timer->start(2000); // 2 seconds
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

QString SailorGram::mediaLocation(quint32 mediatype)
{
    if(mediatype == MessageMedia::typeMessageMediaAudio)
        return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    if(mediatype == MessageMedia::typeMessageMediaPhoto)
        return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    if(mediatype == MessageMedia::typeMessageMediaVideo)
        return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);

    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

void SailorGram::updatePendingState(MessageObject *message, quint32 peerid)
{
    if(this->_topmessages.contains(peerid)) // Top Message has been changed
        this->_topmessages.remove(peerid);

    this->_topmessages[peerid] = message;
    connect(message, SIGNAL(unreadChanged()), this, SLOT(onMessageUnreadedChanged()));
}

void SailorGram::moveMediaToDownloads(const QString& mediafile)
{
    this->moveMediaTo(mediafile, this->mediaLocation(MessageMedia::typeMessageMediaDocument));
}

void SailorGram::moveMediaToGallery(const QString& mediafile, quint32 mediatype)
{
    this->moveMediaTo(mediafile, this->mediaLocation(mediatype));
}

void SailorGram::notify(MessageObject *message, const QString &name, const QString &elaboratedbody)
{
    if(!this->_telegram || this->_telegram->globalMute() || message->out() || !message->unread() || (message->classType() == Message::typeMessageService))
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
        QString photolocation = user->photo()->photoSmall()->download()->location();
        this->updatePendingState(message, peerid);
        this->notify(name, elaboratedbody, photolocation, peerid);
    }
    else
        this->beep();
}
