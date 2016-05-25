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
#include <QSettings>

#include "libqtelegram_global.h"
#include "telegram/types/types.h"
#include "telegram/customtypes/customtypes.h"
#include "secret/secretchat.h"
#include "secret/secretchatmessage.h"
#include "telegram/telegramcore.h"
#include "core/settings.h"

Q_DECLARE_LOGGING_CATEGORY(TG_LIB_API)
Q_DECLARE_LOGGING_CATEGORY(TG_LIB_SECRET)

#define TG_UPLOAD_GET_FILE_CUSTOM_CALLBACK \
    TG_CALLBACK_SIGNATURE(UploadGetFile)
#define TG_UPLOAD_SEND_FILE_CUSTOM_CALLBACK \
    TG_CALLBACK_SIGNATURE(UploadSendFile)
#define TG_UPLOAD_SEND_PHOTO_CUSTOM_CALLBACK \
    TG_CALLBACK_SIGNATURE(UploadSendPhoto)

class CryptoUtils;
class TelegramPrivate;
class FileOperation;
class LIBQTELEGRAMSHARED_EXPORT Telegram : public TelegramCore
{
    Q_OBJECT
    friend class TelegramPrivate;

public:
    Telegram(const QString &defaultHostAddress, qint16 defaultHostPort, qint16 defaultHostDcId, qint32 appId, const QString &appHash,
             const QString &phoneNumber, const QString &configPath = QString("~/.telegram"), const QString &publicKeyFile = QString("tg.pub"));
    ~Telegram();

    QString defaultHostAddress();
    qint16 defaultHostPort();
    qint16 defaultHostDcId();
    qint32 appId();
    QString appHash();
    QString phoneNumber() const;
    QString configPath() const;

    Settings *settings() const;
    CryptoUtils *crypto() const;

    void setAuthConfigMethods(Settings::ReadFunc readFunc, Settings::WriteFunc writeFunc);

    static void setDefaultSettingsFormat(const QSettings::Format &format);
    static QSettings::Format defaultSettingsFormat();

    // Registration / authorization
    qint64 authCheckPhone(Callback<AuthCheckedPhone > callBack = 0, qint32 timeout = timeOut());
    qint64 authSendCode(Callback<AuthSentCode > callBack = 0, qint32 timeout = timeOut());
    qint64 authSignIn(const QString &code, Callback<AuthAuthorization > callBack = 0, qint32 timeout = timeOut());
    qint64 authSignUp(const QString &code, const QString &firstName, const QString &lastName, Callback<AuthAuthorization > callBack = 0, qint32 timeout = timeOut());
    qint64 authSendCall(Callback<AuthSentCode > callBack = 0, qint32 timeout = timeOut());
    qint64 authCheckPassword(const QByteArray &passwordData, Callback<AuthAuthorization > callBack = 0, qint32 timeout = timeOut());

    // Working with push notifications. Settings
    qint64 accountRegisterDevice(const QString &token, const QString &appVersion = QString::null, bool appSandbox = false, Callback<bool> callBack = 0, qint32 timeout = timeOut());
    qint64 accountUnregisterDevice(const QString &token, Callback<bool > callBack = 0, qint32 timeout = timeOut());

    //Photos
    qint64 photosUploadProfilePhoto(const QByteArray &bytes, const QString &fileName, const QString &caption = QString::null, const InputGeoPoint &geoPoint = InputGeoPoint(InputGeoPoint::typeInputGeoPointEmpty), const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto), Callback<UploadSendPhoto> callBack = 0, qint32 timeout = timeOut());
    qint64 photosUploadProfilePhoto(const QString &filePath, const QString &caption = QString::null, const InputGeoPoint &geoPoint = InputGeoPoint(InputGeoPoint::typeInputGeoPointEmpty), const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto), Callback<UploadSendPhoto> callBack = 0, qint32 timeout = timeOut());

    // Working with contacts
    qint64 contactsGetContacts(Callback<ContactsContacts > callBack = 0, qint32 timeout = timeOut());

    // Working with messages
    qint64 messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendGeoPoint(const InputPeer &peer, qint64 randomId, const InputGeoPoint &InputGeoPoint, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendContact(const InputPeer &peer, qint64 randomId, const QString &phoneNumber, const QString &firstName, const QString &lastName, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendVideo(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, qint32 width, qint32 height, const QString &mimeType, const QByteArray &thumbnailBytes = 0, const QString &thumbnailName = QString::null, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendVideo(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QString &thumbnailFilePath = QString::null, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendAudio(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, const QString &mimeType, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendAudio(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendDocument(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, const QString &mimeType, const QByteArray &thumbnailBytes = 0, const QString &thumbnailName = QString::null, const QList<DocumentAttribute> &extraAttributes = QList<DocumentAttribute>(), qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendDocument(const InputPeer &peer, qint64 randomId, const QString &filePath, const QString &thumbnailFilePath = QString::null, bool sendAsSticker = false, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UploadSendFile > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesForwardPhoto(const InputPeer &peer, qint64 randomId, qint64 photoId, qint64 accessHash, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesForwardVideo(const InputPeer &peer, qint64 randomId, qint64 videoId, qint64 accessHash, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesForwardAudio(const InputPeer &peer, qint64 randomId, qint64 audioId, qint64 accessHash, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesForwardDocument(const InputPeer &peer, qint64 randomId, qint64 documentId, qint64 accessHash, qint32 replyToMsgId = 0, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());

    // Working with chats
    qint64 messagesEditChatPhoto(qint32 chatId, const QString &filePath, const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto), Callback<UploadSendFile> callBack = 0, qint32 timeout = timeOut());
    qint64 messagesEditChatPhoto(qint32 chatId, qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop = InputPhotoCrop(InputPhotoCrop::typeInputPhotoCropAuto), Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());

    // Working with secret chats
    qint64 messagesCreateEncryptedChat(const InputUser &user, Callback<EncryptedChat > callBack = 0, qint32 timeout = timeOut()); //this method involves all DH key exchange and encrypted chat creation
    qint64 messagesAcceptEncryptedChat(qint32 chatId, Callback<EncryptedChat > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesDiscardEncryptedChat(qint32 chatId, Callback<bool > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSetEncryptedTyping(qint32 chatId, bool typing, Callback<bool > callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSetEncryptedTTL(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, Callback<UploadSendEncrypted> callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendEncrypted(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &text, Callback<UploadSendEncrypted> callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendEncryptedPhoto(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &filePath, Callback<UploadSendEncrypted> callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendEncryptedVideo(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QByteArray &thumbnailBytes, Callback<UploadSendEncrypted> callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendEncryptedDocument(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &filePath, Callback<UploadSendEncrypted> callBack = 0, qint32 timeout = timeOut());
    qint64 messagesSendEncryptedService(const InputEncryptedChat &chat, qint64 randomId, const DecryptedMessageAction &action, Callback<UploadSendEncrypted> callBack = 0, qint32 timeout = timeOut());//needed?

    // Working with files
    qint64 uploadGetFile(const InputFileLocation &file, qint32 fileSize, qint32 dc = 0, Callback<UploadGetFile> callBack = 0, qint32 timeout = timeOut());
    qint64 uploadGetFile(const InputFileLocation &file, qint32 fileSize, qint32 dc = 0, const QByteArray &key = QByteArray(), const QByteArray &iv = QByteArray(), Callback<UploadGetFile> callBack = 0, qint32 timeout = timeOut());
    qint64 uploadCancelFile(qint64 fileId);

    // additional public methods
    void init(qint32 timeout);
    void init();
    bool isConnected();
    bool isLoggedIn();
    qint32 ourId();
    void setPhoneNumber(const QString &phoneNumber);

    // sleep / wake
    bool sleep();
    bool wake();
    bool isSlept() const;

Q_SIGNALS:
    // Errors
    void error(qint64 id, qint32 errorCode, const QString &errorText, const QString &functionName = QString());

    // Registration / authorization
    void authNeeded();
    void authLoggedIn();

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

    // Working with updates
    void updatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);

    // Working with files
    void uploadGetFileAnswer(qint64 fileId, const UploadGetFile &result);
    void uploadCancelFileAnswer(qint64 fileId, bool cancelled);
    void uploadSendFileAnswer(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize);

    void updateSecretChatMessage(const SecretChatMessage &secretChatMessage, qint32 qts);
    void updates(const UpdatesType &update);

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

private:
    qint64 messagesForwardMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId, const ReplyMarkup &reply_markup = ReplyMarkup::null, bool broadcast = false, bool silent = false, bool background = false, Callback<UpdatesType > callBack = 0, qint32 timeout = timeOut());
    qint64 uploadSendFile(FileOperation &op, int mediaType, const QString &fileName, const QByteArray &bytes, const QByteArray &thumbnailBytes = 0, const QString &thumbnailName = QString::null);
    qint64 uploadSendFile(FileOperation &op, int mediaType, const QString &filePath, const QString &thumbnailPath = QString::null);
    void processSecretChatUpdate(const Update &update);
    qint64 generateGAorB(SecretChat *secretChat, Callback<EncryptedChat> callBack = 0, qint32 timeout = timeOut());
    void createSharedKey(SecretChat * secretChat, BIGNUM *p, QByteArray gAOrB);
    SecretChatMessage toSecretChatMessage(const EncryptedMessage &encryptedMessage);
    void processDifferences(qint64 id, const QList<Message> &messages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    void authorizeUser(qint64 id, const User &user);
    void messagesDhConfigNotModified(qint64 msgId, const QByteArray &random, Callback<EncryptedChat> callBack);

protected:
    void onAuthSendCodeAnswer(qint64 msgId, const AuthSentCode &result, const QVariant &attachedData);
    void onAuthSignUpAnswer(qint64 msgId, const AuthAuthorization &result, const QVariant &attachedData);
    void onAuthSignInAnswer(qint64 msgId, const AuthAuthorization &result, const QVariant &attachedData);
    void onAuthCheckPasswordAnswer(qint64 msgId, const AuthAuthorization &result, const QVariant &attachedData);
    void onAuthLogOutAnswer(qint64 id, bool ok, const QVariant &attachedData);
    void onContactsGetContactsAnswer(qint64 msgId, const ContactsContacts &result, const QVariant &attachedData);
    void onMessagesGetDhConfigAnswer(qint64 msgId, const MessagesDhConfig &result, const QVariant &attachedData);
    void onUpdatesGetDifferenceAnswer(qint64 msgId, const UpdatesDifference &result, const QVariant &attachedData);
    void onMessagesAcceptEncryptionAnswer(qint64 msgId, const EncryptedChat &result, const QVariant &attachedData);
    void onMessagesDiscardEncryptionAnswer(qint64 msgId, bool result, const QVariant &attachedData);

private Q_SLOTS:
    void onError(qint64 id, qint32 errorCode, const QString &errorText, const QString &functionName, const QVariant &attachedData, bool &accepted);
    void onDcProviderReady();
    void onAuthLoggedIn();
    void onMainDcChanged(DC *dc);
    void onSequenceNumberGap(qint32 chatId, qint32 startSeqNo, qint32 endSeqNo);

    // secret chats slots
    void onUpdates(const UpdatesType &upds);

    void onUploadGetFileAnswer(qint64 fileId, const UploadGetFile &result);
    void onUploadSendFileAnswer(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize);

protected:
    void timerEvent(QTimerEvent *e);

private:
    TelegramPrivate *prv;
};

Q_DECLARE_METATYPE(Telegram*)

#endif // TELEGRAM_H
