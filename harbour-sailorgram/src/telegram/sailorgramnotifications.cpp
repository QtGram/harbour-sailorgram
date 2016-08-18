#include "sailorgramnotifications.h"
#include <QGuiApplication>
#include <QDateTime>
#include <QTimer>
#include <telegramshareddatamanager.h>
#include <telegramtools.h>
#include "sailorgramtools.h"

const QString SailorgramNotifications::APPLICATION_PRETTY_NAME = "SailorGram";

SailorgramNotifications::SailorgramNotifications(QObject *parent) : QObject(parent), _engine(NULL), _dialogs(NULL), _currentdialog(NULL), _globalmute(false)
{
    this->_notificationhandler = new TelegramNotificationHandler(this);

    connect(this->_notificationhandler, &TelegramNotificationHandler::unreadCountChanged, this, &SailorgramNotifications::unreadCountChanged);
    connect(this->_notificationhandler, &TelegramNotificationHandler::globalUnreadCountChanged, this, &SailorgramNotifications::globalUnreadCountChanged);
}

TelegramEngine *SailorgramNotifications::engine() const
{
    return this->_engine;
}

SailorgramDialogsModel *SailorgramNotifications::dialogs() const
{
    return this->_dialogs;
}

SailorgramDialogItem *SailorgramNotifications::currentDialog() const
{
    return this->_currentdialog;
}

int SailorgramNotifications::unreadCount() const
{
    return this->_notificationhandler->unreadCount();
}

int SailorgramNotifications::globalUnreadCount() const
{
    return this->_notificationhandler->globalUnreadCount();
}

bool SailorgramNotifications::globalMute() const
{
    return this->_globalmute;
}

void SailorgramNotifications::setEngine(TelegramEngine *engine)
{
    if(this->_engine == engine)
        return;

    if(this->_engine)
    {
        disconnect(this->_engine, &TelegramEngine::telegramChanged, this, &SailorgramNotifications::connectSignals);
        disconnect(this->_engine->telegram(), &Telegram::updates, this, &SailorgramNotifications::onUpdates);
    }

    if(engine)
    {
        connect(engine, &TelegramEngine::telegramChanged, this, &SailorgramNotifications::connectSignals);

        if(engine->telegram())
            connect(engine->telegram(), &Telegram::updates, this, &SailorgramNotifications::onUpdates);
    }

    this->_engine = engine;
    this->_notificationhandler->setEngine(engine);
    emit engineChanged();
}

void SailorgramNotifications::setDialogs(SailorgramDialogsModel *dialogs)
{
    if(this->_dialogs == dialogs)
        return;

    this->_dialogs = dialogs;
    emit dialogsChanged();
}

void SailorgramNotifications::setCurrentDialog(SailorgramDialogItem *dialog)
{
    if(this->_currentdialog == dialog)
        return;

    this->_currentdialog = dialog;

    if(dialog)
        this->closeNotification(dialog->peerKey());

    emit currentDialogChanged();
}

void SailorgramNotifications::setGlobalMute(bool b)
{
   if(this->_globalmute == b)
       return;

   this->_globalmute = b;
   emit globalMuteChanged();
}

void SailorgramNotifications::insertUpdate(const Update &update)
{
    if(this->_globalmute || !this->_engine || !this->_dialogs)
        return;

    Telegram *telegram = this->_engine->telegram();
    TelegramSharedDataManager *tsdm = this->_engine->sharedData();

    if(!telegram || !tsdm)
        return;

    const uint type = static_cast<uint>(update.classType());

    switch(type)
    {
        case Update::typeUpdateNewChannelMessage:
        case Update::typeUpdateNewMessage:
        {
            const Message& message = update.message();

            if(message.out())
                return;

            Peer msgpeer = TelegramTools::messagePeer(message);
            QByteArray peerkey = TelegramTools::identifier(msgpeer);
            TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(peerkey);

            if(dialog && (dialog->notifySettings()->muteUntil() > static_cast<qint32>(QDateTime::currentDateTime().toTime_t())))
                return;

            this->notify(message, peerkey, (type == Update::typeUpdateNewChannelMessage));
        }
        break;

        case Update::typeUpdateReadHistoryInbox:
        {
            const QByteArray& peerkey = TelegramTools::identifier(update.peer());
            this->messageReaded(peerkey, update.maxId());
        }
        break;

        case Update::typeUpdateReadChannelInbox:
        {
            Peer peer(Peer::typePeerChannel);
            peer.setChannelId(update.channelId());
            const QByteArray &peerkey = TelegramTools::identifier(peer);
            this->messageReaded(peerkey, update.maxId());
        }
        break;

        default:
            break;
    }
}

void SailorgramNotifications::messageReaded(const QByteArray &peerkey, qint32 maxid)
{
    if(!this->_topmessageids.contains(peerkey))
        return;

    qint32 topmessageid = this->_topmessageids[peerkey];
    bool isread = maxid >= topmessageid;

    if(!isread)
        return;

    QTimer* timer = new QTimer(this);
    timer->setTimerType(Qt::VeryCoarseTimer);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, &SailorgramNotifications::delayCloseNotification);

    this->_deletednotifications[timer->timerId()] = peerkey;
    timer->start(2000); // 2 seconds
}

void SailorgramNotifications::updatePendingState(const Message &message, const QByteArray &peerkey)
{
    if(this->_topmessageids.contains(peerkey)) // Top Message is changed
        this->_topmessageids.remove(peerkey);

    this->_topmessageids[peerkey] = message.id();
}

void SailorgramNotifications::closeNotification(const QByteArray &peerkey)
{
    if(!this->_notifications.contains(peerkey))
        return;

    Notification* notification = this->_notifications[peerkey];
    notification->close();
    notification->deleteLater();

    this->_notifications.remove(peerkey);
}

void SailorgramNotifications::notify(const Message& message, const QByteArray& peerkey, bool ischannel)
{
    bool active = qApp->applicationState() == Qt::ApplicationActive;

    if((!this->_currentdialog || (this->_currentdialog->peerKey() != peerkey)) || !active || message.mentioned())
    {
        this->updatePendingState(message, peerkey);
        this->sendNotify(message, peerkey, ischannel);
    }
    else
        this->beep();
}

void SailorgramNotifications::sendNotify(const Message &message, const QByteArray &peerkey, bool ischannel)
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

        connect(notification, &Notification::closed, this, &SailorgramNotifications::onNotificationClosed);
    }
    else
        notification = this->_notifications[peerkey];

    notification->setCategory("harbour.sailorgram.notification");
    notification->setAppName(SailorgramNotifications::APPLICATION_PRETTY_NAME);
    notification->setTimestamp(QDateTime::currentDateTime());

    QString title = this->notificationTitle(message, ischannel);
    QString body = this->notificationBody(message);

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

void SailorgramNotifications::beep()
{
    Notification notification;
    notification.setCategory("harbour.sailorgram.notificationfg");
    notification.setAppName(SailorgramNotifications::APPLICATION_PRETTY_NAME);
    notification.setTimestamp(QDateTime::currentDateTime());
    notification.publish();
}

QString SailorgramNotifications::notificationTitle(const Message &message, bool ischannel) const
{
    TelegramSharedDataManager* tsdm = this->_engine->sharedData();
    QString title;

    if(ischannel)
    {
        Peer frompeer;
        frompeer.setChannelId(message.fromId());
        frompeer.setClassType(Peer::typePeerChannel);

        QByteArray fromPeerKey = TelegramTools::identifier(frompeer);
        TelegramSharedPointer<ChatObject> chat = tsdm->getChat(fromPeerKey);

        if(chat)
            title = chat->title();
    }
    else
    {
        Peer frompeer;
        frompeer.setUserId(message.fromId());
        frompeer.setClassType(Peer::typePeerUser);

        QByteArray fromPeerKey = TelegramTools::identifier(frompeer);
        TelegramSharedPointer<UserObject> user = tsdm->getUser(fromPeerKey);

        if(user)
            title = (user->firstName() + " " + user->lastName()).trimmed();
    }

    return title;
}

QString SailorgramNotifications::notificationBody(const Message &message) const
{
    return SailorgramTools::messageText(message);
}

void SailorgramNotifications::resetCurrentDialog()
{
    this->setCurrentDialog(NULL);
}

void SailorgramNotifications::connectSignals()
{
    if(!this->_engine->telegram())
        return;

    connect(this->_engine->telegram(), &Telegram::updates, this, &SailorgramNotifications::onUpdates);
}

void SailorgramNotifications::onUpdates(const UpdatesType &updates)
{
    TelegramTools::analizeUpdatesType(updates, this->_engine, [this](const Update &update){
        this->insertUpdate(update);
    });
}

void SailorgramNotifications::delayCloseNotification()
{
    QTimer* timer = qobject_cast<QTimer*>(this->sender());

    if(!this->_deletednotifications.contains(timer->timerId()))
    {
        timer->deleteLater();
        return;
    }

    this->closeNotification(this->_deletednotifications[timer->timerId()]);
    this->_deletednotifications.remove(timer->timerId());
    timer->deleteLater();
}

void SailorgramNotifications::onNotificationClosed(uint)
{
    Notification* notification = qobject_cast<Notification*>(this->sender());
    QByteArray peerkey = notification->remoteActions().first().toMap()["arguments"].toList().first().toByteArray();

    this->_notifications.remove(peerkey);

    if(this->_topmessageids.contains(peerkey))
        this->_topmessageids.remove(peerkey);

    notification->deleteLater();
}
