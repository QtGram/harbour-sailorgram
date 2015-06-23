/*
 * Copyright 2013 Vitaly Valtman
 * Copyright 2014 Canonical Ltd.
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
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
 */

#ifndef API_H
#define API_H

#include "abstractapi.h"
#include "types/user.h"
#include "types/inputpeer.h"
#include "types/message.h"
#include "types/user.h"
#include "types/dialog.h"
#include "types/config.h"
#include "types/affectedmessages.h"
#include "types/accountpassword.h"
#include "types/chat.h"
#include "types/inputuser.h"
#include "types/contact.h"
#include "types/contactfound.h"
#include "types/contactslink.h"
#include "types/photo.h"
#include "types/contactstatus.h"
#include "types/importedcontact.h"
#include "types/inputcontact.h"
#include "types/contactblocked.h"
#include "types/messagesfilter.h"
#include "types/inputmedia.h"
#include "types/inputchatphoto.h"
#include "types/encryptedmessage.h"
#include "types/updatesstate.h"
#include "types/inputpeernotifysettings.h"
#include "types/inputnotifypeer.h"
#include "types/wallpaper.h"
#include "types/encryptedchat.h"
#include "types/inputencryptedchat.h"
#include "types/inputencryptedfile.h"
#include "types/accountdaysttl.h"
#include "types/disabledfeature.h"
#include "types/inputprivacykey.h"
#include "types/inputprivacyrule.h"
#include "types/messagesstickers.h"
#include "types/messagesallstickers.h"
#include "core/session.h"
#include <QByteArray>
#include <QList>
#include <QStringList>

#include "secret/secretchat.h"

class Api : public AbstractApi
{
    Q_OBJECT
public:
    explicit Api(Session *session, Settings *settings, CryptoUtils *crypto, QObject *parent = 0);

    // Registration / authorization
    qint64 helpGetConfig();
    qint64 helpGetInviteText(const QString &langCode);
    qint64 authCheckPhone(const QString &phoneNumber);
    qint64 authSendCode(const QString &phoneNumber, qint32 smsType, qint32 apiId, const QString &apiHash, const QString &langCode);
    qint64 authSendSms(const QString &phoneNumber, const QString &phoneCodeHash);
    qint64 authSendCall(const QString &phoneNumber, const QString &phoneCodeHash);
    qint64 authSignIn(const QString &phoneNumber, const QString &phoneCodeHash, const QString &phoneCode);
    qint64 authSignUp(const QString &phoneNumber, const QString &phoneCodeHash, const QString &phoneCode, const QString &firstName, const QString &lastName);
    qint64 authLogOut();
    qint64 authSendInvites(const QStringList &phoneNumbers, const QString &message);
    qint64 authResetAuthorizations();
    qint64 authExportAuthorization(qint32 dcId);
    qint64 authImportAuthorization(qint32 id, const QByteArray &bytes);
    qint64 authCheckPassword(const QByteArray &passwordHash);
    // Notifications / settings
    qint64 accountRegisterDevice(qint32 tokenType, const QString &token, const QString &deviceModel, const QString &systemVersion, const QString &appVersion, bool appSandbox, const QString &langCode);
    qint64 accountUnregisterDevice(qint32 tokenType, const QString &token);
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
    qint64 accountSendChangePhoneCode(const QString &phone_number);
    qint64 accountChangePhone(const QString &phone_number, const QString &phone_code_hash, const QString &phone_code);
    qint64 accountUpdateDeviceLocked(int period);
    qint64 accountGetPassword();
    qint64 accountSetPassword(const QByteArray &currentPasswordHash, const QByteArray &newSalt, const QByteArray &newPasswordHash, const QString &hint);
    qint64 photosUploadProfilePhoto(const InputFile &file, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop);
    qint64 photosUpdateProfilePhoto(const InputPhoto &id, const InputPhotoCrop &crop);
    // Users
    qint64 usersGetUsers(const QList<InputUser> &users);
    qint64 usersGetFullUser(const InputUser &user);
    qint64 photosGetUserPhotos(const InputUser &user, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    // Contacts
    qint64 contactsGetStatuses();
    qint64 contactsGetContacts(const QString &hash);
    qint64 contactsImportContacts(const QList<InputContact> &contacts, bool replace);
    qint64 contactsDeleteContact(const InputUser &id);
    qint64 contactsDeleteContacts(const QList<InputUser> &ids);
    qint64 contactsSearch(const QString &q, qint32 limit = 0);
    qint64 contactsResolveUsername(const QString &username);
    // Blacklist
    qint64 contactsBlock(const InputUser &id);
    qint64 contactsUnblock(const InputUser &id);
    qint64 contactsGetBlocked(qint32 offset = 0, qint32 limit = 0);
    // Messages
    qint64 messagesSendMessage(const InputPeer &peer, const QString &message, qint64 randomId, qint32 replyToMsgId);
    qint64 messagesSendMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId);
    qint64 messagesSetTyping(const InputPeer &peer, const SendMessageAction &action);
    qint64 messagesGetMessages(const QList<qint32> &ids);
    qint64 messagesGetDialogs(qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 messagesGetHistory(const InputPeer &peer, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 messagesSearch(const InputPeer &peer, const QString &q, const MessagesFilter &filter, qint32 minDate = 0, qint32 maxDate = 0, qint32 offset = 0, qint32 maxId = 0, qint32 limit = 0);
    qint64 messagesReadHistory(const InputPeer &peer, qint32 maxId = 0, qint32 offset = 0);
    qint64 messagesReadMessageContents(const QList<qint32> &ids);
    qint64 messagesDeleteHistory(const InputPeer &peer, qint32 offset = 0);
    qint64 messagesDeleteMessages(const QList<qint32> &ids);
    qint64 messagesRestoreMessages(const QList<qint32> &ids);
    qint64 messagesReceivedMessages(qint32 maxId);
    qint64 messagesForwardMessage(const InputPeer &peer, qint32 id, qint64 randomId);
    qint64 messagesForwardMessages(const InputPeer &peer, const QList<qint32> &ids, const QList<qint32> &randomIds);
    qint64 messagesSendBroadcast(const QList<InputUser> &contacts, const QString &message, const InputMedia &media);
    // Chats
    qint64 messagesGetChats(const QList<qint32> &chatIds);
    qint64 messagesGetFullChat(qint32 chatId);
    qint64 messagesEditChatTitle(qint32 chatId, const QString &title);
    qint64 messagesEditChatPhoto(qint32 chatId, const InputChatPhoto &photo);
    qint64 messagesAddChatUser(qint32 chatId, const InputUser &user, qint32 fwdLimit = 0);
    qint64 messagesDeleteChatUser(qint32 chatId, const InputUser &user);
    qint64 messagesCreateChat(const QList<InputUser> &users, const QString &title);
    // Secret chats
    qint64 messagesGetDhConfig(qint32 version, qint32 randomLength);
    qint64 messagesRequestEncryption(const InputUser &user, qint32 randomId, QByteArray g_a);
    qint64 messagesDiscardEncryption(qint32 chatId);
    qint64 messagesAcceptEncryption(const InputEncryptedChat &inputEncryptedChat, QByteArray g_b, qint64 keyFingerprint);
    qint64 messagesSetEncryptedTyping(const InputEncryptedChat &inputEncryptedChat, bool typing);
    qint64 messagesReadEncryptedHistory(const InputEncryptedChat &inputEncryptedChat, qint32 maxDate);
    qint64 messagesSendEncrypted(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data);
    qint64 messagesSendEncryptedFile(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data, const InputEncryptedFile &file);
    qint64 messagesSendEncryptedService(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data);
    qint64 messagesReceivedQueue(qint32 maxQts);
    // Stickers
    qint64 messagesGetStickers(const QString &emoticon, const QString &hash);
    qint64 messagesGetAllStickers(const QString &hash);
    // Updates
    qint64 updatesGetState();
    qint64 updatesGetDifference(qint32 pts, qint32 date, qint32 qts);
    // Files
    qint64 uploadSaveFilePart(Session *session, qint64 fileId, qint32 filePart, const QByteArray &bytes);
    qint64 uploadSaveBigFilePart(Session *session, qint64 fileId, qint32 filePart, qint32 fileTotalParts, const QByteArray &bytes);
    qint64 uploadGetFile(Session *session, const InputFileLocation &location, qint32 offset = 0, qint32 limit = BLOCK);

Q_SIGNALS:
    //# Answers
    // Registration / authorization
    void config(qint64 msgId, const Config &config);
    void helpGetInviteTextAnswer(qint64 msgId, const QString &inviteText); //inviteText is localized
    void authCheckedPhone(qint64 msgId, bool phoneRegistered);
    void authCheckPhoneSent(qint64 msgId, const QString &phoneNumber);
    void authSentCode(qint64 msgId, bool phoneRegistered, const QString &phoneCodeHash, qint32 sendCallTimeout, bool isPassword);
    void authSentAppCode(qint64 msgId, bool phoneRegistered, const QString &phoneCodeHahs, qint32 sendCallTimeout, bool isPassword);
    void authSendSmsResult(qint64 msgId, bool ok);
    void authSendCallResult(qint64 msgId, bool ok);
    void authSignUpAuthorization(qint64 msgId, qint32 expires, const User &user);
    void authSignInAuthorization(qint64 msgId, qint32 expires, const User &user);
    void authLogOutResult(qint64 msgId, bool ok);
    void authSendInvitesResult(qint64 msgId, bool ok);
    void authResetAuthorizationsResult(qint64 msgId, bool ok);
    void authExportedAuthorization(qint64 msgId, qint32 id, const QByteArray &bytes);
    void authImportedAuthorization(qint64 msgId, qint32 expires, const User &user);
    void authCheckPasswordResult(qint64 msgId, qint32 expires, const User &user);
    // Notifications / settings
    void accountRegisterDeviceResult(qint64 msgId, bool ok);
    void accountUnregisterDeviceResult(qint64 msgId, bool ok);
    void accountUpdateNotifySettingsResult(qint64 msgId, bool ok);
    void accountPeerNotifySettings(qint64 msgId, const PeerNotifySettings &peerNotifySettings);
    void accountResetNotifySettingsResult(qint64 msgId, bool ok);
    void accountUser(qint64 msgId, const User &user);
    void accountUpdateStatusResult(qint64 msgId, bool ok);
    void accountGetWallPapersResult(qint64 msgId, const QList<WallPaper> &wallpapers);
    void accountCheckUsernameResult(qint64 msgId, bool ok);
    void accountUpdateUsernameResult(qint64 msgId, const User &user);
    void accountPrivacyRules(qint64 msgId, const QList<PrivacyRule> &rules, const QList<User> &users);
    void accountDeleteAccountResult(qint64 msgId, bool ok);
    void accountGetAccountTTLResult(qint64 msgId, const AccountDaysTTL &ttl);
    void accountSetAccountTTLResult(qint64 msgId, bool ok);
    void accountUpdateDeviceLockedResult(qint64 msgId, bool ok);
    void accountChangePhoneResult(qint64 msgId, const User &user);
    void accountSentChangePhoneCode(qint64 msgId, const QString &phone_code_hash, qint32 send_call_timeout);
    void accountSetPasswordResult(qint64 msgId, bool ok);
    void accountGetPasswordResult(qint64 msgId, const AccountPassword &password);
    void photosPhoto(qint64 msgId, const Photo &photo, const QList<User> &users);
    void photosUserProfilePhoto(qint64 msgId, const UserProfilePhoto &photo);
    // Users
    void usersGetUsersResult(qint64 msgId, const QList<User> &users);
    void userFull(qint64 msgId, const User &user, const ContactsLink &link, const Photo &profilePhoto, const PeerNotifySettings &notifySettings, bool blocked, const QString &realFirstName, const QString &realLastName);
    void photosPhotos(qint64 msgId, const QList<Photo> &photos, const QList<User> &users);
    void photosPhotosSlice(qint64 msgId, qint32 count, const QList<Photo> &photos, const QList<User> &users);
    // Contacts
    void contactsGetStatusesResult(qint64 msgId, const QList<ContactStatus> &statuses);
    void contactsContacts(qint64 msgId, const QList<Contact> &contacts, const QList<User> &users);
    void contactsContactsNotModified(qint64 msgId);
    void contactsImportedContacts(qint64 msgId, const QList<ImportedContact> &imported, const QList<qint64> &retryContacts, const QList<User> &users);
    void contactsDeleteContactLink(qint64 msgId, const ContactLink &myLink, const ContactLink &foreignLink, const User &user);
    void contactsDeleteContactsResult(qint64 msgId, bool ok);
    void contactsFound(qint64 msgId, const QList<ContactFound> &founds, const QList<User> &users);
    void contactsResolveUsernameResult(qint64 msgId, const User& user);
    // Blacklist
    void contactsBlockResult(qint64 msgId, bool ok);
    void contactsUnblockResult(qint64 msgId, bool ok);
    void contactsBlocked(qint64 msgId, const QList<ContactBlocked> &blocked, const QList<User> &users);
    void contactsBlockedSlice(qint64 msgId, qint32 count, const QList<ContactBlocked> &blocked, const QList<User> &users);
    // Messages
    void messagesSentMessage(qint64 msgId, qint32 id, qint32 date, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesSentMessageLink(qint64 msgId, qint32 id, qint32 date, qint32 pts, qint32 pts_count, qint32 seq, const QList<ContactsLink> &links);
    void messagesSentMediaStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count);
    void messagesSentMediaStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesSetTypingResult(qint64 msgId, bool ok);
    void messagesGetMessagesMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesGetMessagesMessagesSlice(qint64 msgId, qint32 count, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesDialogs(qint64 msgId, const QList<Dialog> &dialogs, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesDialogsSlice(qint64 msgId, qint32 count, const QList<Dialog> &dialogs, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesGetHistoryMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesGetHistoryMessagesSlice(qint64 msgId, qint32 count, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesSearchMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesSearchMessagesSlice(qint64 msgId, qint32 count, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);
    void messagesReadAffectedHistory(qint64 msgId, qint32 pts, qint32 pts_count, qint32 offset);
    void messagesReadMessageContentsResult(qint64 msgId, const AffectedMessages &watchedIds);
    void messagesDeleteAffectedHistory(qint64 msgId, qint32 pts, qint32 pts_count, qint32 offset);
    void messagesDeleteMessagesResult(qint64 msgId, const AffectedMessages &deletedIds);
    void messagesRestoreMessagesResult(qint64 msgId, const QList<qint32> &restoredIds);
    void messagesReceivedMessagesResult(qint64 msgId, const QList<qint32> &confirmedIds);
    void messagesForwardMsgStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesForwardMsgStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesForwardMsgsStatedMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesForwardMsgsStatedMessagesLinks(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesSendBroadcastStatedMessages(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesSendBroadcastStatedMessagesLinks(qint64 msgId, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    // Chats
    void messagesChats(qint64 msgId, const QList<Chat> &chats);
    void messagesChatFull(qint64 msgId, const ChatFull &fullChat, const QList<Chat> &chats, const QList<User> &users);
    void messagesEditChatTitleStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesEditChatTitleStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesEditChatPhotoStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesEditChatPhotoStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesAddChatUserStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesAddChatUserStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesDeleteChatUserStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesDeleteChatUserStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    void messagesCreateChatStatedMessage(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq = 0);
    void messagesCreateChatStatedMessageLink(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 pts_count, qint32 seq);
    // Secret chats
    void messagesDhConfig(qint64 msgId, qint32 g, const QByteArray &p, qint32 version, const QByteArray &random);
    void messagesDhConfigNotModified(qint64 msgId, const QByteArray &random);
    void messagesRequestEncryptionEncryptedChat(qint64 msgId, const EncryptedChat &encryptedChat);
    void messagesAcceptEncryptionEncryptedChat(qint64 msgId, const EncryptedChat &encryptedChat);
    void messagesDiscardEncryptionResult(qint64 msgId, bool ok);
    void messagesSetEncryptedTypingResult(qint64 msgId, bool ok);
    void messagesReadEncryptedHistoryResult(qint64 msgId, bool ok);
    void messagesSendEncryptedSentEncryptedMessage(qint64 msgId, qint32 date);
    void messagesSendEncryptedSentEncryptedFile(qint64 msgId, qint32 date, const EncryptedFile &file);
    void messagesSendEncryptedFileSentEncryptedMessage(qint64 msgId, qint32 date);
    void messagesSendEncryptedFileSentEncryptedFile(qint64 msgId, qint32 date, const EncryptedFile &file);
    void messagesSendEncryptedServiceSentEncryptedMessage(qint64 msgId, qint32 date);
    void messagesSendEncryptedServiceSentEncryptedFile(qint64 msgId, qint32 date, const EncryptedFile &file);
    void messagesSentEncryptedService(qint64 msgId, qint32 date);
    void messagesReceivedQueueResult(qint64 msgId, const QList<qint64> &randomIds);
    // Stickers
    void messagesGetStickersResult(qint64 msgId, const MessagesStickers &stickers);
    void messagesGetAllStickersResult(qint64 msgId, const MessagesAllStickers &stickers);
    // Updates
    void updatesState(qint64 msgId, qint32 pts, qint32 qts, qint32 date, qint32 seq, qint32 unreadCount);
    void updatesDifferenceEmpty(qint64 msgId, qint32 date, qint32 seq);
    void updatesDifference(qint64 msgId, const QList<Message> &messsages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state);
    void updatesDifferenceSlice(qint64 msgId, const QList<Message> &messsages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &intermediateState);
    // Files
    void uploadSaveFilePartResult(qint64 msgId, qint64 fileId, bool ok);
    void uploadSaveBigFilePartResult(qint64 msgId, qint64 fileId, bool ok);
    void uploadFile(qint64 msgId, const StorageFileType &type, qint32 mtime, QByteArray bytes);
    //errors
    void error(qint64 msgId, qint32 errorCode, const QString &errorText, const QString &functionName);
    void errorRetry(qint64 msgId, qint32 errorCode, const QString &errorText);
    void authSignInError(qint64 msgId, qint32 errorCode, const QString &errorText);
    void authSignUpError(qint64 msgId, qint32 errorCode, const QString &errorText);
    void uploadFileError(qint64 msgId, qint32 errorCode, const QString &errorText);

private:
    Settings *mSettings;
    CryptoUtils *mCrypto;

    QueryMethods helpGetConfigMethods;
    QueryMethods helpGetInviteTextMethods;
    QueryMethods authCheckPhoneMethods;
    QueryMethods authSendCodeMethods;
    QueryMethods authSendSmsMethods;
    QueryMethods authSendCallMethods;
    QueryMethods authSignInMethods;
    QueryMethods authSignUpMethods;
    QueryMethods authLogOutMethods;
    QueryMethods authSendInvitesMethods;
    QueryMethods authResetAuthorizationsMethods;
    QueryMethods authExportAuthorizationMethods;
    QueryMethods authImportAuthorizationMethods;
    QueryMethods authCheckPasswordMethods;
    QueryMethods accountRegisterDeviceMethods;
    QueryMethods accountUnregisterDeviceMethods;
    QueryMethods accountUpdateNotifySettingsMethods;
    QueryMethods accountGetNotifySettingsMethods;
    QueryMethods accountResetNotifySettingsMethods;
    QueryMethods accountUpdateProfileMethods;
    QueryMethods accountUpdateStatusMethods;
    QueryMethods accountGetWallPapersMethods;
    QueryMethods accountCheckUsernameMethods;
    QueryMethods accountUpdateUsernameMethods;
    QueryMethods accountAccountGetPrivacyMethods;
    QueryMethods accountAccountSetPrivacyMethods;
    QueryMethods accountDeleteAccountMethods;
    QueryMethods accountGetAccountTTLMethods;
    QueryMethods accountSetAccountTTLMethods;
    QueryMethods accountSendChangePhoneCodeMethods;
    QueryMethods accountChangePhoneMethods;
    QueryMethods accountUpdateDeviceLockedMethods;
    QueryMethods accountGetPasswordMethods;
    QueryMethods accountSetPasswordMethods;
    QueryMethods photosUploadProfilePhotoMethods;
    QueryMethods photosUpdateProfilePhotoMethods;
    QueryMethods usersGetUsersMethods;
    QueryMethods usersGetFullUserMethods;
    QueryMethods photosGetUserPhotosMethods;
    QueryMethods contactsGetStatusesMethods;
    QueryMethods contactsGetContactsMethods;
    QueryMethods contactsImportContactsMethods;
    QueryMethods contactsDeleteContactMethods;
    QueryMethods contactsDeleteContactsMethods;
    QueryMethods contactsBlockMethods;
    QueryMethods contactsUnblockMethods;
    QueryMethods contactsGetBlockedMethods;
    QueryMethods contactsSearchMethods;
    QueryMethods contactsResolveUsernameMethods;
    QueryMethods messagesSendMessageMethods;
    QueryMethods messagesSendMediaMethods;
    QueryMethods messagesSetTypingMethods;
    QueryMethods messagesGetMessagesMethods;
    QueryMethods messagesGetDialogsMethods;
    QueryMethods messagesGetHistoryMethods;
    QueryMethods messagesSearchMethods;
    QueryMethods messagesReadHistoryMethods;
    QueryMethods messagesReadMessageContentsMethods;
    QueryMethods messagesDeleteHistoryMethods;
    QueryMethods messagesDeleteMessagesMethods;
    QueryMethods messagesRestoreMessagesMethods;
    QueryMethods messagesReceivedMessagesMethods;
    QueryMethods messagesForwardMessageMethods;
    QueryMethods messagesForwardMessagesMethods;
    QueryMethods messagesSendBroadcastMethods;
    QueryMethods messagesGetChatsMethods;
    QueryMethods messagesGetFullChatMethods;
    QueryMethods messagesEditChatTitleMethods;
    QueryMethods messagesEditChatPhotoMethods;
    QueryMethods messagesAddChatUserMethods;
    QueryMethods messagesDeleteChatUserMethods;
    QueryMethods messagesCreateChatMethods;
    QueryMethods messagesGetDhConfigMethods;
    QueryMethods messagesRequestEncryptionMethods;
    QueryMethods messagesDiscardEncryptionMethods;
    QueryMethods messagesAcceptEncryptionMethods;
    QueryMethods messagesSetEncryptedTypingMethods;
    QueryMethods messagesReadEncryptedHistoryMethods;
    QueryMethods messagesSendEncryptedMethods;
    QueryMethods messagesSendEncryptedFileMethods;
    QueryMethods messagesSendEncryptedServiceMethods;
    QueryMethods messagesReceivedQueueMethods;
    QueryMethods messagesGetStickersMethods;
    QueryMethods messagesGetAllStickersMethods;
    QueryMethods updatesGetStateMethods;
    QueryMethods updatesGetDifferenceMethods;
    QueryMethods uploadSaveFilePartMethods;
    QueryMethods uploadSaveBigFilePartMethods;
    QueryMethods uploadGetFileMethods;

    //answers
    void onHelpGetConfigAnswer(Query *q, InboundPkt &inboundPkt);
    void onHelpGetInviteTextAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthCheckPhoneAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthSendCodeAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthSendSmsAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthSendCallAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthSignInAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthSignUpAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthLogOutAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthSendInvitesAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthResetAuthorizationsAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthExportAuthorizationAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthImportAuthorizationAnswer(Query *q, InboundPkt &inboundPkt);
    void onAuthCheckPasswordAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountRegisterDeviceAnswer(Query *q, InboundPkt & inboundPkt);
    void onAccountUnregisterDeviceAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountUpdateNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountGetNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountResetNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountUpdateProfileAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountUpdateStatusAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountGetWallPapersAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountCheckUsernameAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountUpdateUsernameAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountPrivacyRules(Query *q, InboundPkt &inboundPkt);
    void onAccountDeleteAccountAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountGetAccountTTLAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountSetAccountTTLAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountChangePhoneAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountUpdateDeviceLockedAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountGetPasswordAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountSetPasswordAnswer(Query *q, InboundPkt &inboundPkt);
    void onAccountSentChangePhoneCode(Query *q, InboundPkt &inboundPkt);
    void onPhotosUploadProfilePhotoAnswer(Query *q, InboundPkt &inboundPkt);
    void onPhotosUpdateProfilePhotoAnswer(Query *q, InboundPkt &inboundPkt);
    void onUsersGetUsersAnswer(Query *q, InboundPkt &inboundPkt);
    void onUsersGetFullUserAnswer(Query *q, InboundPkt &inboundPkt);
    void onPhotosGetUserPhotos(Query *q, InboundPkt &inboundPkt);
    void onContactsGetStatusesAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsGetContactsAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsImportContactsAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsDeleteContactAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsDeleteContactsAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsSearchAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsBlockAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsUnblockAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsGetBlockedAnswer(Query *q, InboundPkt &inboundPkt);
    void onContactsResolveUsernameAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSendMessageAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSendMediaAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSetTypingAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetMessagesAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetDialogsAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetHistoryAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSearchAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesReadHistoryAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesReadMessageContentsAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesDeleteHistoryAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesDeleteMessagesAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesRestoreMessagesAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesReceivedMessagesAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesForwardMessageAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesForwardMessagesAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSendBroadcastAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetChatsAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetFullChatAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesEditChatTitleAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesEditChatPhotoAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesAddChatUserAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesDeleteChatUserAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesCreateChatAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetDhConfigAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesRequestEncryptionAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesDiscardEncryptionAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesAcceptEncryptionAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSetEncryptedTypingAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesReadEncryptedHistoryAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSendEncryptedAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSendEncryptedFileAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesSendEncryptedServiceAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesReceivedQueueAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetStickersAnswer(Query *q, InboundPkt &inboundPkt);
    void onMessagesGetAllStickersAnswer(Query *q, InboundPkt &inboundPkt);
    void onUpdatesGetStateAnswer(Query *q, InboundPkt &inboundPkt);
    void onUpdatesGetDifferenceAnswer(Query *q, InboundPkt &inboundPkt);
    void onUploadSaveFilePartAnswer(Query *q, InboundPkt &inboundPkt);
    void onUploadSaveBigFilePartAnswer(Query *q, InboundPkt &inboundPkt);
    void onUploadGetFileAnswer(Query *q, InboundPkt &inboundPkt);

    //errors
    void onError(Query *q, qint32 errorCode, const QString &errorText);
    void onErrorRetry(Query *q, qint32 errorCode, const QString &errorText);
    void onAuthSignInError(Query *q, qint32 errorCode, const QString &errorText);
    void onAuthSignUpError(Query *q, qint32 errorCode, const QString &errorText);
    void onUploadGetFileError(Query *q, qint32 errorCode, const QString &errorText);
};

#endif // API_H
