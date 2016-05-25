/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TELEGRAMQML_H
#define TELEGRAMQML_H

#define UNREAD_OUT_TO_FLAG(UNREAD, OUT) ((UNREAD?0x1:0)|(OUT?0x2:0))
#define FLAG_TO_UNREAD(FLAG) (FLAG&0x1?true:false)
#define FLAG_TO_OUT(FLAG) (FLAG&0x2?true:false)

#include <QObject>
#include <QStringList>
#include <QUrl>

#include <telegram/types/types.h>

#include "telegramthumbnailer.h"
#include "telegramqml_global.h"
#include "databaseabstractencryptor.h"

class SecretChatMessage;
class SecretChat;
class DecryptedMessage;
class PhotoSize;
class TelegramSearchModel;
class NewsLetterDialog;
class Database;
class UserData;
class TelegramMessagesModel;
class DownloadObject;
class EncryptedFileObject;
class DecryptedMessageObject;
class EncryptedChatObject;
class EncryptedMessageObject;
class DocumentObject;
class VideoObject;
class AudioObject;
class WallPaperObject;
class FileLocationObject;
class PhotoObject;
class ContactObject;
class DialogObject;
class MessageObject;
class InputPeerObject;
class ChatFullObject;
class ChatObject;
class UserObject;
class UploadObject;
class StickerPackObject;
class StickerSetObject;
class Telegram;
class TelegramThumbnailer;
class TelegramQmlPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramQml : public QObject
{
    Q_OBJECT
    Q_ENUMS(LogLevel)

    Q_PROPERTY(QString defaultHostAddress READ defaultHostAddress WRITE setDefaultHostAddress NOTIFY defaultHostAddressChanged)
    Q_PROPERTY(int defaultHostPort READ defaultHostPort WRITE setDefaultHostPort NOTIFY defaultHostPortChanged)
    Q_PROPERTY(int defaultHostDcId READ defaultHostDcId WRITE setDefaultHostDcId NOTIFY defaultHostDcIdChanged)
    Q_PROPERTY(int appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString appHash READ appHash WRITE setAppHash NOTIFY appHashChanged)

    Q_PROPERTY(QString phoneNumber   READ phoneNumber   WRITE setPhoneNumber   NOTIFY phoneNumberChanged  )
    Q_PROPERTY(QString configPath    READ configPath    WRITE setConfigPath    NOTIFY configPathChanged   )
    Q_PROPERTY(QUrl publicKeyFile READ publicKeyFile WRITE setPublicKeyFile NOTIFY publicKeyFileChanged)
    Q_PROPERTY(QString downloadPath  READ downloadPath  WRITE setDownloadPath  NOTIFY downloadPathChanged )
    Q_PROPERTY(QString tempPath      READ tempPath      WRITE setTempPath      NOTIFY tempPathChanged     )

    Q_PROPERTY(QObject* newsLetterDialog READ newsLetterDialog WRITE setNewsLetterDialog NOTIFY newsLetterDialogChanged     )
    Q_PROPERTY(DatabaseAbstractEncryptor* encrypter READ encrypter WRITE setEncrypter NOTIFY encrypterChanged)
    Q_PROPERTY(bool autoAcceptEncrypted READ autoAcceptEncrypted WRITE setAutoAcceptEncrypted NOTIFY autoAcceptEncryptedChanged)
    Q_PROPERTY(bool autoCleanUpMessages READ autoCleanUpMessages WRITE setAutoCleanUpMessages NOTIFY autoCleanUpMessagesChanged)
    Q_PROPERTY(int  autoRewakeInterval  READ autoRewakeInterval  WRITE setAutoRewakeInterval  NOTIFY autoRewakeIntervalChanged)

    Q_PROPERTY(bool  online               READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(int   unreadCount          READ unreadCount            NOTIFY unreadCountChanged)
    Q_PROPERTY(qreal totalUploadedPercent READ totalUploadedPercent   NOTIFY totalUploadedPercentChanged)

    Q_PROPERTY(bool uploadingProfilePhoto READ uploadingProfilePhoto NOTIFY uploadingProfilePhotoChanged)

    Q_PROPERTY(Telegram*   telegram    READ telegram    NOTIFY telegramChanged)
    Q_PROPERTY(UserData*   userData    READ userData    NOTIFY userDataChanged)
    Q_PROPERTY(Database*   database    READ database    NOTIFY databaseChanged)
    Q_PROPERTY(qint64      me          READ me          NOTIFY meChanged)
    Q_PROPERTY(qint64      cutegramId  READ cutegramId  NOTIFY fakeSignal)
    Q_PROPERTY(UserObject* myUser      READ myUser      NOTIFY myUserChanged)
    Q_PROPERTY(QString     homePath    READ homePath    NOTIFY fakeSignal)
    Q_PROPERTY(QString     currentPath READ currentPath NOTIFY fakeSignal)

    Q_PROPERTY(bool globalMute READ globalMute WRITE setGlobalMute NOTIFY globalMuteChanged)

    Q_PROPERTY(bool authNeeded          READ authNeeded          NOTIFY authNeededChanged         )
    Q_PROPERTY(bool authLoggedIn        READ authLoggedIn        NOTIFY authLoggedInChanged       )
    Q_PROPERTY(bool authPhoneRegistered READ authPhoneRegistered NOTIFY authPhoneRegisteredChanged)
    Q_PROPERTY(bool authPhoneInvited    READ authPhoneInvited    NOTIFY authPhoneInvitedChanged   )
    Q_PROPERTY(bool authPhoneChecked    READ authPhoneChecked    NOTIFY authPhoneCheckedChanged   )
    Q_PROPERTY(bool connected           READ connected           NOTIFY connectedChanged          )

    Q_PROPERTY(QString authSignUpError READ authSignUpError NOTIFY authSignUpErrorChanged)
    Q_PROPERTY(QString authSignInError READ authSignInError NOTIFY authSignInErrorChanged)
    Q_PROPERTY(QString error           READ error           NOTIFY errorChanged          )

    Q_PROPERTY(DialogObject* nullDialog READ nullDialog NOTIFY fakeSignal)
    Q_PROPERTY(MessageObject* nullMessage READ nullMessage NOTIFY fakeSignal)
    Q_PROPERTY(ChatObject* nullChat READ nullChat NOTIFY fakeSignal)
    Q_PROPERTY(UserObject* nullUser READ nullUser NOTIFY fakeSignal)
    Q_PROPERTY(WallPaperObject* nullWallpaper READ nullWallpaper NOTIFY fakeSignal)
    Q_PROPERTY(UploadObject* nullUpload READ nullUpload NOTIFY fakeSignal)
    Q_PROPERTY(ChatFullObject* nullChatFull READ nullChatFull NOTIFY fakeSignal)
    Q_PROPERTY(ContactObject* nullContact READ nullContact NOTIFY fakeSignal)
    Q_PROPERTY(FileLocationObject* nullLocation READ nullLocation NOTIFY fakeSignal)
    Q_PROPERTY(EncryptedChatObject* nullEncryptedChat READ nullEncryptedChat NOTIFY fakeSignal)
    Q_PROPERTY(EncryptedMessageObject* nullEncryptedMessage READ nullEncryptedMessage NOTIFY fakeSignal)
    Q_PROPERTY(DocumentObject* nullSticker READ nullSticker NOTIFY fakeSignal)
    Q_PROPERTY(StickerSetObject* nullStickerSet READ nullStickerSet NOTIFY fakeSignal)
    Q_PROPERTY(StickerPackObject* nullStickerPack READ nullStickerPack NOTIFY fakeSignal)

public:
    enum LogLevel {
        LogLevelClean,
        LogLevelUseful,
        LogLevelFull
    };

    TelegramQml(QObject *parent = 0);
    ~TelegramQml();

    QString phoneNumber() const;
    void setPhoneNumber( const QString & phone );

    QString downloadPath() const;
    void setDownloadPath( const QString & downloadPath );

    QString tempPath() const;
    void setTempPath( const QString & tempPath );

    QString homePath() const;
    QString currentPath() const;

    QString configPath() const;
    void setConfigPath( const QString & conf );

    QUrl publicKeyFile() const;
    void setPublicKeyFile( const QUrl & file );

    void setDefaultHostAddress(const QString &host);
    QString defaultHostAddress() const;

    void setDefaultHostPort(int port);
    int defaultHostPort() const;

    void setDefaultHostDcId(int dcId);
    int defaultHostDcId() const;

    void setAppId(int appId);
    int appId() const;

    void setAppHash(const QString &appHash);
    QString appHash() const;

    void setEncrypter(DatabaseAbstractEncryptor *encrypter);
    DatabaseAbstractEncryptor *encrypter() const;

    void setNewsLetterDialog(QObject *dialog);
    QObject *newsLetterDialog() const;

    void setAutoAcceptEncrypted(bool stt);
    bool autoAcceptEncrypted() const;

    void setAutoCleanUpMessages(bool stt);
    bool autoCleanUpMessages() const;

    void registerMessagesModel(TelegramMessagesModel *model);
    void unregisterMessagesModel(TelegramMessagesModel *model);

    void registerSearchModel(TelegramSearchModel *model);
    void unregisterSearchModel(TelegramSearchModel *model);

    UserData *userData() const;
    Database *database() const;
    Telegram *telegram() const;
    qint64 me() const;
    UserObject *myUser() const;
    qint64 cutegramId() const;

    bool online() const;
    void setOnline( bool stt );

    void setInvisible( bool stt );
    bool invisible() const;

    void setAutoRewakeInterval(int ms);
    int autoRewakeInterval() const;

    int unreadCount() const;
    qreal totalUploadedPercent() const;

    bool authNeeded() const;
    bool authLoggedIn() const;
    bool authPhoneChecked() const;
    bool authPhoneRegistered() const;
    bool authPhoneInvited() const;
    bool connected() const;

    bool uploadingProfilePhoto() const;

    QString authSignUpError() const;
    QString authSignInError() const;
    QString error() const;

    Q_INVOKABLE static void setLogLevel(int level);

    Q_INVOKABLE void authCheckPhone(const QString &phone);

    Q_INVOKABLE void reconnect();

    Q_INVOKABLE void mute(qint64 peerId);
    Q_INVOKABLE void unmute(qint64 peerId);
    void accountUpdateNotifySettings(qint64 peerId, qint32 muteUntil);

    void setGlobalMute(bool stt);
    bool globalMute() const;

    Q_INVOKABLE void helpGetInviteText(const QString &langCode);

    Q_INVOKABLE DialogObject *dialog(qint64 id) const;
    Q_INVOKABLE MessageObject *message(qint64 id) const;
    Q_INVOKABLE ChatObject *chat(qint64 id) const;
    Q_INVOKABLE UserObject *user(qint64 id) const;
    Q_INVOKABLE qint64 messageDialogId(qint64 id) const;
    Q_INVOKABLE DialogObject *messageDialog(qint64 id) const;
    Q_INVOKABLE WallPaperObject *wallpaper(qint64 id) const;
    Q_INVOKABLE MessageObject *upload(qint64 id) const;
    Q_INVOKABLE ChatFullObject *chatFull(qint64 id) const;
    Q_INVOKABLE ContactObject *contact(qint64 id) const;
    Q_INVOKABLE EncryptedChatObject *encryptedChat(qint64 id) const;
    Q_INVOKABLE DocumentObject* sticker(qint64 id) const;
    Q_INVOKABLE StickerSetObject* stickerSet(qint64 id) const;
    Q_INVOKABLE StickerSetObject* stickerSetByShortName(const QString &shortName) const;
    Q_INVOKABLE StickerPackObject* stickerPack(const QString &id) const;

    Q_INVOKABLE FileLocationObject *locationOf(qint64 id, qint64 dcId, qint64 accessHash, QObject *parent);
    Q_INVOKABLE FileLocationObject *locationOfPhoto(PhotoObject *photo);
    Q_INVOKABLE FileLocationObject *locationOfThumbPhoto(PhotoObject *photo);
    Q_INVOKABLE FileLocationObject *locationOfDocument(DocumentObject *doc);
    Q_INVOKABLE FileLocationObject *locationOfVideo(VideoObject *vid);
    Q_INVOKABLE FileLocationObject *locationOfAudio(AudioObject *aud);

    Q_INVOKABLE bool documentIsSticker(DocumentObject *doc);
    Q_INVOKABLE qint64 documentStickerId(DocumentObject *doc);
    Q_INVOKABLE QString documentFileName(DocumentObject *doc);

    Q_INVOKABLE DialogObject *fakeDialogObject( qint64 id, bool isChat );

    DialogObject *nullDialog() const;
    MessageObject *nullMessage() const;
    ChatObject *nullChat() const;
    UserObject *nullUser() const;
    WallPaperObject *nullWallpaper() const;
    UploadObject *nullUpload() const;
    ChatFullObject *nullChatFull() const;
    ContactObject *nullContact() const;
    FileLocationObject *nullLocation() const;
    EncryptedChatObject *nullEncryptedChat() const;
    EncryptedMessageObject *nullEncryptedMessage() const;
    DocumentObject *nullSticker() const;
    StickerSetObject *nullStickerSet() const;
    StickerPackObject *nullStickerPack() const;

    Q_INVOKABLE QString fileLocation( FileLocationObject *location );
    Q_INVOKABLE QString videoThumbLocation( const QString &path, TelegramThumbnailer_Callback callback );
    Q_INVOKABLE QString audioThumbLocation( const QString &path );

    QList<qint64> dialogs() const;
    QList<qint64> messages(qint64 did, qint64 maxId = 0) const;
    QList<qint64> wallpapers() const;
    QList<qint64> uploads() const;
    QList<qint64> contacts() const;
    QList<qint64> stickers() const;
    QList<qint64> installedStickerSets() const;
    QList<qint64> stickerSets() const;
    QList<qint64> stickerSetDocuments(qint64 id) const;
    QList<qint64> stickerSetDocuments(const QString &shortName) const;

    InputUser getInputUser(qint64 userId) const;
    InputPeer getInputPeer(qint64 pid);
    qint64 generateRandomId() const;

    QList<qint64> userIndex(const QString &keyword);

public Q_SLOTS:
    void authLogout();
    void authResetAuthorizations();
    void authSendCall();
    void authSendCode();
    void authSendInvites(const QStringList &phoneNumbers, const QString &inviteText);
    void authSignIn(const QString &code, bool retry = false);
    void authSignUp(const QString &code, const QString &firstName, const QString &lastName);
    void authCheckPassword(const QString &pass);

    void accountRegisterDevice(const QString &token, const QString &appVersion = QString::null);
    void accountUnregisterDevice(const QString &token);
    void accountUpdateProfile(const QString &firstName, const QString &lastName);
    void usersGetFullUser(qint64 userId);
    void accountCheckUsername(const QString &username);
    void accountUpdateUsername(const QString &username);

    void contactsBlock(qint64 userId);
    void contactsUnblock(qint64 userId);

    qint32 sendMessage( qint64 dialogId, const QString & msg, int replyTo = 0 );
    bool sendMessageAsDocument( qint64 dialogId, const QString & msg );
    void sendGeo(qint64 dialogId, qreal latitude, qreal longitude, int replyTo = 0);
    void forwardDocument(qint64 dialogId, DocumentObject *doc);

    void addContact(const QString &firstName, const QString &lastName, const QString &phoneNumber);
    void addContacts(const QVariantList &vcontacts);

    void forwardMessages( QList<int> msgIds, qint64 peerId );
    void deleteMessages(QList<int> msgIds );

    void deleteCutegramDialog();
    void messagesCreateChat(const QList<int> &users, const QString & topic );
    void messagesAddChatUser(qint64 chatId, qint64 userId, qint32 fwdLimit = 0);
    qint64 messagesDeleteChatUser(qint64 chatId, qint64 userId);
    void messagesEditChatTitle(qint32 chatId, const QString &title);
    void messagesEditChatPhoto(qint32 chatId, const QString &filePath);

    void messagesDeleteHistory(qint64 peerId, bool deleteChat = false, bool userRemoved = false);
    void messagesSetTyping(qint64 peerId, bool stt);
    qint64 messagesReadHistory(qint64 peerId, qint32 maxDate = 0);

    void messagesCreateEncryptedChat(qint64 userId);
    void messagesAcceptEncryptedChat(qint32 chatId);
    qint64 messagesDiscardEncryptedChat(qint32 chatId, bool force = false);

    void messagesGetFullChat(qint32 chatId);

    void installStickerSet(const QString &shortName);
    void uninstallStickerSet(const QString &shortName);
    void getStickerSet(const QString &shortName);
    void getStickerSet(DocumentObject *doc);

    void search(const QString &keyword);
    void searchContact(const QString &keyword);

    qint64 sendFile(qint64 dialogId, const QString & file , bool forceDocument = false, bool forceAudio = false);
    void getFile(FileLocationObject *location, qint64 type = InputFileLocation::typeInputFileLocation , qint32 fileSize = 0);
    void getFileJustCheck(FileLocationObject *location);
    void cancelDownload(DownloadObject *download);
    void cancelSendGet( qint64 fileId );

    void setProfilePhoto( const QString & fileName );

    void timerUpdateDialogs( qint32 duration = 1000 );
    void timerUpdateContacts( qint32 duration = 1000 );
    void cleanUpMessages();

    void updatesGetState();
    void updatesGetDifference();

    bool sleep();
    bool wake();

Q_SIGNALS:
    void defaultHostAddressChanged();
    void defaultHostPortChanged();
    void defaultHostDcIdChanged();
    void appIdChanged();
    void appHashChanged();
    void encrypterChanged();

    void globalMuteChanged();
    void phoneNumberChanged();
    void configPathChanged();
    void publicKeyFileChanged();
    void telegramChanged();
    void autoAcceptEncryptedChanged();
    void autoCleanUpMessagesChanged();
    void userDataChanged();
    void databaseChanged();
    void onlineChanged();
    void downloadPathChanged();
    void tempPathChanged();
    void dialogsChanged(bool cachedData);
    void messagesChanged(bool cachedData);
    void usersChanged();
    void chatsChanged();
    void wallpapersChanged();
    void autoRewakeIntervalChanged();
    void uploadsChanged();
    void chatFullsChanged();
    void contactsChanged();
    void autoUpdateChanged();
    void encryptedChatsChanged();
    void uploadingProfilePhotoChanged();
    void newsLetterDialogChanged();
    void installedStickersChanged();
    void stickerInstalled(const QString &shortName, bool ok);
    void stickerUninstalled(const QString &shortName, bool ok);
    void stickersChanged();
    void stickerRecieved(qint64 id);
    void documentStickerRecieved(DocumentObject *document, StickerSetObject *set);

    void unreadCountChanged();
    void totalUploadedPercentChanged();
    void invisibleChanged();

    void authNeededChanged();
    void authLoggedInChanged();
    void authLoggedOut();
    void authPhoneRegisteredChanged();
    void authPhoneInvitedChanged();
    void authPhoneCheckedChanged();
    void authPasswordNeeded();
    void phoneChecked(QString phone, bool phoneRegistered);
    void authPasswordProtectedError();
    void connectedChanged();

    void authSignUpErrorChanged();
    void authSignInErrorChanged();

    void authCodeRequested( bool phoneRegistered, qint32 sendCallTimeout );
    void authCallRequested( bool ok );
    void authInvitesSent( bool ok );

    void accountUsernameChecked(bool ok);
    void accountDeviceRegistered(bool ok);
    void accountDeviceUnregistered(bool ok);

    void userBecomeOnline(qint64 userId);
    void userStartTyping(qint64 userId, qint64 dId);
    void userBlocked(qint64 userId);
    void userUnblocked(qint64 userId);

    void contactsImportedContacts(qint32 importedCount, qint32 retryCount);

    void helpGetInviteTextAnswer(qint64 id, QString message);

    void errorChanged();
    void meChanged();
    void myUserChanged();
    void fakeSignal();

    void incomingMessage( MessageObject *msg );
    void incomingEncryptedMessage( EncryptedMessageObject *msg );

    void searchDone(const QList<qint64> &messages);
    void contactsFounded(const QList<qint32> &contacts);

    void messageSent(qint32 reqId, MessageObject *msg);
    void messagesSent(qint32 count);
    void messagesReceived(qint32 count);

    void errorSignal(qint64 id, qint32 errorCode, QString errorText, QString functionName);

protected:
    void try_init();

private Q_SLOTS:
    void authNeeded_slt();
    void authLoggedIn_slt();
    void authLogOut_slt(qint64 id, bool ok);
    void authSendCode_slt(qint64 id, bool phoneRegistered, qint32 sendCallTimeout);
    void authSendCodeError_slt(qint64 id);
    void authSendCall_slt(qint64 id, bool ok);
    void authSendInvites_slt(qint64 id, bool ok);
    void authCheckPassword_slt(qint64 msgId, qint32 expires, const User &user);
    void authCheckPhone_slt(qint64 id, bool phoneRegistered);
    void authCheckedPhoneError_slt(qint64 msgId);
    void authSignInError_slt(qint64 id, qint32 errorCode, QString errorText);
    void authSignUpError_slt(qint64 id, qint32 errorCode, QString errorText);
    void error_slt(qint64 id, qint32 errorCode, QString errorText, QString functionName);

    void accountRegisterDevice_slt(qint64 id, bool result);
    void accountUnregisterDevice_slt(qint64 id, bool result);
    void accountGetPassword_slt(qint64 msgId, const AccountPassword &password);
    void accountGetWallPapers_slt(qint64 id, const QList<WallPaper> & wallPapers);
    void accountCheckUsername_slt(qint64 id, bool ok);
    void accountUpdateUsername_slt(qint64 id, const User &user);


    void photosUploadProfilePhoto_slt(qint64 id, const Photo & photo, const QList<User> & users);
    void photosUpdateProfilePhoto_slt(qint64 id, const UserProfilePhoto & userProfilePhoto);

    void contactsBlock_slt(qint64 id, bool ok);
    void contactsUnblock_slt(qint64 id, bool ok);
    void contactsImportContacts_slt(qint64 id, const QList<ImportedContact> &importedContacts, const QList<qint64> &retryContacts, const QList<User> &users);
    void contactsFound_slt(qint64 id, const QList<ContactFound> &founds, const QList<User> &users);
    void contactsGetContacts_slt(qint64 id, bool modified, const QList<Contact> & contacts, const QList<User> & users);
    void usersGetFullUser_slt(qint64 id, const User &user, const ContactsLink &link, const Photo &profilePhoto, const PeerNotifySettings &notifySettings, bool blocked, const QString &realFirstName, const QString &realLastName);
    void usersGetUsers_slt(qint64 id, const QList<User> &users);

    void messagesSendMessage_slt(qint64 id, qint32 msgId, qint32 date, const MessageMedia &media, qint32 pts, qint32 pts_count, qint32 seq, const QList<ContactsLink> & links);
    void messagesForwardMessage_slt(qint64 id, const UpdatesType &updates);
    void messagesForwardMessages_slt(qint64 id, const UpdatesType &updates);
    void messagesDeleteMessages_slt(qint64 id, const MessagesAffectedMessages &deletedMessages);
    void messagesGetMessages_slt(qint64 id, qint32 sliceCount, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users);

    void messagesSendMedia_slt(qint64 id, const UpdatesType &updates);
    void messagesSendPhoto_slt(qint64 id, const UpdatesType &updates);
    void messagesSendVideo_slt(qint64 id, const UpdatesType &updates);
    void messagesSendAudio_slt(qint64 id, const UpdatesType &updates);
    void messagesSendDocument_slt(qint64 id, const UpdatesType &updates);
    void messagesForwardMedia_slt(qint64 id, const UpdatesType &updates);
    void messagesForwardPhoto_slt(qint64 id, const UpdatesType &updates);
    void messagesForwardVideo_slt(qint64 id, const UpdatesType &updates);
    void messagesForwardAudio_slt(qint64 id, const UpdatesType &updates);
    void messagesForwardDocument_slt(qint64 id, const UpdatesType &updates);
    void messagesGetDialogs_slt(qint64 id, qint32 sliceCount, const QList<Dialog> & dialogs, const QList<Message> & messages, const QList<Chat> & chats, const QList<User> & users);
    void messagesGetHistory_slt(qint64 id, qint32 sliceCount, const QList<Message> & messages, const QList<Chat> & chats, const QList<User> & users);
    void messagesReadHistory_slt(qint64 id, qint32 pts, qint32 pts_count, qint32 offset);
    void messagesReadEncryptedHistory_slt(qint64 id, bool ok);
    void messagesDeleteHistory_slt(qint64 id, qint32 pts, qint32 seq, qint32 offset);

    void messagesSearch_slt(qint64 id, qint32 sliceCount, const QList<Message> & messages, const QList<Chat> & chats, const QList<User> & users);

    void messagesGetFullChat_slt(qint64 id, const ChatFull & chatFull, const QList<Chat> & chats, const QList<User> & users);
    void messagesCreateChat_slt(qint64 id, const UpdatesType &updates);
    void messagesEditChatTitle_slt(qint64 id, const UpdatesType &updates);
    void messagesEditChatPhoto_slt(qint64 id, const UpdatesType &updates);
    void messagesAddChatUser_slt(qint64 id, const UpdatesType &updates);
    void messagesDeleteChatUser_slt(qint64 id, const UpdatesType &updates);

    void messagesCreateEncryptedChat_slt(qint32 chatId, qint32 date, qint32 peerId, qint64 accessHash);
    void messagesEncryptedChatRequested_slt(qint32 chatId, qint32 date, qint32 creatorId, qint64 creatorAccessHash);
    void messagesEncryptedChatCreated_slt(qint32 chatId);
    void messagesEncryptedChatDiscarded_slt(qint32 chatId);
    void messagesSendEncrypted_slt(qint64 id, qint32 date, const EncryptedFile &encryptedFile);
    void messagesSendEncryptedFile_slt(qint64 id, qint32 date, const EncryptedFile &encryptedFile);

    void messagesGetStickers_slt(qint64 msgId, const MessagesStickers &stickers);
    void messagesGetAllStickers_slt(qint64 msgId, const MessagesAllStickers &stickers);
    void messagesGetStickerSet_slt(qint64 msgId, const MessagesStickerSet &stickerset);
    void messagesInstallStickerSet_slt(qint64 msgId, bool ok);
    void messagesUninstallStickerSet_slt(qint64 msgId, bool ok);

    void updatesTooLong_slt();
    void updateShortMessage_slt(qint32 id, qint32 userId, QString message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShortChatMessage_slt(qint32 id, qint32 fromId, qint32 chatId, QString message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShort_slt(const Update & update, qint32 date);
    void updatesCombined_slt(const QList<Update> & updates, const QList<User> & users, const QList<Chat> & chats, qint32 date, qint32 seqStart, qint32 seq);
    void updates_slt(const QList<Update> & udts, const QList<User> & users, const QList<Chat> & chats, qint32 date, qint32 seq);
    void updateSecretChatMessage_slt(const SecretChatMessage &secretChatMessage, qint32 qts);
    void updatesGetDifference_slt(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    void updatesGetState_slt(qint64 id, qint32 pts, qint32 qts, qint32 date, qint32 seq, qint32 unreadCount);

    void uploadGetFile_slt(qint64 id, const StorageFileType & type, qint32 mtime, const QByteArray & bytes, qint32 partId, qint32 downloaded, qint32 total);
    void uploadSendFile_slt(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize);
    void uploadCancelFile_slt(qint64 fileId, bool cancelled);

    void fatalError_slt();

    void incomingAsemanMessage(const Message &msg, const Dialog &dialog);

private:
    void insertDialog(const Dialog & dialog , bool encrypted = false, bool fromDb = false);
    void insertMessage(const Message & message , bool encrypted = false, bool fromDb = false, bool tempMsg = false);
    void insertUser( const User & user, bool fromDb = false );
    void insertChat( const Chat & chat, bool fromDb = false );
    void insertStickerSet(const StickerSet &set, bool fromDb = false);
    void insertStickerPack(const StickerPack &pack, bool fromDb = false);
    void insertDocument(const Document &doc, bool fromDb = false);
    void insertUpdates(const UpdatesType &updates);
    void insertUpdate( const Update & update );
    void insertContact(const Contact & contact , bool fromDb = false);
    void insertEncryptedMessage(const EncryptedMessage & emsg);
    void insertEncryptedChat(const EncryptedChat & c);
    void insertSecretChatMessage(const SecretChatMessage & sc, bool cachedMsg = false);
    PhotoSize insertCachedPhotoSize(const PhotoSize &photo);
    void deleteLocalHistory(qint64 peerId);
    void blockUser(qint64 userId);
    void unblockUser(qint64 userId);

    QString fileLocation_old( FileLocationObject *location );
    QString fileLocation_old2( FileLocationObject *location );

    static QString localFilesPrePath();
    static bool createAudioThumbnail(const QString &audio, const QString &output);
    QString publicKeyPath() const;

protected:
    void timerEvent(QTimerEvent *e);
    Message newMessage(qint64 dId);
    SecretChat *getSecretChat(qint64 chatId);

    void startGarbageChecker();
    void insertToGarbeges(QObject *obj);

private Q_SLOTS:
    void dbUserFounded(const User &user);
    void dbChatFounded(const Chat &chat);
    void dbDialogFounded(const Dialog &dialog, bool encrypted);
    void dbContactFounded(const Contact &contact);
    void dbMessageFounded(const Message &message);
    void dbMediaKeysFounded(qint64 mediaId, const QByteArray &key, const QByteArray &iv);

    void refreshUnreadCount();
    void refreshTotalUploadedPercent();
    void refreshSecretChats();
    void updateEncryptedTopMessage(const Message &message);
    void getMyUser();

    InputPeer::InputPeerType getInputPeerType(qint64 pid);
    Peer::PeerType getPeerType(qint64 pid);

    QStringList stringToIndex(const QString & str);

    void objectDestroyed(QObject *obj);
    void cleanUpMessages_prv();

    bool requestReadMessage(qint32 msgId);
    void requestReadMessage_prv();

    static void removeFiles(const QString &dir);

private:
    TelegramQmlPrivate *p;
};

Q_DECLARE_METATYPE(TelegramQml*)

#endif // TELEGRAMQML_H
