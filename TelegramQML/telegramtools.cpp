#include "telegramtools.h"
#include "telegramsharedpointer.h"
#include "telegramengine.h"
#include "telegramshareddatamanager.h"

#include <QDataStream>
#include <QDateTime>

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

QByteArray TelegramTools::identifier(qint32 peerType, qint32 peerId)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << peerType;
    stream << peerId;
    return res;
}

QByteArray TelegramTools::identifier(const Peer &peer)
{
    QByteArray res;
    switch(static_cast<int>(peer.classType()))
    {
    case Peer::typePeerChannel:
        res = identifier(peer.classType(), peer.channelId());
        break;
    case Peer::typePeerChat:
        res = identifier(peer.classType(), peer.chatId());
        break;
    case Peer::typePeerUser:
        res = identifier(peer.classType(), peer.userId());
        break;
    }
    return res;
}

QByteArray TelegramTools::identifier(const InputPeer &peer)
{
    return identifier(inputPeerPeer(peer));
}

QByteArray TelegramTools::identifier(const Dialog &dialog)
{
    return identifier(dialog.peer());
}

QByteArray TelegramTools::identifier(const Dialog &dialog, qint32 messageId)
{
    return identifier(dialog.peer(), messageId);
}

QByteArray TelegramTools::identifier(const Peer &peer, qint32 messageId)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << identifier(peer);
    stream << messageId;

    return res;
}

QByteArray TelegramTools::identifier(const Message &msg)
{
    return identifier(msg.toId(), msg.id());
}

QByteArray TelegramTools::identifier(const Chat &chat)
{
    return identifier(chatPeer(chat));
}

QByteArray TelegramTools::identifier(const User &user)
{
    return identifier(userPeer(user));
}

QByteArray TelegramTools::identifier(const UserFull &user)
{
    return identifier(user.user());
}

QByteArray TelegramTools::identifier(const ChatFull &chat)
{
    Peer peer(Peer::typePeerChat);
    peer.setChatId(chat.id());
    return identifier(peer);
}

QByteArray TelegramTools::identifier(const StickerSet &stickerSet)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << static_cast<int>(stickerSet.classType());
    stream << stickerSet.id();
    stream << stickerSet.accessHash();

    return res;
}

QByteArray TelegramTools::identifier(const Document &document)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream << static_cast<int>(document.classType());
    stream << document.id();
    stream << document.accessHash();

    return res;
}

QByteArray TelegramTools::identifier(SecretChat *secretChat)
{
    return identifier(secretChatInputPeer(secretChat));
}

InputPeer TelegramTools::chatInputPeer(const Chat &chat)
{
    InputPeer res;
    res.setAccessHash(chat.accessHash());
    switch(static_cast<int>(chat.classType()))
    {
    case Chat::typeChat:
        res.setClassType(InputPeer::typeInputPeerChat);
        res.setChatId(chat.id());
        break;
    case Chat::typeChannel:
        res.setClassType(InputPeer::typeInputPeerChannel);
        res.setChannelId(chat.id());
        break;
    }
    return res;
}

InputPeer TelegramTools::userInputPeer(const User &user)
{
    InputPeer res;
    res.setUserId(user.id());
    res.setAccessHash(user.accessHash());
    if(user.self())
        res.setClassType(InputPeer::typeInputPeerSelf);
    else
    if(user.deleted())
        res.setClassType(InputPeer::typeInputPeerEmpty);
    else
        res.setClassType(InputPeer::typeInputPeerUser);

    return res;
}

InputPeer TelegramTools::peerInputPeer(const Peer &peer, qint64 accessHash)
{
    InputPeer res;
    switch(static_cast<uint>(peer.classType()))
    {
    case Peer::typePeerUser:
        res.setClassType(InputPeer::typeInputPeerUser);
        res.setUserId(peer.userId());
        res.setAccessHash(accessHash);
        break;
    case Peer::typePeerChat:
        res.setClassType(InputPeer::typeInputPeerChat);
        res.setChatId(peer.chatId());
        res.setAccessHash(accessHash);
        break;
    case Peer::typePeerChannel:
        res.setClassType(InputPeer::typeInputPeerChannel);
        res.setChannelId(peer.channelId());
        res.setAccessHash(accessHash);
        break;
    }
    return res;
}

InputPeer TelegramTools::secretChatInputPeer(SecretChat *secretChat)
{
    InputPeer inputPeer(InputPeer::typeInputPeerChat);
    inputPeer.setChatId(secretChat->chatId());
    inputPeer.setAccessHash(secretChat->accessHash());
    return inputPeer;
}

Peer TelegramTools::chatPeer(const Chat &chat)
{
    Peer peer;
    switch(static_cast<int>(chat.classType()))
    {
    case Chat::typeChat:
    case Chat::typeChatForbidden:
        peer.setClassType(Peer::typePeerChat);
        peer.setChatId(chat.id());
        break;
    case Chat::typeChannel:
    case Chat::typeChannelForbidden:
        peer.setClassType(Peer::typePeerChannel);
        peer.setChannelId(chat.id());
        break;
    }
    return peer;
}

Peer TelegramTools::userPeer(const User &user)
{
    Peer peer;
    switch(static_cast<int>(user.classType()))
    {
    case User::typeUser:
        peer.setClassType(Peer::typePeerUser);
        peer.setUserId(user.id());
        break;
    }
    return peer;
}

Peer TelegramTools::dialogPeer(const Dialog &dialog)
{
    return dialog.peer();
}

Peer TelegramTools::messagePeer(const Message &msg)
{
    Peer peer = msg.toId();
    if(!msg.out() && peer.classType() == Peer::typePeerUser)
        peer.setUserId(msg.fromId());
    return peer;
}

Peer TelegramTools::inputPeerPeer(const InputPeer &inputPeer)
{
    Peer peer;
    switch(static_cast<uint>(inputPeer.classType()))
    {
    case InputPeer::typeInputPeerSelf:
    case InputPeer::typeInputPeerUser:
        peer.setClassType(Peer::typePeerUser);
        peer.setUserId(inputPeer.userId());
        break;
    case InputPeer::typeInputPeerChat:
        peer.setClassType(Peer::typePeerChat);
        peer.setChatId(inputPeer.chatId());
        break;
    case InputPeer::typeInputPeerChannel:
        peer.setClassType(Peer::typePeerChannel);
        peer.setChannelId(inputPeer.channelId());
        break;
    }
    return peer;
}

InputMedia TelegramTools::mediaInputMedia(const MessageMedia &media)
{
    InputMedia result;
    switch(static_cast<int>(media.classType()))
    {
    case MessageMedia::typeMessageMediaDocument:
    {
        InputDocument doc(InputDocument::typeInputDocument);
        doc.setAccessHash(media.document().accessHash());
        doc.setId(media.document().id());

        result.setIdInputDocument(doc);
        result.setClassType(InputMedia::typeInputMediaDocument);
    }
        break;
    case MessageMedia::typeMessageMediaGeo:
    {
        InputGeoPoint geo(InputGeoPoint::typeInputGeoPoint);
        geo.setLat(media.geo().lat());
        geo.setLongValue(media.geo().longValue());

        result.setGeoPoint(geo);
        result.setClassType(InputMedia::typeInputMediaGeoPoint);
    }
        break;
    case MessageMedia::typeMessageMediaPhoto:
    {
        InputPhoto photo(InputPhoto::typeInputPhoto);
        photo.setId(media.photo().id());
        photo.setAccessHash(media.photo().accessHash());

        result.setIdInputPhoto(photo);
        result.setClassType(InputMedia::typeInputMediaPhoto);
    }
        break;
    case MessageMedia::typeMessageMediaContact:
    {
        result.setPhoneNumber(media.phoneNumber());
        result.setFirstName(media.firstName());
        result.setLastName(media.lastName());
        result.setClassType(InputMedia::typeInputMediaContact);
    }
        break;
    case MessageMedia::typeMessageMediaVenue:
    {
        InputGeoPoint geo(InputGeoPoint::typeInputGeoPoint);
        geo.setLat(media.geo().lat());
        geo.setLongValue(media.geo().longValue());

        result.setGeoPoint(geo);
        result.setTitle(media.title());
        result.setAddress(media.address());
        result.setProvider(media.provider());
        result.setVenueId(media.venueId());
        result.setClassType(InputMedia::typeInputMediaVenue);
    }
        break;
    }

    return result;
}

qint64 TelegramTools::generateRandomId()
{
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    return randomId;
}

QString TelegramTools::convertErrorToText(const QString &error)
{
    if(error.isEmpty())
        return error;

    QString result = error.toLower();
    result.replace("_", " ");
    result[0] = result[0].toUpper();

    return result;
}

TelegramEnums::MessageType TelegramTools::messageType(MessageObject *msg)
{
    if(!msg)
        return TelegramEnums::TypeUnsupportedMessage;

    switch(static_cast<int>(msg->classType()))
    {
    case MessageObject::TypeMessage:
    {
        if(!msg->media())
            return TelegramEnums::TypeTextMessage;

        switch(static_cast<int>(msg->media()->classType()))
        {
        case MessageMediaObject::TypeMessageMediaEmpty:
            return TelegramEnums::TypeTextMessage;
            break;
        case MessageMediaObject::TypeMessageMediaPhoto:
            return TelegramEnums::TypePhotoMessage;
            break;
        case MessageMediaObject::TypeMessageMediaGeo:
            return TelegramEnums::TypeGeoMessage;
            break;
        case MessageMediaObject::TypeMessageMediaContact:
            return TelegramEnums::TypeContactMessage;
            break;
        case MessageMediaObject::TypeMessageMediaUnsupported:
            return TelegramEnums::TypeUnsupportedMessage;
            break;
        case MessageMediaObject::TypeMessageMediaDocument:
        {
            if(!msg->media()->document())
                return TelegramEnums::TypeTextMessage;

            Q_FOREACH(const DocumentAttribute &attr, msg->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAnimated)
                    return TelegramEnums::TypeAnimatedMessage;
            Q_FOREACH(const DocumentAttribute &attr, msg->media()->document()->attributes())
            {
                switch(static_cast<int>(attr.classType()))
                {
                case DocumentAttribute::typeDocumentAttributeAudio:
                    return TelegramEnums::TypeAudioMessage;
                    break;
                case DocumentAttribute::typeDocumentAttributeSticker:
                    return TelegramEnums::TypeStickerMessage;
                    break;
                case DocumentAttribute::typeDocumentAttributeVideo:
                    return TelegramEnums::TypeVideoMessage;
                    break;
                }
            }
            return TelegramEnums::TypeDocumentMessage;
        }
            break;
        case MessageMediaObject::TypeMessageMediaWebPage:
            return TelegramEnums::TypeWebPageMessage;
            break;
        case MessageMediaObject::TypeMessageMediaVenue:
            return TelegramEnums::TypeVenueMessage;
            break;
        }

        return TelegramEnums::TypeTextMessage;
    }
        break;

    case MessageObject::TypeMessageEmpty:
        return TelegramEnums::TypeUnsupportedMessage;
        break;

    case MessageObject::TypeMessageService:
        return TelegramEnums::TypeActionMessage;
        break;
    }

    return TelegramEnums::TypeUnsupportedMessage;
}

TelegramTypeQObject *TelegramTools::objectRoot(TelegramTypeQObject *object)
{
    TelegramTypeQObject *root = 0;
    TelegramTypeQObject *tmp = object;
    while(tmp)
    {
        root = tmp;
        tmp = qobject_cast<TelegramTypeQObject*>(tmp->parent());
    }
    return root;
}


void TelegramTools::analizeUpdatesType(const UpdatesType &updates, TelegramEngine *engine, std::function<void (const Update &)> callback, const Message &sentMsg)
{
    if(!engine || !engine->sharedData())
        return;

    TelegramSharedDataManager *tsdm = engine->sharedData();
    QSet< TelegramSharedPointer<TelegramTypeQObject> > cache;

    switch(static_cast<int>(updates.classType()))
    {
    case UpdatesType::typeUpdatesTooLong:
        break;
    case UpdatesType::typeUpdateShortMessage:
    {
        Peer peer(Peer::typePeerUser);
        peer.setUserId(updates.out()? updates.userId() : engine->telegram()->ourId());

        Message msg(Message::typeMessage);
        msg.setId(updates.id());
        msg.setFromId(updates.out()? engine->telegram()->ourId() : updates.userId());
        msg.setToId(peer);
        msg.setMessage(updates.message());
        msg.setDate(updates.date());
        msg.setFwdFrom(updates.fwdFrom());
        msg.setReplyToMsgId(updates.replyToMsgId());
        msg.setOut(updates.out());
        msg.setEntities(updates.entities());
        msg.setViaBotId(updates.viaBotId());
        msg.setSilent(updates.silent());
        msg.setMentioned(updates.mentioned());
        msg.setMediaUnread(updates.mediaUnread());

        Update update(Update::typeUpdateNewMessage);
        update.setMessage(msg);
        update.setPts(updates.pts());
        update.setPtsCount(updates.ptsCount());

        callback(update);
    }
        break;
    case UpdatesType::typeUpdateShortChatMessage:
    {
        Peer peer(Peer::typePeerChat);
        peer.setChatId(updates.chatId());

        Message msg(Message::typeMessage);
        msg.setId(updates.id());
        msg.setFromId(updates.fromId());
        msg.setToId(peer);
        msg.setMessage(updates.message());
        msg.setDate(updates.date());
        msg.setFwdFrom(updates.fwdFrom());
        msg.setReplyToMsgId(updates.replyToMsgId());
        msg.setOut(updates.out());
        msg.setEntities(updates.entities());
        msg.setViaBotId(updates.viaBotId());
        msg.setSilent(updates.silent());
        msg.setMentioned(updates.mentioned());
        msg.setMediaUnread(updates.mediaUnread());

        Update update(Update::typeUpdateNewMessage);
        update.setMessage(msg);
        update.setPts(updates.pts());
        update.setPtsCount(updates.ptsCount());

        callback(update);
    }
        break;
    case UpdatesType::typeUpdateShort:
    {
        callback(updates.update());
    }
        break;
    case UpdatesType::typeUpdatesCombined:
    case UpdatesType::typeUpdates:
    {
        Q_FOREACH(const User &user, updates.users())
            cache.insert( tsdm->insertUser(user).data() );
        Q_FOREACH(const Chat &chat, updates.chats())
            cache.insert( tsdm->insertChat(chat).data() );
        Q_FOREACH(const Update &upd, updates.updates())
            callback(upd);
    }
        break;
    case UpdatesType::typeUpdateShortSentMessage:
    {
        Message msg = sentMsg;
        msg.setId(updates.id());
        msg.setFwdFrom(updates.fwdFrom());
        msg.setDate(updates.date());
        msg.setMedia(updates.media());
        msg.setOut(updates.out());
        msg.setEntities(updates.entities());

        Update update(Update::typeUpdateNewMessage);
        update.setMessage(msg);
        update.setPts(updates.pts());
        update.setPtsCount(updates.ptsCount());

        callback(update);
    }
        break;
    }

    // Cache will clear at the end of the function
}

QString TelegramTools::userStatus(UserObject *user, std::function<QString (const QDateTime &)> dateConversationMethod)
{
    switch(user->status()->classType())
    {
    case UserStatusObject::TypeUserStatusEmpty:
        return QString();
        break;
    case UserStatusObject::TypeUserStatusLastMonth:
        return QObject::tr("Last month");
        break;
    case UserStatusObject::TypeUserStatusLastWeek:
        return QObject::tr("Last week");
        break;
    case UserStatusObject::TypeUserStatusOffline:
        return QObject::tr("Last seen %1").arg( dateConversationMethod(QDateTime::fromTime_t(user->status()->wasOnline())) );
        break;
    case UserStatusObject::TypeUserStatusOnline:
        return QObject::tr("Online");
        break;
    case UserStatusObject::TypeUserStatusRecently:
        return QObject::tr("Last seen recently");
        break;
    default:
        return QString();
    }
}
