#include "sailorgram.h"

const QString SailorGram::DAEMON_FILE = "daemon";
const QString SailorGram::CONFIG_FOLDER = "telegram";
const QString SailorGram::PUBLIC_KEY_FILE = "server.pub";
const QString SailorGram::EMOJI_FOLDER = "emoji";
const QString SailorGram::APPLICATION_PRETTY_NAME = "SailorGram";

SailorGram::SailorGram(QObject *parent): QObject(parent), _engine(NULL), _connectivitychecker(NULL), _connected(-1), _daemonized(false), _globalmute(false)
{
    QDir cfgdir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    cfgdir.mkpath(qApp->applicationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + SailorGram::CONFIG_FOLDER);

    QDir cachedir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    cachedir.mkpath(qApp->applicationName() + QDir::separator() + qApp->applicationName());

    this->_interface = new SailorgramInterface(this);
    this->_autostart = !SailorGram::hasDaemonFile();

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
    connect(this, SIGNAL(daemonizedChanged()), this, SLOT(updateOnlineState()));
    connect(this->_interface, SIGNAL(wakeUpRequested()), this, SLOT(onWakeUpRequested()));
    connect(this->_interface, SIGNAL(openDialogRequested(qint32)), this, SIGNAL(openDialogRequested(qint32)));
}

bool SailorGram::autostart()
{
    return this->_autostart;
}

bool SailorGram::globalMute() const
{
    return this->_globalmute;
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

TelegramEngine *SailorGram::engine() const
{
    return this->_engine;
}

const QString &SailorGram::currentPeerKey() const
{
    return this->_currentpeerkey;
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

void SailorGram::setEngine(TelegramEngine *engine)
{
    if(this->_engine == engine)
        return;

    this->_engine = engine;

    if(this->_connectivitychecker)
        this->_connectivitychecker->deleteLater();

    this->_connectivitychecker = new ConnectivityChecker(engine, this);
    connect(this->_connectivitychecker, SIGNAL(connectedChanged()), this, SIGNAL(connectedChanged()));

    emit engineChanged();
    emit connectedChanged();
}

void SailorGram::setCurrentPeerKey(const QString& peerkey)
{
    if(this->_currentpeerkey == peerkey)
        return;

    this->_currentpeerkey = peerkey;
    emit currentPeerKey();
}

void SailorGram::setKeepRunning(bool keep)
{
    if(keep == this->keepRunning())
        return;

    qApp->setQuitOnLastWindowClosed(!keep);
    emit keepRunningChanged();
}

void SailorGram::setGlobalMute(bool globalmute)
{
    if(this->_globalmute == globalmute)
        return;

    this->_globalmute = globalmute;
    emit globalMuteChanged();
}

void SailorGram::setAutostart(bool autostart)
{
    if(this->_autostart == autostart)
        return;

    this->_autostart = autostart;

    QDir dir(this->configPath());

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
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QDir::separator() + SailorGram::CONFIG_FOLDER);
    return QFile::exists(dir.absoluteFilePath(SailorGram::DAEMON_FILE));
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

void SailorGram::sendNotify(const QString& title, const QString& body, const QString &peerkey)
{
    Notification* notification = NULL;

    if(!this->_notifications.contains(peerkey))
    {
        notification = new Notification(this);

        QVariantMap remoteaction;
        remoteaction["name"] = "default";
        remoteaction["service"] = "org.harbour.sailorgram";
        remoteaction["path"] = "/";
        remoteaction["iface"] = "org.harbour.sailorgram";
        remoteaction["method"] = "openDialog";
        remoteaction["arguments"] = (QVariantList() << peerkey);
        remoteaction["icon"] = "icon-m-notifications";

        notification->setRemoteAction(remoteaction);
        this->_notifications[peerkey] = notification;

        connect(notification, SIGNAL(closed(uint)), this, SLOT(onNotificationClosed(uint)));
    }
    else
        notification = this->_notifications[peerkey];

    notification->setCategory("harbour.sailorgram.notification");
    notification->setAppName(SailorGram::APPLICATION_PRETTY_NAME);
    notification->setTimestamp(QDateTime::currentDateTime());

    if(!title.isEmpty())
    {
        notification->setPreviewSummary(title);
        notification->setSummary(title);
    }

    if(!body.isEmpty())
    {
        notification->setPreviewBody(body);
        notification->setBody(body);
    }

    notification->publish();
}

void SailorGram::closeNotification(const QString& peerkey)
{
    if(!this->_notifications.contains(peerkey))
        return;

    Notification* notification = this->_notifications[peerkey];
    notification->close();
    notification->deleteLater();
    this->_notifications.remove(peerkey);
}

void SailorGram::updateOnlineState()
{
    /*
    if(!this->_telegram)
        return;

    this->_telegram->setOnline(!this->_daemonized);
    */
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
    /*
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
    */
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
    QString peerkey = notification->remoteActions().first().toMap()["arguments"].toList().first().toString();

    this->_notifications.remove(peerkey);

    if(this->_topmessages.contains(peerkey))
        this->_topmessages.remove(peerkey);

    notification->deleteLater();
}

void SailorGram::onMessageUnreadedChanged()
{
    /*
    MessageObject* message = qobject_cast<MessageObject*>(this->sender());

    if(message->unread())
        return;

    QTimer* timer = new QTimer(this);
    timer->setTimerType(Qt::VeryCoarseTimer);
    timer->setSingleShot(true);

    connect(timer, SIGNAL(timeout()), this, SLOT(delayedCloseNotification()));

    //FIXME: this->_deletednotifications[timer->timerId()] = message;
    timer->start(2000); // 2 seconds
    */
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
    /* FIXME:
    if(mediatype == MessageMedia::typeMessageMediaAudio)
        return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    if(mediatype == MessageMedia::typeMessageMediaPhoto)
        return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    if(mediatype == MessageMedia::typeMessageMediaVideo)
        return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    */

    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

void SailorGram::updatePendingState(const QString& peerkey)
{
    if(this->_topmessages.contains(peerkey)) // Top Message has been changed
        this->_topmessages.remove(peerkey);

    this->_topmessages[peerkey] = NULL; //message;

    //connect(message, SIGNAL(unreadChanged()), this, SLOT(onMessageUnreadedChanged()));
}

void SailorGram::moveMediaToDownloads(const QString& mediafile)
{
    //this->moveMediaTo(mediafile, this->mediaLocation(MessageMedia::typeMessageMediaDocument));
}

void SailorGram::moveMediaToGallery(const QString& mediafile, quint32 mediatype)
{
    this->moveMediaTo(mediafile, this->mediaLocation(mediatype));
}

void SailorGram::notify(const QString& title, const QString& body, const QString& peerkey)
{
    if(!this->_engine || this->_globalmute)
        return;

    bool active = qApp->applicationState() == Qt::ApplicationActive;

    if((this->_currentpeerkey != peerkey) || !active)
    {
        //this->updatePendingState(peerid);
        this->sendNotify(title, body, peerkey);
    }
    else
        this->beep();
}
