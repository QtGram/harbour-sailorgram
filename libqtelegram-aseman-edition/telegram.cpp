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

#include "util/tlvalues.h"
#include "telegram/types/types.h"
#include "core/dcprovider.h"
#include "core/api.h"
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

#define CHECK_API if(!prv->mApi) {qDebug() << __FUNCTION__ << "Error: API is not ready."; return 0;}

QHash<QString, Settings*> qtelegram_settings_per_number;
QHash<QString, CryptoUtils*> qtelegram_cryptos_per_number;
QSettings::Format qtelegram_default_settings_format = QSettings::IniFormat;

class TelegramPrivate
{
public:
    TelegramPrivate() :
        mLibraryState(Telegram::LoggedOut),
        mLastRetryType(Telegram::NotRetry),
        mSlept(false),
        mApi(0),
        mSecretState(0) {}

    Telegram::LibraryState mLibraryState;
    Telegram::LastRetryType mLastRetryType;

    bool mSlept;

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
    QHash<qint64, int> pendingForwards;
    QHash<qint64, int> pendingMediaSends;

    // encrypted chats
    SecretState mSecretState;
    Encrypter *mEncrypter;
    Decrypter *mDecrypter;

    bool mLoggedIn;
    bool mCreatedSharedKeys;

    //info for retries
    QString mLastPhoneChecked;
    QString mLastLangCode;
    QList<InputContact> mLastContacts;
};

Telegram::Telegram(const QString &defaultHostAddress, qint16 defaultHostPort, qint16 defaultHostDcId, qint32 appId, const QString &appHash, const QString &phoneNumber, const QString &configPath, const QString &publicKeyFile) {
    // Qt5.2 doesn't support .ini files to control logging, so use this
    // manual workaround instead.
    // http://blog.qt.digia.com/blog/2014/03/11/qt-weekly-1-categorized-logging/
    prv = new TelegramPrivate;

    QLoggingCategory::setFilterRules(QString(qgetenv("QT_LOGGING_RULES")));

    prv->mSettingsId = defaultHostAddress + ":" + QString::number(defaultHostPort) + ":" + configPath +
            ":" + QString::number(defaultHostPort) + ":" + QString::number(appId) + ":" + appHash +
            phoneNumber;
    prv->mSettings = qtelegram_settings_per_number.value(prv->mSettingsId);
    if(!prv->mSettings) {
        prv->mSettings = new Settings();
        prv->mSettings->setAppHash(appHash);
        prv->mSettings->setAppId(appId);
        prv->mSettings->setDefaultHostAddress(defaultHostAddress);
        prv->mSettings->setDefaultHostDcId(defaultHostDcId);
        prv->mSettings->setDefaultHostPort(defaultHostPort);
        qtelegram_settings_per_number[prv->mSettingsId] = prv->mSettings;
    }

    // load settings
    if (!prv->mSettings->loadSettings(phoneNumber, configPath, publicKeyFile)) {
        throw std::runtime_error("loadSettings failure");
    }

    prv->mCrypto = qtelegram_cryptos_per_number.value(phoneNumber);
    if(!prv->mCrypto) {
        prv->mCrypto = new CryptoUtils(prv->mSettings);
        qtelegram_cryptos_per_number[phoneNumber] = prv->mCrypto;
    }

    prv->mDcProvider = new DcProvider(prv->mSettings, prv->mCrypto);
    prv->mDcProvider->setParent(this);

    prv->mSecretState = SecretState(prv->mSettings);
    prv->mEncrypter = new Encrypter(prv->mSettings);
    prv->mDecrypter = new Decrypter(prv->mSettings);

    connect(prv->mDecrypter, SIGNAL(sequenceNumberGap(qint32,qint32,qint32)), SLOT(onSequenceNumberGap(qint32,qint32,qint32)));

    prv->mSecretState.load();

    connect(prv->mDcProvider, SIGNAL(fatalError()), this, SIGNAL(fatalError()));
    // activate dc provider ready signal
    connect(prv->mDcProvider, SIGNAL(dcProviderReady()), this, SLOT(onDcProviderReady()));
    // activate rest of dc provider signal connections
    connect(prv->mDcProvider, SIGNAL(authNeeded()), this, SIGNAL(authNeeded()));
    connect(prv->mDcProvider, SIGNAL(authTransferCompleted()), this, SLOT(onAuthLoggedIn()));
    connect(prv->mDcProvider, SIGNAL(error(qint64,qint32,const QString&)), this, SIGNAL(error(qint64,qint32,const QString&)));
}

bool Telegram::sleep() {
    // sleep only if not slept and library already logged in. Returns true if sleep operations completes
    if (!prv->mSlept && prv->mLibraryState >= LoggedIn) {
        if (prv->mApi && prv->mApi->mainSession()) {
            prv->mApi->mainSession()->close();
        }
        prv->mSlept = true;
        return true;
    }
    return false;
}

bool Telegram::wake() {
    // wake only if slept and library already logged in. Returns true if wake operation completes
    if (prv->mSlept && prv->mLibraryState >= LoggedIn) {
        CHECK_API;
        connect(prv->mApi, SIGNAL(mainSessionReady()), this, SIGNAL(woken()), Qt::UniqueConnection);
        DC *dc = prv->mDcProvider->getWorkingDc();
        prv->mApi->createMainSessionToDc(dc);
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
}

void Telegram::init() {
    // check the auth values stored in settings, check the available DCs config data if there is
    // connection to servers, and emit signals depending on user authenticated or not.
    prv->mDcProvider->initialize();
}

Telegram::~Telegram() {
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

Settings *Telegram::settings() const
{
    return prv->mSettings;
}

CryptoUtils *Telegram::crypto() const
{
    return prv->mCrypto;
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
    if (prv->mApi && prv->mApi->mainSession()) {
        return prv->mApi->mainSession()->state() == QAbstractSocket::ConnectedState;
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

void Telegram::onAuthLogOutAnswer(qint64 id, bool ok) {
    prv->mDcProvider->logOut();
    prv->mLibraryState = LoggedOut;
    Q_EMIT authLogOutAnswer(id,ok);
}

qint32 Telegram::ourId() {
    return prv->mSettings->ourId();
}

void Telegram::onDcProviderReady() {
    prv->mLibraryState = CreatedSharedKeys;
    prv->mApi = prv->mDcProvider->getApi();
    // api signal-signal and signal-slot connections
    // updates
    connect(prv->mApi, SIGNAL(updatesTooLong()), this, SIGNAL(updatesTooLong()));
    connect(prv->mApi, SIGNAL(updateShortMessage(qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)), this, SIGNAL(updateShortMessage(qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)));
    connect(prv->mApi, SIGNAL(updateShortChatMessage(qint32,qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)), this, SIGNAL(updateShortChatMessage(qint32,qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)));
    connect(prv->mApi, SIGNAL(updateShort(const Update&,qint32)), this, SIGNAL(updateShort(const Update&,qint32)));
    connect(prv->mApi, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)), this, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)));
    connect(prv->mApi, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)), this, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)));
    // errors
    connect(prv->mApi, SIGNAL(error(qint64,qint32,const QString&,const QString&)), this, SLOT(onError(qint64,qint32,const QString&,const QString&)));
    connect(prv->mApi, SIGNAL(errorRetry(qint64,qint32,const QString&)), this, SLOT(onErrorRetry(qint64,qint32,const QString&)));
    connect(prv->mApi, SIGNAL(authSignInError(qint64,qint32,const QString&)), this, SIGNAL(authSignInError(qint64,qint32,const QString&)));
    connect(prv->mApi, SIGNAL(authSignUpError(qint64,qint32,const QString&)), this, SIGNAL(authSignUpError(qint64,qint32,const QString&)));
    // positive responses
    connect(prv->mApi, SIGNAL(helpGetInviteTextAnswer(qint64,const QString&)), this, SIGNAL(helpGetInviteTextAnswer(qint64,const QString&)));
    connect(prv->mApi, SIGNAL(authCheckedPhone(qint64,bool)), this, SIGNAL(authCheckPhoneAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(authCheckPhoneSent(qint64,const QString&)), this, SIGNAL(authCheckPhoneSent(qint64,const QString&)));
    connect(prv->mApi, SIGNAL(authSentCode(qint64,bool,const QString&,qint32,bool)), this, SLOT(onAuthSentCode(qint64,bool,const QString&,qint32,bool)));
    connect(prv->mApi, SIGNAL(authSentAppCode(qint64,bool,const QString&,qint32,bool)), this, SLOT(onAuthSentCode(qint64,bool,const QString&,qint32,bool)));
    connect(prv->mApi, SIGNAL(authSendSmsResult(qint64,bool)), this, SIGNAL(authSendSmsAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(authSendCallResult(qint64,bool)), this, SIGNAL(authSendCallAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(authSignInAuthorization(qint64,qint32,const User&)), this, SLOT(onUserAuthorized(qint64,qint32,const User&)));
    connect(prv->mApi, SIGNAL(authSignUpAuthorization(qint64,qint32,const User&)), this, SLOT(onUserAuthorized(qint64,qint32,const User&)));
    connect(prv->mApi, SIGNAL(authLogOutResult(qint64,bool)), this, SLOT(onAuthLogOutAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(authSendInvitesResult(qint64,bool)), this, SIGNAL(authSendInvitesAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(authResetAuthorizationsResult(qint64,bool)), this, SIGNAL(authResetAuthorizationsAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(authCheckPasswordResult(qint64,qint32,const User&)), this, SIGNAL(authCheckPasswordAnswer(qint64,qint32,const User&)));
    connect(prv->mApi, SIGNAL(authRequestPasswordRecoveryResult(qint64,AuthPasswordRecovery)), this, SIGNAL(authRequestPasswordRecoveryAnswer(qint64,AuthPasswordRecovery)));
    connect(prv->mApi, SIGNAL(authRecoverPasswordResult(qint64,AuthAuthorization)), this, SIGNAL(authRecoverPasswordAnswer(qint64,AuthAuthorization)));
    connect(prv->mApi, SIGNAL(accountRegisterDeviceResult(qint64,bool)), this, SIGNAL(accountRegisterDeviceAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountUnregisterDeviceResult(qint64,bool)), this, SIGNAL(accountUnregisterDeviceAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountUpdateNotifySettingsResult(qint64,bool)), this, SIGNAL(accountUpdateNotifySettingsAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountPeerNotifySettings(qint64,const PeerNotifySettings&)), this, SIGNAL(accountGetNotifySettingsAnswer(qint64,const PeerNotifySettings&)));
    connect(prv->mApi, SIGNAL(accountResetNotifySettingsResult(qint64,bool)), this, SIGNAL(accountResetNotifySettingsAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountUser(qint64,const User&)), this, SIGNAL(accountUpdateProfileAnswer(qint64,const User&)));
    connect(prv->mApi, SIGNAL(accountUpdateStatusResult(qint64,bool)), this, SIGNAL(accountUpdateStatusAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountGetWallPapersResult(qint64,const QList<WallPaper>&)), this, SIGNAL(accountGetWallPapersAnswer(qint64,const QList<WallPaper>&)));
    connect(prv->mApi, SIGNAL(accountCheckUsernameResult(qint64,bool)), this, SIGNAL(accountCheckUsernameAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountUpdateUsernameResult(qint64,const User&)), this, SIGNAL(accountUpdateUsernameAnswer(qint64,const User&)));
    connect(prv->mApi, SIGNAL(accountDeleteAccountResult(qint64,bool)), this, SIGNAL(accountDeleteAccountAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountGetAccountTTLResult(qint64,const AccountDaysTTL&)), this, SIGNAL(accountGetAccountTTLAnswer(qint64,const AccountDaysTTL&)));
    connect(prv->mApi, SIGNAL(accountSetAccountTTLResult(qint64,bool)), this, SIGNAL(accountSetAccountTTLAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountUpdateDeviceLockedResult(qint64,bool)), this, SIGNAL(accountUpdateDeviceLockedAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountSentChangePhoneCode(qint64,const QString&,qint32)), this, SIGNAL(accountSentChangePhoneCode(qint64,const QString&,qint32)));
    connect(prv->mApi, SIGNAL(accountGetPasswordResult(qint64,const AccountPassword&)), this, SIGNAL(accountGetPasswordAnswer(qint64,const AccountPassword&)));
    connect(prv->mApi, SIGNAL(accountGetAuthorizationsResult(qint64,AccountAuthorizations)), this, SIGNAL(accountGetAuthorizationsAnswer(qint64,AccountAuthorizations)));
    connect(prv->mApi, SIGNAL(accountResetAuthorizationResult(qint64,bool)), this, SIGNAL(accountResetAuthorizationAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountGetPasswordSettingsResult(qint64,AccountPasswordSettings)), this, SIGNAL(accountGetPasswordSettingsAnswer(qint64,AccountPasswordSettings)));
    connect(prv->mApi, SIGNAL(accountUpdatePasswordSettingsResult(qint64,bool)), this, SIGNAL(accountUpdatePasswordSettingsAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(accountChangePhoneResult(qint64,const User&)), this, SIGNAL(accountChangePhoneAnswer(qint64,const User&)));
    connect(prv->mApi, SIGNAL(photosPhoto(qint64,const Photo&,const QList<User>&)), this, SIGNAL(photosUploadProfilePhotoAnswer(qint64,const Photo&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(photosUserProfilePhoto(qint64,const UserProfilePhoto&)), this, SIGNAL(photosUpdateProfilePhotoAnswer(qint64,const UserProfilePhoto&)));
    connect(prv->mApi, SIGNAL(usersGetUsersResult(qint64,const QList<User>&)), this, SIGNAL(usersGetUsersAnswer(qint64,const QList<User>&)));
    connect(prv->mApi, SIGNAL(userFull(qint64,const User&,const ContactsLink&,const Photo&,const PeerNotifySettings&,bool,const QString&,const QString&)), this, SIGNAL(usersGetFullUserAnswer(qint64,const User&,const ContactsLink&,const Photo&,const PeerNotifySettings&,bool,const QString&,const QString&)));
    connect(prv->mApi, SIGNAL(photosPhotos(qint64,const QList<Photo>&,const QList<User>&)), this, SLOT(onPhotosPhotos(qint64, const QList<Photo>&, const QList<User>&)));
    connect(prv->mApi, SIGNAL(photosPhotosSlice(qint64,qint32,const QList<Photo>&,const QList<User>&)), this, SIGNAL(photosGetUserPhotosAnswer(qint64,qint32,const QList<Photo>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(contactsGetStatusesResult(qint64,const QList<ContactStatus>&)), this, SIGNAL(contactsGetStatusesAnswer(qint64,const QList<ContactStatus>&)));
    connect(prv->mApi, SIGNAL(contactsContacts(qint64,const QList<Contact>&,const QList<User>&)), this, SLOT(onContactsContacts(qint64,const QList<Contact>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(contactsContactsNotModified(qint64)), this, SLOT(onContactsContactsNotModified(qint64)));
    connect(prv->mApi, SIGNAL(contactsImportedContacts(qint64,const QList<ImportedContact>&,const QList<qint64>&,const QList<User>&)), this, SIGNAL(contactsImportContactsAnswer(qint64,const QList<ImportedContact>&,const QList<qint64>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(contactsImportedContacts(qint64,const QList<ImportedContact>&,const QList<qint64>&,const QList<User>&)), this, SLOT(onContactsImportContactsAnswer()));
    connect(prv->mApi, SIGNAL(contactsFound(qint64,const QList<ContactFound>&,const QList<User>&)), this, SIGNAL(contactsFound(qint64,const QList<ContactFound>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(contactsResolveUsernameResult(qint64,const User&)), this, SIGNAL(contactsResolveUsernameAnswer(qint64,const User&)));
    connect(prv->mApi, SIGNAL(contactsDeleteContactLink(qint64,const ContactLink&,const ContactLink&,const User&)), this, SIGNAL(contactsDeleteContactAnswer(qint64,const ContactLink&,const ContactLink&,const User&)));
    connect(prv->mApi, SIGNAL(contactsDeleteContactsResult(qint64,bool)), this, SIGNAL(contactsDeleteContactsAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(contactsBlockResult(qint64,bool)), this, SIGNAL(contactsBlockAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(contactsUnblockResult(qint64,bool)), this, SIGNAL(contactsUnblockAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(contactsBlocked(qint64,const QList<ContactBlocked>&,const QList<User>&)), this, SLOT(onContactsBlocked(qint64,const QList<ContactBlocked>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(contactsBlockedSlice(qint64,qint32,const QList<ContactBlocked>&,const QList<User>&)), this, SIGNAL(contactsGetBlockedAnswer(qint64,qint32,QList<ContactBlocked>,QList<User>)));
    connect(prv->mApi, SIGNAL(messagesSentMessage(qint64,qint32,qint32,const MessageMedia&,qint32,qint32,qint32)), this, SLOT(onMessagesSentMessage(qint64,qint32,qint32,const MessageMedia&,qint32,qint32,qint32)));
    connect(prv->mApi, SIGNAL(messagesSentMessageLink(qint64,qint32,qint32,const MessageMedia&,qint32,qint32,qint32,const QList<ContactsLink>&)), this, SIGNAL(messagesSendMessageAnswer(qint64,qint32,qint32,const MessageMedia&,qint32,qint32,qint32,const QList<ContactsLink>&)));
    connect(prv->mApi, SIGNAL(messagesSetTypingResult(qint64,bool)), this, SIGNAL(messagesSetTypingAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(messagesGetMessagesMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesGetMessagesMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesGetMessagesMessagesSlice(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetMessagesAnswer(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesDialogs(qint64, const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesDialogs(qint64,const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesDialogsSlice(qint64,qint32,const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetDialogsAnswer(qint64,qint32,const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesGetHistoryMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesGetHistoryMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesGetHistoryMessagesSlice(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetHistoryAnswer(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesSearchMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesSearchMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesSearchMessagesSlice(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesSearchAnswer(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(prv->mApi, SIGNAL(messagesReadAffectedHistory(qint64,qint32,qint32,qint32)), this, SIGNAL(messagesReadHistoryAnswer(qint64,qint32,qint32,qint32)));
    connect(prv->mApi, SIGNAL(messagesReadMessageContentsResult(qint64,const MessagesAffectedMessages&)), this, SIGNAL(messagesReadMessageContentsAnswer(qint64,const MessagesAffectedMessages&)));
    connect(prv->mApi, SIGNAL(messagesDeleteAffectedHistory(qint64,qint32,qint32,qint32)), this, SIGNAL(messagesDeleteHistoryAnswer(qint64,qint32,qint32,qint32)));
    connect(prv->mApi, SIGNAL(messagesDeleteMessagesResult(qint64,const MessagesAffectedMessages&)), this, SIGNAL(messagesDeleteMessagesAnswer(qint64,const MessagesAffectedMessages&)));
    connect(prv->mApi, SIGNAL(messagesReceivedMessagesResult(qint64,const QList<ReceivedNotifyMessage>&)), this, SIGNAL(messagesReceivedMessagesAnswer(qint64,const QList<ReceivedNotifyMessage>&)));
    connect(prv->mApi, SIGNAL(messagesForwardedMessage(qint64,const UpdatesType&)), this, SIGNAL(messagesForwardMessageAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesForwardedMessages(qint64,const UpdatesType&)), this, SIGNAL(messagesForwardMessagesAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesSentBroadcast(qint64,const UpdatesType&)), this, SIGNAL(messagesSendBroadcastAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesGetWebPagePreviewResult(qint64,MessageMedia)), this, SIGNAL(messagesGetWebPagePreviewAnswer(qint64,MessageMedia)));
    connect(prv->mApi, SIGNAL(messagesForwardedMedia(qint64,UpdatesType)), SLOT(onMessagesForwardMediaAnswer(qint64,UpdatesType)));
    connect(prv->mApi, SIGNAL(messagesChats(qint64,const QList<Chat>&)), this, SIGNAL(messagesGetChatsAnswer(qint64,const QList<Chat>&)));
    connect(prv->mApi, SIGNAL(messagesChatFull(qint64,const ChatFull&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetFullChatAnswer(qint64,const ChatFull&,const QList<Chat>&,const QList<User>&)));

    connect(prv->mApi, SIGNAL(messagesEditedChatTitle(qint64,const UpdatesType&)), this, SIGNAL(messagesEditChatTitleAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesEditedChatPhoto(qint64,const UpdatesType&)), this, SIGNAL(messagesEditChatPhotoStatedMessageAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesAddedChatUser(qint64,const UpdatesType&)), this, SIGNAL(messagesAddChatUserAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesDeletedChat(qint64,const UpdatesType&)), this, SIGNAL(messagesDeleteChatUserAnswer(qint64,const UpdatesType&)));
    connect(prv->mApi, SIGNAL(messagesCreatedChat(qint64,const UpdatesType&)), this, SIGNAL(messagesCreateChatAnswer(qint64,const UpdatesType&)));

    // secret chats
    connect(prv->mApi, SIGNAL(messagesDhConfig(qint64,qint32,const QByteArray&,qint32,const QByteArray&)), this, SLOT(onMessagesDhConfig(qint64,qint32,const QByteArray&,qint32,const QByteArray&)));
    connect(prv->mApi, SIGNAL(messagesDhConfigNotModified(qint64,const QByteArray&)), this, SLOT(onMessagesDhConfigNotModified(qint64,const QByteArray&)));
    connect(prv->mApi, SIGNAL(messagesRequestEncryptionEncryptedChat(qint64,const EncryptedChat&)), this, SLOT(onMessagesRequestEncryptionEncryptedChat(qint64,const EncryptedChat&)));
    connect(prv->mApi, SIGNAL(messagesAcceptEncryptionEncryptedChat(qint64,const EncryptedChat&)), this, SLOT(onMessagesAcceptEncryptionEncryptedChat(qint64,const EncryptedChat&)));
    connect(prv->mApi, SIGNAL(messagesDiscardEncryptionResult(qint64,bool)), this, SLOT(onMessagesDiscardEncryptionResult(qint64,bool)));
    connect(prv->mApi, SIGNAL(messagesReadEncryptedHistoryResult(qint64,bool)), this, SIGNAL(messagesReadEncryptedHistoryAnswer(qint64,bool)));
    connect(prv->mApi, SIGNAL(messagesSendEncryptedSentEncryptedMessage(qint64,qint32)), this, SIGNAL(messagesSendEncryptedAnswer(qint64,qint32)));
    connect(prv->mApi, SIGNAL(messagesSendEncryptedSentEncryptedFile(qint64,qint32,const EncryptedFile&)), this, SIGNAL(messagesSendEncryptedAnswer(qint64,qint32,const EncryptedFile&)));
    connect(prv->mApi, SIGNAL(messagesSendEncryptedServiceSentEncryptedMessage(qint64,qint32)), this, SIGNAL(messagesSendEncryptedServiceAnswer(qint64,qint32)));
    connect(prv->mApi, SIGNAL(messagesSendEncryptedServiceSentEncryptedFile(qint64,qint32,const EncryptedFile&)), this, SIGNAL(messagesSendEncryptedServiceAnswer(qint64,qint32,const EncryptedFile&)));
    connect(prv->mApi, SIGNAL(messagesGetStickersResult(qint64,const MessagesStickers&)), this, SIGNAL(messagesGetStickersAnwer(qint64,const MessagesStickers&)));
    connect(prv->mApi, SIGNAL(messagesGetAllStickersResult(qint64,const MessagesAllStickers&)), this, SIGNAL(messagesGetAllStickersAnwer(qint64,const MessagesAllStickers&)));
    connect(prv->mApi, SIGNAL(messagesGetStickerSetResult(qint64,MessagesStickerSet)), this, SIGNAL(messagesGetStickerSetAnwer(qint64,MessagesStickerSet)));
    connect(prv->mApi, SIGNAL(messagesInstallStickerSetResult(qint64,bool)), this, SIGNAL(messagesInstallStickerSetAnwer(qint64,bool)));
    connect(prv->mApi, SIGNAL(messagesUninstallStickerSetResult(qint64,bool)), this, SIGNAL(messagesUninstallStickerSetAnwer(qint64,bool)));
    connect(prv->mApi, SIGNAL(messagesExportChatInviteResult(qint64,ExportedChatInvite)), this, SIGNAL(messagesExportChatInviteAnwer(qint64,ExportedChatInvite)));
    connect(prv->mApi, SIGNAL(messagesCheckChatInviteResult(qint64,ChatInvite)), this, SIGNAL(messagesCheckChatInviteAnwer(qint64,ChatInvite)));
    connect(prv->mApi, SIGNAL(messagesImportChatInviteResult(qint64,UpdatesType)), this, SIGNAL(messagesImportChatInviteAnwer(qint64,UpdatesType)));
    connect(prv->mApi, SIGNAL(updateShort(const Update&,qint32)), SLOT(onUpdateShort(const Update&)));
    connect(prv->mApi, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)), SLOT(onUpdatesCombined(const QList<Update>&)));
    connect(prv->mApi, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)), SLOT(onUpdates(const QList<Update>&)));
    // updates
    connect(prv->mApi, SIGNAL(updatesState(qint64,qint32,qint32,qint32,qint32,qint32)), this, SIGNAL(updatesGetStateAnswer(qint64,qint32,qint32,qint32,qint32,qint32)));
    connect(prv->mApi, SIGNAL(updatesDifferenceEmpty(qint64,qint32,qint32)), this, SIGNAL(updatesGetDifferenceAnswer(qint64,qint32,qint32)));
    connect(prv->mApi, SIGNAL(updatesDifference(qint64,const QList<Message>&,const QList<EncryptedMessage>&,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)), this, SLOT(onUpdatesDifference(qint64,const QList<Message>&,const QList<EncryptedMessage>&,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)));
    connect(prv->mApi, SIGNAL(updatesDifferenceSlice(qint64,const QList<Message>,const QList<EncryptedMessage>,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)), this, SLOT(onUpdatesDifferenceSlice(qint64,const QList<Message>&,const QList<EncryptedMessage>&,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)));
    // logic additional signal slots
    connect(prv->mApi, SIGNAL(mainSessionDcChanged(DC*)), this, SLOT(onAuthCheckPhoneDcChanged()));
    connect(prv->mApi, SIGNAL(mainSessionDcChanged(DC*)), this, SLOT(onHelpGetInviteTextDcChanged()));
    connect(prv->mApi, SIGNAL(mainSessionDcChanged(DC*)), this, SLOT(onImportContactsDcChanged()));
    connect(prv->mApi, SIGNAL(mainSessionReady()), this, SIGNAL(connected()));
    connect(prv->mApi, SIGNAL(mainSessionClosed()), this, SIGNAL(disconnected()));

    prv->mFileHandler = FileHandler::Ptr(new FileHandler(prv->mApi, prv->mCrypto, prv->mSettings, *prv->mDcProvider, prv->mSecretState));
    connect(prv->mFileHandler.data(), SIGNAL(uploadSendFileAnswer(qint64,qint32,qint32,qint32)), SIGNAL(uploadSendFileAnswer(qint64,qint32,qint32,qint32)));
    connect(prv->mFileHandler.data(), SIGNAL(uploadGetFileAnswer(qint64,const StorageFileType&,qint32,const QByteArray&,qint32,qint32,qint32)), SIGNAL(uploadGetFileAnswer(qint64,const StorageFileType&,qint32,const QByteArray&,qint32,qint32,qint32)));
    connect(prv->mFileHandler.data(), SIGNAL(uploadCancelFileAnswer(qint64,bool)), SIGNAL(uploadCancelFileAnswer(qint64,bool)));
    connect(prv->mFileHandler.data(), SIGNAL(error(qint64,qint32,const QString&)), SIGNAL(error(qint64,qint32,const QString&)));
    connect(prv->mFileHandler.data(), SIGNAL(messagesSentMedia(qint64,const UpdatesType&)), SLOT(onMessagesSendMediaAnswer(qint64,const UpdatesType&)));
    connect(prv->mFileHandler.data(), SIGNAL(messagesSendEncryptedFileAnswer(qint64,qint32,const EncryptedFile&)), SIGNAL(messagesSendEncryptedFileAnswer(qint64,qint32,const EncryptedFile&)));

    // At this point we should test the main session state and emit by hand signals of connected/disconnected
    // depending on the connection state of the session. This is so because first main session connection, if done,
    // is performed before we could connect the signal-slots to advise about it;
    if (prv->mApi->mainSession()->state() == QAbstractSocket::ConnectedState) {
        Q_EMIT connected();
    } else {
        Q_EMIT disconnected();
    }
}

qint64 Telegram::messagesCreateEncryptedChat(const InputUser &user) {
    qCDebug(TG_LIB_SECRET) << "creating new encrypted chat";
    // generate a new object where store all the needed secret chat data
    SecretChat *secretChat = new SecretChat(prv->mSettings);
    secretChat->setRequestedUser(user);
    return generateGAorB(secretChat);
}

qint64 Telegram::messagesAcceptEncryptedChat(qint32 chatId) {
    qCDebug(TG_LIB_SECRET) << "accepting requested encrypted chat with id" << chatId;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);

    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Not found any chat related to" << chatId;
        return -1;
    }
    return generateGAorB(secretChat);
}

qint64 Telegram::messagesDiscardEncryptedChat(qint32 chatId) {
    CHECK_API;
    qCDebug(TG_LIB_SECRET) << "discarding encrypted chat with id" << chatId;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Trying to discard a not existant chat";
        Q_EMIT messagesEncryptedChatDiscarded(chatId);
        return -1;
    }

    qint64 requestId = prv->mApi->messagesDiscardEncryption(chatId);
    // insert another entry related to this request for deleting chat only when response is ok
    prv->mSecretState.chats().insert(requestId, secretChat);
    return requestId;
}

qint64 Telegram::messagesSetEncryptedTTL(qint64 randomId, qint32 chatId, qint32 ttl) {
    CHECK_API;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not set secret chat TTL, chat not found.";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForTtl(randomId, ttl);

    prv->mEncrypter->setSecretChat(secretChat);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    QList<qint64> previousMsgs = secretChat->sequence();
    qint64 requestId = prv->mApi->messagesSendEncrypted(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();

    return requestId;
}

qint64 Telegram::messagesReadEncryptedHistory(qint32 chatId, qint32 maxDate) {
    CHECK_API;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not read history of a not yet existant chat";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(chatId);
    inputEncryptedChat.setAccessHash(secretChat->accessHash());
    return prv->mApi->messagesReadEncryptedHistory(inputEncryptedChat, maxDate);
}

qint64 Telegram::messagesSendEncrypted(qint32 chatId, qint64 randomId, qint32 ttl, const QString &text) {
    CHECK_API;

    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not find any related secret chat to send the message";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForSendMessage(randomId, ttl, text);

    prv->mEncrypter->setSecretChat(secretChat);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    QList<qint64> previousMsgs = secretChat->sequence();
    qint64 request = prv->mApi->messagesSendEncrypted(previousMsgs, inputEncryptedChat, randomId, data);

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
    prv->mApi->messagesSendEncrypted(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();
}

qint64 Telegram::messagesSendEncryptedPhoto(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath) {

    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not find any related secret chat to send the photo";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    FileOperation *op = new FileOperation(FileOperation::sendEncryptedFile);
    op->setInputEncryptedChat(inputEncryptedChat);
    op->setRandomId(randomId);
    op->initializeKeyAndIv();
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

qint64 Telegram::messagesSendEncryptedVideo(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QByteArray &thumbnailBytes) {
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not find any related secret chat to send the video";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    FileOperation *op = new FileOperation(FileOperation::sendEncryptedFile);
    op->setInputEncryptedChat(inputEncryptedChat);
    op->setRandomId(randomId);
    op->initializeKeyAndIv();
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

qint64 Telegram::messagesSendEncryptedDocument(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath) {

    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not find any related secret chat to send the document";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    FileOperation *op = new FileOperation(FileOperation::sendEncryptedFile);
    op->setInputEncryptedChat(inputEncryptedChat);
    op->setRandomId(randomId);
    op->initializeKeyAndIv();
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

qint64 Telegram::messagesReceivedQueue(qint32 maxQts) {
    CHECK_API;
    return prv->mApi->messagesReceivedQueue(maxQts);
}

qint64 Telegram::messagesGetStickers(const QString &emoticon, const QString &hash) {
    CHECK_API;
    return prv->mApi->messagesGetStickers(emoticon, hash);
}

qint64 Telegram::messagesGetAllStickers(const QString &hash) {
    CHECK_API;
    return prv->mApi->messagesGetAllStickers(hash);
}

qint64 Telegram::messagesGetStickerSet(const InputStickerSet &stickerset) {
    CHECK_API;
    return prv->mApi->messagesGetStickerSet(stickerset);
}

qint64 Telegram::messagesInstallStickerSet(const InputStickerSet &stickerset) {
    CHECK_API;
    return prv->mApi->messagesInstallStickerSet(stickerset);
}

qint64 Telegram::messagesUninstallStickerSet(const InputStickerSet &stickerset) {
    CHECK_API;
    return prv->mApi->messagesUninstallStickerSet(stickerset);
}

qint64 Telegram::messagesExportChatInvite(qint32 chatId) {
    CHECK_API;
    return prv->mApi->messagesExportChatInvite(chatId);
}

qint64 Telegram::messagesCheckChatInvite(const QString &hash) {
    CHECK_API;
    return prv->mApi->messagesCheckChatInvite(hash);
}

qint64 Telegram::messagesImportChatInvite(const QString &hash) {
    CHECK_API;
    return prv->mApi->messagesImportChatInvite(hash);
}

qint64 Telegram::generateGAorB(SecretChat *secretChat) {
    CHECK_API;
    qCDebug(TG_LIB_SECRET) << "requesting for DH config parameters";
    // call messages.getDhConfig to get p and g for start creating shared key
    qint64 reqId = prv->mApi->messagesGetDhConfig(prv->mSecretState.version(), DH_CONFIG_SERVER_RANDOM_LENGTH);
    // store in secret chats map related to this request id, temporally
    prv->mSecretState.chats().insert(reqId, secretChat);
    return reqId;
}

void Telegram::onMessagesDhConfig(qint64 msgId, qint32 g, const QByteArray &p, qint32 version, const QByteArray &random) {
    qCDebug(TG_LIB_SECRET) << "received new DH parameters g ="<< QString::number(g) << ",p =" << p.toBase64()
                           << ",version =" << version;
    prv->mSecretState.setVersion(version);
    prv->mSecretState.setG(g);
    prv->mSecretState.setP(p);

    if (prv->mCrypto->checkDHParams(prv->mSecretState.p(), g) < 0) {
        qCCritical(TG_TELEGRAM) << "Diffie-Hellman config parameters are not valid";
        return;
    }

    onMessagesDhConfigNotModified(msgId, random);
}

void Telegram::onMessagesDhConfigNotModified(qint64 msgId, const QByteArray &random) {
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
        prv->mApi->messagesRequestEncryption(secretChat->requestedUser(), randomId, gAOrB);
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
        prv->mApi->messagesAcceptEncryption(inputEncryptedChat, gAOrB, keyFingerprint);
        break;
    }
    default:
        Q_ASSERT("Not handled");
        break;
    }

    BN_free(r);
    prv->mSecretState.save();
}

void Telegram::onMessagesRequestEncryptionEncryptedChat(qint64, const EncryptedChat &chat) {
    Q_EMIT messagesCreateEncryptedChatAnswer(chat.id(), chat.date(), chat.participantId(), chat.accessHash());
}

void Telegram::onMessagesAcceptEncryptionEncryptedChat(qint64, const EncryptedChat &chat) {
    qCDebug(TG_LIB_SECRET) << "Joined to secret chat" << chat.id() << "with peer" << chat.adminId();
    SecretChat *secretChat = prv->mSecretState.chats().value(chat.id());
    secretChat->setState(SecretChat::Accepted);
    prv->mSecretState.save();
    Q_EMIT messagesEncryptedChatCreated(chat.id(), chat.date(), chat.adminId(), chat.accessHash());

    //notify peer about our layer
    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(chat.id());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    prv->mEncrypter->setSecretChat(secretChat);
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    QList<qint64> previousMsgs = secretChat->sequence();
    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForNotifyLayer(randomId, CoreTypes::typeLayerVersion);
    QByteArray data = prv->mEncrypter->generateEncryptedData(decryptedMessage);
    prv->mApi->messagesSendEncryptedService(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    prv->mSecretState.save();

    qCDebug(TG_LIB_SECRET) << "Notified our layer:" << CoreTypes::typeLayerVersion;
}

void Telegram::onMessagesDiscardEncryptionResult(qint64 requestId, bool ok) {
    SecretChat *secretChat = prv->mSecretState.chats().take(requestId);
    ASSERT(secretChat);
    qint32 chatId = secretChat->chatId();
    if (ok) {
        prv->mSecretState.chats().remove(chatId);
        prv->mSecretState.save();
        qCDebug(TG_LIB_SECRET) << "Discarded secret chat" << chatId;
        delete secretChat;
        secretChat = 0;
        Q_EMIT messagesEncryptedChatDiscarded(chatId);
    } else {
        qCWarning(TG_LIB_SECRET) << "Could not discard secret chat with id" << chatId;
    }
}

void Telegram::onUpdateShort(const Update &update) {
    processSecretChatUpdate(update);
}

void Telegram::onUpdatesCombined(const QList<Update> &updates) {
    Q_FOREACH (const Update &update, updates) {
        processSecretChatUpdate(update);
    }
}

void Telegram::onUpdates(const QList<Update> &udts) {
    Q_FOREACH (const Update &update, udts) {
        processSecretChatUpdate(update);
    }
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
                prv->mApi->messagesSendEncryptedService(previousMsgs, inputEncryptedChat, randomId, data);

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
void Telegram::onError(qint64 id, qint32 errorCode, const QString &errorText, const QString &functionName) {
    if(errorCode == 400) {
        if(errorText == "ENCRYPTION_ALREADY_DECLINED") { /* This is an already declined chat, remove it from DB */
            this->onMessagesDiscardEncryptionResult(id, true);
            return;
        }
    }
    else if (errorCode == 401) {
        onAuthLogOutAnswer(id, false);
    }

    Q_EMIT error(id, errorCode, errorText, functionName);
}

void Telegram::onErrorRetry(qint64 id, qint32 errorCode, const QString &errorText) {
    // check for error and resend authCheckPhone() request
    if (errorText.contains("_MIGRATE_")) {
        qint32 newDc = errorText.mid(errorText.lastIndexOf("_") + 1).toInt();
        qDebug() << "migrated to dc" << newDc;
        prv->mSettings->setWorkingDcNum(newDc);
        DC *dc = prv->mDcProvider->getDc(newDc);
        prv->mApi->changeMainSessionToDc(dc);
    } else {
        Q_EMIT error(id, errorCode, errorText, QString());
    }
}

void Telegram::onAuthCheckPhoneDcChanged() {
    if (prv->mLastRetryType != PhoneCheck) return;
    authCheckPhone(prv->mLastPhoneChecked);
}
void Telegram::onHelpGetInviteTextDcChanged() {
    if (prv->mLastRetryType != GetInviteText) return;
    helpGetInviteText(prv->mLastLangCode);
}
void Telegram::onImportContactsDcChanged() {
    if (prv->mLastRetryType != ImportContacts)
        return;
    // Retry is hardcoded to not overwrite contacts.
    contactsImportContacts(prv->mLastContacts, false);
}


void Telegram::onUserAuthorized(qint64, qint32 expires, const User &) {
    // change state of current dc
    qint32 workingDcNum = prv->mSettings->workingDcNum();
    DC *dc = prv->mDcProvider->getDc(workingDcNum);
    dc->setState(DC::userSignedIn);
    dc->setExpires(expires);
    QList<DC *> dcsList = prv->mDcProvider->getDcs();
    // save the settings here, after user auth ready in current dc
    prv->mSettings->setDcsList(dcsList);
    prv->mSettings->writeAuthFile();
    // transfer current dc authorization to other dcs
    prv->mDcProvider->transferAuth();
}

void Telegram::onPhotosPhotos(qint64 msgId, const QList<Photo> &photos, const QList<User> &users) {
    Q_EMIT photosGetUserPhotosAnswer(msgId, photos.size(), photos, users);
}

void Telegram::onContactsContacts(qint64 msgId, const QList<Contact> &contacts, const QList<User> &users) {
    prv->m_cachedContacts = contacts;
    prv->m_cachedUsers = users;
    Q_EMIT contactsGetContactsAnswer(msgId, true, contacts, users);
}

void Telegram::onContactsImportContactsAnswer() {
    prv->mLastContacts.clear();
}

void Telegram::onContactsContactsNotModified(qint64 msgId) {
    Q_EMIT contactsGetContactsAnswer(msgId, false, prv->m_cachedContacts, prv->m_cachedUsers);
}

// not direct Responses
void Telegram::onAuthSentCode(qint64 id, bool phoneRegistered, const QString &phoneCodeHash, qint32 sendCallTimeout, bool /* unused isPassword*/) {
    prv->m_phoneCodeHash = phoneCodeHash;
    Q_EMIT authSendCodeAnswer(id, phoneRegistered, sendCallTimeout);
}

void Telegram::onContactsBlocked(qint64 id, const QList<ContactBlocked> &blocked, const QList<User> &users) {
    Q_EMIT contactsGetBlockedAnswer(id, blocked.size(), blocked, users);
}

void Telegram::onMessagesSentMessage(qint64 id, qint32 msgId, qint32 date, const MessageMedia &media, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesSendMessageAnswer(id, msgId, date, media, pts, pts_count, seq, links);
}

void Telegram::onMessagesSendMediaAnswer(qint64 fileId, const UpdatesType &updates) {
    //depending on responded media, emit one signal or another
    const int mediaType = prv->pendingMediaSends.take(fileId);
    switch (mediaType) {
    case MessageMedia::typeMessageMediaPhoto:
        Q_EMIT messagesSendPhotoAnswer(fileId, updates);
        break;
    case MessageMedia::typeMessageMediaVideo:
        Q_EMIT messagesSendVideoAnswer(fileId, updates);
        break;
    case MessageMedia::typeMessageMediaGeo:
        Q_EMIT messagesSendGeoPointAnswer(fileId, updates);
        break;
    case MessageMedia::typeMessageMediaContact:
        Q_EMIT messagesSendContactAnswer(fileId, updates);
        break;
    case MessageMedia::typeMessageMediaDocument:
        Q_EMIT messagesSendDocumentAnswer(fileId, updates);
        break;
    case MessageMedia::typeMessageMediaAudio:
        Q_EMIT messagesSendAudioAnswer(fileId, updates);
        break;
    default:
        Q_EMIT messagesSendMediaAnswer(fileId, updates);
    }
}

void Telegram::onMessagesForwardMediaAnswer(qint64 msgId, const UpdatesType &updates)
{
    const int mediaType = prv->pendingForwards.take(msgId);
    switch (mediaType) {
    case MessageMedia::typeMessageMediaPhoto:
        Q_EMIT messagesForwardPhotoAnswer(msgId, updates);
        break;
    case MessageMedia::typeMessageMediaVideo:
        Q_EMIT messagesForwardVideoAnswer(msgId, updates);
        break;
    case MessageMedia::typeMessageMediaDocument:
        Q_EMIT messagesForwardDocumentAnswer(msgId, updates);
        break;
    case MessageMedia::typeMessageMediaAudio:
        Q_EMIT messagesForwardAudioAnswer(msgId, updates);
        break;
    default:
        Q_EMIT messagesForwardMediaAnswer(msgId, updates);
    }
}

void Telegram::onMessagesGetMessagesMessages(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users) {
    Q_EMIT messagesGetMessagesAnswer(id, messages.size(), messages, chats, users);
}

void Telegram::onMessagesDialogs(qint64 id, const QList<Dialog> &dialogs, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users) {
    Q_EMIT messagesGetDialogsAnswer(id, dialogs.size(), dialogs, messages, chats, users);
}

void Telegram::onMessagesGetHistoryMessages(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users) {
    Q_EMIT messagesGetHistoryAnswer(id, messages.size(), messages, chats, users);
}

void Telegram::onMessagesSearchMessages(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users) {
    Q_EMIT messagesSearchAnswer(id, messages.size(), messages, chats, users);
}

void Telegram::onUpdatesDifference(qint64 id, const QList<Message> &messages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state) {
    processDifferences(id, messages, newEncryptedMessages, otherUpdates, chats, users, state, false);
}

void Telegram::onUpdatesDifferenceSlice(qint64 id, const QList<Message> &messages, const QList<EncryptedMessage> &newEncryptedMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state) {
    processDifferences(id, messages, newEncryptedMessages, otherUpdates, chats, users, state, true);
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

// Requests
qint64 Telegram::helpGetInviteText(const QString &langCode) {
    CHECK_API;
    prv->mLastLangCode = langCode;
    prv->mLastRetryType = GetInviteText;
    return prv->mApi->helpGetInviteText(langCode);
}

qint64 Telegram::authCheckPhone() {
   return authCheckPhone(prv->mSettings->phoneNumber());
}
qint64 Telegram::authCheckPhone(const QString &phoneNumber) {
    CHECK_API;
    prv->mLastRetryType = PhoneCheck;
    prv->mLastPhoneChecked = phoneNumber;
    return prv->mApi->authCheckPhone(phoneNumber);
}
qint64 Telegram::authSendCode() {
    CHECK_API;
    return prv->mApi->authSendCode(prv->mSettings->phoneNumber(), 0, prv->mSettings->appId(), prv->mSettings->appHash(), LANG_CODE);
}

qint64 Telegram::authSendSms() {
    CHECK_API;
    return prv->mApi->authSendSms(prv->mSettings->phoneNumber(), prv->m_phoneCodeHash);
}

qint64 Telegram::authSendCall() {
    CHECK_API;
    return prv->mApi->authSendCall(prv->mSettings->phoneNumber(), prv->m_phoneCodeHash);
}

qint64 Telegram::authSignIn(const QString &code) {
    CHECK_API;
    return prv->mApi->authSignIn(prv->mSettings->phoneNumber(), prv->m_phoneCodeHash, code);
}

qint64 Telegram::authSignUp(const QString &code, const QString &firstName, const QString &lastName) {
    CHECK_API;
    return prv->mApi->authSignUp(prv->mSettings->phoneNumber(), prv->m_phoneCodeHash, code, firstName, lastName);
}

qint64 Telegram::authLogOut() {
    CHECK_API;
    return prv->mApi->authLogOut();
}

qint64 Telegram::authSendInvites(const QStringList &phoneNumbers, const QString &inviteText) {
    CHECK_API;
    return prv->mApi->authSendInvites(phoneNumbers, inviteText);
}

qint64 Telegram::authResetAuthorizations() {
    CHECK_API;
    return prv->mApi->authResetAuthorizations();
}

qint64 Telegram::authCheckPassword(const QByteArray &passwordHash) {
    CHECK_API;
    return prv->mApi->authCheckPassword(passwordHash);
}

qint64 Telegram::accountRegisterDevice(const QString &token, const QString &appVersion, bool appSandbox) {
    CHECK_API;
    if (token.length() == 0) {
        qCCritical(TG_TELEGRAM) << "refuse to register with empty token!";
        return -1;
    }
    QString version = appVersion;
    if (!version.length()) {
        version = Utils::getAppVersion();
    }
    qCDebug(TG_TELEGRAM) << "registering device for push - app version" << version;
    return prv->mApi->accountRegisterDevice(UBUNTU_PHONE_TOKEN_TYPE, token, Utils::getDeviceModel(), Utils::getSystemVersion(), version, appSandbox, prv->mSettings->langCode());
}

qint64 Telegram::accountUnregisterDevice(const QString &token) {
    CHECK_API;
    return prv->mApi->accountUnregisterDevice(UBUNTU_PHONE_TOKEN_TYPE, token);
}

qint64 Telegram::accountUpdateNotifySettings(const InputNotifyPeer &peer, const InputPeerNotifySettings &settings) {
    CHECK_API;
    return prv->mApi->accountUpdateNotifySettings(peer, settings);
}

qint64 Telegram::accountGetNotifySettings(const InputNotifyPeer &peer) {
    CHECK_API;
    return prv->mApi->accountGetNotifySettings(peer);
}

qint64 Telegram::accountResetNotifySettings() {
    CHECK_API;
    return prv->mApi->accountResetNotifySettings();
}

qint64 Telegram::accountUpdateProfile(const QString &firstName, const QString &lastName) {
    CHECK_API;
    return prv->mApi->accountUpdateProfile(firstName, lastName);
}

qint64 Telegram::accountUpdateStatus(bool offline) {
    CHECK_API;
    return prv->mApi->accountUpdateStatus(offline);
}

qint64 Telegram::accountGetWallPapers() {
    CHECK_API;
    return prv->mApi->accountGetWallPapers();
}

qint64 Telegram::accountCheckUsername(const QString &username) {
    CHECK_API;
    return prv->mApi->accountCheckUsername(username);
}

qint64 Telegram::accountUpdateUsername(const QString &username) {
    CHECK_API;
    return prv->mApi->accountUpdateUsername(username);
}

qint64 Telegram::accountGetPrivacy(const InputPrivacyKey &key) {
    CHECK_API;
    return prv->mApi->accountGetPrivacy(key);
}

qint64 Telegram::accountSetPrivacy(const InputPrivacyKey &key, const QList<InputPrivacyRule> &rules) {
    CHECK_API;
    return prv->mApi->accountSetPrivacy(key, rules);
}

qint64 Telegram::accountDeleteAccount(const QString &reason) {
    CHECK_API;
    return prv->mApi->accountDeleteAccount(reason);
}

qint64 Telegram::accountGetAccountTTL() {
    CHECK_API;
    return prv->mApi->accountGetAccountTTL();
}

qint64 Telegram::accountSetAccountTTL(const AccountDaysTTL &ttl) {
    CHECK_API;
    return prv->mApi->accountSetAccountTTL(ttl);
}

qint64 Telegram::accountUpdateDeviceLocked(int period) {
    CHECK_API;
    return prv->mApi->accountUpdateDeviceLocked(period);
}

qint64 Telegram::accountSendChangePhoneCode(const QString &phone_number) {
    CHECK_API;
    return prv->mApi->accountSendChangePhoneCode(phone_number);
}

qint64 Telegram::accountChangePhone(const QString &phone_number, const QString &phone_code_hash, const QString &phone_code) {
    CHECK_API;
    return prv->mApi->accountChangePhone(phone_number, phone_code_hash, phone_code);
}

qint64 Telegram::accountGetPassword() {
    CHECK_API;
    return prv->mApi->accountGetPassword();
}

qint64 Telegram::accountGetAuthorizations() {
    CHECK_API;
    return prv->mApi->accountGetAuthorizations();
}

qint64 Telegram::accountResetAuthorization(qint64 hash) {
    CHECK_API;
    return prv->mApi->accountResetAuthorization(hash);
}

qint64 Telegram::accountGetPasswordSettings(const QByteArray &currentPasswordHash) {
    CHECK_API;
    return prv->mApi->accountGetPasswordSettings(currentPasswordHash);
}

qint64 Telegram::accountUpdatePasswordSettings(const QByteArray &currentPasswordHash, const AccountPasswordInputSettings &newSettings) {
    CHECK_API;
    return prv->mApi->accountUpdatePasswordSettings(currentPasswordHash, newSettings);
}

qint64 Telegram::photosUploadProfilePhoto(const QByteArray &bytes, const QString &fileName, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop) {
    FileOperation *op = new FileOperation(FileOperation::uploadProfilePhoto);
    op->setCaption(caption);
    op->setGeoPoint(geoPoint);
    op->setCrop(crop);
    return prv->mFileHandler->uploadSendFile(*op, fileName, bytes);
}

qint64 Telegram::photosUploadProfilePhoto(const QString &filePath, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop) {
    FileOperation *op = new FileOperation(FileOperation::uploadProfilePhoto);
    op->setCaption(caption);
    op->setGeoPoint(geoPoint);
    op->setCrop(crop);
    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::photosUpdateProfilePhoto(qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop) {
    CHECK_API;
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    return prv->mApi->photosUpdateProfilePhoto(inputPhoto, crop);
}

qint64 Telegram::usersGetUsers(const QList<InputUser> &users) {
    CHECK_API;
    return prv->mApi->usersGetUsers(users);
}

qint64 Telegram::usersGetFullUser(const InputUser &user) {
    CHECK_API;
    return prv->mApi->usersGetFullUser(user);
}

qint64 Telegram::photosGetUserPhotos(const InputUser &user, qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_API;
    return prv->mApi->photosGetUserPhotos(user, offset, maxId, limit);
}

qint64 Telegram::contactsGetStatuses() {
    CHECK_API;
    return prv->mApi->contactsGetStatuses();
}

bool lessThan(const Contact &c1, const Contact &c2) {
    return c1.userId() < c2.userId();
}

qint64 Telegram::contactsGetContacts() {
    //If there already is a full contact list on the client, an md5-hash of a comma-separated list of contact IDs
    //in ascending order may be passed in this 'hash' parameter. If the contact set was not changed,
    //contactsContactsNotModified() will be returned from Api, so the cached client list is returned with the
    //signal that they are the same contacts as previous request
    CHECK_API;
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
    return prv->mApi->contactsGetContacts(hash);
}

qint64 Telegram::contactsImportContacts(const QList<InputContact> &contacts, bool replace) {
    CHECK_API;
    prv->mLastContacts = contacts;
    prv->mLastRetryType = ImportContacts;
    return prv->mApi->contactsImportContacts(contacts, replace);
}

qint64 Telegram::contactsDeleteContact(const InputUser &user) {
    CHECK_API;
    return prv->mApi->contactsDeleteContact(user);
}

qint64 Telegram::contactsDeleteContacts(const QList<InputUser> &users) {
    CHECK_API;
    return prv->mApi->contactsDeleteContacts(users);
}

qint64 Telegram::contactsSearch(const QString &q, qint32 limit) {
    CHECK_API;
    return prv->mApi->contactsSearch(q, limit);
}

qint64 Telegram::contactsResolveUsername(const QString &username) {
    CHECK_API;
    return prv->mApi->contactsResolveUsername(username);
}

qint64 Telegram::contactsBlock(const InputUser &user) {
    CHECK_API;
    return prv->mApi->contactsBlock(user);
}

qint64 Telegram::contactsUnblock(const InputUser &user) {
    CHECK_API;
    return prv->mApi->contactsUnblock(user);
}

qint64 Telegram::contactsGetBlocked(qint32 offset, qint32 limit) {
    CHECK_API;
    return prv->mApi->contactsGetBlocked(offset, limit);
}

qint64 Telegram::messagesSendMessage(const InputPeer &peer, qint64 randomId, const QString &message, int replyToMsgId) {
    CHECK_API;
    return prv->mApi->messagesSendMessage(peer, message, randomId, replyToMsgId);
}

qint64 Telegram::messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedPhoto);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes);
}

qint64 Telegram::messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedPhoto);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return uploadSendFile(*op, inputMedia.classType(), filePath);
}

qint64 Telegram::messagesSendGeoPoint(const InputPeer &peer, qint64 randomId, const InputGeoPoint &inputGeoPoint, qint32 replyToMsgId) {
    CHECK_API;
    InputMedia inputMedia(InputMedia::typeInputMediaGeoPoint);
    inputMedia.setGeoPoint(inputGeoPoint);
    qint64 msgId = prv->mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
    prv->pendingMediaSends[msgId] = inputMedia.classType();
    return msgId;
}

qint64 Telegram::messagesSendContact(const InputPeer &peer, qint64 randomId, const QString &phoneNumber, const QString &firstName, const QString &lastName, qint32 replyToMsgId) {
    CHECK_API;
    InputMedia inputMedia(InputMedia::typeInputMediaContact);
    inputMedia.setPhoneNumber(phoneNumber);
    inputMedia.setFirstName(firstName);
    inputMedia.setLastName(lastName);
    qint64 msgId = prv->mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
    prv->pendingMediaSends[msgId] = inputMedia.classType();
    return msgId;
}

qint64 Telegram::messagesSendVideo(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, qint32 width, qint32 height, const QString &mimeType, const QByteArray &thumbnailBytes, const QString &thumbnailName, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedVideo);
    inputMedia.setDuration(duration);
    inputMedia.setW(width);
    inputMedia.setH(height);
    inputMedia.setMimeType(mimeType);
    if (!thumbnailBytes.isEmpty()) {
        inputMedia.setClassType(InputMedia::typeInputMediaUploadedThumbVideo);
    }
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes, thumbnailBytes, thumbnailName);
}

qint64 Telegram::messagesSendVideo(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QString &thumbnailFilePath, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedVideo);
    inputMedia.setDuration(duration);
    inputMedia.setW(width);
    inputMedia.setH(height);
    inputMedia.setMimeType(QMimeDatabase().mimeTypeForFile(QFileInfo(filePath)).name());
    if (thumbnailFilePath.length() > 0) {
        inputMedia.setClassType(InputMedia::typeInputMediaUploadedThumbVideo);
    }
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return uploadSendFile(*op, inputMedia.classType(), filePath, thumbnailFilePath);
}

qint64 Telegram::messagesSendAudio(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 duration, const QString &mimeType, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedAudio);
    inputMedia.setDuration(duration);
    inputMedia.setMimeType(mimeType);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes);
}

qint64 Telegram::messagesSendAudio(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 duration, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedAudio);
    inputMedia.setDuration(duration);
    inputMedia.setMimeType(QMimeDatabase().mimeTypeForFile(QFileInfo(filePath)).name());
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return uploadSendFile(*op, inputMedia.classType(), filePath);
}

qint64 Telegram::messagesSendDocument(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, const QString &mimeType, const QByteArray &thumbnailBytes, const QString &thumbnailName, const QList<DocumentAttribute> &extraAttributes, qint32 replyToMsgId) {
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
    return uploadSendFile(*op, inputMedia.classType(), fileName, bytes, thumbnailBytes, thumbnailName);
}

qint64 Telegram::messagesSendDocument(const InputPeer &peer, qint64 randomId, const QString &filePath, const QString &thumbnailFilePath, bool sendAsSticker, qint32 replyToMsgId) {
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
    return uploadSendFile(*op, inputMedia.classType(), filePath, thumbnailFilePath);
}

qint64 Telegram::messagesForwardPhoto(const InputPeer &peer, qint64 randomId, qint64 photoId, qint64 accessHash, qint32 replyToMsgId) {
    CHECK_API;
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaPhoto);
    inputMedia.setIdInputPhoto(inputPhoto);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardVideo(const InputPeer &peer, qint64 randomId, qint64 videoId, qint64 accessHash, qint32 replyToMsgId) {
    CHECK_API;
    InputVideo inputVideo(InputVideo::typeInputVideo);
    inputVideo.setId(videoId);
    inputVideo.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaVideo);
    inputMedia.setIdInputVideo(inputVideo);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardAudio(const InputPeer &peer, qint64 randomId, qint64 audioId, qint64 accessHash, qint32 replyToMsgId) {
    CHECK_API;
    InputAudio inputAudio(InputAudio::typeInputAudio);
    inputAudio.setId(audioId);
    inputAudio.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaAudio);
    inputMedia.setIdInputAudio(inputAudio);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardDocument(const InputPeer &peer, qint64 randomId, qint64 documentId, qint64 accessHash, qint32 replyToMsgId) {
    CHECK_API;
    InputDocument inputDocument(InputDocument::typeInputDocument);
    inputDocument.setId(documentId);
    inputDocument.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaDocument);
    inputMedia.setIdInputDocument(inputDocument);
    return messagesForwardMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId) {
    const qint64 msgId =  prv->mApi->messagesForwardMedia(peer, media, randomId, replyToMsgId);
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

qint64 Telegram::messagesSetEncryptedTyping(qint32 chatId, bool typing) {
    CHECK_API;
    SecretChat *secretChat = prv->mSecretState.chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not read history of a not yet existant chat";
        return -1;
    }
    InputEncryptedChat mChat;
    mChat.setChatId(chatId);
    mChat.setAccessHash(secretChat->accessHash());
    return prv->mApi->messagesSetEncryptedTyping(mChat,typing);
}

qint64 Telegram::messagesSetTyping(const InputPeer &peer, const SendMessageAction &action) {
    CHECK_API;
    return prv->mApi->messagesSetTyping(peer, action);
}

qint64 Telegram::messagesGetMessages(const QList<qint32> &msgIds) {
    CHECK_API;
    return prv->mApi->messagesGetMessages(msgIds);
}

qint64 Telegram::messagesGetDialogs(qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_API;
    return prv->mApi->messagesGetDialogs(offset, maxId, limit);
}

qint64 Telegram::messagesGetHistory(const InputPeer &peer, qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_API;
    return prv->mApi->messagesGetHistory(peer, offset, maxId, limit);
}

qint64 Telegram::messagesSearch(const InputPeer &peer, const QString &query, const MessagesFilter &filter, qint32 minDate, qint32 maxDate, qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_API;
    return prv->mApi->messagesSearch(peer, query, filter, minDate, maxDate, offset, maxId, limit);
}

qint64 Telegram::messagesReadHistory(const InputPeer &peer, qint32 maxId, qint32 offset) {
    CHECK_API;
    return prv->mApi->messagesReadHistory(peer, maxId, offset);
}

qint64 Telegram::messagesReadMessageContents(const QList<qint32> &ids) {
    CHECK_API;
    return prv->mApi->messagesReadMessageContents(ids);
}

qint64 Telegram::messagesDeleteHistory(const InputPeer &peer, qint32 offset) {
    CHECK_API;
    return prv->mApi->messagesDeleteHistory(peer, offset);
}

qint64 Telegram::messagesDeleteMessages(const QList<qint32> &msgIds) {
    CHECK_API;
    return prv->mApi->messagesDeleteMessages(msgIds);
}

qint64 Telegram::messagesReceivedMessages(qint32 maxId) {
    CHECK_API;
    return prv->mApi->messagesReceivedMessages(maxId);
}

qint64 Telegram::messagesForwardMessage(const InputPeer &peer, qint32 msgId) {
    CHECK_API;
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    return prv->mApi->messagesForwardMessage(peer, msgId, randomId);
}

qint64 Telegram::messagesForwardMessages(const InputPeer &peer, const QList<qint32> &msgIds, const QList<qint64> &randomIds) {
    CHECK_API;
    return prv->mApi->messagesForwardMessages(peer, msgIds, randomIds);
}

qint64 Telegram::messagesSendBroadcast(const QList<InputUser> &users, const QList<qint64> &randomIds, const QString &message, const InputMedia &media) {
    CHECK_API;
    return prv->mApi->messagesSendBroadcast(users, randomIds, message, media);
}

qint64 Telegram::messagesGetChats(const QList<qint32> &chatIds) {
    CHECK_API;
    return prv->mApi->messagesGetChats(chatIds);
}

qint64 Telegram::messagesGetFullChat(qint32 chatId) {
    CHECK_API;
    return prv->mApi->messagesGetFullChat(chatId);
}

qint64 Telegram::messagesEditChatTitle(qint32 chatId, const QString &title) {
    CHECK_API;
    return prv->mApi->messagesEditChatTitle(chatId, title);
}

qint64 Telegram::messagesEditChatPhoto(qint32 chatId, const QString &filePath, const InputPhotoCrop &crop) {
    InputChatPhoto inputChatPhoto(InputChatPhoto::typeInputChatUploadedPhoto);
    inputChatPhoto.setCrop(crop);
    FileOperation *op = new FileOperation(FileOperation::editChatPhoto);
    op->setChatId(chatId);
    op->setInputChatPhoto(inputChatPhoto);
    return prv->mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesEditChatPhoto(qint32 chatId, qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop) {
    CHECK_API;
    InputChatPhoto inputChatPhoto(InputChatPhoto::typeInputChatPhoto);
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    inputChatPhoto.setId(inputPhoto);
    inputChatPhoto.setCrop(crop);
    return prv->mApi->messagesEditChatPhoto(chatId, inputChatPhoto);
}

qint64 Telegram::messagesAddChatUser(qint32 chatId, const InputUser &user, qint32 fwdLimit) {
    CHECK_API;
    return prv->mApi->messagesAddChatUser(chatId, user, fwdLimit);
}

qint64 Telegram::messagesDeleteChatUser(qint32 chatId, const InputUser &user) {
    CHECK_API;
    return prv->mApi->messagesDeleteChatUser(chatId, user);
}

qint64 Telegram::messagesCreateChat(const QList<InputUser> &users, const QString &chatTopic) {
    CHECK_API;
    return prv->mApi->messagesCreateChat(users, chatTopic);
}

qint64 Telegram::updatesGetState() {
    CHECK_API;
    return prv->mApi->updatesGetState();
}

qint64 Telegram::updatesGetDifference(qint32 pts, qint32 date, qint32 qts) {
    CHECK_API;
    return prv->mApi->updatesGetDifference(pts, date, qts);
}

qint64 Telegram::uploadGetFile(const InputFileLocation &location, qint32 fileSize, qint32 dcNum, const QByteArray &key, const QByteArray &iv) {
    if(!prv->mFileHandler) return 0;
    return prv->mFileHandler->uploadGetFile(location, fileSize, dcNum, key, iv);
}

qint64 Telegram::uploadCancelFile(qint64 fileId) {
    if(!prv->mFileHandler) return 0;
    return prv->mFileHandler->uploadCancelFile(fileId);
}
