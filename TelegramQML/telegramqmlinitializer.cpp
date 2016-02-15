#include "telegramqmlinitializer.h"
#include "backgroundmanager.h"
#include "chatparticipantlist.h"
#include "database.h"
#include "dialogfilesmodel.h"
#include "mp3converterengine.h"
#include "photosizelist.h"
#include "profilesmodel.h"
#include "telegramsearchmodel.h"
#include "telegramuploadsmodel.h"
#include "telegramwallpapersmodel.h"
#include "userdata.h"
#include "usernamefiltermodel.h"
#include "telegramqml.h"
#include "tagfiltermodel.h"
#include "telegramchatparticipantsmodel.h"
#include "telegramcontactsmodel.h"
#include "telegramdetailedcontactsmodel.h"
#include "telegramcontactsfiltermodel.h"
#include "telegramdialogsmodel.h"
#include "telegramfilehandler.h"
#include "telegrammessagesmodel.h"
#include "stickersmodel.h"
#include "objects/types.h"

#include <qqml.h>

void TelegramQmlInitializer::init(const char *uri)
{
    qRegisterMetaType<TelegramQml*>("TelegramQml*");
    qRegisterMetaType<UserData*>("UserData*");
    qRegisterMetaType< QList<qint32> >("QList<qint32>");
    qRegisterMetaType< QList<DocumentAttribute> >("QList<DocumentAttribute>");

    qmlRegisterType<StickerSetObject>(uri, 1, 0, "StickerSet");
    qmlRegisterType<StickerPackObject>(uri, 1, 0, "StickerPack");
    qmlRegisterType<DocumentAttributeObject>(uri, 1, 0, "DocumentAttribute");
    qmlRegisterType<BackgroundManager>(uri, 1, 0, "BackgroundManager");
    qmlRegisterType<ChatParticipantList>(uri, 1, 0, "ChatParticipantList");
    qmlRegisterType<Database>(uri, 1, 0, "Database");
    qmlRegisterType<DialogFilesModel>(uri, 1, 0, "DialogFilesModel");
    qmlRegisterType<MP3ConverterEngine>(uri, 1, 0, "MP3ConverterEngine");
    qmlRegisterType<PhotoSizeList>(uri, 1, 0, "PhotoSizeList");
    qmlRegisterType<ProfilesModel>(uri, 1, 0, "ProfilesModel");
    qmlRegisterType<ProfilesModelItem>(uri, 1, 0, "ProfilesModelItem");
    qmlRegisterType<TelegramSearchModel>(uri, 1, 0, "SearchModel");
    qmlRegisterType<TelegramUploadsModel>(uri, 1, 0, "UploadsModel");
    qmlRegisterType<TelegramWallpapersModel>(uri, 1, 0, "WallpapersModel");
    qmlRegisterType<StickersModel>(uri, 1, 0, "StickersModel");
    qmlRegisterType<UserData>(uri, 1, 0, "UserData");
    qmlRegisterType<UserNameFilterModel>(uri, 1, 0, "UserNameFilterModel");
    qmlRegisterType<TelegramQml>(uri, 1, 0, "Telegram");
    qmlRegisterType<TagFilterModel>(uri, 1, 0, "TagFilterModel");
    qmlRegisterType<TelegramChatParticipantsModel>(uri, 1, 0, "ChatParticipantsModel");
    qmlRegisterType<TelegramContactsModel>(uri, 1, 0, "ContactsModel");
    qmlRegisterType<TelegramContactsFilterModel>(uri, 1, 0, "ContactsFilterModel");
    qmlRegisterType<TelegramDetailedContactsModel>(uri, 1, 0, "DetailedContactsModel");
    qmlRegisterType<TelegramDialogsModel>(uri, 1, 0, "DialogsModel");
    qmlRegisterType<TelegramFileHandler>(uri, 1, 0, "FileHandler");
    qmlRegisterType<TelegramMessagesModel>(uri, 1, 0, "MessagesModel");
    qmlRegisterUncreatableType<UserData>(uri, 1, 0, "UserData", "");

    initializeTypes(uri);
}

void TelegramQmlInitializer::initializeTypes(const char *uri) {
    qmlRegisterType<DownloadObject>(uri, 1, 0, "Download");
    qRegisterMetaType<DownloadObject*>("DownloadObject*");

    qmlRegisterType<UploadObject>(uri, 1, 0, "Upload");
    qRegisterMetaType<UploadObject*>("UploadObject*");

    qmlRegisterType<FileLocationObject>(uri, 1, 0, "FileLocation");
    qRegisterMetaType<FileLocationObject*>("FileLocationObject*");

    qmlRegisterType<PeerObject>(uri, 1, 0, "Peer");
    qRegisterMetaType<PeerObject*>("PeerObject*");

    qmlRegisterType<ContactObject>(uri, 1, 0, "Contact");
    qRegisterMetaType<ContactObject*>("ContactObject*");

    qmlRegisterType<InputPeerObject>(uri, 1, 0, "InputPeer");
    qRegisterMetaType<InputPeerObject*>("InputPeerObject*");

    qmlRegisterType<UserStatusObject>(uri, 1, 0, "UserStatus");
    qRegisterMetaType<UserStatusObject*>("UserStatusObject*");

    qmlRegisterType<GeoPointObject>(uri, 1, 0, "GeoPoint");
    qRegisterMetaType<GeoPointObject*>("GeoPointObject*");

    qmlRegisterType<PeerNotifySettingsObject>(uri, 1, 0, "PeerNotifySettings");
    qRegisterMetaType<PeerNotifySettingsObject*>("PeerNotifySettingsObject*");

    qmlRegisterType<EncryptedFileObject>(uri, 1, 0, "EncryptedFile");
    qRegisterMetaType<EncryptedFileObject*>("EncryptedFileObject*");

    qmlRegisterType<EncryptedChatObject>(uri, 1, 0, "EncryptedChat");
    qRegisterMetaType<EncryptedChatObject*>("EncryptedChatObject*");

    qmlRegisterType<EncryptedMessageObject>(uri, 1, 0, "EncryptedMessage");
    qRegisterMetaType<EncryptedMessageObject*>("EncryptedMessageObject*");

    qmlRegisterType<ContactLinkObject>(uri, 1, 0, "ContactsForeignLink");
    qRegisterMetaType<ContactLinkObject*>("ContactLinkObject*");

    qmlRegisterType<NotifyPeerObject>(uri, 1, 0, "NotifyPeer");
    qRegisterMetaType<NotifyPeerObject*>("NotifyPeerObject*");

    qmlRegisterType<ChatParticipantObject>(uri, 1, 0, "ChatParticipant");
    qRegisterMetaType<ChatParticipantObject*>("ChatParticipantObject*");

    qmlRegisterType<ChatParticipantsObject>(uri, 1, 0, "ChatParticipants");
    qRegisterMetaType<ChatParticipantsObject*>("ChatParticipantsObject*");

    qmlRegisterType<PhotoSizeObject>(uri, 1, 0, "PhotoSize");
    qRegisterMetaType<PhotoSizeObject*>("PhotoSizeObject*");

    qmlRegisterType<AudioObject>(uri, 1, 0, "Audio");
    qRegisterMetaType<AudioObject*>("AudioObject*");

    qmlRegisterType<DocumentObject>(uri, 1, 0, "Document");
    qRegisterMetaType<DocumentObject*>("DocumentObject*");

    qmlRegisterType<VideoObject>(uri, 1, 0, "Video");
    qRegisterMetaType<VideoObject*>("VideoObject*");

    qmlRegisterType<WebPageObject>(uri, 1, 0, "WebPage");
    qRegisterMetaType<WebPageObject*>("WebPageObject*");

    qmlRegisterType<PhotoObject>(uri, 1, 0, "Photo");
    qRegisterMetaType<PhotoObject*>("PhotoObject*");

    qmlRegisterType<WallPaperObject>(uri, 1, 0, "WallPaper");
    qRegisterMetaType<WallPaperObject*>("WallPaperObject*");

    qmlRegisterType<MessageActionObject>(uri, 1, 0, "MessageAction");
    qRegisterMetaType<MessageActionObject*>("MessageActionObject*");

    qmlRegisterType<ChatPhotoObject>(uri, 1, 0, "ChatPhoto");
    qRegisterMetaType<ChatPhotoObject*>("ChatPhotoObject*");

    qmlRegisterType<ChatFullObject>(uri, 1, 0, "ChatFull");
    qRegisterMetaType<ChatFullObject*>("ChatFullObject*");

    qmlRegisterType<UserProfilePhotoObject>(uri, 1, 0, "UserProfilePhoto");
    qRegisterMetaType<UserProfilePhotoObject*>("UserProfilePhotoObject*");

    qmlRegisterType<ChatObject>(uri, 1, 0, "Chat");
    qRegisterMetaType<ChatObject*>("ChatObject*");

    qmlRegisterType<DialogObject>(uri, 1, 0, "Dialog");
    qRegisterMetaType<DialogObject*>("DialogObject*");

    qmlRegisterType<SendMessageActionObject>(uri, 1, 0, "SendMessageAction");
    qRegisterMetaType<SendMessageActionObject*>("SendMessageActionObject*");

    qmlRegisterType<DecryptedMessageActionObject>(uri, 1, 0, "DecryptedMessageAction");
    qRegisterMetaType<DecryptedMessageActionObject*>("DecryptedMessageActionObject*");

    qmlRegisterType<DecryptedMessageMediaObject>(uri, 1, 0, "DecryptedMessageMedia");
    qRegisterMetaType<DecryptedMessageMediaObject*>("DecryptedMessageMediaObject*");

    qmlRegisterType<DecryptedMessageObject>(uri, 1, 0, "DecryptedMessage");
    qRegisterMetaType<DecryptedMessageObject*>("DecryptedMessageObject*");

    qmlRegisterType<MessageMediaObject>(uri, 1, 0, "MessageMedia");
    qRegisterMetaType<MessageMediaObject*>("MessageMediaObject*");

    qmlRegisterType<MessageObject>(uri, 1, 0, "Message");
    qRegisterMetaType<MessageObject*>("MessageObject*");

    qmlRegisterType<GeoChatMessageObject>(uri, 1, 0, "GeoChatMessage");
    qRegisterMetaType<GeoChatMessageObject*>("GeoChatMessageObject*");

    qmlRegisterType<UserObject>(uri, 1, 0, "User");
    qRegisterMetaType<UserObject*>("UserObject*");

    qmlRegisterType<PhotoSizeList>(uri, 1, 0, "PhotoSizeList");
    qRegisterMetaType<PhotoSizeList*>("PhotoSizeList*");

    qmlRegisterType<ChatParticipantList>(uri, 1, 0, "ChatParticipantList");
    qRegisterMetaType<ChatParticipantList*>("ChatParticipantList*");
}

