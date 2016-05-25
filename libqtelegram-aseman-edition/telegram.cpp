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

#include "telegram.h"

#include <exception>
#include <stdexcept>
#include <openssl/sha.h>
#include <openssl/md5.h>

#include <QDebug>
#include <QLoggingCategory>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QtEndian>
#include <QImage>
#include <QImageReader>
#include <QCryptographicHash>

#include "util/tlvalues.h"
#include "telegram/types/types.h"
#include "core/dcprovider.h"
#include "telegram/telegramapi.h"
#include "secret/secretstate.h"
#include "secret/encrypter.h"
#include "secret/decrypter.h"
#include "secret/secretchatmessage.h"
#include "secret/decryptedmessagebuilder.h"
#include "file/filehandler.h"
#include "core/dcprovider.h"
#include "telegram/coretypes.h"

Q_LOGGING_CATEGORY(TG_TELEGRAM, "tg.telegram")
Q_LOGGING_CATEGORY(TG_LIB_SECRET, "tg.lib.secret")

#define CHECK_API if(!mApi) {qDebug() << __FUNCTION__ << "Error: API is not ready."; return 0;}

QSettings::Format qtelegram_default_settings_format = QSettings::IniFormat;

class TelegramPrivate
{
public:
    TelegramPrivate() :
        mLibraryState(Telegram::LoggedOut),
        mSlept(false),
        mSecretState(0) {}

    Telegram::LibraryState mLibraryState;
    QList<qint64> mLastRetryMessages;

    bool mSlept;
    int initTimeout;
    int initTimerId;

    QString configPath;
    QString phoneNumber;
    QString publicKeyFile;
    Settings *mSettings;
    CryptoUtils *mCrypto;

    QPointer<DcProvider> mDcProvider;
    FileHandler::Ptr mFileHandler;

    QString m_phoneCodeHash;

    // cached contacts
    QList<Contact> m_cachedContacts;
    QList<User> m_cachedUsers;
    QHash<qint64, int> pendingForwards;
    QHash<qint64, int> pendingMediaSends;

    // encrypted chats
    SecretState mSecretState;
    Encrypter *mEncrypter;
    Decrypter *mDecrypter;
    QHash<qint64, TelegramCore::Callback<EncryptedChat> > secretChatCallbacks;

    bool mLoggedIn;
    bool mCreatedSharedKeys;

    QString appHash;
    qint32 appId;

    QString defaultHostAddress;
    qint16 defaultHostPort;
    qint16 defaultHostDcId;

    Settings::ReadFunc telegram_settings_read_fnc;
    Settings::WriteFunc telegram_settings_write_fnc;
};

Telegram::Telegram(const QString &defaultHostAddress, qint16 defaultHostPort, qint16 defaultHostDcId, qint32 appId, const QString &appHash, const QString &phoneNumber, const QString &configPath, const QString &publicKeyFile) :
    TelegramCore()
{
    // Qt5.2 doesn't support .ini files to control logging, so use this
    // manual workaround instead.
    // http://blog.qt.digia.com/blog/2014/03/11/qt-weekly-1-categorized-logging/
    prv = new TelegramPrivate;
    prv->initTimeout = 0;
    prv->initTimerId = 0;
    prv->configPath = configPath;
    prv->phoneNumber = phoneNumber;
    prv->publicKeyFile = publicKeyFile;
    prv->appHash = appHash;
    prv->appId = appId;
    prv->defaultHostAddress = defaultHostAddress;
    prv->defaultHostPort = defaultHostPort;
    prv->defaultHostDcId = defaultHostDcId;

    prv->mEncrypter = 0;
    prv->mDecrypter = 0;
    prv->mDcProvider = 0;
    prv->mCrypto = 0;
    prv->mSettings = 0;

    prv->telegram_settings_read_fnc = 0;
    prv->telegram_settings_write_fnc = 0;

    QLoggingCategory::setFilterRules(QString(qgetenv("QT_LOGGING_RULES")));
}

bool Telegram::sleep() {
    // sleep only if not slept and library already logged in. Returns true if sleep operations completes
    if (!prv->mSlept && prv->mLibraryState >= LoggedIn) {
        if (mApi && mApi->mainSession()) {
            mApi->mainSession()->close();
        }
        prv->mSlept = true;
        return true;
    }
    return false;
}

bool Telegram::wake() {
    // wake only if slept and library already logged in. Returns true if wake operation completes
    if (prv->mSlept && prv->mDcProvider && prv->mLibraryState >= LoggedIn) {
        connect(mApi.data(), &TelegramApi::mainSessionReady, this, &Telegram::woken, Qt::UniqueConnection);
        DC *dc = prv->mDcProvider->getWorkingDc();
        mApi->createMainSessionToDc(dc);
        prv->mSlept = false;
        return true;
    }
    return false;
}

bool Telegram::isSlept() const
{
    return prv->mSlept;
}

void Telegram::setPhoneNumber(const QString &phoneNumber) {
    if (!prv->mSettings->loadSettings(phoneNumber)) {
        throw std::runtime_error("setPhoneNumber: could not load settings");
    }
    prv->mSecretState.load();
    prv->phoneNumber = phoneNumber;
}

void Telegram::init() {
    init(timeOut());
}

void Telegram::init(qint32 timeout) {
    // check the auth values stored in settings, check the available DCs config data if there is
    // connection to servers, and emit signals depending on user authenticated or not.
    if(mApi)
        return;

    if(prv->mEncrypter) delete prv->mEncrypter;
    if(prv->mDecrypter) delete prv->mDecrypter;
    if(prv->mDcProvider) delete prv->mDcProvider;
    if(prv->mCrypto) delete prv->mCrypto;
    if(prv->mSettings) delete prv->mSettings;

    prv->mSettings = new Settings(this);
    prv->mSettings->setAppHash(prv->appHash);
    prv->mSettings->setAppId(prv->appId);
    prv->mSettings->setDefaultHostAddress(prv->defaultHostAddress);
    prv->mSettings->setDefaultHostDcId(prv->defaultHostDcId);
    prv->mSettings->setDefaultHostPort(prv->defaultHostPort);
    if(prv->telegram_settings_read_fnc && prv->telegram_settings_write_fnc)
        prv->mSettings->setAuthConfigMethods(prv->telegram_settings_read_fnc, prv->telegram_settings_write_fnc);

    // load settings
    if (!prv->mSettings->loadSettings(prv->phoneNumber, prv->configPath, prv->publicKeyFile)) {
        throw std::runtime_error("loadSettings failure");
    }

    prv->mCrypto = new CryptoUtils(prv->mSettings);

    prv->mDcProvider = new DcProvider(prv->mSettings, prv->mCrypto);
    prv->mDcProvider->setParent(this);

    connect(prv->mDcProvider.data(), &DcProvider::fatalError, this, &Telegram::fatalError);
    // activate dc provider ready signal
    connect(prv->mDcProvider.data(), &DcProvider::dcProviderReady, this, &Telegram::onDcProviderReady);
    // activate rest of dc provider signal connections
    connect(prv->mDcProvider.data(), &DcProvider::authNeeded, this, &Telegram::authNeeded);
    connect(prv->mDcProvider.data(), &DcProvider::authTransferCompleted, this, &Telegram::onAuthLoggedIn);
    connect(prv->mDcProvider.data(), &DcProvider::error, this, &Telegram::error);

    prv->mSecretState = SecretState(prv->mSettings);
    prv->mEncrypter = new Encrypter(prv->mSettings);
    prv->mDecrypter = new Decrypter(prv->mSettings);

    connect(prv->mDecrypter, &Decrypter::sequenceNumberGap,
            this, &Telegram::onSequenceNumberGap);

    prv->mSecretState.load();
    prv->mDcProvider->initialize();

    prv->initTimeout = timeout;
    prv->initTimerId = startTimer(prv->initTimeout);
}

Telegram::~Telegram() {
    if(prv->mEncrypter) delete prv->mEncrypter;
    if(prv->mDecrypter) delete prv->mDecrypter;
    if(prv->mDcProvider) delete prv->mDcProvider;
    if(prv->mCrypto) delete prv->mCrypto;
    if(prv->mSettings) delete prv->mSettings;
    delete prv;
}

QString Telegram::defaultHostAddress()
{
    return prv->mSettings->defaultHostAddress();
}

qint16 Telegram::defaultHostPort()
{
    return prv->mSettings->defaultHostPort();
}

qint16 Telegram::defaultHostDcId()
{
    return prv->mSettings->defaultHostDcId();
}

qint32 Telegram::appId()
{
    return prv->mSettings->appId();
}

QString Telegram::appHash()
{
    return prv->mSettings->appHash();
}

QString Telegram::phoneNumber() const
{
    return prv->phoneNumber;
}

QString Telegram::configPath() const
{
    return prv->configPath;
}

Settings *Telegram::settings() const
{
    return prv->mSettings;
}

CryptoUtils *Telegram::crypto() const
{
    return prv->mCrypto;
}

void Telegram::setAuthConfigMethods(Settings::ReadFunc readFunc, Settings::WriteFunc writeFunc)
{
    prv->telegram_settings_read_fnc = readFunc;
    prv->telegram_settings_write_fnc = writeFunc;
}

void Telegram::setDefaultSettingsFormat(const QSettings::Format &format)
{
    qtelegram_default_settings_format = format;
}

QSettings::Format Telegram::defaultSettingsFormat()
{
    return qtelegram_default_settings_format;
}

bool Telegram::isConnected() {
    if (mApi && mApi->mainSession()) {
        return mApi->mainSession()->state() == QAbstractSocket::ConnectedState;
    }
    return false;
}

bool Telegram::isLoggedIn() {
    return prv->mLibraryState == LoggedIn;
}

void Telegram::onAuthLoggedIn() {
    prv->mLibraryState = LoggedIn;
    Q_EMIT authLoggedIn();
}

void Telegram::onMainDcChanged(DC *dc)
{
    Q_UNUSED(dc)
    while(!prv->mLastRetryMessages.isEmpty())
        retry(prv->mLastRetryMessages.takeFirst());
}

void Telegram::onAuthLogOutAnswer(qint64 id, bool result, const QVariant &attachedData) {
    prv->mDcProvider->logOut();
    prv->mLibraryState = LoggedOut;
    TelegramCore::onAuthLogOutAnswer(id, result, attachedData);
}

qint32 Telegram::ourId() {
    return prv->mSettings->ourId();
}

void Telegram::onDcProviderReady() {
    prv->mLibraryState = CreatedSharedKeys;
    setApi( prv->mDcProvider->getApi() );
    // api signal-signal and signal-slot connections
    // errors
    connect(mApi.data(), &TelegramApi::fatalError, this, &Telegram::fatalError);

    // updates
    connect(mApi.data(), &TelegramApi::updates, this, &Telegram::onUpdates);
    // logic additional signal slots
    connect(mApi.data(), &TelegramApi::mainSessionDcChanged, this, &Telegram::onMainDcChanged, Qt::QueuedConnection);
    connect(mApi.data(), &TelegramApi::mainSessionReady, this, &Telegram::connected);
    connect(mApi.data(), &TelegramApi::mainSessionClosed, this, &Telegram::disconnected);

    prv->mFileHandler = FileHandler::Ptr(new FileHandler(this, mApi, prv->mCrypto, prv->mSettings, *prv->mDcProvider, prv->mSecretState));
    connect(prv->mFileHandler.data(), &FileHandler::uploadSendFileAnswer, this, &Telegram::onUploadSendFileAnswer);
    connect(prv->mFileHandler.data(), &FileHandler::uploadGetFileAnswer, this, &Telegram::onUploadGetFileAnswer);
    connect(prv->mFileHandler.data(), &FileHandler::uploadCancelFileAnswer, this, &Telegram::uploadCancelFileAnswer);
    connect(prv->mFileHandler.data(), &FileHandler::error, this, &Telegram::error);
    connect(prv->mFileHandler.data(), &FileHandler::messagesSentMedia, this, &Telegram::onMessagesSendMediaAnswer);
    connect(prv->mFileHandler.data(), &FileHandler::messagesSendEncryptedFileAnswer, this, &Telegram::messagesSendEncryptedFileAnswer);

    // At this point we should test the main session state and emit by hand signals of connected/disconnected
    // depending on the connection state of the session. This is so because first main session connection, if done,
    // is performed before we could connect the signal-slots to advise about it;
    if (mApi->mainSession()->state() == QAbstractSocket::ConnectedState) {
        Q_EMIT connected();
    } else {
        Q_EMIT disconnected();
    }
}

qint64 Telegram::messagesCreateEncryptedChat(const InputUser &user, Callback<EncryptedChat> callBack, qint32 timeout) {
    qCDebug(TG_LIB_SECRET) << "creating new encrypted chat";
    // generate a new object where store all the needed secret chat data
    SecretChat *secretChat = new SecretChat(prv->mSettings);
    secretChat->setAdminId(prv->mSettings->ourId());
    secretChat->setParticipantId(user.userId());
    secretChat->setRequestedUser(user);
    return generateGAorB(secretChat, callBack, timeout);
}

qint64 Telegram::messagesAcceptEncryptedChat(qint32 chatId, Callback<EncryptedChat> callBack, qint32 timeout) {
    qCDebug(TG_LIB_SECRET) << "accepting requested encrypted chat with id" << chatId;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);

    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Not found any chat related to" << chatId;
        return -1;
    }
    return generateGAorB(secretChat, callBack, timeout);
}

qint64 Telegram::messagesDiscardEncryptedChat(qint32 chatId, Callback<bool> callBack, qint32 timeout) {
    qCDebug(TG_LIB_SECRET) << "discarding encrypted chat with id" << chatId;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Trying to discard a not existant chat";
        if(callBack) callBack(0, true, CallbackError());
        return -1;
    }

    qint64 requestId = TelegramCore::messagesDiscardEncryption(chatId, callBack, timeout);
    // insert another entry related to this request for deleting chat only when response is ok
    prv->mSecretState.chats().insert(requestId, secretChat);
    return requestId;
}

qint64 Telegram::messagesSetEncryptedTTL(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, Callback<UploadSendEncrypted> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.chatId());
    if(!secretChat)
    {
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        return 0;
    }

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForTtl(randomId, ttl);

    prv->mEncrypter->setSecretChat(secretChat);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    qint64 requestId = TelegramCore::messagesSendEncrypted(chat, randomId, data, [this, callBack](TG_MESSAGES_SEND_ENCRYPTED_CALLBACK){
        UploadSendEncrypted use(error.null? UploadSendEncrypted::typeUploadSendEncryptedFinished :
                                            UploadSendEncrypted::typeUploadSendEncryptedCanceled);
        use.setMessage(result);
        callBack(msgId, use, error);
    }, timeout);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();

    return requestId;
}

qint64 Telegram::messagesSendEncrypted(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &text, Callback<UploadSendEncrypted> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.chatId());
    if(!secretChat)
    {
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        return 0;
    }

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForSendMessage(randomId, ttl, text);

    prv->mEncrypter->setSecretChat(secretChat);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    qint64 request = TelegramCore::messagesSendEncrypted(chat, randomId, data, [this, callBack](TG_MESSAGES_SEND_ENCRYPTED_CALLBACK){
        UploadSendEncrypted use(error.null? UploadSendEncrypted::typeUploadSendEncryptedFinished :
                                            UploadSendEncrypted::typeUploadSendEncryptedCanceled);
        use.setMessage(result);
        callBack(msgId, use, error);
    }, timeout);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();

    return request;
}

void Telegram::onSequenceNumberGap(qint32 chatId, qint32 startSeqNo, qint32 endSeqNo) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    ASSERT(secretChat);

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    qint64 randomId;
    Utils::randomBytes(&randomId, 8);

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForResend(randomId, startSeqNo, endSeqNo);

    prv->mEncrypter->setSecretChat(secretChat);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    QList<qint64> previousMsgs = secretChat->sequence();
    TelegramCore::messagesSendEncrypted(inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();
}

qint64 Telegram::messagesSendEncryptedPhoto(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &filePath, Callback<UploadSendEncrypted> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.chatId());
    if(!secretChat)
    {
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        return 0;
    }

    FileOperation *op = new FileOperation(FileOperation::sendEncryptedFile);
    op->setInputEncryptedChat(chat);
    op->setRandomId(randomId);
    op->initializeKeyAndIv();
    op->setResultCallback<UploadSendEncrypted>(callBack);
    op->setTimeOut(timeout);
    QByteArray key = op->key();
    QByteArray iv = op->iv();

    QFileInfo fileInfo(filePath);
    qint32 size = fileInfo.size();

    QImage image;
    image.load(filePath);
    qint32 width = image.width();
    qint32 height = image.height();

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForSendPhoto(randomId, ttl, key, iv, size, width, height);
    op->setDecryptedMessage(decryptedMessage);

    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendEncryptedVideo(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QByteArray &thumbnailBytes, Callback<UploadSendEncrypted> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.chatId());
    if(!secretChat)
    {
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        return 0;
    }

    FileOperation *op = new FileOperation(FileOperation::sendEncryptedFile);
    op->setInputEncryptedChat(chat);
    op->setRandomId(randomId);
    op->initializeKeyAndIv();
    op->setResultCallback<UploadSendEncrypted>(callBack);
    op->setTimeOut(timeout);
    QByteArray key = op->key();
    QByteArray iv = op->iv();

    QFileInfo fileInfo(filePath);
    qint32 size = fileInfo.size();
    QString mimeType = QMimeDatabase().mimeTypeForFile(QFileInfo(filePath)).name();

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage =
            builder.buildDecryptedMessageForSendVideo(randomId, ttl, key, iv, size, mimeType, duration, width, height, thumbnailBytes);
    op->setDecryptedMessage(decryptedMessage);

    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendEncryptedDocument(const InputEncryptedChat &chat, qint64 randomId, qint32 ttl, const QString &filePath, Callback<UploadSendEncrypted> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.chatId());
    if(!secretChat)
    {
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        return 0;
    }

    FileOperation *op = new FileOperation(FileOperation::sendEncryptedFile);
    op->setInputEncryptedChat(chat);
    op->setRandomId(randomId);
    op->initializeKeyAndIv();
    op->setResultCallback<UploadSendEncrypted>(callBack);
    op->setTimeOut(timeout);
    QByteArray key = op->key();
    QByteArray iv = op->iv();

    QFileInfo fileInfo(filePath);
    qint32 size = fileInfo.size();
    QString fileName = fileInfo.fileName();
    QString mimeType = QMimeDatabase().mimeTypeForFile(filePath).name();

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForSendDocument(randomId, ttl, key, iv, size, fileName, mimeType);
    op->setDecryptedMessage(decryptedMessage);

    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendEncryptedService(const InputEncryptedChat &chat, qint64 randomId, const DecryptedMessageAction &action, Callback<UploadSendEncrypted> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.chatId());
    if(!secretChat)
    {
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        return 0;
    }

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForNotifyLayer(randomId, CoreTypes::typeLayerVersion);
    decryptedMessage.setAction(action);

    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    return TelegramCore::messagesSendEncryptedService(chat, randomId, data, [this, callBack](TG_MESSAGES_SEND_ENCRYPTED_CALLBACK){
        UploadSendEncrypted use(error.null? UploadSendEncrypted::typeUploadSendEncryptedFinished :
                                            UploadSendEncrypted::typeUploadSendEncryptedCanceled);
        use.setMessage(result);
        callBack(msgId, use, error);
    }, timeout);
}

qint64 Telegram::generateGAorB(SecretChat *secretChat, Callback<EncryptedChat> callBack, qint32 timeout) {
    qCDebug(TG_LIB_SECRET) << "requesting for DH config parameters";
    // call messages.getDhConfig to get p and g for start creating shared key
    qint64 reqId = messagesGetDhConfig(prv->mSecretState.version(), DH_CONFIG_SERVER_RANDOM_LENGTH, 0, timeout);
    // store in secret chats map related to this request id, temporally
    prv->mSecretState.chats().insert(reqId, secretChat);
    prv->secretChatCallbacks[reqId] = callBack;
    return reqId;
}

void Telegram::onMessagesGetDhConfigAnswer(qint64 msgId, const MessagesDhConfig &result, const QVariant &attachedData)
{
    qCDebug(TG_LIB_SECRET) << "received new DH parameters g ="<< result.g() << ",p =" << result.p().toBase64()
                           << ",version =" << result.version();

    Callback<EncryptedChat> callBack = prv->secretChatCallbacks.take(msgId);

    prv->mSecretState.setVersion(result.version());
    prv->mSecretState.setG(result.g());
    prv->mSecretState.setP(result.p());

    if (prv->mCrypto->checkDHParams(prv->mSecretState.p(), result.g()) < 0 &&
        result.classType() == MessagesDhConfig::typeMessagesDhConfig) {
        qCCritical(TG_TELEGRAM) << "Diffie-Hellman config parameters are not valid";

    } else {
        messagesDhConfigNotModified(msgId, result.random(), callBack);
    }

    TelegramCore::onMessagesGetDhConfigAnswer(msgId, result, attachedData);
}

void Telegram::messagesDhConfigNotModified(qint64 msgId, const QByteArray &random, Callback<EncryptedChat> callBack) {
    qCDebug(TG_LIB_SECRET) << "processing DH parameters";
    SecretChat *secretChat = prv->mSecretState.chats().take(msgId);
    ASSERT(secretChat);
    // create secret a number by taking server random (and generating a client random also to have more entrophy)
    secretChat->createMyKey(random);
    // create empty bignum where store the result of operation g^a mod p
    BIGNUM *r = BN_new();
    Utils::ensurePtr(r);
    // do the opeation -> r = g^a mod p
    Utils::ensure(prv->mCrypto->BNModExp(r, prv->mSecretState.g(), secretChat->myKey(), prv->mSecretState.p()));
    // check that g and r are greater than one and smaller than p-1. Also checking that r is between 2^{2048-64} and p - 2^{2048-64}
    if (prv->mCrypto->checkCalculatedParams(r, prv->mSecretState.g(), prv->mSecretState.p()) < 0) {
        qCCritical(TG_LIB_SECRET) << "gAOrB or g params are not valid";
        return;
    }
    // convert result to big endian before sending request encryption query
    uchar rawGAOrB[256];
    memset(rawGAOrB, 0, 256);
    BN_bn2bin(r, rawGAOrB);
    QByteArray gAOrB = QByteArray::fromRawData(reinterpret_cast<char*>(rawGAOrB), 256);

    switch (static_cast<qint32>(secretChat->state())) {
    case SecretChat::Init: {
        // generate randomId, used not only to request encryption but as chatId
        qint32 randomId;
        Utils::randomBytes(&randomId, 4);
        secretChat->setChatId(randomId);
        prv->mSecretState.chats().insert(randomId, secretChat);
        qCDebug(TG_LIB_SECRET) << "Requesting encryption for chatId" << secretChat->chatId();
        TelegramCore::messagesRequestEncryption(secretChat->requestedUser(), randomId, gAOrB, callBack);
        break;
    }
    case SecretChat::Requested: {
        QByteArray gA = secretChat->gAOrB();

        createSharedKey(secretChat, prv->mSecretState.p(), gA);
        qint64 keyFingerprint = secretChat->keyFingerprint();

        InputEncryptedChat inputEncryptedChat;
        inputEncryptedChat.setChatId(secretChat->chatId());
        inputEncryptedChat.setAccessHash(secretChat->accessHash());
        qCDebug(TG_LIB_SECRET) << "Accepting encryption for chatId" << secretChat->chatId();
        TelegramCore::messagesAcceptEncryption(inputEncryptedChat, gAOrB, keyFingerprint, callBack);
        break;
    }
    default:
        Q_ASSERT("Not handled");
        break;
    }

    BN_free(r);
    prv->mSecretState.save();
}

void Telegram::onUpdates(const UpdatesType &upds) {
    switch(static_cast<int>(upds.classType()))
    {
    case UpdatesType::typeUpdates:
    case UpdatesType::typeUpdatesCombined:
        Q_FOREACH (const Update &update, upds.updates()) {
            processSecretChatUpdate(update);
        }
        break;
    case UpdatesType::typeUpdateShort:
        processSecretChatUpdate(upds.update());
        break;
    }

    Q_EMIT updates(upds);
}

void Telegram::onUploadGetFileAnswer(qint64 fileId, const UploadGetFile &result)
{
    switch(static_cast<int>(result.classType()))
    {
    case UploadGetFile::typeUploadGetFileProgress:
    {
        Callback<UploadGetFile> callBack = callBackGet<UploadGetFile>(fileId);
        if(callBack)
            callBack(fileId, result, TelegramCore::CallbackError());
    }
        break;

    case UploadGetFile::typeUploadGetFileFinished:
    case UploadGetFile::typeUploadGetFileCanceled:
        callBackCall<UploadGetFile>(fileId, result, TelegramCore::CallbackError());
        break;
    }

    Q_EMIT uploadGetFileAnswer(fileId, result);
}

void Telegram::onUploadSendFileAnswer(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize)
{
    Q_EMIT uploadSendFileAnswer(fileId, partId, uploaded, totalSize);
}

void Telegram::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == prv->initTimerId)
    {
        killTimer(prv->initTimerId);
        prv->initTimerId = 0;
        if(!mApi)
        {
            qDebug() << "Timeout error initializing. Retrying...";
            init(prv->initTimeout);
        }
    }
    else
        TelegramCore::timerEvent(e);
}

SecretChatMessage Telegram::toSecretChatMessage(const EncryptedMessage &encrypted) {

    SecretChatMessage secretChatMessage;

    qint32 chatId = encrypted.chatId();
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);

    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "received encrypted message does not belong to any known secret chat";
        return secretChatMessage;
    }

    secretChatMessage.setChatId(chatId);
    secretChatMessage.setDate(encrypted.date());

    prv->mDecrypter->setSecretChat(secretChat);
    DecryptedMessage decrypted = prv->mDecrypter->decryptEncryptedData(encrypted.randomId(), encrypted.bytes());
    secretChatMessage.setDecryptedMessage(decrypted);

    // if having a not 0 randomId, the decrypted message is valid
    if (decrypted.randomId()) {

        EncryptedFile attachment = encrypted.file();

        //if attachment, check keyFingerprint
        if (attachment.classType() != EncryptedFile::typeEncryptedFileEmpty) {

            qint32 receivedKeyFingerprint = attachment.keyFingerprint();
            const QByteArray &key = decrypted.media().key();
            const QByteArray &iv = decrypted.media().iv();
            qint32 computedKeyFingerprint = prv->mCrypto->computeKeyFingerprint(key, iv);

            qCDebug(TG_LIB_SECRET) << "received keyFingerprint:" << receivedKeyFingerprint;
            qCDebug(TG_LIB_SECRET) << "computed keyFingerprint:" << computedKeyFingerprint;

            if (receivedKeyFingerprint != computedKeyFingerprint) {
                 qCWarning(TG_LIB_SECRET) << "Computed and received key fingerprints are not equals. Discarding message";
                 return secretChatMessage;
            }

            secretChatMessage.setAttachment(attachment);
        }

        prv->mSecretState.save();
    }

    return secretChatMessage;
}

void Telegram::processSecretChatUpdate(const Update &update) {
    switch (static_cast<qint32>(update.classType())) {
    case Update::typeUpdateNewEncryptedMessage: {
        EncryptedMessage encrypted = update.messageEncrypted();

        SecretChatMessage secretChatMessage = toSecretChatMessage(encrypted);

        // if having a not 0 randomId, the decrypted message is valid
        if (secretChatMessage.decryptedMessage().randomId()) {
            prv->mSecretState.save();
            qint32 qts = update.qts();
            Q_EMIT updateSecretChatMessage(secretChatMessage, qts);
        }

        break;
    }
    case Update::typeUpdateEncryption: {

        const EncryptedChat &encryptedChat = update.chat();
        qint32 chatId = encryptedChat.id();
        switch (static_cast<qint32>(encryptedChat.classType())) {
        case EncryptedChat::typeEncryptedChatRequested: {

            // here, we have received a request of creating a new secret chat. Emit a signal
            // with chat details for user B to accept or reject chat creation
            qint64 accessHash = encryptedChat.accessHash();
            qint32 date = encryptedChat.date();
            qint32 adminId = encryptedChat.adminId();
            qint32 participantId = encryptedChat.participantId();
            QByteArray gA = encryptedChat.gA();

            qCDebug(TG_LIB_SECRET) << "Requested secret chat creation:";
            qCDebug(TG_LIB_SECRET) << "chatId:" << chatId;
            qCDebug(TG_LIB_SECRET) << "date:" << date;
            qCDebug(TG_LIB_SECRET) << "adminId:" << adminId;
            qCDebug(TG_LIB_SECRET) << "participantId:" << participantId;
            qCDebug(TG_LIB_SECRET) << "gA:" << gA.toBase64();
            qCDebug(TG_LIB_SECRET) << "ourId:" << ourId();

            ASSERT(participantId == ourId());

            SecretChat* secretChat = new SecretChat(prv->mSettings);
            secretChat->setChatId(encryptedChat.id());
            secretChat->setAccessHash(encryptedChat.accessHash());
            secretChat->setDate(encryptedChat.date());
            secretChat->setAdminId(encryptedChat.adminId());
            secretChat->setParticipantId(encryptedChat.participantId());
            secretChat->setGAOrB(gA);
            secretChat->setState(SecretChat::Requested);

            prv->mSecretState.chats().insert(chatId, secretChat);
            Q_EMIT messagesEncryptedChatRequested(chatId, date, adminId, accessHash);
            break;
        }
        case EncryptedChat::typeEncryptedChat: {

            qCDebug(TG_LIB_SECRET) << "received encrypted chat creation update";
            // here, the request for encryption has been accepted. Take the secret chat data
            qint64 accessHash = encryptedChat.accessHash();
            qint32 date = encryptedChat.date();
            qint32 adminId = encryptedChat.adminId();
            qint32 participantId = encryptedChat.participantId();
            QByteArray gB = encryptedChat.gAOrB();
            qint64 keyFingerprint = encryptedChat.keyFingerprint();

            qCDebug(TG_LIB_SECRET) << "Peer accepted secret chat creation:";
            qCDebug(TG_LIB_SECRET) << "chatId:" << chatId;
            qCDebug(TG_LIB_SECRET) << "accessHash:" << accessHash;
            qCDebug(TG_LIB_SECRET) << "date:" << date;
            qCDebug(TG_LIB_SECRET) << "adminId:" << adminId;
            qCDebug(TG_LIB_SECRET) << "participantId:" << participantId;
            qCDebug(TG_LIB_SECRET) << "gB:" << gB.toBase64();
            qCDebug(TG_LIB_SECRET) << "received keyFingerprint:" << keyFingerprint;

            SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
            if (!secretChat) {
                qCWarning(TG_LIB_SECRET) << "Could not find secret chat with id" << chatId;
                return;
            }

            createSharedKey(secretChat, prv->mSecretState.p(), gB);

            qint64 calculatedKeyFingerprint = secretChat->keyFingerprint();
            qCDebug(TG_LIB_SECRET) << "calculated keyFingerprint:" << calculatedKeyFingerprint;

            if (calculatedKeyFingerprint == keyFingerprint) {
                qCDebug(TG_LIB_SECRET) << "Generated shared key for secret chat" << chatId;
                secretChat->setChatId(chatId);
                secretChat->setAccessHash(accessHash);
                secretChat->setDate(date);
                secretChat->setAdminId(adminId);
                secretChat->setParticipantId(participantId);
                secretChat->setState(SecretChat::Accepted);
                qCDebug(TG_LIB_SECRET) << "Joined to secret chat" << chatId << "with peer" << participantId;
                prv->mSecretState.save();
                Q_EMIT messagesEncryptedChatCreated(chatId, date, participantId, accessHash);

                //notify peer about our layer
                InputEncryptedChat inputEncryptedChat;
                inputEncryptedChat.setChatId(chatId);
                inputEncryptedChat.setAccessHash(accessHash);

                prv->mEncrypter->setSecretChat(secretChat);
                qint64 randomId;
                Utils::randomBytes(&randomId, 8);
                QList<qint64> previousMsgs = secretChat->sequence();
                DecryptedMessageBuilder builder(secretChat->layer());
                DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForNotifyLayer(randomId, CoreTypes::typeLayerVersion);
                QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
                TelegramCore::messagesSendEncryptedService(inputEncryptedChat, randomId, data);

                secretChat->increaseOutSeqNo();
                secretChat->appendToSequence(randomId);
                prv->mSecretState.save();

                qCDebug(TG_LIB_SECRET) << "Notified our layer:" << CoreTypes::typeLayerVersion;
            } else {
                qCCritical(TG_LIB_SECRET) << "Key fingerprint mismatch. Discarding encryption";
                messagesDiscardEncryptedChat(chatId);
            }
            break;
        }
        case EncryptedChat::typeEncryptedChatDiscarded: {
            qCDebug(TG_LIB_SECRET) << "Discarded chat" << chatId;
            SecretChat *secretChat = prv->mSecretState.chats().take(chatId);
            if (secretChat) {
                prv->mSecretState.save();
                delete secretChat;
                secretChat = 0;
            }
            Q_EMIT messagesEncryptedChatDiscarded(chatId);
            break;
        }
        case EncryptedChat::typeEncryptedChatWaiting: {
            qCDebug(TG_LIB_SECRET) << "Waiting for peer to accept chat" << chatId;

            if (encryptedChat.participantId() != ourId()) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
                qCritical(TG_LIB_SECRET()) << "Received request to create a secret chat is not for you!";
#endif
                return;
            }

            SecretChat* secretChat = prv->mSecretState.chats().value(chatId);
            if (secretChat) {
                secretChat->setState(SecretChat::Requested);
                qint32 date = encryptedChat.date();
                qint32 adminId = encryptedChat.adminId();
                qint64 accessHash = encryptedChat.accessHash();
                Q_EMIT messagesEncryptedChatRequested(chatId, date, adminId, accessHash);
            }
            break;
        }
        default:
            Q_ASSERT("Not handled");
            break;
        }
    }
    default:
        Q_ASSERT("Not handled");
        break;
    }
}

void Telegram::createSharedKey(SecretChat *secretChat, BIGNUM *p, QByteArray gAOrB) {
    // calculate the shared key by doing key = B^a mod p
    BIGNUM *myKey = secretChat->myKey();
    // padding of B (gAOrB) must have exactly 256 bytes. After pad, transform to bignum
    BIGNUM *bigNumGAOrB = Utils::padBytesAndGetBignum(gAOrB);

    // create empty bignum where store the result of operation g^a mod p
    BIGNUM *result = BN_new();
    Utils::ensurePtr(result);
    // do the opeation -> k = g_b^a mod p
    Utils::ensure(prv->mCrypto->BNModExp(result, bigNumGAOrB, myKey, p));

    // move r (BIGNUM) to shared key (char[]) array format
    uchar *sharedKey = secretChat->sharedKey();
    memset(sharedKey, 0, SHARED_KEY_LENGTH);
    BN_bn2bin(result, sharedKey + (SHARED_KEY_LENGTH - BN_num_bytes (result)));

    qint64 keyFingerprint = Utils::getKeyFingerprint(sharedKey);
    secretChat->setKeyFingerprint(keyFingerprint);

    BN_free(bigNumGAOrB);
}

// error and internal managements
void Telegram::onError(qint64 id, qint32 errorCode, const QString &errorText, const QString &functionName, const QVariant &attachedData, bool &accepted) {
    if (errorText.contains("_MIGRATE_"))
    {
        prv->mLastRetryMessages << id;
        qint32 newDc = errorText.mid(errorText.lastIndexOf("_") + 1).toInt();
        qDebug() << "migrated to dc" << newDc;
        prv->mSettings->setWorkingDcNum(newDc);
        prv->mSettings->writeAuthFile();
        DC *dc = prv->mDcProvider->getDc(newDc);
        mApi->changeMainSessionToDc(dc);
        accepted = true;
    }
    else
    if(errorCode == 400)
    {
        if(errorText == "ENCRYPTION_ALREADY_DECLINED") { /* This is an already declined chat, remove it from DB */
            onMessagesDiscardEncryptionAnswer(id, true, attachedData);
            return;
        }
    }
    else
    if (errorCode == 401)
    {
        if(errorText == "SESSION_PASSWORD_NEEDED")
            ; // Nothing to do
        else
            onAuthLogOutAnswer(id, false, attachedData);
    }

    TelegramCore::onError(id, errorCode, errorText, functionName, attachedData, accepted);
}


void Telegram::authorizeUser(qint64, const User &) {
    // change state of current dc
    qint32 workingDcNum = prv->mSettings->workingDcNum();
    DC *dc = prv->mDcProvider->getDc(workingDcNum);
    dc->setState(DC::userSignedIn);
    QList<DC *> dcsList = prv->mDcProvider->getDcs();
    // save the settings here, after user auth ready in current dc
    prv->mSettings->setDcsList(dcsList);
    prv->mSettings->writeAuthFile();
    // transfer current dc authorization to other dcs
    prv->mDcProvider->transferAuth();
}

void Telegram::onContactsGetContactsAnswer(qint64 msgId, const ContactsContacts &result, const QVariant &attachedData) {
    prv->m_cachedContacts = result.contacts();
    prv->m_cachedUsers = result.users();
    TelegramCore::onContactsGetContactsAnswer(msgId, result, attachedData);
}

// not direct Responses
void Telegram::onAuthSendCodeAnswer(qint64 msgId, const AuthSentCode &result, const QVariant &attachedData) {
    prv->m_phoneCodeHash = result.phoneCodeHash();
    TelegramCore::onAuthSendCodeAnswer(msgId, result, attachedData);
}

void Telegram::onAuthSignUpAnswer(qint64 msgId, const AuthAuthorization &result, const QVariant &attachedData)
{
    authorizeUser(msgId, result.user());
    TelegramCore::onAuthSignUpAnswer(msgId, result, attachedData);
}

void Telegram::onAuthSignInAnswer(qint64 msgId, const AuthAuthorization &result, const QVariant &attachedData)
{
    authorizeUser(msgId, result.user());
    TelegramCore::onAuthSignInAnswer(msgId, result, attachedData);
}

void Telegram::onAuthCheckPasswordAnswer(qint64 msgId, const AuthAuthorization &result, const QVariant &attachedData)
{
    authorizeUser(msgId, result.user());
    TelegramCore::onAuthCheckPasswordAnswer(msgId, result, attachedData);
}

void Telegram::onUpdatesGetDifferenceAnswer(qint64 id, const UpdatesDifference &result, const QVariant &attachedData) {
    processDifferences(id, result.newMessages(), result.newEncryptedMessages(), result.otherUpdates(),
                       result.chats(), result.users(), result.state(),
                       (result.classType() == UpdatesDifference::typeUpdatesDifferenceSlice));
    TelegramCore::onUpdatesGetDifferenceAnswer(id, result, attachedData);
}

void Telegram::onMessagesAcceptEncryptionAnswer(qint64 msgId, const EncryptedChat &result, const QVariant &attachedData)
{
    qCDebug(TG_LIB_SECRET) << "Joined to secret chat" << result.id() << "with peer" << result.adminId();
    SecretChat *secretChat = prv->mSecretState.chats().value(result.id());
    secretChat->setState(SecretChat::Accepted);
    prv->mSecretState.save();
    Q_EMIT messagesEncryptedChatCreated(result.id(), result.date(), result.adminId(), result.accessHash());

    //notify peer about our layer
    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(result.id());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    prv->mEncrypter->setSecretChat(secretChat);
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForNotifyLayer(randomId, CoreTypes::typeLayerVersion);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    TelegramCore::messagesSendEncryptedService(inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();

    qCDebug(TG_LIB_SECRET) << "Notified our layer:" << CoreTypes::typeLayerVersion;
    TelegramCore::onMessagesAcceptEncryptionAnswer(msgId, result, attachedData);
}

void Telegram::onMessagesDiscardEncryptionAnswer(qint64 msgId, bool result, const QVariant &attachedData)
{
    SecretChat *secretChat = prv->mSecretState.chats().take(msgId);
    if(!secretChat) {
        onMessagesDiscardEncryptionError(msgId, -1, "LIBQTELEGRAM_SECRETCHAT_ERROR", attachedData);
        return;
    }

    qint32 chatId = secretChat->chatId();
    if (result) {
        prv->mSecretState.chats().remove(chatId);
        prv->mSecretState.save();
        qCDebug(TG_LIB_SECRET) << "Discarded secret chat" << chatId;
        delete secretChat;
        secretChat = 0;
        TelegramCore::onMessagesDiscardEncryptionAnswer(msgId, result, attachedData);
    } else {
        qCWarning(TG_LIB_SECRET) << "Could not discard secret chat with id" << chatId;
        onMessagesDiscardEncryptionError(msgId, -1, "LIBQTELEGRAM_SECRETCHAT_ERROR", attachedData);
    }
}

void Telegram::processDifferences(qint64 id, const QList<Message> &messages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState) {

    Q_FOREACH (const Update &update, otherUpdates) {
        processSecretChatUpdate(update);
    }

    QList<SecretChatMessage> secretChatMessages;

    Q_FOREACH (const EncryptedMessage &encrypted, newEncryptedMessages) {
        SecretChatMessage secretChatMessage = toSecretChatMessage(encrypted);

        if (secretChatMessage.decryptedMessage().randomId()) {
            secretChatMessages << secretChatMessage;
        }
    }

    Q_EMIT updatesGetDifferenceAnswer(id, messages, secretChatMessages, otherUpdates, chats, users, state, isIntermediateState);
}

qint64 Telegram::authCheckPhone(Callback<AuthCheckedPhone> callBack, qint32 timeout) {
   return TelegramCore::authCheckPhone(prv->mSettings->phoneNumber(), callBack, timeout);
}

qint64 Telegram::authSendCode(Callback<AuthSentCode> callBack, qint32 timeout) {
    return TelegramCore::authSendCode(false, prv->mSettings->phoneNumber(), 0, prv->mSettings->appId(), prv->mSettings->appHash(), callBack, timeout);
}

qint64 Telegram::authSendCall(Callback<AuthSentCode> callBack, qint32 timeout) {
    return TelegramCore::authSendCode(true, prv->mSettings->phoneNumber(), 0, prv->mSettings->appId(), prv->mSettings->appHash(), callBack, timeout);
}

qint64 Telegram::authSignIn(const QString &code, Callback<AuthAuthorization> callBack, qint32 timeout) {
    return TelegramCore::authSignIn(prv->mSettings->phoneNumber(), prv->m_phoneCodeHash, code, callBack, timeout);
}

qint64 Telegram::authSignUp(const QString &code, const QString &firstName, const QString &lastName, Callback<AuthAuthorization> callBack, qint32 timeout) {
    return TelegramCore::authSignUp(prv->mSettings->phoneNumber(), prv->m_phoneCodeHash, code, firstName, lastName, callBack, timeout);
}

qint64 Telegram::authCheckPassword(const QByteArray &passwordData, Callback<AuthAuthorization> callBack, qint32 timeout) {
    return TelegramCore::authCheckPassword( QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256), callBack, timeout);
}

qint64 Telegram::accountRegisterDevice(const QString &token, const QString &appVersion, bool appSandbox, Callback<bool > callBack, qint32 timeout) {
    Q_UNUSED(appSandbox)
    if (token.length() == 0) {
        qCCritical(TG_TELEGRAM) << "refuse to register with empty token!";
        return -1;
    }
    QString version = appVersion;
    if (!version.length()) {
        version = Utils::getAppVersion();
    }
    qCDebug(TG_TELEGRAM) << "registering device for push - app version" << version;
    return TelegramCore::accountRegisterDevice(UBUNTU_PHONE_TOKEN_TYPE, token, callBack, timeout);
}

qint64 Telegram::accountUnregisterDevice(const QString &token, Callback<bool> callBack, qint32 timeout) {
    return TelegramCore::accountUnregisterDevice(UBUNTU_PHONE_TOKEN_TYPE, token, callBack, timeout);
}

qint64 Telegram::photosUploadProfilePhoto(const QByteArray &bytes, const QString &fileName, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop, Callback<UploadSendPhoto> callBack, qint32 timeout) {
    FileOperation *op = new FileOperation(FileOperation::uploadProfilePhoto);
    op->setCaption(caption);
    op->setGeoPoint(geoPoint);
    op->setCrop(crop);
    op->setResultCallback<UploadSendPhoto>(callBack);
    op->setTimeOut(timeout);
    return prv->mFileHandler->uploadSendFile(*op, fileName, bytes);
}

qint64 Telegram::photosUploadProfilePhoto(const QString &filePath, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop, Callback<UploadSendPhoto> callBack, qint32 timeout) {
    FileOperation *op = new FileOperation(FileOperation::uploadProfilePhoto);
    op->setCaption(caption);
    op->setGeoPoint(geoPoint);
    op->setCrop(crop);
    op->setResultCallback<UploadSendPhoto>(callBack);
    op->setTimeOut(timeout);
    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

bool lessThan(const Contact &c1, const Contact &c2) {
    return c1.userId() < c2.userId();
}

qint64 Telegram::contactsGetContacts(Callback<ContactsContacts> callBack, qint32 timeout) {
    //If there already is a full contact list on the client, an md5-hash of a comma-separated list of contact IDs
    //in ascending order may be passed in this 'hash' parameter. If the contact set was not changed,
    //contactsContactsNotModified() will be returned from Api, so the cached client list is returned with the
    //signal that they are the same contacts as previous request
    QString hash;
    if (!prv->m_cachedContacts.isEmpty()) {
        qSort(prv->m_cachedContacts.begin(), prv->m_cachedContacts.end(), lessThan); //lessThan method must be outside any class or be static
        QString hashBase;
        if (prv->m_cachedContacts.size() > 0) {
            hashBase.append(QString::number(prv->m_cachedContacts.at(0).userId()));
        }
        for (qint32 i = 1; i < prv->m_cachedContacts.size(); i++) {
            hashBase.append(",");
            hashBase.append(QString::number(prv->m_cachedContacts.at(i).userId()));
        }
        QCryptographicHash md5Generator(QCryptographicHash::Md5);
        md5Generator.addData(hashBase.toStdString().c_str());
        hash = md5Generator.result().toHex();
    }
    return TelegramCore::contactsGetContacts(hash, callBack, timeout);
}

qint64 Telegram::messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedPhoto);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes);
}

qint64 Telegram::messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedPhoto);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), filePath);
}

qint64 Telegram::messagesSendGeoPoint(const InputPeer &peer, qint64 randomId, const InputGeoPoint &inputGeoPoint, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    InputMedia inputMedia(InputMedia::typeInputMediaGeoPoint);
    inputMedia.setGeoPoint(inputGeoPoint);
    qint64 msgId = TelegramCore::messagesSendMedia(broadcast, silent, background, peer, replyToMsgId, inputMedia, randomId, reply_markup, callBack, timeout);
    prv->pendingMediaSends[msgId] = inputMedia.classType();
    return msgId;
}

qint64 Telegram::messagesSendContact(const InputPeer &peer, qint64 randomId, const QString &phoneNumber, const QString &firstName, const QString &lastName, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    InputMedia inputMedia(InputMedia::typeInputMediaContact);
    inputMedia.setPhoneNumber(phoneNumber);
    inputMedia.setFirstName(firstName);
    inputMedia.setLastName(lastName);
    qint64 msgId = TelegramCore::messagesSendMedia(broadcast, silent, background, peer, replyToMsgId, inputMedia, randomId, reply_markup, callBack, timeout);
    prv->pendingMediaSends[msgId] = inputMedia.classType();
    return msgId;
}

qint64 Telegram::messagesSendVideo(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, qint32 width, qint32 height, const QString &mimeType, const QByteArray &thumbnailBytes, const QString &thumbnailName, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    DocumentAttribute attr(DocumentAttribute::typeDocumentAttributeVideo);
    attr.setDuration(duration);
    attr.setW(width);
    attr.setH(height);

    InputMedia inputMedia(InputMedia::typeInputMediaUploadedDocument);
    inputMedia.setAttributes( inputMedia.attributes() << attr );
    inputMedia.setMimeType(mimeType);
    if (!thumbnailBytes.isEmpty()) {
        inputMedia.setClassType(InputMedia::typeInputMediaUploadedThumbDocument);
    }
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes, thumbnailBytes, thumbnailName);
}

qint64 Telegram::messagesSendVideo(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QString &thumbnailFilePath, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    DocumentAttribute attr(DocumentAttribute::typeDocumentAttributeVideo);
    attr.setDuration(duration);
    attr.setW(width);
    attr.setH(height);

    InputMedia inputMedia(InputMedia::typeInputMediaUploadedDocument);
    inputMedia.setAttributes( inputMedia.attributes() << attr );
    inputMedia.setMimeType(QMimeDatabase().mimeTypeForFile(QFileInfo(filePath)).name());
    if (thumbnailFilePath.length() > 0) {
        inputMedia.setClassType(InputMedia::typeInputMediaUploadedThumbDocument);
    }
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), filePath, thumbnailFilePath);
}

qint64 Telegram::messagesSendAudio(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, const QString &mimeType, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    DocumentAttribute attr(DocumentAttribute::typeDocumentAttributeAudio);
    attr.setDuration(duration);

    InputMedia inputMedia(InputMedia::typeInputMediaUploadedDocument);
    inputMedia.setAttributes( inputMedia.attributes() << attr );
    inputMedia.setMimeType(mimeType);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes);
}

qint64 Telegram::messagesSendAudio(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    DocumentAttribute attr(DocumentAttribute::typeDocumentAttributeAudio);
    attr.setDuration(duration);

    InputMedia inputMedia(InputMedia::typeInputMediaUploadedDocument);
    inputMedia.setAttributes( inputMedia.attributes() << attr );
    inputMedia.setMimeType(QMimeDatabase().mimeTypeForFile(QFileInfo(filePath)).name());

    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), filePath);
}

qint64 Telegram::messagesSendDocument(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, const QString &mimeType, const QByteArray &thumbnailBytes, const QString &thumbnailName, const QList<DocumentAttribute> &extraAttributes, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    DocumentAttribute fileAttr(DocumentAttribute::typeDocumentAttributeFilename);
    fileAttr.setFileName(fileName);

    QList<DocumentAttribute> attributes;
    attributes << fileAttr;
    attributes << extraAttributes;

    InputMedia inputMedia(InputMedia::typeInputMediaUploadedDocument);
    inputMedia.setAttributes(attributes);
    inputMedia.setMimeType(mimeType);
    if (!thumbnailBytes.isEmpty()) {
        inputMedia.setClassType(InputMedia::typeInputMediaUploadedThumbDocument);
    }
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes, thumbnailBytes, thumbnailName);
}

qint64 Telegram::messagesSendDocument(const InputPeer &peer, qint64 randomId, const QString &filePath, const QString &thumbnailFilePath, bool sendAsSticker, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UploadSendFile > callBack, qint32 timeout) {
    const QMimeType t = QMimeDatabase().mimeTypeForFile(QFileInfo(filePath));
    QString mimeType = t.name();

    DocumentAttribute fileAttr(DocumentAttribute::typeDocumentAttributeFilename);
    fileAttr.setFileName(QFileInfo(filePath).fileName());

    QList<DocumentAttribute> attributes;
    attributes << fileAttr;
    if(sendAsSticker) {
        QImageReader reader(filePath);
        DocumentAttribute imageSizeAttr(DocumentAttribute::typeDocumentAttributeImageSize);
        imageSizeAttr.setH(reader.size().height());
        imageSizeAttr.setW(reader.size().width());

        attributes << DocumentAttribute(DocumentAttribute::typeDocumentAttributeSticker) << imageSizeAttr;

        if(mimeType.contains("webp"))
            mimeType = "image/webp";
    }

    InputMedia inputMedia(InputMedia::typeInputMediaUploadedDocument);
    inputMedia.setMimeType(mimeType);
    inputMedia.setAttributes(attributes);
    if (thumbnailFilePath.length() > 0) {
        inputMedia.setClassType(InputMedia::typeInputMediaUploadedThumbDocument);
    }
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    op->setBroadcast(broadcast);
    op->setBackground(background);
    op->setSilent(silent);
    op->setReplyMarkup(reply_markup);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return uploadSendFile(*op, inputMedia.classType(), filePath, thumbnailFilePath);
}

qint64 Telegram::messagesForwardPhoto(const InputPeer &peer, qint64 randomId, qint64 photoId, qint64 accessHash, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaPhoto);
    inputMedia.setIdInputPhoto(inputPhoto);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId, reply_markup, broadcast, silent, background, callBack, timeout);
}

qint64 Telegram::messagesForwardVideo(const InputPeer &peer, qint64 randomId, qint64 videoId, qint64 accessHash, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    InputDocument inputVideo(InputDocument::typeInputDocument);
    inputVideo.setId(videoId);
    inputVideo.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaDocument);
    inputMedia.setIdInputDocument(inputVideo);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId, reply_markup, broadcast, silent, background, callBack, timeout);
}

qint64 Telegram::messagesForwardAudio(const InputPeer &peer, qint64 randomId, qint64 audioId, qint64 accessHash, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    InputDocument inputAudio(InputDocument::typeInputDocument);
    inputAudio.setId(audioId);
    inputAudio.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaDocument);
    inputMedia.setIdInputDocument(inputAudio);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId, reply_markup, broadcast, silent, background, callBack, timeout);
}

qint64 Telegram::messagesForwardDocument(const InputPeer &peer, qint64 randomId, qint64 documentId, qint64 accessHash, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    InputDocument inputDocument(InputDocument::typeInputDocument);
    inputDocument.setId(documentId);
    inputDocument.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaDocument);
    inputMedia.setIdInputDocument(inputDocument);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId, reply_markup, broadcast, silent, background, callBack, timeout);
}

qint64 Telegram::messagesForwardMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId, const ReplyMarkup &reply_markup, bool broadcast, bool silent, bool background, Callback<UpdatesType> callBack, qint32 timeout) {
    const qint64 msgId =  TelegramCore::messagesSendMedia(broadcast, silent, background, peer, replyToMsgId, media, randomId, reply_markup, callBack, timeout);
    prv->pendingForwards[msgId] = media.classType();
    return msgId;
}

qint64 Telegram::uploadSendFile(FileOperation &op, int mediaType, const QString &fileName, const QByteArray &bytes, const QByteArray &thumbnailBytes, const QString &thumbnailName)
{
    const qint64 fileId = prv->mFileHandler->uploadSendFile(op, fileName, bytes, thumbnailBytes, thumbnailName);
    prv->pendingMediaSends[fileId] = mediaType;
    return fileId;
}

qint64 Telegram::uploadSendFile(FileOperation &op, int mediaType, const QString &filePath, const QString &thumbnailPath)
{
    const qint64 fileId = prv->mFileHandler->uploadSendFile(op, filePath, thumbnailPath);
    prv->pendingMediaSends[fileId] = mediaType;
    return fileId;
}

qint64 Telegram::messagesSetEncryptedTyping(qint32 chatId, bool typing, Callback<bool> callBack, qint32 timeout) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not read history of a not yet existant chat";
        CallbackError error;
        error.errorCode = -1;
        error.errorText = "LIBQTELEGRAM_SECRETCHAT_ERROR";
        error.null = false;
        if(callBack) callBack(0, false, error);
        return -1;
    }
    InputEncryptedChat mChat;
    mChat.setChatId(chatId);
    mChat.setAccessHash(secretChat->accessHash());
    return TelegramCore::messagesSetEncryptedTyping(mChat,typing, callBack, timeout);
}

qint64 Telegram::messagesEditChatPhoto(qint32 chatId, const QString &filePath, const InputPhotoCrop &crop, Callback<UploadSendFile > callBack, qint32 timeout) {
    InputChatPhoto inputChatPhoto(InputChatPhoto::typeInputChatUploadedPhoto);
    inputChatPhoto.setCrop(crop);
    FileOperation *op = new FileOperation(FileOperation::editChatPhoto);
    op->setChatId(chatId);
    op->setInputChatPhoto(inputChatPhoto);
    op->setResultCallback<UploadSendFile>(callBack);
    op->setTimeOut(timeout);
    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesEditChatPhoto(qint32 chatId, qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop, Callback<UpdatesType> callBack, qint32 timeout) {
    InputChatPhoto inputChatPhoto(InputChatPhoto::typeInputChatPhoto);
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    inputChatPhoto.setId(inputPhoto);
    inputChatPhoto.setCrop(crop);
    return TelegramCore::messagesEditChatPhoto(chatId, inputChatPhoto, callBack, timeout);
}

qint64 Telegram::uploadGetFile(const InputFileLocation &file, qint32 fileSize, qint32 dc, Callback<UploadGetFile> callBack, qint32 timeout) {
    return uploadGetFile(file, fileSize, dc, QByteArray(), QByteArray(), callBack, timeout);
}

qint64 Telegram::uploadGetFile(const InputFileLocation &location, qint32 fileSize, qint32 dcNum, const QByteArray &key, const QByteArray &iv, Callback<UploadGetFile> callBack, qint32 timeout) {
    if(!prv->mFileHandler) return 0;
    qint64 msgId = prv->mFileHandler->uploadGetFile(location, fileSize, dcNum, key, iv, timeout);
    if(msgId && callBack)
        callBackPush<UploadGetFile>(msgId, callBack);

    return msgId;
}

qint64 Telegram::uploadCancelFile(qint64 fileId) {
    if(!prv->mFileHandler) return 0;
    return prv->mFileHandler->uploadCancelFile(fileId);
}
