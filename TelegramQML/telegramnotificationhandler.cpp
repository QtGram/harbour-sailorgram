#include "telegramnotificationhandler.h"
#include "telegramengine.h"
#include "telegram.h"
#include "telegramsharedpointer.h"
#include "telegramshareddatamanager.h"

#include <QDateTime>
#include <QPointer>

class TelegramNotificationHandlerPrivate
{
public:
    QPointer<TelegramEngine> engine;
    QPointer<Telegram> telegram;
    int unreadCount;
    int globalUnreadCount;

    QSet<DialogObject*> connectedDialogs;
};

TelegramNotificationHandler::TelegramNotificationHandler(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramNotificationHandlerPrivate;
    p->unreadCount = 0;
    p->globalUnreadCount = 0;
}

void TelegramNotificationHandler::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramNotificationHandler::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramNotificationHandler::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramNotificationHandler::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramNotificationHandler::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramNotificationHandler::engine() const
{
    return p->engine;
}

int TelegramNotificationHandler::unreadCount() const
{
    return p->unreadCount;
}

int TelegramNotificationHandler::globalUnreadCount() const
{
    return p->globalUnreadCount;
}

QStringList TelegramNotificationHandler::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine);
}

void TelegramNotificationHandler::refresh()
{
    if(!p->engine || !p->engine->telegram() || !p->engine->sharedData())
        return;

    if(p->telegram)
    {
        disconnect(p->telegram.data(), &Telegram::updates, this, &TelegramNotificationHandler::onUpdates);
        disconnect(p->telegram.data(), &Telegram::messagesGetDialogsAnswer, this, &TelegramNotificationHandler::refreshUnreads);
    }

    p->telegram = p->engine->telegram();
    if(p->telegram)
    {
        connect(p->telegram.data(), &Telegram::updates, this, &TelegramNotificationHandler::onUpdates);
        connect(p->telegram.data(), &Telegram::messagesGetDialogsAnswer, this, &TelegramNotificationHandler::refreshUnreads);
    }
}

void TelegramNotificationHandler::onUpdates(const UpdatesType &updates)
{
    TelegramTools::analizeUpdatesType(updates, p->engine, [this](const Update &update){
        insertUpdate(update);
    });
}

void TelegramNotificationHandler::refreshUnreads()
{
    if(!p->engine)
        return;

    Telegram *tg = p->engine->telegram();
    TelegramSharedDataManager *tsdm = p->engine->sharedData();
    if(!tg || !tsdm)
        return;

    int unreadCount = 0;
    int globalUnreadCount = 0;
    QList<DialogObject*> dialogs = tsdm->dialogs();
    Q_FOREACH(DialogObject *dlg, dialogs)
    {
        globalUnreadCount += dlg->unreadCount();
        if(QDateTime::fromTime_t(dlg->notifySettings()->muteUntil()) > QDateTime::currentDateTime())
            continue;

        unreadCount += dlg->unreadCount();
        if(!p->connectedDialogs.contains(dlg))
        {
            connect(dlg, &DialogObject::unreadCountChanged, this, &TelegramNotificationHandler::refreshUnreads, Qt::QueuedConnection);
            connect(dlg, &DialogObject::destroyed, this, [this, dlg](){
                p->connectedDialogs.remove(dlg);
            });
            p->connectedDialogs.insert(dlg);
        }
    }

    if(p->unreadCount != unreadCount)
    {
        p->unreadCount = unreadCount;
        Q_EMIT unreadCountChanged();
    }
    if(p->globalUnreadCount != globalUnreadCount)
    {
        p->globalUnreadCount = globalUnreadCount;
        Q_EMIT globalUnreadCountChanged();
    }
}

void TelegramNotificationHandler::insertUpdate(const Update &update)
{
    if(!p->engine)
        return;

    Telegram *tg = p->engine->telegram();
    TelegramSharedDataManager *tsdm = p->engine->sharedData();
    if(!tg || !tsdm)
        return;

    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
    {
        const Message &msg = update.message();
        if(msg.out())
            return;
        Peer msgPeer = TelegramTools::messagePeer(msg);
        QByteArray msgPeerKey = TelegramTools::identifier(msgPeer);

        QString title;
        if(update.classType()==Update::typeUpdateNewChannelMessage) {
            Peer fromPeer;
            fromPeer.setChannelId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerChannel);

            QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);
            TelegramSharedPointer<ChatObject> chat = tsdm->getChat(fromPeerKey);
            if(chat)
                title = chat->title();
        } else {
            Peer fromPeer;
            fromPeer.setUserId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerUser);

            QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);
            TelegramSharedPointer<UserObject> user = tsdm->getUser(fromPeerKey);
            if(user)
                title = (user->firstName() + " " + user->lastName()).trimmed();
        }

        TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(msgPeerKey);
        if(dialog && dialog->notifySettings()->muteUntil() > (qint32)QDateTime::currentDateTime().toTime_t() )
            return;

        Q_EMIT newMessage(title, msg.message(), msgPeerKey.toHex());
    }
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteMessages:
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
        break;
    case Update::typeUpdateChatParticipants:
        break;
    case Update::typeUpdateUserStatus:
        break;
    case Update::typeUpdateUserName:
        break;
    case Update::typeUpdateUserPhoto:
        break;
    case Update::typeUpdateContactRegistered:
        break;
    case Update::typeUpdateContactLink:
        break;
    case Update::typeUpdateNewAuthorization:
        break;
    case Update::typeUpdateNewEncryptedMessage:
        break;
    case Update::typeUpdateEncryptedChatTyping:
        break;
    case Update::typeUpdateEncryption:
        break;
    case Update::typeUpdateEncryptedMessagesRead:
        break;
    case Update::typeUpdateChatParticipantAdd:
        break;
    case Update::typeUpdateChatParticipantDelete:
        break;
    case Update::typeUpdateDcOptions:
        break;
    case Update::typeUpdateUserBlocked:
        break;
    case Update::typeUpdateNotifySettings:
        break;
    case Update::typeUpdateServiceNotification:
        break;
    case Update::typeUpdatePrivacy:
        break;
    case Update::typeUpdateUserPhone:
        break;
    case Update::typeUpdateReadHistoryInbox:
        break;
    case Update::typeUpdateReadHistoryOutbox:
        break;
    case Update::typeUpdateWebPage:
        break;
    case Update::typeUpdateReadMessagesContents:
        break;
    case Update::typeUpdateChannelTooLong:
        break;
    case Update::typeUpdateChannel:
        break;
    case Update::typeUpdateReadChannelInbox:
        break;
    case Update::typeUpdateDeleteChannelMessages:
        break;
    case Update::typeUpdateChannelMessageViews:
        break;
    case Update::typeUpdateChatAdmins:
        break;
    case Update::typeUpdateChatParticipantAdmin:
        break;
    case Update::typeUpdateNewStickerSet:
        break;
    case Update::typeUpdateStickerSetsOrder:
        break;
    case Update::typeUpdateStickerSets:
        break;
    case Update::typeUpdateSavedGifs:
        break;
    case Update::typeUpdateBotInlineQuery:
        break;
    case Update::typeUpdateBotInlineSend:
        break;
    }
}

TelegramNotificationHandler::~TelegramNotificationHandler()
{
    delete p;
}

