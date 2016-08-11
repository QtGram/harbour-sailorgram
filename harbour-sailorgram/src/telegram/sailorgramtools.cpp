#include "sailorgramtools.h"
#include <telegramenums.h>
#include <telegram/objects/messageactionobject.h>
#include "sailorgramenums.h"

SailorgramTools::SailorgramTools()
{

}

qint32 SailorgramTools::peerId(DialogObject *dialog)
{
    PeerObject* peer = dialog->peer();

    if(peer->classType() == PeerObject::TypePeerChat)
        return peer->chatId();

    if(peer->classType() == PeerObject::TypePeerChannel)
        return peer->channelId();

    return peer->userId();
}

QString SailorgramTools::completeName(UserObject *user)
{
    if(!user->lastName().isEmpty())
        return user->firstName() + " " + user->lastName();

    return user->firstName();
}

int SailorgramTools::messageType(int mt)
{
    if(mt == TelegramEnums::TypeTextMessage)
        return SailorgramEnums::MessageTypeText;

    if(mt == TelegramEnums::TypeDocumentMessage)
        return SailorgramEnums::MessageTypeDocument;

    if(mt == TelegramEnums::TypeVideoMessage)
        return SailorgramEnums::MessageTypeVideo;

    if(mt == TelegramEnums::TypeAudioMessage)
        return SailorgramEnums::MessageTypeAudio;

    if(mt == TelegramEnums::TypeVenueMessage)
        return SailorgramEnums::MessageTypeVenue;

    if(mt == TelegramEnums::TypeWebPageMessage)
        return SailorgramEnums::MessageTypeWebPage;

    if(mt == TelegramEnums::TypeGeoMessage)
        return SailorgramEnums::MessageTypeGeo;

    if(mt == TelegramEnums::TypeContactMessage)
        return SailorgramEnums::MessageTypeContact;

    if(mt == TelegramEnums::TypePhotoMessage)
        return SailorgramEnums::MessageTypePhoto;

    if(mt == TelegramEnums::TypeStickerMessage)
        return SailorgramEnums::MessageTypeSticker;

    if(mt == TelegramEnums::TypeAnimatedMessage)
        return SailorgramEnums::MessageTypeAnimated;

    return SailorgramEnums::MessageTypeUnsupported;
}

int SailorgramTools::actionType(int at)
{
    if(at == MessageActionObject::TypeMessageActionChatCreate)
        return SailorgramEnums::ActionTypeChatCreate;

    if(at == MessageActionObject::TypeMessageActionChatEditTitle)
        return SailorgramEnums::ActionTypeChatEditTitle;

    if(at == MessageActionObject::TypeMessageActionChatEditPhoto)
        return SailorgramEnums::ActionTypeChatEditPhoto;

    if(at == MessageActionObject::TypeMessageActionChatDeletePhoto)
        return SailorgramEnums::ActionTypeChatDeletePhoto;

    if(at == MessageActionObject::TypeMessageActionChatAddUser)
        return SailorgramEnums::ActionTypeChatAddUser;

    if(at == MessageActionObject::TypeMessageActionChatDeleteUser)
        return SailorgramEnums::ActionTypeChatDeleteUser;

    if(at == MessageActionObject::TypeMessageActionChatJoinedByLink)
        return SailorgramEnums::ActionTypeChatJoinedByLink;

    if(at == MessageActionObject::TypeMessageActionChannelCreate)
        return SailorgramEnums::ActionTypeChannelCreate;

    if(at == MessageActionObject::TypeMessageActionChatMigrateTo)
        return SailorgramEnums::ActionTypeChatMigrateTo;

    if(at == MessageActionObject::TypeMessageActionChannelMigrateFrom)
        return SailorgramEnums::ActionTypeChannelMigrateFrom;

    if(at == MessageActionObject::TypeMessageActionPinMessage)
        return SailorgramEnums::ActionTypePinMessage;

    if(at == MessageActionObject::TypeMessageActionHistoryClear)
        return SailorgramEnums::ActionTypeHistoryClear;

    return SailorgramEnums::ActionTypeEmpty;
}

