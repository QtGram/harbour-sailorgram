#define DEFINE_DIS \
    QPointer<TelegramPeerDetails> dis = this;

#include "telegrampeerdetails.h"
#include "telegramsharedpointer.h"
#include "telegramtools.h"
#include "telegramengine.h"
#include "telegramshareddatamanager.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QJSValue>
#include <QPointer>
#include <QQmlEngine>
#include <QtQml>

class TelegramPeerDetailsPrivate
{
public:
    TelegramSharedPointer<TelegramTypeQObject> peerRoot;
    TelegramSharedPointer<DialogObject> dialog;
    TelegramSharedPointer<UserObject> user;
    TelegramSharedPointer<ChatObject> chat;
    TelegramSharedPointer<UserFullObject> userFull;
    TelegramSharedPointer<ChatFullObject> chatFull;

    QList< TelegramSharedPointer<UserObject> > chatUsers;

    QPointer<InputPeerObject> peer;
    QPointer<TelegramEngine> engine;
    QPointer<Telegram> lastTelegram;
    QJSValue dateConvertorMethod;

    bool joined;
    bool refreshing;
    QString username;
};

TelegramPeerDetails::TelegramPeerDetails(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramPeerDetailsPrivate;
    p->joined = false;
    p->refreshing = false;
}

void TelegramPeerDetails::setPeer(InputPeerObject *peer)
{
    if(p->peer == peer)
        return;

    p->peer = peer;
    refresh();

    Q_EMIT peerChanged();
    Q_EMIT keyChanged();
    Q_EMIT isChatChanged();
    Q_EMIT isUserChanged();
    Q_EMIT isChannelChanged();
    Q_EMIT muteChanged();
}

InputPeerObject *TelegramPeerDetails::peer() const
{
    return p->peer;
}

bool TelegramPeerDetails::isChat() const
{
    if(!p->peer) return false;
    return (p->peer->classType() == InputPeerObject::TypeInputPeerChat);
}

bool TelegramPeerDetails::isUser() const
{
    if(!p->peer) return false;
    return (p->peer->classType() == InputPeerObject::TypeInputPeerUser);
}

bool TelegramPeerDetails::isChannel() const
{
    if(!p->peer) return false;
    return (p->peer->classType() == InputPeerObject::TypeInputPeerChannel);
}

void TelegramPeerDetails::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramPeerDetails::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramPeerDetails::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramPeerDetails::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramPeerDetails::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramPeerDetails::engine() const
{
    return p->engine;
}

QJSValue TelegramPeerDetails::dateConvertorMethod() const
{
    return p->dateConvertorMethod;
}

void TelegramPeerDetails::setDateConvertorMethod(const QJSValue &method)
{
    if(p->dateConvertorMethod.isNull() && method.isNull())
        return;

    p->dateConvertorMethod = method;
    Q_EMIT dateConvertorMethodChanged();
}

QString TelegramPeerDetails::key() const
{
    if(!p->peer)
        return QString();

    Peer peer = TelegramTools::inputPeerPeer(p->peer->core());
    return TelegramTools::identifier(peer).toHex();
}

QString TelegramPeerDetails::displayName() const
{
    if(p->chat)
        return p->chat->title();
    else
    if(p->user)
        return (p->user->firstName() + " " + p->user->lastName()).trimmed();
    else
        return "";
}

int TelegramPeerDetails::participantsCount() const
{
    if(!p->chat)
        return 0;

    switch(p->chat->classType())
    {
    case ChatObject::TypeChat:
    case ChatObject::TypeChatForbidden:
        return p->chat->participantsCount();
        break;

    case ChatObject::TypeChannel:
    case ChatObject::TypeChannelForbidden:
        if(p->chatFull)
            return p->chatFull->participantsCount();
        break;
    }
    return 0;
}

QString TelegramPeerDetails::statusText() const
{
    if(p->user)
    {
        return TelegramTools::userStatus(p->user, [this](const QDateTime &dt){
            return convertDate(dt);
        });
    }
    else
    if(p->chat)
    {
        if(isChat())
            return tr("%1 members").arg(p->chat->participantsCount());
        else
        if(isChannel() && p->chatFull && p->chatFull->participantsCount())
            return tr("%1 members").arg(p->chatFull->participantsCount());
    }

    return QString();
}

QString TelegramPeerDetails::phoneNumber() const
{
    if(p->user)
        return p->user->phone();
    else
        return QString();
}

QString TelegramPeerDetails::username() const
{
    if(!p->username.isEmpty())
        return p->username;
    if(p->user)
        return p->user->username();
    else
    if(p->chat)
        return p->chat->username();
    else
        return QString();
}

void TelegramPeerDetails::setUsername(const QString &username)
{
    if(p->username == username)
        return;

    p->username = username;
    fetchUsername();
    Q_EMIT usernameChanged();
}

void TelegramPeerDetails::setMute(bool mute)
{
    if(mute == TelegramPeerDetails::mute())
        return;

    PeerNotifySettings settings = p->dialog->notifySettings()->core();
    Peer peer = p->dialog->peer()->core();

    qint64 accessHash = 0;
    if(p->chat)
        accessHash = p->chat->accessHash();
    else
    if(p->user)
        accessHash = p->user->accessHash();

    InputNotifyPeer inputPeer(InputNotifyPeer::typeInputNotifyPeer);
    inputPeer.setPeer(TelegramTools::peerInputPeer(peer, accessHash));

    qint32 muteUntil = 0;
    if(mute)
        muteUntil = 1490644268;

    InputPeerNotifySettings inputSettings;
    inputSettings.setMuteUntil(muteUntil);
    inputSettings.setSilent(settings.silent());
    inputSettings.setShowPreviews(settings.showPreviews());
    inputSettings.setSound(settings.sound());

    if(!p->engine)
        return;
    Telegram *tg = p->engine->telegram();
    if(!tg)
        return;

    p->dialog->notifySettings()->setMuteUntil(muteUntil);

    DEFINE_DIS;
    tg->accountUpdateNotifySettings(inputPeer, inputSettings, [this, dis, settings](TG_ACCOUNT_UPDATE_NOTIFY_SETTINGS_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        if(result) return;
        p->dialog->notifySettings()->operator =(settings);
    });

    Q_EMIT muteChanged();
}

bool TelegramPeerDetails::mute() const
{
    if(!p->dialog)
        return false;

    return (QDateTime::fromTime_t(p->dialog->notifySettings()->muteUntil()) > QDateTime::currentDateTime());
}

void TelegramPeerDetails::setBlocked(bool blocked)
{
    if(blocked == TelegramPeerDetails::blocked())
        return;
    if(!p->userFull || !p->user)
        return;

    InputUser inputUser(InputUser::typeInputUser);
    inputUser.setUserId(p->user->id());
    inputUser.setAccessHash(p->user->accessHash());

    if(!p->engine)
        return;
    Telegram *tg = p->engine->telegram();
    if(!tg)
        return;

    p->userFull->setBlocked(blocked);

    DEFINE_DIS;
    if(blocked)
        tg->contactsBlock(inputUser, [this, dis](TG_CONTACTS_BLOCK_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            if(result || !p->userFull) return;
            p->userFull->setBlocked(true);
        });
    else
        tg->contactsUnblock(inputUser, [this, dis](TG_CONTACTS_UNBLOCK_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            if(result || !p->userFull) return;
            p->userFull->setBlocked(false);
        });

    Q_EMIT blockedChanged();
}

bool TelegramPeerDetails::blocked() const
{
    if(p->userFull)
        return p->userFull->blocked();
    else
        return false;
}

void TelegramPeerDetails::setJoined(bool joined)
{
    if(joined == p->joined)
        return;
    if(!p->peer || !p->chat)
        return;
    if(joined && p->peer->classType() != InputPeerObject::TypeInputPeerChannel)
        return;

    InputUser inputUser(InputUser::typeInputUser);
    inputUser.setUserId(p->user->id());
    inputUser.setAccessHash(p->user->accessHash());

    if(!p->engine)
        return;
    Telegram *tg = p->engine->telegram();
    if(!tg)
        return;

    p->joined = joined;

    DEFINE_DIS;
    if(joined)
    {
        InputChannel channel(InputChannel::typeInputChannel);
        channel.setChannelId(p->peer->channelId());
        channel.setAccessHash(p->peer->accessHash());

        tg->channelsJoinChannel(channel, [this, dis](TG_CHANNELS_JOIN_CHANNEL_CALLBACK){
            Q_UNUSED(msgId)
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            p->joined = true;
            Q_EMIT joinedChanged();
        });
    }
    else
    if(p->chat->classType() == ChatObject::TypeChannel)
    {
        InputChannel channel(InputChannel::typeInputChannel);
        channel.setChannelId(p->peer->channelId());
        channel.setAccessHash(p->peer->accessHash());

        tg->channelsLeaveChannel(channel, [this, dis](TG_CHANNELS_LEAVE_CHANNEL_CALLBACK){
            Q_UNUSED(msgId)
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            p->joined = false;
            Q_EMIT joinedChanged();
        });
    }
    else
    {
        tg->messagesDeleteChatUser(p->peer->chatId(), InputUser::typeInputUserSelf, [this, dis](TG_MESSAGES_DELETE_CHAT_USER_CALLBACK){
            Q_UNUSED(msgId)
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            p->joined = false;
            Q_EMIT joinedChanged();
        });
    }

    Q_EMIT joinedChanged();
}

bool TelegramPeerDetails::joined() const
{
    return p->joined;
}

bool TelegramPeerDetails::refreshing() const
{
    return p->refreshing;
}

void TelegramPeerDetails::setRefreshing(bool refreshing)
{
    if(p->refreshing == refreshing)
        return;

    p->refreshing = refreshing;
    Q_EMIT refreshingChanged();
}

UserFullObject *TelegramPeerDetails::userFull() const
{
    return p->userFull;
}

ChatFullObject *TelegramPeerDetails::chatFull() const
{
    return p->chatFull;
}

QVariantList TelegramPeerDetails::chatUsers() const
{
    QVariantList result;
    Q_FOREACH(const TelegramSharedPointer<UserObject> &user, p->chatUsers)
        result << QVariant::fromValue<QObject*>(user.data());
    return result;
}

QStringList TelegramPeerDetails::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine) << FUNCTION_NAME(peer);
}

void TelegramPeerDetails::initTelegram()
{
    if(p->engine->telegram() == p->lastTelegram)
        return;

    if(p->lastTelegram)
    {
        disconnect(p->lastTelegram.data(), &Telegram::updates, this, &TelegramPeerDetails::onUpdates);
    }

    p->lastTelegram = p->engine->telegram();
    if(p->lastTelegram)
    {
        connect(p->lastTelegram.data(), &Telegram::updates, this, &TelegramPeerDetails::onUpdates);
    }
}

void TelegramPeerDetails::refresh()
{
    initTelegram();

    connectDialogSignals(p->dialog, true);
    connectUserSignals(p->user, true);
    connectChatSignals(p->chat, true);

    p->peerRoot = TelegramTools::objectRoot(p->peer);
    if(!p->engine || !p->peer || !p->engine->telegram() || !p->engine->sharedData())
    {
        if(!p->username.isEmpty())
        {
            fetchUsername();
            return;
        }

        p->dialog = 0;
        p->user = 0;
        p->chat = 0;
        p->userFull = 0;
        p->chatFull = 0;
        p->chatUsers.clear();
        p->joined = false;
        Q_EMIT displayNameChanged();
        Q_EMIT userFullChanged();
        Q_EMIT chatFullChanged();
        Q_EMIT chatUsersChanged();
        Q_EMIT participantsCountChanged();
        Q_EMIT statusTextChanged();
        Q_EMIT phoneNumberChanged();
        Q_EMIT usernameChanged();
        Q_EMIT blockedChanged();
        Q_EMIT joinedChanged();
        return;
    }

    QPointer<TelegramSharedDataManager> tsdm = p->engine->sharedData();

    const Peer &peer = TelegramTools::inputPeerPeer(p->peer->core());
    const QByteArray &key = TelegramTools::identifier(peer);

    p->dialog = tsdm->getDialog(key);
    p->user = tsdm->getUser(key);
    p->chat = tsdm->getChat(key);
    p->userFull = tsdm->getUserFull(key);
    p->chatFull = tsdm->getChatFull(key);
    p->chatUsers.clear();

    p->joined = p->dialog;

    connectDialogSignals(p->dialog);
    connectUserSignals(p->user);
    connectChatSignals(p->chat);

    Telegram *tg = p->engine->telegram();
    DEFINE_DIS;
    if(p->user)
    {
        InputUser user(InputUser::typeInputUser);
        user.setUserId(p->user->id());
        user.setAccessHash(p->user->accessHash());
        tg->usersGetFullUser(user, [this, dis, tsdm](TG_USERS_GET_FULL_USER_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;

            setRefreshing(false);
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            if(p->userFull)
                disconnect(p->userFull.data(), &UserFullObject::blockedChanged, this, &TelegramPeerDetails::blockedChanged);

            p->userFull = tsdm->insertUserFull(result);
            if(p->userFull)
                connect(p->userFull.data(), &UserFullObject::blockedChanged, this, &TelegramPeerDetails::blockedChanged);

            Q_EMIT userFullChanged();
            Q_EMIT blockedChanged();
        });
    }
    if(p->chat)
    {
        switch(p->chat->classType())
        {
        case ChatObject::TypeChat:
        case ChatObject::TypeChatForbidden:
            setRefreshing(true);
            tg->messagesGetFullChat(p->chat->id(), [this, dis](TG_MESSAGES_GET_FULL_CHAT_CALLBACK){
                Q_UNUSED(msgId)
                if(!dis) return;

                setRefreshing(false);
                if(!error.null) {
                    setError(error.errorText, error.errorCode);
                    return;
                }
                insertChatFull(result);
            });
            break;

        case ChatObject::TypeChannel:
        case ChatObject::TypeChannelForbidden:
        {
            InputChannel channel(InputChannel::typeInputChannel);
            channel.setChannelId(p->chat->id());
            channel.setAccessHash(p->chat->accessHash());
            tg->channelsGetFullChannel(channel, [this, dis](TG_CHANNELS_GET_FULL_CHANNEL_CALLBACK){
                Q_UNUSED(msgId)
                if(!dis) return;

                setRefreshing(false);
                if(!error.null) {
                    setError(error.errorText, error.errorCode);
                    return;
                }
                insertChatFull(result);

                if(result.chats().length())
                {
                    p->joined = (!result.chats().first().left() && !result.chats().first().kicked());
                    Q_EMIT joinedChanged();
                }
            });
        }
            break;
        }
    }

    Q_EMIT displayNameChanged();
    Q_EMIT userFullChanged();
    Q_EMIT chatFullChanged();
    Q_EMIT chatUsersChanged();
    Q_EMIT participantsCountChanged();
    Q_EMIT statusTextChanged();
    Q_EMIT phoneNumberChanged();
    Q_EMIT usernameChanged();
    Q_EMIT blockedChanged();
    Q_EMIT joinedChanged();
}

void TelegramPeerDetails::fetchUsername()
{
    if(!p->engine || p->username.isEmpty() || !p->engine->telegram())
        return;

    Telegram *tg = p->engine->telegram();

    setRefreshing(true);
    DEFINE_DIS;
    tg->contactsResolveUsername(p->username, [this, dis](TG_CONTACTS_RESOLVE_USERNAME_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        setRefreshing(false);
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        TelegramSharedDataManager *tsdm = p->engine->sharedData();
        QSet< TelegramSharedPointer<TelegramTypeQObject> > cache;

        Peer peer = result.peer();
        InputPeer inputPeer = TelegramTools::peerInputPeer(peer, 0);

        Q_FOREACH(const Chat &chat, result.chats())
        {
            cache.insert( tsdm->insertChat(chat).data() );
            if(peer.classType() == Peer::typePeerChat && peer.chatId() == chat.id())
                inputPeer.setAccessHash(chat.accessHash());
            else
            if(peer.classType() == Peer::typePeerChannel && peer.channelId() == chat.id())
                inputPeer.setAccessHash(chat.accessHash());
        }
        Q_FOREACH(const User &user, result.users())
        {
            cache.insert( tsdm->insertUser(user).data() );
            if(peer.classType() == Peer::typePeerUser && peer.userId() == user.id())
                inputPeer.setAccessHash(user.accessHash());
        }

        TelegramSharedPointer<InputPeerObject> obj = p->engine->sharedData()->insertInputPeer(inputPeer);
        setPeer(obj);
    });
}

void TelegramPeerDetails::connectChatSignals(ChatObject *chat, bool dc)
{
    if(!chat)
        return;

    if(dc) {
        disconnect(chat, &ChatObject::titleChanged, this, &TelegramPeerDetails::displayNameChanged);
        disconnect(chat, &ChatObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
        disconnect(chat, &ChatObject::usernameChanged, this, &TelegramPeerDetails::usernameChanged);
    } else {
        connect(chat, &ChatObject::titleChanged, this, &TelegramPeerDetails::displayNameChanged);
        connect(chat, &ChatObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
        connect(chat, &ChatObject::usernameChanged, this, &TelegramPeerDetails::usernameChanged);
    }
}

void TelegramPeerDetails::connectUserSignals(UserObject *user, bool dc)
{
    if(!user)
        return;

    if(dc) {
        disconnect(user, &UserObject::firstNameChanged, this, &TelegramPeerDetails::displayNameChanged);
        disconnect(user, &UserObject::lastNameChanged, this, &TelegramPeerDetails::displayNameChanged);
        disconnect(user, &UserObject::phoneChanged, this, &TelegramPeerDetails::phoneNumberChanged);
        disconnect(user, &UserObject::usernameChanged, this, &TelegramPeerDetails::usernameChanged);
    } else {
        connect(user, &UserObject::firstNameChanged, this, &TelegramPeerDetails::displayNameChanged);
        connect(user, &UserObject::lastNameChanged, this, &TelegramPeerDetails::displayNameChanged);
        connect(user, &UserObject::phoneChanged, this, &TelegramPeerDetails::phoneNumberChanged);
        connect(user, &UserObject::usernameChanged, this, &TelegramPeerDetails::usernameChanged);
    }
}

void TelegramPeerDetails::connectDialogSignals(DialogObject *dialog, bool dc)
{
    if(!dialog)
        return;

    if(dc) {
        disconnect(dialog->notifySettings(), &PeerNotifySettingsObject::muteUntilChanged, this, &TelegramPeerDetails::muteChanged);
    } else {
        connect(dialog->notifySettings(), &PeerNotifySettingsObject::muteUntilChanged, this, &TelegramPeerDetails::muteChanged);
    }
}

void TelegramPeerDetails::insertChatFull(const MessagesChatFull &result)
{
    if(!p->engine || !p->engine->sharedData())
        return;
    if(p->chatFull)
    {
        disconnect(p->chatFull.data(), &ChatFullObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
    }

    TelegramSharedDataManager *tsdm = p->engine->sharedData();

    p->chatUsers.clear();
    Q_FOREACH(const User &user, result.users())
        p->chatUsers << tsdm->insertUser(user);

    p->chatFull = tsdm->insertChatFull(result.fullChat());
    if(p->chatFull)
    {
        connect(p->chatFull.data(), &ChatFullObject::participantsCountChanged, this, &TelegramPeerDetails::participantsCountChanged);
    }

    Q_EMIT chatFullChanged();
    Q_EMIT chatUsersChanged();
    Q_EMIT participantsCountChanged();
    Q_EMIT statusTextChanged();
}

QString TelegramPeerDetails::convertDate(const QDateTime &td) const
{
    QQmlEngine *engine = qmlEngine(this);
    if(p->dateConvertorMethod.isCallable() && engine)
        return p->dateConvertorMethod.call(QList<QJSValue>()<<engine->toScriptValue<QDateTime>(td)).toString();
    else
    if(!p->dateConvertorMethod.isNull() && !p->dateConvertorMethod.isUndefined())
        return p->dateConvertorMethod.toString();
    else
    {
        const QDateTime &current = QDateTime::currentDateTime();
        const qint64 secs = td.secsTo(current);
        const int days = td.daysTo(current);
        if(secs < 24*60*60)
            return days? "Yesterday " + td.toString("HH:mm") : td.toString("HH:mm");
        else
            return td.toString("MMM dd, HH:mm");
    }
}

void TelegramPeerDetails::onUpdates(const UpdatesType &updates)
{
    TelegramTools::analizeUpdatesType(updates, p->engine, [this](const Update &update){
        insertUpdate(update);
    });
}

void TelegramPeerDetails::insertUpdate(const Update &update)
{
    if(!p->engine)
        return;

    Telegram *tg = p->engine->telegram();
    if(!tg)
        return;

    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteMessages:
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
        break;
    case Update::typeUpdateChatParticipants:
    {
        if(p->chat && p->chat->id() == update.participants().chatId())
        {
            p->chat->setParticipantsCount(update.participants().participants().count());
            if(p->chatFull)
                p->chatFull->participants()->operator =(update.participants());
        }
    }
        break;
    case Update::typeUpdateUserStatus:
    {
        if(p->user && p->user->id() == update.userId())
            p->user->status()->operator =(update.status());
    }
        break;
    case Update::typeUpdateUserName:
    {
        if(p->user && p->user->id() == update.userId())
        {
            p->user->setFirstName(update.firstName());
            p->user->setLastName(update.lastName());
            p->user->setUsername(update.username());
        }
    }
        break;
    case Update::typeUpdateUserPhoto:
    {
        if(p->user && p->user->id() == update.userId())
            p->user->photo()->operator =(update.photo());
    }
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
    {
        if(p->chat && p->chat->id() == update.chatId())
            p->chat->setParticipantsCount(p->chat->participantsCount()+1);
    }
        break;
    case Update::typeUpdateChatParticipantDelete:
    {
        if(p->chat && p->chat->id() == update.chatId())
            p->chat->setParticipantsCount(p->chat->participantsCount()-1);
    }
        break;
    case Update::typeUpdateDcOptions:
        break;
    case Update::typeUpdateUserBlocked:
        break;
    case Update::typeUpdateNotifySettings:
    {
        const NotifyPeer &peer = update.peerNotify();
        const PeerNotifySettings &settings = update.notifySettings();
        switch(static_cast<int>(peer.classType()))
        {
        case NotifyPeer::typeNotifyAll:
            break;
        case NotifyPeer::typeNotifyChats:
            break;
        case NotifyPeer::typeNotifyUsers:
            break;
        case NotifyPeer::typeNotifyPeer:
        {
            if(p->dialog && p->dialog->peer()->core() == peer.peer())
                p->dialog->notifySettings()->operator =(settings);
        }
            break;
        }
    }
        break;
    case Update::typeUpdateServiceNotification:
        break;
    case Update::typeUpdatePrivacy:
        break;
    case Update::typeUpdateUserPhone:
    {
        if(p->user && p->user->id() == update.userId())
            p->user->setPhone(update.phone());
    }
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

TelegramPeerDetails::~TelegramPeerDetails()
{
    delete p;
}

