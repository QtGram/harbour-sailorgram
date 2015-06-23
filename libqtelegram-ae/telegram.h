/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 */

#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include <QMap>
#include <QLoggingCategory>
#include <QSharedPointer>
#include "libqtelegram_global.h"
#include "types/types.h"
#include "core/dcprovider.h"
#include "core/api.h"
#include "secret/secretstate.h"
#include "secret/encrypter.h"
#include "secret/decrypter.h"
#include "secret/secretchatmessage.h"
#include "secret/decryptedmessagebuilder.h"
#include "file/filehandler.h"

Q_DECLARE_LOGGING_CATEGORY(TG_LIB_API)
Q_DECLARE_LOGGING_CATEGORY(TG_LIB_SECRET)

class LIBQTELEGRAMSHARED_EXPORT Telegram : public QObject
{
    Q_OBJECT
public:
    Telegram(const QString &defaultHostAddress, qint16 defaultHostPort, qint16 defaultHostDcId, qint32 appId, const QString &appHash,
             const QString &phoneNumber, const QString &configPath = QString("~/.telegram"), const QString &publicKeyFile = QString("tg.pub"));
    ~Telegram();

    QString defaultHostAddress();
    qint16 defaultHostPort();
    qint16 defaultHostDcId();
    qint32 appId();
    QString appHash();

    Settings *settings() const;
    CryptoUtils *crypto() const;

    // Registration / authorization
    qint64 authCheckPhone();
    qint64 authCheckPhone(const QString &phoneNumber);
    qint64 authSendCode();
    qint64 authSendSms();
    qint64 authSignIn(const QString &code);
    qint64 authSignUp(const QString &code, const QString &firstName, const QString &lastName);
    qint64 authSendCall();
    qint64 authLogOut();
    qint64 authSendInvites(const QStringList &phoneNumbers, const QString &inviteText); //Saves information that the current user sent SMS-messages with invitations to its unregistered contacts.
    qint64 authResetAuthorizations();
    qint64 authCheckPassword(const QByteArray &passwordHash);

    // Working with push notifications. Settings
    qint64 accountRegisterDevice(const QString &token, const QString &appVersion = QString::null, bool appSandbox = false);
    qint64 accountUnregisterDevice(const QString &token);
    qint64 accountUpdateNotifySettings(const InputNotifyPeer &peer, const InputPeerNotifySettings &settings);
    qint64 accountGetNotifySettings(const InputNotifyPeer &peer);
    qint64 accountResetNotifySettings();
    qint64 accountUpdateProfile(const QString &firstName, const QString &lastName);
    qint64 accountUpdateStatus(bool offline);
    qint64 accountGetWallPapers();
    qint64 accountCheckUsername(const QString &username);
    qint64 accountUpdateUsername(const QString &username);
    qint64 accountGetPrivacy(const InputPrivacyKey &key);
    qint64 accountSetPrivacy(const InputPrivacyKey &key, const QList<InputPrivacyRule> &rules);
    qint64 accountDeleteAccount(const QString &reason);
    qint64 accountGetAccountTTL();
    qint64 accountSetAccountTTL(const AccountDaysTTL &ttl);
    qint64 accountUpdateDeviceLocked(int period);
    qint64 accountSendChangePhoneCode(const QString &phone_number);
    qint64 accountChangePhone(const QString &phone_number, const QString &phone_code_hash, const QString &phone_code);
    qint64 accountGetPassword();
    qint64 accountSetPassword(const QByteArray &currentPasswordHash, const QByteArray &newSalt, const QByteArray &newPasswordHash, const QString &hint);
    qint64 photosUploadProfilePhoto(const QByteArray &bytes, const QString &fileName, const QString &caption = QString::null, const InputGeoPoint &geoPoint = InputGeoPoint(InputGeoPoint::typeInputGeoPointEmpty), const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto));
    qint64 photosUploadProfilePhoto(const QString &filePath, const QString &caption = QString::null, const InputGeoPoint &geoPoint = InputGeoPoint(InputGeoPoint::typeInputGeoPointEmpty), const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto));
    qint64 photosUpdateProfilePhoto(qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto));

    // Working with users
    qint64 usersGetUsers (const QList<InputUser> &users);
    qint64 usersGetFullUser (const InputUser &user);
    qint64 photosGetUserPhotos (const InputUser &user, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);

    // Working with contacts
    qint64 contactsGetStatuses();
    qint64 contactsGetContacts();
    qint64 contactsImportContacts (const QList<InputContact> &contacts, bool replace);
    qint64 contactsDeleteContact(const InputUser &user);
    qint64 contactsDeleteContacts(const QList<InputUser> &users);
    qint64 contactsSearch(const QString &q, qint32 limit = 0);
    qint64 contactsResolveUsername(const QString &username);

    // Working with blacklist
    qint64 contactsBlock(const InputUser &user);
    qint64 contactsUnblock(const InputUser &user);
    qint64 contactsGetBlocked(qint32 offset = 0, qint32 limit = 0);

    // Working with messages
    qint64 messagesSendMessage (const InputPeer &peer, qint64 randomId, const QString &message, int replyToMsgId = 0);
    qint64 messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 replyToMsgId = 0);
    qint64 messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 replyToMsgId = 0);
    qint64 messagesSendGeoPoint(const InputPeer &peer, qint64 randomId, const InputGeoPoint &InputGeoPoint, qint32 replyToMsgId = 0);
    qint64 messagesSendContact(const InputPeer &peer, qint64 randomId, const QString &phoneNumber, const QString &firstName, const QString &lastName, qint32 replyToMsgId = 0);
    qint64 messagesSendVideo(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, qint32 width, qint32 height, const QString &mimeType, const QByteArray &thumbnailBytes = 0, const QString &thumbnailName = QString::null, qint32 replyToMsgId = 0);
    qint64 messagesSendVideo(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QString &thumbnailFilePath = QString::null, qint32 replyToMsgId = 0);
    qint64 messagesSendAudio(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, const QString &mimeType, qint32 replyToMsgId = 0);
    qint64 messagesSendAudio(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 replyToMsgId = 0);
    qint64 messagesSendDocument(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, const QString &mimeType, const QByteArray &thumbnailBytes = 0, const QString &thumbnailName = QString::null, const QList<DocumentAttribute> &extraAttributes = QList<DocumentAttribute>(), qint32 replyToMsgId = 0);
    qint64 messagesSendDocument(const InputPeer &peer, qint64 randomId, const QString &filePath, const QString &thumbnailFilePath = QString::null, bool sendAsSticker = false, qint32 replyToMsgId = 0);
    qint64 messagesForwardPhoto(const InputPeer &peer, qint64 randomId, qint64 photoId, qint64 accessHash, qint32 replyToMsgId = 0);
    qint64 messagesForwardVideo(const InputPeer &peer, qint64 randomId, qint64 videoId, qint64 accessHash, qint32 replyToMsgId = 0);
    qint64 messagesForwardAudio(const InputPeer &peer, qint64 randomId, qint64 audioId, qint64 accessHash, qint32 replyToMsgId = 0);
    qint64 messagesForwardDocument(const InputPeer &peer, qint64 randomId, qint64 documentId, qint64 accessHash, qint32 replyToMsgId = 0);
    qint64 messagesSetTyping(const InputPeer &peer, const SendMessageAction &action);
    qint64 messagesGetMessages(const QList<qint32> &msgIds);
    qint64 messagesGetDialogs(qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 messagesGetHistory(const InputPeer &peer, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 messagesSearch(const InputPeer &peer, const QString &query, const MessagesFilter &filter, qint32 minDate = 0, qint32 maxDate = 0, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 messagesReadHistory(const InputPeer &peer, qint32 maxId = 0, qint32 offset = 0);
    qint64 messagesReadMessageContents(const QList<qint32> &ids);
    qint64 messagesDeleteHistory(const InputPeer &peer, qint32 offset = 0);
    qint64 messagesDeleteMessages(const QList<qint32> &msgIds);
    qint64 messagesRestoreMessages(const QList<qint32> &msgIds);
    qint64 messagesReceivedMessages(qint32 maxId);
    qint64 messagesForwardMessage(const InputPeer &peer, qint32 msgId);
    qint64 messagesForwardMessages(const InputPeer &peer, const QList<qint32> &msgIds, const QList<qint32> &randomIds);
    qint64 messagesSendBroadcast(const QList<InputUser> &users, const QString &message, const InputMedia &media);

    // Working with chats
    qint64 messagesGetChats(const QList<qint32> &chatIds);
    qint64 messagesGetFullChat(qint32 chatId);
    qint64 messagesEditChatTitle(qint32 chatId, const QString &title);
    qint64 messagesEditChatPhoto(qint32 chatId, const QString &filePath, const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto));
    qint64 messagesEditChatPhoto(qint32 chatId, qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto));
    qint64 messagesAddChatUser(qint32 chatId, const InputUser &user, qint32 fwdLimit = 0); // Adds a user to a chat and sends a service message on it.
    qint64 messagesDeleteChatUser(qint32 chatId, const InputUser &user);
    qint64 messagesCreateChat(const QList<InputUser> &users, const QString &chatTopic);

    // Working with secret chats
    qint64 messagesCreateEncryptedChat(const InputUser &user); //this method involves all DH key exchange and encrypted chat creation
    qint64 messagesAcceptEncryptedChat(qint32 chatId);
    qint64 messagesDiscardEncryptedChat(qint32 chatId);
    qint64 messagesSetEncryptedTyping(qint32 chatId, bool typing);
    qint64 messagesSetEncryptedTTL(qint64 randomId, qint32 chatId, qint32 ttl);
    qint64 messagesReadEncryptedHistory(qint32 chatId, qint32 maxDate);
    qint64 messagesSendEncrypted(qint32 chatId, qint64 randomId, qint32 ttl, const QString &text);
    qint64 messagesSendEncryptedPhoto(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath);
    qint64 messagesSendEncryptedVideo(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QByteArray &thumbnailBytes);
    qint64 messagesSendEncryptedDocument(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath);
    qint64 messagesReceivedQueue(qint32 maxQts);

    qint64 messagesSendEncryptedFile(const InputEncryptedChat &chat, const InputEncryptedFile &file);//needed?
    qint64 messagesSendEncryptedService(const InputEncryptedChat &chat, const DecryptedMessageAction &action);//needed?


    // Working with geochats
    qint64 geochatsGetLocated(const InputGeoPoint &geoPoint, qint32 radius, qint32 limit);
    qint64 geochatsGetRecents(qint32 sliceCount, const QList<GeoChatMessage> &messages, const QList<Chat> &chats, const QList<User> &users);
    qint64 geochatsCheckin(const InputGeoChat &peer);
    qint64 geochatsGetFullChat(const InputGeoChat &peer);
    qint64 geochatsEditChatTitle(const InputGeoChat &peer, const QString &title, const QString &address);
    qint64 geochatsEditChatPhoto(const InputGeoChat &peer, const InputChatPhoto &photo);
    qint64 geochatSearch(const InputGeoChat &peer, const QString &query, const MessagesFilter &filter, qint32 minDate = 0, qint32 maxDate = 0, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 geochatGetHistory(const InputGeoChat &peer, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 geochatSetTyping(const InputGeoChat &peer, bool typing);
    qint64 geochatSendMessage(const InputGeoChat &peer, const QString &message);
    qint64 geochatSendMedia(const InputGeoChat &peer, const InputMedia &media);
    qint64 geochatCreateGeoChat(const QString &title, const InputGeoPoint &geoPoint, const QString &address, const QString &venue);

    // Stickers
    qint64 messagesGetStickers(const QString &emoticon, const QString &hash);
    qint64 messagesGetAllStickers(const QString &hash);

    // Working with updates
    qint64 updatesGetState();
    qint64 updatesGetDifference(qint32 pts, qint32 date, qint32 qts);

    // Working with files
    qint64 uploadGetFile(const InputFileLocation &file, qint32 fileSize, qint32 dc = 0, const QByteArray &key = QByteArray(), const QByteArray &iv = QByteArray());
    qint64 uploadCancelFile(qint64 fileId);

    // Miscellaneous
    qint64 helpGetSuppport();
    qint64 helpGetInviteText(const QString &langCode); // Returns text of a text message with an invitation.
    qint64 helpGetAppUpdate(const QString &deviceModel, const QString &systemVersion, const QString &appVersion, const QString &langCode);

    // additional public methods
    void init();
    bool isConnected();
    bool isLoggedIn();
    qint32 ourId();
    void setPhoneNumber(const QString &phoneNumber);

    // sleep / wake
    bool sleep();
    bool wake();

Q_SIGNALS:
    // Errors
    void error(qint64 id, qint32 errorCode, const QString &errorText, const QString &functionName = QString());
    void authSignInError(qint64 id, qint32 errorCode, const QString &errorText);
    void authSignUpError(qint64 id, qint32 errorCode, const QString &errorText);

    // Registration / authorization
    void authNeeded();
    void authLoggedIn();
    void authCheckPhoneAnswer(qint64 id, bool phoneRegistered);
    void authCheckPhoneSent(qint64 msgId, const QString &phoneNumber);
    void authSendCodeAnswer(qint64 id, bool phoneRegistered, qint32 sendCallTimeout);
    void authSendSmsAnswer(qint64 id, bool ok);
    void authSendCallAnswer(qint64 id, bool ok);
    void authLogOutAnswer(qint64 id, bool ok);
    void authSendInvitesAnswer(qint64 id, bool ok);
    void authResetAuthorizationsAnswer(qint64 id, bool ok);
    void authCheckPasswordAnswer(qint64 msgId, qint32 expires, const User &user);

    // Working with Notifications. Settings
    void accountRegisterDeviceAnswer(qint64 id, bool ok);
    void accountUnregisterDeviceAnswer(qint64 id, bool ok);
    void accountUpdateNotifySettingsAnswer(qint64 id, bool ok);
    void accountGetNotifySettingsAnswer(qint64 id, const PeerNotifySettings &peerNotifySettings);
    void accountResetNotifySettingsAnswer(qint64 id, bool ok);
    void accountUpdateProfileAnswer(qint64 id, const User &user);
    void accountUpdateStatusAnswer(qint64 id, bool ok);
    void accountGetWallPapersAnswer(qint64 id, const QList<WallPaper> &wallPapers);
    void accountCheckUsernameAnswer(qint64 id, bool ok);
    void accountUpdateUsernameAnswer(qint64 id, const User &user);
    void accountPrivacyRules(qint64 msgId, const QList<PrivacyRule> &rules, const QList<User> &users);
    void accountDeleteAccountAnswer(qint64 msgId, bool ok);
    void accountGetAccountTTLAnswer(qint64 msgId, const AccountDaysTTL &ttl);
    void accountSetAccountTTLAnswer(qint64 msgId, bool ok);
    void accountUpdateDeviceLockedAnswer(qint64 msgId, bool ok);
    void accountChangePhoneAnswer(qint64 msgId, const User &user);
    void accountSentChangePhoneCode(qint64 msgId, const QString &phone_code_hash, qint32 send_call_timeout);
    void accountSetPasswordAnswer(qint64 msgId, bool ok);
    void accountGetPasswordAnswer(qint64 msgId, const AccountPassword &password);
    void photosUploadProfilePhotoAnswer(qint64 id, const Photo &photo, const QList<User> &users);
    void photosUpdateProfilePhotoAnswer(qint64 id, const UserProfilePhoto &userProfilePhoto);

    // Working with users
    void usersGetUsersAnswer(qint64 id, const QList<User> &users);
    void usersGetFullUserAnswer(qint64 id, const User &user, const ContactsLink &link, const Photo &profilePhoto, const PeerNotifySettings &notifySettings, bool blocked, const QString &realFirstName, const QString &realLastName);
    void photosGetUserPhotosAnswer(qint64 id, qint32 sliceCount, const QList<Photo> &photos, const QList<User> &users);

    // Working with contacts
    void contactsGetStatusesAnswer(qint64 id, const QList<ContactStatus> &statuses);
    void contactsGetContactsAnswer(qint64 id, bool modified, QList<Contact> contacts, QList<User> users);
    void contactsImportContactsAnswer(qint64 id, const QList<ImportedContact> &importedContacts, const QList<qint64> &retryContacts, const QList<User> &users);
    void contactsDeleteContactAnswer(qint64 id, const ContactLink &myLink, const ContactLink &foreignLink, const User &user);
    void contactsDeleteContactsAnswer(qint64 id, bool ok);
    void contactsFound(qint64 id, const QList<ContactFound> &founds, const QList<User> &users);
    void contactsResolveUsernameAnswer(qint64 msgId, const User& user);

    // Working with blacklist
    void contactsBlockAnswer(qint64 id, bool ok);
    void contactsUnblockAnswer(qint64 id, bool ok);
    void contactsBlockResult(qint64 id, bool ok);
    void contactsGetBlockedAnswer(qint64 id, qint32 sliceCount, const QList<ContactBlocked> &blocked, const QList<User> &users);

    // Working with messages
    void messagesSendMessageAnswer(qint64 id, qint32 msgId, qint32 date, qint32 pts, qint32 pts_count, qint32 seq, const QList<ContactsLink> &links);
    void messagesSendMediaAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSendPhotoAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSendGeoPointAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSendContactAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSendVideoAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSendAudioAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSendDocumentAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void messagesSetTypingAnswer(qint64 id, bool ok);
    void messagesGetMessagesAnswer(qint64 id, qint32 sliceCount, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesGetDialogsAnswer(qint64 id, qint32 sliceCount, const QList<Dialog> &dialogs, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesGetHistoryAnswer(qint64 id, qint32 sliceCount, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesSearchAnswer(qint64 id, qint32 sliceCount, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesReadHistoryAnswer(qint64 id, qint32 pts, qint32 pts_count, qint32 offset);
    void messagesDeleteHistoryAnswer(qint64 id, qint32 pts, qint32 pts_count, qint32 offset);
    void messagesReadMessageContentsAnswer(qint64 msgId, const AffectedMessages &watchedMessages);
    void messagesDeleteMessagesAnswer(qint64 id, const AffectedMessages &deletedMessages);
    void messagesRestoreMessagesAnswer(qint64 id, const QList<qint32> &restoredMsgIds);
    void messagesReceivedMessagesAnswer(qint64 id, const QList<qint32> &confirmedMsgIds);
    void messagesForwardMessageAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesForwardMessagesAnswer(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesSendBroadcastAnswer(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);

    // Working with chats
    void messagesGetChatsAnswer(qint64 id, const QList<Chat> &chats);
    void messagesGetFullChatAnswer(qint64 id, const ChatFull &chatFull, const QList<Chat> &chats, const QList<User> &users);
    void messagesEditChatTitleAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);// statedMessage or statedMessageLink
    void messagesEditChatPhotoStatedMessageAnswer(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq); // statedMessage or statedMessageLink
    void messagesAddChatUserAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq); // statedMessage or statedMessageLink
    void messagesDeleteChatUserAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq); // statedMessage or statedMessageLink
    void messagesCreateChatAnswer(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq); // statedMessage or statedMessageLink

    // Working with secret chats
    void messagesCreateEncryptedChatAnswer(qint32 chatId, qint32 date, qint32 peerId, qint64 accessHash);
    void messagesEncryptedChatRequested(qint32 chatId, qint32 date, qint32 peerId, qint64 accessHash);
    void messagesEncryptedChatCreated(qint32 chatId, qint32 date, qint32 peerId, qint64 accessHash);
    void messagesEncryptedChatDiscarded(qint32 chatId);
    void messagesSetEncryptedTypingAnswer(qint64 id, bool ok);
    void messagesReadEncryptedHistoryAnswer(qint64 id, bool ok);
    void messagesSendEncryptedAnswer(qint64 id, qint32 date, const EncryptedFile &encryptedFile = EncryptedFile()); // messagesSentEncryptedMessage/messagesSentEncryptedFile
    void messagesSendEncryptedFileAnswer(qint64 id, qint32 date, const EncryptedFile &encryptedFile = EncryptedFile()); // messagesSentEncryptedMessage/messagesSentEncryptedFile
    void messagesSendEncryptedServiceAnswer(qint64 id, qint32 date, EncryptedFile encryptedFile = EncryptedFile()); // messagesSentEncryptedMessage/messagesSentEncryptedFile
    void messagesReceivedQueueAnwer(qint64 id, const QList<qint64> &msgIds);

    // Stickers
    void messagesGetStickersAnwer(qint64 msgId, const MessagesStickers &stickers);
    void messagesGetAllStickersAnwer(qint64 msgId, const MessagesAllStickers &stickers);

    // Working with geochats
    void geochatsGetLocatedAnswer(qint64 id, const QList<ChatLocated> &results, const QList<GeoChatMessage> &messages, const QList<Chat> &chats, const QList<User> &users);
    void geochatsGetRecentsAnswer(qint64 id, qint32 sliceCount, const QList<GeoChatMessage> &messages, const QList<Chat> &chats, const QList<User> &users);
    void geochatsCheckinAnswer(qint64 id, const GeoChatMessage &message, const QList<Chat> &chats, const QList<User> &users, qint32 seq);
    void geochatsGetFullChatAnswer(qint64 id, const ChatFull &fullChat, const QList<Chat> &chats, const QList<User> &users);
    void geochatsEditChatTitleAnswer(qint64 id, const GeoChatMessage &message, const QList<Chat> &chats, const QList<User> &user, qint32 seq);
    void geochatsEditChatPhotoAnswer(qint64 id, const GeoChatMessage &message, const QList<Chat> &chats, const QList<User> &user, qint32 seq);
    void geochatsSearchAnswer(qint64 id, qint32 sliceCount, const QList<GeoChatMessage> &messages, const QList<Chat> &chats, const QList<User> &users);
    void geochatsGetHistory(qint64 id, qint32 sliceCount, const QList<GeoChatMessage> &messages, const QList<Chat> &chats, const QList<User> &users);
    void geochatsSetTyping(qint64 id, bool ok);
    void geochatsSendMessage(qint64 id, const GeoChatMessage &message, const QList<Chat> &chats, const QList<User> &user, qint32 seq);
    void geochatsSendMedia(qint64 id, const GeoChatMessage &message, const QList<Chat> &chats, const QList<User> &user, qint32 seq);
    void geochatsCreateGeoChatAnswer(qint64 id, const GeoChatMessage &message, const QList<Chat> &chats, const QList<User> &users, qint32 seq);

    // Working with updates
    void updatesGetStateAnswer(qint64 id, qint32 pts, qint32 qts, qint32 date, qint32 seq, qint32 unreadCount);
    void updatesGetDifferenceAnswer(qint64 id, qint32 date, qint32 seq);
    void updatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);

    // Working with files
    void uploadGetFileAnswer(qint64 fileId, const StorageFileType &type, qint32 mtime, const QByteArray &bytes, qint32 partId, qint32 downloaded, qint32 total);
    void uploadCancelFileAnswer(qint64 fileId, bool cancelled);
    void uploadSendFileAnswer(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize);

    // Miscellaneous
    void helpGetSuppportAnswer(qint64 id, const QString &phoneNumber, const User &user);
    void helpGetInviteTextAnswer(qint64 id, const QString &message);
    void helpGetAppUpdateAnswer(qint64 id, qint32 updateId, bool critical, QString url, QString text); // if updateId <=0 there isn't any update

    // Updates
    void updatesTooLong();
    void updateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShort(const Update &update, qint32 date);
    void updatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    void updates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

    void updateSecretChatMessage(const SecretChatMessage &secretChatMessage, qint32 qts);

    // Additional signals
    void disconnected();
    void connected();
    void woken();

    void fatalError();

protected:

    enum LibraryState {
        LoggedOut,
        CreatedSharedKeys,
        LoggedIn
    };

    enum LastRetryType {
        PhoneCheck,
        GetInviteText,
        ImportContacts,
        NotRetry
    };

    LibraryState mLibraryState;
    LastRetryType mLastRetryType;

    bool mSlept;

private:
    Settings *mSettings;
    CryptoUtils *mCrypto;

    Api *mApi;
    DcProvider *mDcProvider;
    FileHandler::Ptr mFileHandler;

    QString m_phoneCodeHash;
    QString mSettingsId;

    // cached contacts
    QList<Contact> m_cachedContacts;
    QList<User> m_cachedUsers;

    // encrypted chats
    SecretState *mSecretState;
    Encrypter *mEncrypter;
    Decrypter *mDecrypter;
    void processSecretChatUpdate(const Update &update);
    qint64 generateGAorB(SecretChat *secretChat);
    void createSharedKey(SecretChat * secretChat, BIGNUM *p, QByteArray gAOrB);
    SecretChatMessage toSecretChatMessage(const EncryptedMessage &encryptedMessage);
    void processDifferences(qint64 id, const QList<Message> &messages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);

    bool mLoggedIn;
    bool mCreatedSharedKeys;

    //info for retries
    QString mLastPhoneChecked;
    QString mLastLangCode;
    QList<InputContact> mLastContacts;

private Q_SLOTS:
    void onDcProviderReady();
    void onAuthLoggedIn();
    void onError(qint64 id, qint32 errorCode, const QString &errorText, const QString &functionName = QString());
    void onErrorRetry(qint64 id, qint32 errorCode, const QString &errorText);
    void onAuthCheckPhoneDcChanged();
    void onHelpGetInviteTextDcChanged();
    void onImportContactsDcChanged();
    void onAuthSentCode(qint64 id, bool phoneRegistered, const QString &phoneCodeHash, qint32 sendCallTimeout, bool isPassword);
    void onUserAuthorized(qint64 id, qint32 expires, const User &user);
    void onAuthLogOutAnswer(qint64 id, bool ok);
    void onPhotosPhotos(qint64 msgId, const QList<Photo> &photos, const QList<User> &users);
    void onContactsImportContactsAnswer();
    void onContactsContacts(qint64 msgId, const QList<Contact> &contacts, const QList<User> &users);
    void onContactsContactsNotModified(qint64 msgId);
    void onContactsBlocked(qint64 msgId, const QList<ContactBlocked> &blocked, const QList<User> &users);
    void onMessagesSentMessage(qint64 id, qint32 msgId, qint32 date, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesSendMediaAnswer(qint64 fileId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount);
    void onMessagesGetMessagesMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void onMessagesDialogs(qint64 msgId, const QList<Dialog> &dialogs, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void onMessagesGetHistoryMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void onMessagesSearchMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void onMessagesForwardMsgStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesForwardMsgsStatedMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesSendBroadcastStatedMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesEditChatTitleStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesEditChatPhotoStatedMessageAnswer(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesAddChatUserStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesDeleteChatUserStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesCreateChatStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq);
    void onMessagesDhConfig(qint64 msgId, qint32 g, const QByteArray &p, qint32 version, const QByteArray &random);
    void onMessagesDhConfigNotModified(qint64 msgId, const QByteArray &random);
    void onMessagesRequestEncryptionEncryptedChat(qint64 msgId, const EncryptedChat &chat);
    void onUpdatesDifference(qint64 msgId, const QList<Message> &messsages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state);
    void onUpdatesDifferenceSlice(qint64 msgId, const QList<Message> &messsages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state);

    // secret chats slots
    void onUpdateShort(const Update &update);
    void onUpdatesCombined(const QList<Update> &updates);
    void onUpdates(const QList<Update> &udts);
    void onMessagesAcceptEncryptionEncryptedChat(qint64, const EncryptedChat &chat);
    void onMessagesDiscardEncryptionResult(qint64, bool ok);
    void onSequenceNumberGap(qint32 chatId, qint32 startSeqNo, qint32 endSeqNo);
};

#endif // TELEGRAM_H
