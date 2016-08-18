#include "sailorgramtools.h"
#include <QObject>
#include <QDateTime>
#include <telegramenums.h>
#include <telegram/objects/messageactionobject.h>
#include <telegram/objects/sendmessageactionobject.h>
#include <telegramdialoglistmodel.h>
#include <telegramenums.h>
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

QString SailorgramTools::messageText(MessageObject *message)
{
    return SailorgramTools::messageText(message->core());
}

QString SailorgramTools::messageDate(MessageObject *message)
{
    QDateTime messagedate = QDateTime::fromTime_t(message->date());
    const QDateTime& currentdate = QDateTime::currentDateTime();
    const qint64 seconds = messagedate.secsTo(currentdate);
    const int days = messagedate.secsTo(currentdate);

    if(seconds < (24 * 60 * 60))
        return days ? QObject::tr("Yesterday %1").arg(messagedate.toString("HH:mm")) : messagedate.toString("HH:mm");

    return messagedate.toString("MMM dd, HH:mm");
}

QString SailorgramTools::messageText(const Message &message)
{
    if(!message.media().caption().isEmpty())
        return message.media().caption();

    if(message.media().classType() != MessageMedia::typeMessageMediaEmpty)
        return SailorgramTools::mediaType(message);

    return message.message();
}

QString SailorgramTools::mediaType(const Message &message)
{
    const MessageMedia& messagemedia = message.media();

    if(messagemedia.classType() == MessageMedia::typeMessageMediaDocument)
    {
        foreach(const DocumentAttribute& attribute, messagemedia.document().attributes())
        {
            if(attribute.classType() == DocumentAttribute::typeDocumentAttributeAnimated)
                return QObject::tr("GIF");

            if(attribute.classType() == DocumentAttribute::typeDocumentAttributeSticker)
                return QObject::tr("Sticker");

            if(attribute.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                return QObject::tr("Audio");

            if(attribute.classType() == DocumentAttribute::typeDocumentAttributeVideo)
                return QObject::tr("Video");
        }

        return QObject::tr("Document");
    }

    if(messagemedia.classType() == MessageMedia::typeMessageMediaContact)
        return QObject::tr("Contact");

    if(messagemedia.classType() == MessageMedia::typeMessageMediaPhoto)
        return QObject::tr("Photo");

    if((messagemedia.classType() == MessageMedia::typeMessageMediaGeo) || (messagemedia.classType() == MessageMedia::typeMessageMediaVenue))
        return QObject::tr("Location");

    if(messagemedia.classType() == MessageMedia::typeMessageMediaWebPage)
        return QObject::tr("Web Page");

    if(messagemedia.classType() == MessageMedia::typeMessageMediaUnsupported)
        return QObject::tr("Unsupported");

    return QObject::tr("Unknown");
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

int SailorgramTools::sendFileType(int sft)
{
    if(sft == TelegramEnums::SendFileTypeDocument)
        return SailorgramEnums::SendFileTypeDocument;

    if(sft == TelegramEnums::SendFileTypeSticker)
        return SailorgramEnums::SendFileTypeSticker;

    if(sft == TelegramEnums::SendFileTypeAnimated)
        return SailorgramEnums::SendFileTypeAnimated;

    if(sft == TelegramEnums::SendFileTypeVideo)
        return SailorgramEnums::SendFileTypeVideo;

    if(sft == TelegramEnums::SendFileTypePhoto)
        return SailorgramEnums::SendFileTypePhoto;

    if(sft == TelegramEnums::SendFileTypeAudio)
        return SailorgramEnums::SendFileTypeAudio;

    return SailorgramEnums::SendFileTypeAutoDetect;
}

int SailorgramTools::typingStatus(int smat)
{
    if(smat == SendMessageActionObject::TypeSendMessageCancelAction)
        return SailorgramEnums::TypingStatusCancel;

    if(smat == SendMessageActionObject::TypeSendMessageRecordVideoAction)
        return SailorgramEnums::TypingStatusRecordVideo;

    if(smat == SendMessageActionObject::TypeSendMessageUploadVideoAction)
        return SailorgramEnums::TypingStatusUploadVideo;

    if(smat == SendMessageActionObject::TypeSendMessageRecordAudioAction)
        return SailorgramEnums::TypingStatusRecordAudio;

    if(smat == SendMessageActionObject::TypeSendMessageUploadAudioAction)
        return SailorgramEnums::TypingStatusUploadAudio;

    if(smat == SendMessageActionObject::TypeSendMessageUploadPhotoAction)
        return SailorgramEnums::TypingStatusUploadPhoto;

    if(smat == SendMessageActionObject::TypeSendMessageUploadDocumentAction)
        return SailorgramEnums::TypingStatusUploadDocument;

    if(smat == SendMessageActionObject::TypeSendMessageGeoLocationAction)
        return SailorgramEnums::TypingStatusGeoLocation;

    if(smat == SendMessageActionObject::TypeSendMessageChooseContactAction)
        return SailorgramEnums::TypingStatusChooseContact;

    return SailorgramEnums::TypingStatusTyping;
}

int SailorgramTools::secretChatState(int scs)
{
    if(scs == TelegramDialogListModel::SecretChatStateRequested)
        return SailorgramEnums::SecretChatStateRequested;

    if(scs == TelegramDialogListModel::SecretChatStateAccepted)
        return SailorgramEnums::SecretChatStateAccepted;

    if(scs == TelegramDialogListModel::SecretChatStateCancelled)
        return SailorgramEnums::SecretChatStateCancelled;

    return SailorgramEnums::SecretChatStateInit;
}

quint32 SailorgramTools::reverseSendFileType(int sft)
{
    if(sft == SailorgramEnums::SendFileTypeDocument)
        return TelegramEnums::SendFileTypeDocument;

    if(sft == SailorgramEnums::SendFileTypeSticker)
        return TelegramEnums::SendFileTypeSticker;

    if(sft == SailorgramEnums::SendFileTypeAnimated)
        return TelegramEnums::SendFileTypeAnimated;

    if(sft == SailorgramEnums::SendFileTypeVideo)
        return TelegramEnums::SendFileTypeVideo;

    if(sft == SailorgramEnums::SendFileTypePhoto)
        return TelegramEnums::SendFileTypePhoto;

    if(sft == SailorgramEnums::SendFileTypeAudio)
        return TelegramEnums::SendFileTypeAudio;

    return TelegramEnums::SendFileTypeAutoDetect;
}

quint32 SailorgramTools::reverseTypingStatus(int smat)
{
    if(smat == SailorgramEnums::TypingStatusCancel)
        return SendMessageActionObject::TypeSendMessageCancelAction;

    if(smat == SailorgramEnums::TypingStatusRecordVideo)
        return SendMessageActionObject::TypeSendMessageRecordVideoAction;

    if(smat == SailorgramEnums::TypingStatusUploadVideo)
        return SendMessageActionObject::TypeSendMessageUploadVideoAction;

    if(smat == SailorgramEnums::TypingStatusRecordAudio)
        return SendMessageActionObject::TypeSendMessageRecordAudioAction;

    if(smat == SailorgramEnums::TypingStatusUploadAudio)
        return SendMessageActionObject::TypeSendMessageUploadAudioAction;

    if(smat == SailorgramEnums::TypingStatusUploadPhoto)
        return SendMessageActionObject::TypeSendMessageUploadPhotoAction;

    if(smat == SailorgramEnums::TypingStatusUploadDocument)
        return SendMessageActionObject::TypeSendMessageUploadDocumentAction;

    if(smat == SailorgramEnums::TypingStatusGeoLocation)
        return SendMessageActionObject::TypeSendMessageGeoLocationAction;

    if(smat == SailorgramEnums::TypingStatusChooseContact)
        return SendMessageActionObject::TypeSendMessageChooseContactAction;

    return SendMessageActionObject::TypeSendMessageTypingAction;
}
