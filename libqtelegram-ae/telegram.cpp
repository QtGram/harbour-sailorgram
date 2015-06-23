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

#include "core/dcprovider.h"
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

Q_LOGGING_CATEGORY(TG_TELEGRAM, "tg.telegram")
Q_LOGGING_CATEGORY(TG_LIB_SECRET, "tg.lib.secret")

QHash<QString, Settings*> qtelegram_settings_per_number;
QHash<QString, CryptoUtils*> qtelegram_cryptos_per_number;

Telegram::Telegram(const QString &defaultHostAddress, qint16 defaultHostPort, qint16 defaultHostDcId, qint32 appId, const QString &appHash, const QString &phoneNumber, const QString &configPath, const QString &publicKeyFile) :
    mLibraryState(LoggedOut),
    mLastRetryType(NotRetry),
    mSlept(false),
    mApi(0) {
    // Qt5.2 doesn't support .ini files to control logging, so use this
    // manual workaround instead.
    // http://blog.qt.digia.com/blog/2014/03/11/qt-weekly-1-categorized-logging/
    QLoggingCategory::setFilterRules(QString(qgetenv("QT_LOGGING_RULES")));

    mSettingsId = defaultHostAddress + ":" + QString::number(defaultHostPort) + ":" + configPath +
            ":" + QString::number(defaultHostPort) + ":" + QString::number(appId) + ":" + appHash +
            phoneNumber;
    mSettings = qtelegram_settings_per_number.value(mSettingsId);
    if(!mSettings) {
        mSettings = new Settings();
        mSettings->setAppHash(appHash);
        mSettings->setAppId(appId);
        mSettings->setDefaultHostAddress(defaultHostAddress);
        mSettings->setDefaultHostDcId(defaultHostDcId);
        mSettings->setDefaultHostPort(defaultHostPort);
        qtelegram_settings_per_number[mSettingsId] = mSettings;
    }

    // load settings
    if (!mSettings->loadSettings(phoneNumber, configPath, publicKeyFile)) {
        throw std::runtime_error("loadSettings failure");
    }

    mCrypto = qtelegram_cryptos_per_number.value(phoneNumber);
    if(!mCrypto) {
        mCrypto = new CryptoUtils(mSettings);
        qtelegram_cryptos_per_number[phoneNumber] = mCrypto;
    }

    mDcProvider = new DcProvider(mSettings, mCrypto);
    mDcProvider->setParent(this);

    mSecretState = new SecretState(mSettings, this);
    mEncrypter = new Encrypter(mSettings);
    mDecrypter = new Decrypter(mSettings);

    connect(mDecrypter, SIGNAL(sequenceNumberGap(qint32,qint32,qint32)), SLOT(onSequenceNumberGap(qint32,qint32,qint32)));

    mSecretState->load();

    connect(mDcProvider, SIGNAL(fatalError()), this, SIGNAL(fatalError()));
    // activate dc provider ready signal
    connect(mDcProvider, SIGNAL(dcProviderReady()), this, SLOT(onDcProviderReady()));
    // activate rest of dc provider signal connections
    connect(mDcProvider, SIGNAL(authNeeded()), this, SIGNAL(authNeeded()));
    connect(mDcProvider, SIGNAL(authTransferCompleted()), this, SLOT(onAuthLoggedIn()));
    connect(mDcProvider, SIGNAL(error(qint64,qint32,const QString&)), this, SIGNAL(error(qint64,qint32,const QString&)));
}

bool Telegram::sleep() {
    // sleep only if not slept and library already logged in. Returns true if sleep operations completes
    if (!mSlept && mLibraryState >= LoggedIn) {
        if (mApi && mApi->mainSession()) {
            mApi->mainSession()->close();
        }
        mSlept = true;
        return true;
    }
    return false;
}

bool Telegram::wake() {
    // wake only if slept and library already logged in. Returns true if wake operation completes
    if (mSlept && mLibraryState >= LoggedIn) {
        connect(mApi, SIGNAL(mainSessionReady()), this, SIGNAL(woken()), Qt::UniqueConnection);
        DC *dc = mDcProvider->getWorkingDc();
        mApi->createMainSessionToDc(dc);
        mSlept = false;
        return true;
    }
    return false;
}

void Telegram::setPhoneNumber(const QString &phoneNumber) {
    if (!mSettings->loadSettings(phoneNumber)) {
        throw std::runtime_error("setPhoneNumber: could not load settings");
    }
    mSecretState->load();
}

void Telegram::init() {
    // check the auth values stored in settings, check the available DCs config data if there is
    // connection to servers, and emit signals depending on user authenticated or not.
    mDcProvider->initialize();
}

Telegram::~Telegram() {
}

QString Telegram::defaultHostAddress()
{
    return mSettings->defaultHostAddress();
}

qint16 Telegram::defaultHostPort()
{
    return mSettings->defaultHostPort();
}

qint16 Telegram::defaultHostDcId()
{
    return mSettings->defaultHostDcId();
}

qint32 Telegram::appId()
{
    return mSettings->appId();
}

QString Telegram::appHash()
{
    return mSettings->appHash();
}

Settings *Telegram::settings() const
{
    return mSettings;
}

CryptoUtils *Telegram::crypto() const
{
    return mCrypto;
}

bool Telegram::isConnected() {
    if (mApi && mApi->mainSession()) {
        return mApi->mainSession()->state() == QAbstractSocket::ConnectedState;
    }
    return false;
}

bool Telegram::isLoggedIn() {
    return mLibraryState == LoggedIn;
}

void Telegram::onAuthLoggedIn() {
    mLibraryState = LoggedIn;
    Q_EMIT authLoggedIn();
}

void Telegram::onAuthLogOutAnswer(qint64 id, bool ok) {
    mDcProvider->logOut();
    mLibraryState = LoggedOut;
    Q_EMIT authLogOutAnswer(id,ok);
}

qint32 Telegram::ourId() {
    return mSettings->ourId();
}

void Telegram::onDcProviderReady() {
    mLibraryState = CreatedSharedKeys;
    mApi = mDcProvider->getApi();
    // api signal-signal and signal-slot connections
    // updates
    connect(mApi, SIGNAL(updatesTooLong()), this, SIGNAL(updatesTooLong()));
    connect(mApi, SIGNAL(updateShortMessage(qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)), this, SIGNAL(updateShortMessage(qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)));
    connect(mApi, SIGNAL(updateShortChatMessage(qint32,qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)), this, SIGNAL(updateShortChatMessage(qint32,qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)));
    connect(mApi, SIGNAL(updateShort(const Update&,qint32)), this, SIGNAL(updateShort(const Update&,qint32)));
    connect(mApi, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)), this, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)), this, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)));
    // errors
    connect(mApi, SIGNAL(error(qint64,qint32,const QString&,const QString&)), this, SLOT(onError(qint64,qint32,const QString&,const QString&)));
    connect(mApi, SIGNAL(errorRetry(qint64,qint32,const QString&)), this, SLOT(onErrorRetry(qint64,qint32,const QString&)));
    connect(mApi, SIGNAL(authSignInError(qint64,qint32,const QString&)), this, SIGNAL(authSignInError(qint64,qint32,const QString&)));
    connect(mApi, SIGNAL(authSignUpError(qint64,qint32,const QString&)), this, SIGNAL(authSignUpError(qint64,qint32,const QString&)));
    // positive responses
    connect(mApi, SIGNAL(helpGetInviteTextAnswer(qint64,const QString&)), this, SIGNAL(helpGetInviteTextAnswer(qint64,const QString&)));
    connect(mApi, SIGNAL(authCheckedPhone(qint64,bool)), this, SIGNAL(authCheckPhoneAnswer(qint64,bool)));
    connect(mApi, SIGNAL(authCheckPhoneSent(qint64,const QString&)), this, SIGNAL(authCheckPhoneSent(qint64,const QString&)));
    connect(mApi, SIGNAL(authSentCode(qint64,bool,const QString&,qint32,bool)), this, SLOT(onAuthSentCode(qint64,bool,const QString&,qint32,bool)));
    connect(mApi, SIGNAL(authSentAppCode(qint64,bool,const QString&,qint32,bool)), this, SLOT(onAuthSentCode(qint64,bool,const QString&,qint32,bool)));
    connect(mApi, SIGNAL(authSendSmsResult(qint64,bool)), this, SIGNAL(authSendSmsAnswer(qint64,bool)));
    connect(mApi, SIGNAL(authSendCallResult(qint64,bool)), this, SIGNAL(authSendCallAnswer(qint64,bool)));
    connect(mApi, SIGNAL(authSignInAuthorization(qint64,qint32,const User&)), this, SLOT(onUserAuthorized(qint64,qint32,const User&)));
    connect(mApi, SIGNAL(authSignUpAuthorization(qint64,qint32,const User&)), this, SLOT(onUserAuthorized(qint64,qint32,const User&)));
    connect(mApi, SIGNAL(authLogOutResult(qint64,bool)), this, SLOT(onAuthLogOutAnswer(qint64,bool)));
    connect(mApi, SIGNAL(authSendInvitesResult(qint64,bool)), this, SIGNAL(authSendInvitesAnswer(qint64,bool)));
    connect(mApi, SIGNAL(authResetAuthorizationsResult(qint64,bool)), this, SIGNAL(authResetAuthorizationsAnswer(qint64,bool)));
    connect(mApi, SIGNAL(authCheckPasswordResult(qint64,qint32,const User&)), this, SIGNAL(authCheckPasswordAnswer(qint64,qint32,const User&)));
    connect(mApi, SIGNAL(accountRegisterDeviceResult(qint64,bool)), this, SIGNAL(accountRegisterDeviceAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountUnregisterDeviceResult(qint64,bool)), this, SIGNAL(accountUnregisterDeviceAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountUpdateNotifySettingsResult(qint64,bool)), this, SIGNAL(accountUpdateNotifySettingsAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountPeerNotifySettings(qint64,const PeerNotifySettings&)), this, SIGNAL(accountGetNotifySettingsAnswer(qint64,const PeerNotifySettings&)));
    connect(mApi, SIGNAL(accountResetNotifySettingsResult(qint64,bool)), this, SIGNAL(accountResetNotifySettingsAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountUser(qint64,const User&)), this, SIGNAL(accountUpdateProfileAnswer(qint64,const User&)));
    connect(mApi, SIGNAL(accountUpdateStatusResult(qint64,bool)), this, SIGNAL(accountUpdateStatusAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountGetWallPapersResult(qint64,const QList<WallPaper>&)), this, SIGNAL(accountGetWallPapersAnswer(qint64,const QList<WallPaper>&)));
    connect(mApi, SIGNAL(accountCheckUsernameResult(qint64,bool)), this, SIGNAL(accountCheckUsernameAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountUpdateUsernameResult(qint64,const User&)), this, SIGNAL(accountUpdateUsernameAnswer(qint64,const User&)));
    connect(mApi, SIGNAL(accountPrivacyRules(qint64,const QList<PrivacyRule>&,const QList<User>&)), this, SIGNAL(accountPrivacyRules(qint64,const QList<PrivacyRule>&,const QList<User>&)));
    connect(mApi, SIGNAL(accountDeleteAccountResult(qint64,bool)), this, SIGNAL(accountDeleteAccountAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountGetAccountTTLResult(qint64,const AccountDaysTTL&)), this, SIGNAL(accountGetAccountTTLAnswer(qint64,const AccountDaysTTL&)));
    connect(mApi, SIGNAL(accountSetAccountTTLResult(qint64,bool)), this, SIGNAL(accountSetAccountTTLAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountUpdateDeviceLockedResult(qint64,bool)), this, SIGNAL(accountUpdateDeviceLockedAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountSentChangePhoneCode(qint64,const QString&,qint32)), this, SIGNAL(accountSentChangePhoneCode(qint64,const QString&,qint32)));
    connect(mApi, SIGNAL(accountSetPasswordResult(qint64,bool)), this, SIGNAL(accountSetPasswordAnswer(qint64,bool)));
    connect(mApi, SIGNAL(accountGetPasswordResult(qint64,const AccountPassword&)), this, SIGNAL(accountGetPasswordAnswer(qint64,const AccountPassword&)));
    connect(mApi, SIGNAL(accountChangePhoneResult(qint64,const User&)), this, SIGNAL(accountChangePhoneAnswer(qint64,const User&)));
    connect(mApi, SIGNAL(photosPhoto(qint64,const Photo&,const QList<User>&)), this, SIGNAL(photosUploadProfilePhotoAnswer(qint64,const Photo&,const QList<User>&)));
    connect(mApi, SIGNAL(photosUserProfilePhoto(qint64,const UserProfilePhoto&)), this, SIGNAL(photosUpdateProfilePhotoAnswer(qint64,const UserProfilePhoto&)));
    connect(mApi, SIGNAL(usersGetUsersResult(qint64,const QList<User>&)), this, SIGNAL(usersGetUsersAnswer(qint64,const QList<User>&)));
    connect(mApi, SIGNAL(userFull(qint64,const User&,const ContactsLink&,const Photo&,const PeerNotifySettings&,bool,const QString&,const QString&)), this, SIGNAL(usersGetFullUserAnswer(qint64,const User&,const ContactsLink&,const Photo&,const PeerNotifySettings&,bool,const QString&,const QString&)));
    connect(mApi, SIGNAL(photosPhotos(qint64,const QList<Photo>&,const QList<User>&)), this, SLOT(onPhotosPhotos(qint64, const QList<Photo>&, const QList<User>&)));
    connect(mApi, SIGNAL(photosPhotosSlice(qint64,qint32,const QList<Photo>&,const QList<User>&)), this, SIGNAL(photosGetUserPhotosAnswer(qint64,qint32,const QList<Photo>&,const QList<User>&)));
    connect(mApi, SIGNAL(contactsGetStatusesResult(qint64,const QList<ContactStatus>&)), this, SIGNAL(contactsGetStatusesAnswer(qint64,const QList<ContactStatus>&)));
    connect(mApi, SIGNAL(contactsContacts(qint64,const QList<Contact>&,const QList<User>&)), this, SLOT(onContactsContacts(qint64,const QList<Contact>&,const QList<User>&)));
    connect(mApi, SIGNAL(contactsContactsNotModified(qint64)), this, SLOT(onContactsContactsNotModified(qint64)));
    connect(mApi, SIGNAL(contactsImportedContacts(qint64,const QList<ImportedContact>&,const QList<qint64>&,const QList<User>&)), this, SIGNAL(contactsImportContactsAnswer(qint64,const QList<ImportedContact>&,const QList<qint64>&,const QList<User>&)));
    connect(mApi, SIGNAL(contactsImportedContacts(qint64,const QList<ImportedContact>&,const QList<qint64>&,const QList<User>&)), this, SLOT(onContactsImportContactsAnswer()));
    connect(mApi, SIGNAL(contactsFound(qint64,const QList<ContactFound>&,const QList<User>&)), this, SIGNAL(contactsFound(qint64,const QList<ContactFound>&,const QList<User>&)));
    connect(mApi, SIGNAL(contactsResolveUsernameResult(qint64,const User&)), this, SIGNAL(contactsResolveUsernameAnswer(qint64,const User&)));
    connect(mApi, SIGNAL(contactsDeleteContactLink(qint64,const ContactLink&,const ContactLink&,const User&)), this, SIGNAL(contactsDeleteContactAnswer(qint64,const ContactLink&,const ContactLink&,const User&)));
    connect(mApi, SIGNAL(contactsDeleteContactsResult(qint64,bool)), this, SIGNAL(contactsDeleteContactsAnswer(qint64,bool)));
    connect(mApi, SIGNAL(contactsBlockResult(qint64,bool)), this, SIGNAL(contactsBlockAnswer(qint64,bool)));
    connect(mApi, SIGNAL(contactsUnblockResult(qint64,bool)), this, SIGNAL(contactsUnblockAnswer(qint64,bool)));
    connect(mApi, SIGNAL(contactsBlocked(qint64,const QList<ContactBlocked>&,const QList<User>&)), this, SLOT(onContactsBlocked(qint64,const QList<ContactBlocked>&,const QList<User>&)));
    connect(mApi, SIGNAL(contactsBlockedSlice(qint64,qint32,const QList<ContactBlocked>&,const QList<User>&)), this, SIGNAL(contactsGetBlockedAnswer(qint64,qint32,QList<ContactBlocked>,QList<User>)));
    connect(mApi, SIGNAL(messagesSentMessage(qint64,qint32,qint32,qint32,qint32,qint32)), this, SLOT(onMessagesSentMessage(qint64,qint32,qint32,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesSentMessageLink(qint64,qint32,qint32,qint32,qint32,qint32,const QList<ContactsLink>&)), this, SIGNAL(messagesSendMessageAnswer(qint64,qint32,qint32,qint32,qint32,qint32,const QList<ContactsLink>&)));
    connect(mApi, SIGNAL(messagesSetTypingResult(qint64,bool)), this, SIGNAL(messagesSetTypingAnswer(qint64,bool)));
    connect(mApi, SIGNAL(messagesGetMessagesMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesGetMessagesMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesGetMessagesMessagesSlice(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetMessagesAnswer(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesDialogs(qint64, const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesDialogs(qint64,const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesDialogsSlice(qint64,qint32,const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetDialogsAnswer(qint64,qint32,const QList<Dialog>&,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesGetHistoryMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesGetHistoryMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesGetHistoryMessagesSlice(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetHistoryAnswer(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesSearchMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SLOT(onMessagesSearchMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesSearchMessagesSlice(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesSearchAnswer(qint64,qint32,const QList<Message>&,const QList<Chat>&,const QList<User>&)));
    connect(mApi, SIGNAL(messagesReadAffectedHistory(qint64,qint32,qint32,qint32)), this, SIGNAL(messagesReadHistoryAnswer(qint64,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesReadMessageContentsResult(qint64,const AffectedMessages&)), this, SIGNAL(messagesReadMessageContentsAnswer(qint64,const AffectedMessages&)));
    connect(mApi, SIGNAL(messagesDeleteAffectedHistory(qint64,qint32,qint32,qint32)), this, SIGNAL(messagesDeleteHistoryAnswer(qint64,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesDeleteMessagesResult(qint64,const AffectedMessages&)), this, SIGNAL(messagesDeleteMessagesAnswer(qint64,const AffectedMessages&)));
    connect(mApi, SIGNAL(messagesRestoreMessagesResult(qint64,const QList<qint32>&)), this, SIGNAL(messagesRestoreMessagesAnswer(qint64,const QList<qint32>&)));
    connect(mApi, SIGNAL(messagesReceivedMessagesResult(qint64,const QList<qint32>&)), this, SIGNAL(messagesReceivedMessagesAnswer(qint64,const QList<qint32>&)));
    connect(mApi, SIGNAL(messagesForwardMsgStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesForwardMsgStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesForwardMsgStatedMessageLink(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesForwardMessageAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesForwardMsgsStatedMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesForwardMsgsStatedMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesForwardMsgsStatedMessagesLinks(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesForwardMessagesAnswer(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesSendBroadcastStatedMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesSendBroadcastStatedMessages(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesSendBroadcastStatedMessagesLinks(qint64,const QList<Message>&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesSendBroadcastAnswer(qint64,const QList<Message>,const QList<Chat>,const QList<User>,const QList<ContactsLink>,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesChats(qint64,const QList<Chat>&)), this, SIGNAL(messagesGetChatsAnswer(qint64,const QList<Chat>&)));
    connect(mApi, SIGNAL(messagesChatFull(qint64,const ChatFull&,const QList<Chat>&,const QList<User>&)), this, SIGNAL(messagesGetFullChatAnswer(qint64,const ChatFull&,const QList<Chat>&,const QList<User>&)));

    connect(mApi, SIGNAL(messagesEditChatTitleStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesEditChatTitleStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesEditChatTitleStatedMessageLink(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesEditChatTitleAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesEditChatPhotoStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesEditChatPhotoStatedMessageAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesEditChatPhotoStatedMessageLink(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesEditChatPhotoStatedMessageAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesAddChatUserStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesAddChatUserStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesAddChatUserStatedMessageLink(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesAddChatUserAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesDeleteChatUserStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesDeleteChatUserStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesDeleteChatUserStatedMessageLink(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesDeleteChatUserAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesCreateChatStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)), this, SLOT(onMessagesCreateChatStatedMessage(qint64,const Message&,const QList<Chat>&,const QList<User>&,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(messagesCreateChatStatedMessageLink(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)), this, SIGNAL(messagesCreateChatAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32,qint32)));

    // secret chats
    connect(mApi, SIGNAL(messagesDhConfig(qint64,qint32,const QByteArray&,qint32,const QByteArray&)), this, SLOT(onMessagesDhConfig(qint64,qint32,const QByteArray&,qint32,const QByteArray&)));
    connect(mApi, SIGNAL(messagesDhConfigNotModified(qint64,const QByteArray&)), this, SLOT(onMessagesDhConfigNotModified(qint64,const QByteArray&)));
    connect(mApi, SIGNAL(messagesRequestEncryptionEncryptedChat(qint64,const EncryptedChat&)), this, SLOT(onMessagesRequestEncryptionEncryptedChat(qint64,const EncryptedChat&)));
    connect(mApi, SIGNAL(messagesAcceptEncryptionEncryptedChat(qint64,const EncryptedChat&)), this, SLOT(onMessagesAcceptEncryptionEncryptedChat(qint64,const EncryptedChat&)));
    connect(mApi, SIGNAL(messagesDiscardEncryptionResult(qint64,bool)), this, SLOT(onMessagesDiscardEncryptionResult(qint64,bool)));
    connect(mApi, SIGNAL(messagesReadEncryptedHistoryResult(qint64,bool)), this, SIGNAL(messagesReadEncryptedHistoryAnswer(qint64,bool)));
    connect(mApi, SIGNAL(messagesSendEncryptedSentEncryptedMessage(qint64,qint32)), this, SIGNAL(messagesSendEncryptedAnswer(qint64,qint32)));
    connect(mApi, SIGNAL(messagesSendEncryptedSentEncryptedFile(qint64,qint32,const EncryptedFile&)), this, SIGNAL(messagesSendEncryptedAnswer(qint64,qint32,const EncryptedFile&)));
    connect(mApi, SIGNAL(messagesSendEncryptedServiceSentEncryptedMessage(qint64,qint32)), this, SIGNAL(messagesSendEncryptedServiceAnswer(qint64,qint32)));
    connect(mApi, SIGNAL(messagesSendEncryptedServiceSentEncryptedFile(qint64,qint32,const EncryptedFile&)), this, SIGNAL(messagesSendEncryptedServiceAnswer(qint64,qint32,const EncryptedFile&)));
    connect(mApi, SIGNAL(messagesGetStickersResult(qint64,const MessagesStickers&)), this, SIGNAL(messagesGetStickersAnwer(qint64,const MessagesStickers&)));
    connect(mApi, SIGNAL(messagesGetAllStickersResult(qint64,const MessagesAllStickers&)), this, SIGNAL(messagesGetAllStickersAnwer(qint64,const MessagesAllStickers&)));
    connect(mApi, SIGNAL(updateShort(const Update&,qint32)), SLOT(onUpdateShort(const Update&)));
    connect(mApi, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)), SLOT(onUpdatesCombined(const QList<Update>&)));
    connect(mApi, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)), SLOT(onUpdates(const QList<Update>&)));
    // updates
    connect(mApi, SIGNAL(updatesState(qint64,qint32,qint32,qint32,qint32,qint32)), this, SIGNAL(updatesGetStateAnswer(qint64,qint32,qint32,qint32,qint32,qint32)));
    connect(mApi, SIGNAL(updatesDifferenceEmpty(qint64,qint32,qint32)), this, SIGNAL(updatesGetDifferenceAnswer(qint64,qint32,qint32)));
    connect(mApi, SIGNAL(updatesDifference(qint64,const QList<Message>&,const QList<EncryptedMessage>&,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)), this, SLOT(onUpdatesDifference(qint64,const QList<Message>&,const QList<EncryptedMessage>&,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)));
    connect(mApi, SIGNAL(updatesDifferenceSlice(qint64,const QList<Message>,const QList<EncryptedMessage>,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)), this, SLOT(onUpdatesDifferenceSlice(qint64,const QList<Message>&,const QList<EncryptedMessage>&,const QList<Update>&,const QList<Chat>&,const QList<User>&,const UpdatesState&)));
    // logic additional signal slots
    connect(mApi, SIGNAL(mainSessionDcChanged(DC*)), this, SLOT(onAuthCheckPhoneDcChanged()));
    connect(mApi, SIGNAL(mainSessionDcChanged(DC*)), this, SLOT(onHelpGetInviteTextDcChanged()));
    connect(mApi, SIGNAL(mainSessionDcChanged(DC*)), this, SLOT(onImportContactsDcChanged()));
    connect(mApi, SIGNAL(mainSessionReady()), this, SIGNAL(connected()));
    connect(mApi, SIGNAL(mainSessionClosed()), this, SIGNAL(disconnected()));

    mFileHandler = FileHandler::Ptr(new FileHandler(mApi, mCrypto, mSettings, *mDcProvider, *mSecretState));
    connect(mFileHandler.data(), SIGNAL(uploadSendFileAnswer(qint64,qint32,qint32,qint32)), SIGNAL(uploadSendFileAnswer(qint64,qint32,qint32,qint32)));
    connect(mFileHandler.data(), SIGNAL(uploadGetFileAnswer(qint64,const StorageFileType&,qint32,const QByteArray&,qint32,qint32,qint32)), SIGNAL(uploadGetFileAnswer(qint64,const StorageFileType&,qint32,const QByteArray&,qint32,qint32,qint32)));
    connect(mFileHandler.data(), SIGNAL(uploadCancelFileAnswer(qint64,bool)), SIGNAL(uploadCancelFileAnswer(qint64,bool)));
    connect(mFileHandler.data(), SIGNAL(error(qint64,qint32,const QString&)), SIGNAL(error(qint64,qint32,const QString&)));
    connect(mFileHandler.data(), SIGNAL(messagesSendMediaAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32)), SLOT(onMessagesSendMediaAnswer(qint64,const Message&,const QList<Chat>&,const QList<User>&,const QList<ContactsLink>&,qint32,qint32)));
    connect(mFileHandler.data(), SIGNAL(messagesSendEncryptedFileAnswer(qint64,qint32,const EncryptedFile&)), SIGNAL(messagesSendEncryptedFileAnswer(qint64,qint32,const EncryptedFile&)));

    // At this point we should test the main session state and emit by hand signals of connected/disconnected
    // depending on the connection state of the session. This is so because first main session connection, if done,
    // is performed before we could connect the signal-slots to advise about it;
    if (mApi->mainSession()->state() == QAbstractSocket::ConnectedState) {
        Q_EMIT connected();
    } else {
        Q_EMIT disconnected();
    }
}

qint64 Telegram::messagesCreateEncryptedChat(const InputUser &user) {
    qCDebug(TG_LIB_SECRET) << "creating new encrypted chat";
    // generate a new object where store all the needed secret chat data
    SecretChat *secretChat = new SecretChat(mSettings, this);
    secretChat->setRequestedUser(user);
    return generateGAorB(secretChat);
}

qint64 Telegram::messagesAcceptEncryptedChat(qint32 chatId) {
    qCDebug(TG_LIB_SECRET) << "accepting requested encrypted chat with id" << chatId;
    SecretChat *secretChat = mSecretState->chats().value(chatId);

    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Not found any chat related to" << chatId;
        return -1;
    }
    return generateGAorB(secretChat);
}

qint64 Telegram::messagesDiscardEncryptedChat(qint32 chatId) {
    qCDebug(TG_LIB_SECRET) << "discarding encrypted chat with id" << chatId;
    SecretChat *secretChat = mSecretState->chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Trying to discard a not existant chat";
        Q_EMIT messagesEncryptedChatDiscarded(chatId);
        return -1;
    }

    qint64 requestId = mApi->messagesDiscardEncryption(chatId);
    // insert another entry related to this request for deleting chat only when response is ok
    mSecretState->chats().insert(requestId, secretChat);
    return requestId;
}

qint64 Telegram::messagesSetEncryptedTTL(qint64 randomId, qint32 chatId, qint32 ttl) {
    SecretChat *secretChat = mSecretState->chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not set secret chat TTL, chat not found.";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForTtl(randomId, ttl);

    mEncrypter->setSecretChat(secretChat);
    QByteArray data = mEncrypter->generateEncryptedData(decryptedMessage);
    QList<qint64> previousMsgs = secretChat->sequence();
    qint64 requestId = mApi->messagesSendEncrypted(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    mSecretState->save();

    return requestId;
}

qint64 Telegram::messagesReadEncryptedHistory(qint32 chatId, qint32 maxDate) {
    SecretChat *secretChat = mSecretState->chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not read history of a not yet existant chat";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(chatId);
    inputEncryptedChat.setAccessHash(secretChat->accessHash());
    return mApi->messagesReadEncryptedHistory(inputEncryptedChat, maxDate);
}

qint64 Telegram::messagesSendEncrypted(qint32 chatId, qint64 randomId, qint32 ttl, const QString &text) {

    SecretChat *secretChat = mSecretState->chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not find any related secret chat to send the message";
        return -1;
    }

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForSendMessage(randomId, ttl, text);

    mEncrypter->setSecretChat(secretChat);
    QByteArray data = mEncrypter->generateEncryptedData(decryptedMessage);
    QList<qint64> previousMsgs = secretChat->sequence();
    qint64 request = mApi->messagesSendEncrypted(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    mSecretState->save();

    return request;
}

void Telegram::onSequenceNumberGap(qint32 chatId, qint32 startSeqNo, qint32 endSeqNo) {
    SecretChat *secretChat = mSecretState->chats().value(chatId);
    ASSERT(secretChat);

    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(secretChat->chatId());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    qint64 randomId;
    Utils::randomBytes(&randomId, 8);

    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForResend(randomId, startSeqNo, endSeqNo);

    mEncrypter->setSecretChat(secretChat);
    QByteArray data = mEncrypter->generateEncryptedData(decryptedMessage);
    QList<qint64> previousMsgs = secretChat->sequence();
    mApi->messagesSendEncrypted(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    mSecretState->save();
}

qint64 Telegram::messagesSendEncryptedPhoto(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath) {

    SecretChat *secretChat = mSecretState->chats().value(chatId);
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

    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendEncryptedVideo(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath, qint32 duration, qint32 width, qint32 height, const QByteArray &thumbnailBytes) {
    SecretChat *secretChat = mSecretState->chats().value(chatId);
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

    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendEncryptedDocument(qint32 chatId, qint64 randomId, qint32 ttl, const QString &filePath) {

    SecretChat *secretChat = mSecretState->chats().value(chatId);
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

    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesReceivedQueue(qint32 maxQts) {
    return mApi->messagesReceivedQueue(maxQts);
}

qint64 Telegram::messagesGetStickers(const QString &emoticon, const QString &hash) {
    return mApi->messagesGetStickers(emoticon, hash);
}

qint64 Telegram::messagesGetAllStickers(const QString &hash) {
    return mApi->messagesGetAllStickers(hash);
}

qint64 Telegram::generateGAorB(SecretChat *secretChat) {
    qCDebug(TG_LIB_SECRET) << "requesting for DH config parameters";
    // call messages.getDhConfig to get p and g for start creating shared key
    qint64 reqId = mApi->messagesGetDhConfig(mSecretState->version(), DH_CONFIG_SERVER_RANDOM_LENGTH);
    // store in secret chats map related to this request id, temporally
    mSecretState->chats().insert(reqId, secretChat);
    return reqId;
}

void Telegram::onMessagesDhConfig(qint64 msgId, qint32 g, const QByteArray &p, qint32 version, const QByteArray &random) {
    qCDebug(TG_LIB_SECRET) << "received new DH parameters g ="<< QString::number(g) << ",p =" << p.toBase64()
                           << ",version =" << version;
    mSecretState->setVersion(version);
    mSecretState->setG(g);
    mSecretState->setP(p);

    if (mCrypto->checkDHParams(mSecretState->p(), g) < 0) {
        qCCritical(TG_TELEGRAM) << "Diffie-Hellman config parameters are not valid";
        return;
    }

    onMessagesDhConfigNotModified(msgId, random);
}

void Telegram::onMessagesDhConfigNotModified(qint64 msgId, const QByteArray &random) {
    qCDebug(TG_LIB_SECRET) << "processing DH parameters";
    SecretChat *secretChat = mSecretState->chats().take(msgId);
    ASSERT(secretChat);
    // create secret a number by taking server random (and generating a client random also to have more entrophy)
    secretChat->createMyKey(random);
    // create empty bignum where store the result of operation g^a mod p
    BIGNUM *r = BN_new();
    Utils::ensurePtr(r);
    // do the opeation -> r = g^a mod p
    Utils::ensure(mCrypto->BNModExp(r, mSecretState->g(), secretChat->myKey(), mSecretState->p()));
    // check that g and r are greater than one and smaller than p-1. Also checking that r is between 2^{2048-64} and p - 2^{2048-64}
    if (mCrypto->checkCalculatedParams(r, mSecretState->g(), mSecretState->p()) < 0) {
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
        mSecretState->chats().insert(randomId, secretChat);
        qCDebug(TG_LIB_SECRET) << "Requesting encryption for chatId" << secretChat->chatId();
        mApi->messagesRequestEncryption(secretChat->requestedUser(), randomId, gAOrB);
        break;
    }
    case SecretChat::Requested: {
        QByteArray gA = secretChat->gAOrB();

        createSharedKey(secretChat, mSecretState->p(), gA);
        qint64 keyFingerprint = secretChat->keyFingerprint();

        InputEncryptedChat inputEncryptedChat;
        inputEncryptedChat.setChatId(secretChat->chatId());
        inputEncryptedChat.setAccessHash(secretChat->accessHash());
        qCDebug(TG_LIB_SECRET) << "Accepting encryption for chatId" << secretChat->chatId();
        mApi->messagesAcceptEncryption(inputEncryptedChat, gAOrB, keyFingerprint);
        break;
    }
    default:
        Q_ASSERT("Not handled");
        break;
    }

    BN_free(r);
    mSecretState->save();
}

void Telegram::onMessagesRequestEncryptionEncryptedChat(qint64, const EncryptedChat &chat) {
    Q_EMIT messagesCreateEncryptedChatAnswer(chat.id(), chat.date(), chat.participantId(), chat.accessHash());
}

void Telegram::onMessagesAcceptEncryptionEncryptedChat(qint64, const EncryptedChat &chat) {
    qCDebug(TG_LIB_SECRET) << "Joined to secret chat" << chat.id() << "with peer" << chat.adminId();
    SecretChat *secretChat = mSecretState->chats().value(chat.id());
    secretChat->setState(SecretChat::Accepted);
    mSecretState->save();
    Q_EMIT messagesEncryptedChatCreated(chat.id(), chat.date(), chat.adminId(), chat.accessHash());

    //notify peer about our layer
    InputEncryptedChat inputEncryptedChat;
    inputEncryptedChat.setChatId(chat.id());
    inputEncryptedChat.setAccessHash(secretChat->accessHash());

    mEncrypter->setSecretChat(secretChat);
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    QList<qint64> previousMsgs = secretChat->sequence();
    DecryptedMessageBuilder builder(secretChat->layer());
    DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForNotifyLayer(randomId, LAYER);
    QByteArray data = mEncrypter->generateEncryptedData(decryptedMessage);
    mApi->messagesSendEncryptedService(previousMsgs, inputEncryptedChat, randomId, data);

    secretChat->increaseOutSeqNo();
    secretChat->appendToSequence(randomId);
    mSecretState->save();

    qCDebug(TG_LIB_SECRET) << "Notified our layer:" << LAYER;
}

void Telegram::onMessagesDiscardEncryptionResult(qint64 requestId, bool ok) {
    SecretChat *secretChat = mSecretState->chats().take(requestId);
    ASSERT(secretChat);
    qint32 chatId = secretChat->chatId();
    if (ok) {
        mSecretState->chats().remove(chatId);
        mSecretState->save();
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
    SecretChat *secretChat = mSecretState->chats().value(chatId);

    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "received encrypted message does not belong to any known secret chat";
        return secretChatMessage;
    }

    secretChatMessage.setChatId(chatId);
    secretChatMessage.setDate(encrypted.date());

    mDecrypter->setSecretChat(secretChat);
    DecryptedMessage decrypted = mDecrypter->decryptEncryptedData(encrypted.randomId(), encrypted.bytes());
    secretChatMessage.setDecryptedMessage(decrypted);

    // if having a not 0 randomId, the decrypted message is valid
    if (decrypted.randomId()) {

        EncryptedFile attachment = encrypted.file();

        //if attachment, check keyFingerprint
        if (attachment.classType() != EncryptedFile::typeEncryptedFileEmpty) {

            qint32 receivedKeyFingerprint = attachment.keyFingerprint();
            const QByteArray &key = decrypted.media().key();
            const QByteArray &iv = decrypted.media().iv();
            qint32 computedKeyFingerprint = mCrypto->computeKeyFingerprint(key, iv);

            qCDebug(TG_LIB_SECRET) << "received keyFingerprint:" << receivedKeyFingerprint;
            qCDebug(TG_LIB_SECRET) << "computed keyFingerprint:" << computedKeyFingerprint;

            if (receivedKeyFingerprint != computedKeyFingerprint) {
                 qCWarning(TG_LIB_SECRET) << "Computed and received key fingerprints are not equals. Discarding message";
                 return secretChatMessage;
            }

            secretChatMessage.setAttachment(attachment);
        }

        mSecretState->save();
    }

    return secretChatMessage;
}

void Telegram::processSecretChatUpdate(const Update &update) {
    switch (static_cast<qint32>(update.classType())) {
    case Update::typeUpdateNewEncryptedMessage: {
        EncryptedMessage encrypted = update.encryptedMessage();

        SecretChatMessage secretChatMessage = toSecretChatMessage(encrypted);

        // if having a not 0 randomId, the decrypted message is valid
        if (secretChatMessage.decryptedMessage().randomId()) {
            mSecretState->save();
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

            SecretChat* secretChat = new SecretChat(mSettings, this);
            secretChat->setChatId(encryptedChat.id());
            secretChat->setAccessHash(encryptedChat.accessHash());
            secretChat->setDate(encryptedChat.date());
            secretChat->setAdminId(encryptedChat.adminId());
            secretChat->setParticipantId(encryptedChat.participantId());
            secretChat->setGAOrB(gA);
            secretChat->setState(SecretChat::Requested);

            mSecretState->chats().insert(chatId, secretChat);
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

            SecretChat *secretChat = mSecretState->chats().value(chatId);
            if (!secretChat) {
                qCWarning(TG_LIB_SECRET) << "Could not find secret chat with id" << chatId;
                return;
            }

            createSharedKey(secretChat, mSecretState->p(), gB);

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
                mSecretState->save();
                Q_EMIT messagesEncryptedChatCreated(chatId, date, participantId, accessHash);

                //notify peer about our layer
                InputEncryptedChat inputEncryptedChat;
                inputEncryptedChat.setChatId(chatId);
                inputEncryptedChat.setAccessHash(accessHash);

                mEncrypter->setSecretChat(secretChat);
                qint64 randomId;
                Utils::randomBytes(&randomId, 8);
                QList<qint64> previousMsgs = secretChat->sequence();
                DecryptedMessageBuilder builder(secretChat->layer());
                DecryptedMessage decryptedMessage = builder.buildDecryptedMessageForNotifyLayer(randomId, LAYER);
                QByteArray data = mEncrypter->generateEncryptedData(decryptedMessage);
                mApi->messagesSendEncryptedService(previousMsgs, inputEncryptedChat, randomId, data);

                secretChat->increaseOutSeqNo();
                secretChat->appendToSequence(randomId);
                mSecretState->save();

                qCDebug(TG_LIB_SECRET) << "Notified our layer:" << LAYER;
            } else {
                qCCritical(TG_LIB_SECRET) << "Key fingerprint mismatch. Discarding encryption";
                messagesDiscardEncryptedChat(chatId);
            }
            break;
        }
        case EncryptedChat::typeEncryptedChatDiscarded: {
            qCDebug(TG_LIB_SECRET) << "Discarded chat" << chatId;
            SecretChat *secretChat = mSecretState->chats().take(chatId);
            if (secretChat) {
                mSecretState->save();
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

            SecretChat* secretChat = mSecretState->chats().value(chatId);
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
    Utils::ensure(mCrypto->BNModExp(result, bigNumGAOrB, myKey, p));

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
    if (errorCode == 401) {
        onAuthLogOutAnswer(id, false);
    }
    Q_EMIT error(id, errorCode, errorText, functionName);
}

void Telegram::onErrorRetry(qint64 id, qint32 errorCode, const QString &errorText) {
    // check for error and resend authCheckPhone() request
    if (errorText.contains("_MIGRATE_")) {
        qint32 newDc = errorText.mid(errorText.lastIndexOf("_") + 1).toInt();
        qDebug() << "migrated to dc" << newDc;
        mSettings->setWorkingDcNum(newDc);
        DC *dc = mDcProvider->getDc(newDc);
        mApi->changeMainSessionToDc(dc);
    } else {
        Q_EMIT error(id, errorCode, errorText, QString());
    }
}

void Telegram::onAuthCheckPhoneDcChanged() {
    if (mLastRetryType != PhoneCheck) return;
    authCheckPhone(mLastPhoneChecked);
}
void Telegram::onHelpGetInviteTextDcChanged() {
    if (mLastRetryType != GetInviteText) return;
    helpGetInviteText(mLastLangCode);
}
void Telegram::onImportContactsDcChanged() {
    if (mLastRetryType != ImportContacts)
        return;
    // Retry is hardcoded to not overwrite contacts.
    contactsImportContacts(mLastContacts, false);
}


void Telegram::onUserAuthorized(qint64, qint32 expires, const User &) {
    // change state of current dc
    qint32 workingDcNum = mSettings->workingDcNum();
    DC *dc = mDcProvider->getDc(workingDcNum);
    dc->setState(DC::userSignedIn);
    dc->setExpires(expires);
    QList<DC *> dcsList = mDcProvider->getDcs();
    // save the settings here, after user auth ready in current dc
    mSettings->setDcsList(dcsList);
    mSettings->writeAuthFile();
    // transfer current dc authorization to other dcs
    mDcProvider->transferAuth();
}

void Telegram::onPhotosPhotos(qint64 msgId, const QList<Photo> &photos, const QList<User> &users) {
    Q_EMIT photosGetUserPhotosAnswer(msgId, photos.size(), photos, users);
}

void Telegram::onContactsContacts(qint64 msgId, const QList<Contact> &contacts, const QList<User> &users) {
    m_cachedContacts = contacts;
    m_cachedUsers = users;
    Q_EMIT contactsGetContactsAnswer(msgId, true, contacts, users);
}

void Telegram::onContactsImportContactsAnswer() {
    mLastContacts.clear();
}

void Telegram::onContactsContactsNotModified(qint64 msgId) {
    Q_EMIT contactsGetContactsAnswer(msgId, false, m_cachedContacts, m_cachedUsers);
}

// not direct Responses
void Telegram::onAuthSentCode(qint64 id, bool phoneRegistered, const QString &phoneCodeHash, qint32 sendCallTimeout, bool /* unused isPassword*/) {
    m_phoneCodeHash = phoneCodeHash;
    Q_EMIT authSendCodeAnswer(id, phoneRegistered, sendCallTimeout);
}

void Telegram::onContactsBlocked(qint64 id, const QList<ContactBlocked> &blocked, const QList<User> &users) {
    Q_EMIT contactsGetBlockedAnswer(id, blocked.size(), blocked, users);
}

void Telegram::onMessagesSentMessage(qint64 id, qint32 msgId, qint32 date, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesSendMessageAnswer(id, msgId, date, pts, pts_count, seq, links);
}

void Telegram::onMessagesSendMediaAnswer(qint64 fileId, const Message &message, const QList<Chat> &chats, const QList<User> &users, const QList<ContactsLink> &links, qint32 pts, qint32 ptsCount) {
    //depending on responded media, emit one signal or another
    switch (message.media().classType()) {
    case MessageMedia::typeMessageMediaPhoto:
        Q_EMIT messagesSendPhotoAnswer(fileId, message, chats, users, links, pts, ptsCount);
        break;
    case MessageMedia::typeMessageMediaVideo:
        Q_EMIT messagesSendVideoAnswer(fileId, message, chats, users, links, pts, ptsCount);
        break;
    case MessageMedia::typeMessageMediaGeo:
        Q_EMIT messagesSendGeoPointAnswer(fileId, message, chats, users, links, pts, ptsCount);
        break;
    case MessageMedia::typeMessageMediaContact:
        Q_EMIT messagesSendContactAnswer(fileId, message, chats, users, links, pts, ptsCount);
        break;
    case MessageMedia::typeMessageMediaDocument:
        Q_EMIT messagesSendDocumentAnswer(fileId, message, chats, users, links, pts, ptsCount);
        break;
    case MessageMedia::typeMessageMediaAudio:
        Q_EMIT messagesSendAudioAnswer(fileId, message, chats, users, links, pts, ptsCount);
        break;
    default:
        Q_EMIT messagesSendMediaAnswer(fileId, message, chats, users, links, pts, ptsCount);
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

void Telegram::onMessagesForwardMsgStatedMessage(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesForwardMessageAnswer(id, message, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesForwardMsgsStatedMessages(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesForwardMessagesAnswer(id, messages, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesSendBroadcastStatedMessages(qint64 id, const QList<Message> &messages, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesSendBroadcastAnswer(id, messages, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesEditChatTitleStatedMessage(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesEditChatTitleAnswer(id, message, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesEditChatPhotoStatedMessageAnswer(qint64 msgId, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesEditChatPhotoStatedMessageAnswer(msgId, message, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesAddChatUserStatedMessage(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesAddChatUserAnswer(id, message, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesDeleteChatUserStatedMessage(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesDeleteChatUserAnswer(id, message, chats, users, links, pts, pts_count, seq);
}

void Telegram::onMessagesCreateChatStatedMessage(qint64 id, const Message &message, const QList<Chat> &chats, const QList<User> &users, qint32 pts, qint32 pts_count, qint32 seq) {
    QList<ContactsLink> links;
    Q_EMIT messagesCreateChatAnswer(id, message, chats, users, links, pts, pts_count, seq);
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
    mLastLangCode = langCode;
    mLastRetryType = GetInviteText;
    return mApi->helpGetInviteText(langCode);
}

qint64 Telegram::authCheckPhone() {
   return authCheckPhone(mSettings->phoneNumber());
}
qint64 Telegram::authCheckPhone(const QString &phoneNumber) {
    mLastRetryType = PhoneCheck;
    mLastPhoneChecked = phoneNumber;
    return mApi->authCheckPhone(phoneNumber);
}
qint64 Telegram::authSendCode() {
    return mApi->authSendCode(mSettings->phoneNumber(), 0, mSettings->appId(), mSettings->appHash(), LANG_CODE);
}

qint64 Telegram::authSendSms() {
    return mApi->authSendSms(mSettings->phoneNumber(), m_phoneCodeHash);
}

qint64 Telegram::authSendCall() {
    return mApi->authSendCall(mSettings->phoneNumber(), m_phoneCodeHash);
}

qint64 Telegram::authSignIn(const QString &code) {
    return mApi->authSignIn(mSettings->phoneNumber(), m_phoneCodeHash, code);
}

qint64 Telegram::authSignUp(const QString &code, const QString &firstName, const QString &lastName) {
    return mApi->authSignUp(mSettings->phoneNumber(), m_phoneCodeHash, code, firstName, lastName);
}

qint64 Telegram::authLogOut() {
    return mApi->authLogOut();
}

qint64 Telegram::authSendInvites(const QStringList &phoneNumbers, const QString &inviteText) {
    return mApi->authSendInvites(phoneNumbers, inviteText);
}

qint64 Telegram::authResetAuthorizations() {
    return mApi->authResetAuthorizations();
}

qint64 Telegram::authCheckPassword(const QByteArray &passwordHash) {
    return mApi->authCheckPassword(passwordHash);
}

qint64 Telegram::accountRegisterDevice(const QString &token, const QString &appVersion, bool appSandbox) {
    if (token.length() == 0) {
        qCCritical(TG_TELEGRAM) << "refuse to register with empty token!";
        return -1;
    }
    QString version = appVersion;
    if (!version.length()) {
        version = Utils::getAppVersion();
    }
    qCDebug(TG_TELEGRAM) << "registering device for push - app version" << version;
    return mApi->accountRegisterDevice(UBUNTU_PHONE_TOKEN_TYPE, token, Utils::getDeviceModel(), Utils::getSystemVersion(), version, appSandbox, mSettings->langCode());
}

qint64 Telegram::accountUnregisterDevice(const QString &token) {
    return mApi->accountUnregisterDevice(UBUNTU_PHONE_TOKEN_TYPE, token);
}

qint64 Telegram::accountUpdateNotifySettings(const InputNotifyPeer &peer, const InputPeerNotifySettings &settings) {
    return mApi->accountUpdateNotifySettings(peer, settings);
}

qint64 Telegram::accountGetNotifySettings(const InputNotifyPeer &peer) {
    return mApi->accountGetNotifySettings(peer);
}

qint64 Telegram::accountResetNotifySettings() {
    return mApi->accountResetNotifySettings();
}

qint64 Telegram::accountUpdateProfile(const QString &firstName, const QString &lastName) {
    return mApi->accountUpdateProfile(firstName, lastName);
}

qint64 Telegram::accountUpdateStatus(bool offline) {
    return mApi->accountUpdateStatus(offline);
}

qint64 Telegram::accountGetWallPapers() {
    return mApi->accountGetWallPapers();
}

qint64 Telegram::accountCheckUsername(const QString &username) {
    return mApi->accountCheckUsername(username);
}

qint64 Telegram::accountUpdateUsername(const QString &username) {
    return mApi->accountUpdateUsername(username);
}

qint64 Telegram::accountGetPrivacy(const InputPrivacyKey &key) {
    return mApi->accountGetPrivacy(key);
}

qint64 Telegram::accountSetPrivacy(const InputPrivacyKey &key, const QList<InputPrivacyRule> &rules) {
    return mApi->accountSetPrivacy(key, rules);
}

qint64 Telegram::accountDeleteAccount(const QString &reason) {
    return mApi->accountDeleteAccount(reason);
}

qint64 Telegram::accountGetAccountTTL() {
    return mApi->accountGetAccountTTL();
}

qint64 Telegram::accountSetAccountTTL(const AccountDaysTTL &ttl) {
    return mApi->accountSetAccountTTL(ttl);
}

qint64 Telegram::accountUpdateDeviceLocked(int period) {
    return mApi->accountUpdateDeviceLocked(period);
}

qint64 Telegram::accountSendChangePhoneCode(const QString &phone_number) {
    return mApi->accountSendChangePhoneCode(phone_number);
}

qint64 Telegram::accountChangePhone(const QString &phone_number, const QString &phone_code_hash, const QString &phone_code) {
    return mApi->accountChangePhone(phone_number, phone_code_hash, phone_code);
}

qint64 Telegram::accountGetPassword() {
    return mApi->accountGetPassword();
}

qint64 Telegram::accountSetPassword(const QByteArray &currentPasswordHash, const QByteArray &newSalt, const QByteArray &newPasswordHash, const QString &hint) {
    return mApi->accountSetPassword(currentPasswordHash, newSalt, newPasswordHash, hint);
}

qint64 Telegram::photosUploadProfilePhoto(const QByteArray &bytes, const QString &fileName, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop) {
    FileOperation *op = new FileOperation(FileOperation::uploadProfilePhoto);
    op->setCaption(caption);
    op->setGeoPoint(geoPoint);
    op->setCrop(crop);
    return mFileHandler->uploadSendFile(*op, fileName, bytes);
}

qint64 Telegram::photosUploadProfilePhoto(const QString &filePath, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop) {
    FileOperation *op = new FileOperation(FileOperation::uploadProfilePhoto);
    op->setCaption(caption);
    op->setGeoPoint(geoPoint);
    op->setCrop(crop);
    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::photosUpdateProfilePhoto(qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop) {
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    return mApi->photosUpdateProfilePhoto(inputPhoto, crop);
}

qint64 Telegram::usersGetUsers(const QList<InputUser> &users) {
    return mApi->usersGetUsers(users);
}

qint64 Telegram::usersGetFullUser(const InputUser &user) {
    return mApi->usersGetFullUser(user);
}

qint64 Telegram::photosGetUserPhotos(const InputUser &user, qint32 offset, qint32 maxId, qint32 limit) {
    return mApi->photosGetUserPhotos(user, offset, maxId, limit);
}

qint64 Telegram::contactsGetStatuses() {
    return mApi->contactsGetStatuses();
}

bool lessThan(const Contact &c1, const Contact &c2) {
    return c1.userId() < c2.userId();
}

qint64 Telegram::contactsGetContacts() {
    //If there already is a full contact list on the client, an md5-hash of a comma-separated list of contact IDs
    //in ascending order may be passed in this 'hash' parameter. If the contact set was not changed,
    //contactsContactsNotModified() will be returned from Api, so the cached client list is returned with the
    //signal that they are the same contacts as previous request
    QString hash;
    if (!m_cachedContacts.isEmpty()) {
        qSort(m_cachedContacts.begin(), m_cachedContacts.end(), lessThan); //lessThan method must be outside any class or be static
        QString hashBase;
        if (m_cachedContacts.size() > 0) {
            hashBase.append(QString::number(m_cachedContacts.at(0).userId()));
        }
        for (qint32 i = 1; i < m_cachedContacts.size(); i++) {
            hashBase.append(",");
            hashBase.append(QString::number(m_cachedContacts.at(i).userId()));
        }
        QCryptographicHash md5Generator(QCryptographicHash::Md5);
        md5Generator.addData(hashBase.toStdString().c_str());
        hash = md5Generator.result().toHex();
    }
    return mApi->contactsGetContacts(hash);
}

qint64 Telegram::contactsImportContacts(const QList<InputContact> &contacts, bool replace) {
    mLastContacts = contacts;
    mLastRetryType = ImportContacts;
    return mApi->contactsImportContacts(contacts, replace);
}

qint64 Telegram::contactsDeleteContact(const InputUser &user) {
    return mApi->contactsDeleteContact(user);
}

qint64 Telegram::contactsDeleteContacts(const QList<InputUser> &users) {
    return mApi->contactsDeleteContacts(users);
}

qint64 Telegram::contactsSearch(const QString &q, qint32 limit) {
    return mApi->contactsSearch(q, limit);
}

qint64 Telegram::contactsResolveUsername(const QString &username) {
    return mApi->contactsResolveUsername(username);
}

qint64 Telegram::contactsBlock(const InputUser &user) {
    return mApi->contactsBlock(user);
}

qint64 Telegram::contactsUnblock(const InputUser &user) {
    return mApi->contactsUnblock(user);
}

qint64 Telegram::contactsGetBlocked(qint32 offset, qint32 limit) {
    return mApi->contactsGetBlocked(offset, limit);
}

qint64 Telegram::messagesSendMessage(const InputPeer &peer, qint64 randomId, const QString &message, int replyToMsgId) {
    return mApi->messagesSendMessage(peer, message, randomId, replyToMsgId);
}

qint64 Telegram::messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedPhoto);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return mFileHandler->uploadSendFile(*op, fileName, bytes);
}

qint64 Telegram::messagesSendPhoto(const InputPeer &peer, qint64 randomId, const QString &filePath, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaUploadedPhoto);
    FileOperation *op = new FileOperation(FileOperation::sendMedia);
    op->setInputPeer(peer);
    op->setInputMedia(inputMedia);
    op->setRandomId(randomId);
    op->setReplyToMsgId(replyToMsgId);
    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendGeoPoint(const InputPeer &peer, qint64 randomId, const InputGeoPoint &inputGeoPoint, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaGeoPoint);
    inputMedia.setGeoPoint(inputGeoPoint);
    return mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesSendContact(const InputPeer &peer, qint64 randomId, const QString &phoneNumber, const QString &firstName, const QString &lastName, qint32 replyToMsgId) {
    InputMedia inputMedia(InputMedia::typeInputMediaContact);
    inputMedia.setPhoneNumber(phoneNumber);
    inputMedia.setFirstName(firstName);
    inputMedia.setLastName(lastName);
    return mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
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
    return mFileHandler->uploadSendFile(*op, fileName, bytes, thumbnailBytes, thumbnailName);
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
    return mFileHandler->uploadSendFile(*op, filePath, thumbnailFilePath);
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
    return mFileHandler->uploadSendFile(*op, fileName, bytes);
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
    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesSendDocument(const InputPeer &peer, qint64 randomId, const QByteArray &bytes, const QString &fileName, const QString &mimeType, const QByteArray &thumbnailBytes, const QString &thumbnailName, const QList<DocumentAttribute> &extraAttributes, qint32 replyToMsgId) {
    DocumentAttribute fileAttr(DocumentAttribute::typeAttributeFilename);
    fileAttr.setFilename(fileName);

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
    return mFileHandler->uploadSendFile(*op, fileName, bytes, thumbnailBytes, thumbnailName);
}

qint64 Telegram::messagesSendDocument(const InputPeer &peer, qint64 randomId, const QString &filePath, const QString &thumbnailFilePath, bool sendAsSticker, qint32 replyToMsgId) {
    const QMimeType t = QMimeDatabase().mimeTypeForFile(QFileInfo(filePath));
    QString mimeType = t.name();

    DocumentAttribute fileAttr(DocumentAttribute::typeAttributeFilename);
    fileAttr.setFilename(QFileInfo(filePath).fileName());

    QList<DocumentAttribute> attributes;
    attributes << fileAttr;
    if(sendAsSticker) {
        QImageReader reader(filePath);
        DocumentAttribute imageSizeAttr(DocumentAttribute::typeAttributeImageSize);
        imageSizeAttr.setH(reader.size().height());
        imageSizeAttr.setW(reader.size().width());

        attributes << DocumentAttribute(DocumentAttribute::typeAttributeSticker) << imageSizeAttr;

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
    return mFileHandler->uploadSendFile(*op, filePath, thumbnailFilePath);
}

qint64 Telegram::messagesForwardPhoto(const InputPeer &peer, qint64 randomId, qint64 photoId, qint64 accessHash, qint32 replyToMsgId) {
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaPhoto);
    inputMedia.setPhotoId(inputPhoto);
    return mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardVideo(const InputPeer &peer, qint64 randomId, qint64 videoId, qint64 accessHash, qint32 replyToMsgId) {
    InputVideo inputVideo(InputVideo::typeInputVideo);
    inputVideo.setId(videoId);
    inputVideo.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaVideo);
    inputMedia.setVideoId(inputVideo);
    return mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardAudio(const InputPeer &peer, qint64 randomId, qint64 audioId, qint64 accessHash, qint32 replyToMsgId) {
    InputAudio inputAudio(InputAudio::typeInputAudio);
    inputAudio.setId(audioId);
    inputAudio.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaAudio);
    inputMedia.setAudioId(inputAudio);
    return mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesForwardDocument(const InputPeer &peer, qint64 randomId, qint64 documentId, qint64 accessHash, qint32 replyToMsgId) {
    InputDocument inputDocument(InputDocument::typeInputDocument);
    inputDocument.setId(documentId);
    inputDocument.setAccessHash(accessHash);
    InputMedia inputMedia(InputMedia::typeInputMediaDocument);
    inputMedia.setDocumentId(inputDocument);
    return mApi->messagesSendMedia(peer, inputMedia, randomId, replyToMsgId);
}

qint64 Telegram::messagesSetEncryptedTyping(qint32 chatId, bool typing) {
    SecretChat *secretChat = mSecretState->chats().value(chatId);
    if (!secretChat) {
        qCWarning(TG_LIB_SECRET) << "Could not read history of a not yet existant chat";
        return -1;
    }
    InputEncryptedChat mChat;
    mChat.setChatId(chatId);
    mChat.setAccessHash(secretChat->accessHash());
    return mApi->messagesSetEncryptedTyping(mChat,typing);
}

qint64 Telegram::messagesSetTyping(const InputPeer &peer, const SendMessageAction &action) {
    return mApi->messagesSetTyping(peer, action);
}

qint64 Telegram::messagesGetMessages(const QList<qint32> &msgIds) {
    return mApi->messagesGetMessages(msgIds);
}

qint64 Telegram::messagesGetDialogs(qint32 offset, qint32 maxId, qint32 limit) {
    return mApi->messagesGetDialogs(offset, maxId, limit);
}

qint64 Telegram::messagesGetHistory(const InputPeer &peer, qint32 offset, qint32 maxId, qint32 limit) {
    return mApi->messagesGetHistory(peer, offset, maxId, limit);
}

qint64 Telegram::messagesSearch(const InputPeer &peer, const QString &query, const MessagesFilter &filter, qint32 minDate, qint32 maxDate, qint32 offset, qint32 maxId, qint32 limit) {
    return mApi->messagesSearch(peer, query, filter, minDate, maxDate, offset, maxId, limit);
}

qint64 Telegram::messagesReadHistory(const InputPeer &peer, qint32 maxId, qint32 offset) {
    return mApi->messagesReadHistory(peer, maxId, offset);
}

qint64 Telegram::messagesReadMessageContents(const QList<qint32> &ids) {
    return mApi->messagesReadMessageContents(ids);
}

qint64 Telegram::messagesDeleteHistory(const InputPeer &peer, qint32 offset) {
    return mApi->messagesDeleteHistory(peer, offset);
}

qint64 Telegram::messagesDeleteMessages(const QList<qint32> &msgIds) {
    return mApi->messagesDeleteMessages(msgIds);
}

qint64 Telegram::messagesRestoreMessages(const QList<qint32> &msgIds) {
    return mApi->messagesRestoreMessages(msgIds);
}

qint64 Telegram::messagesReceivedMessages(qint32 maxId) {
    return mApi->messagesReceivedMessages(maxId);
}

qint64 Telegram::messagesForwardMessage(const InputPeer &peer, qint32 msgId) {
    qint64 randomId;
    Utils::randomBytes(&randomId, 8);
    return mApi->messagesForwardMessage(peer, msgId, randomId);
}

qint64 Telegram::messagesForwardMessages(const InputPeer &peer, const QList<qint32> &msgIds, const QList<qint32> &randomIds) {
    return mApi->messagesForwardMessages(peer, msgIds, randomIds);
}

qint64 Telegram::messagesSendBroadcast(const QList<InputUser> &users, const QString &message, const InputMedia &media) {
    return mApi->messagesSendBroadcast(users, message, media);
}

qint64 Telegram::messagesGetChats(const QList<qint32> &chatIds) {
    return mApi->messagesGetChats(chatIds);
}

qint64 Telegram::messagesGetFullChat(qint32 chatId) {
    return mApi->messagesGetFullChat(chatId);
}

qint64 Telegram::messagesEditChatTitle(qint32 chatId, const QString &title) {
    return mApi->messagesEditChatTitle(chatId, title);
}

qint64 Telegram::messagesEditChatPhoto(qint32 chatId, const QString &filePath, const InputPhotoCrop &crop) {
    InputChatPhoto inputChatPhoto(InputChatPhoto::typeInputChatUploadedPhoto);
    inputChatPhoto.setCrop(crop);
    FileOperation *op = new FileOperation(FileOperation::editChatPhoto);
    op->setChatId(chatId);
    op->setInputChatPhoto(inputChatPhoto);
    return mFileHandler->uploadSendFile(*op, filePath);
}

qint64 Telegram::messagesEditChatPhoto(qint32 chatId, qint64 photoId, qint64 accessHash, const InputPhotoCrop &crop) {
    InputChatPhoto inputChatPhoto(InputChatPhoto::typeInputChatPhoto);
    InputPhoto inputPhoto(InputPhoto::typeInputPhoto);
    inputPhoto.setId(photoId);
    inputPhoto.setAccessHash(accessHash);
    inputChatPhoto.setId(inputPhoto);
    inputChatPhoto.setCrop(crop);
    return mApi->messagesEditChatPhoto(chatId, inputChatPhoto);
}

qint64 Telegram::messagesAddChatUser(qint32 chatId, const InputUser &user, qint32 fwdLimit) {
    return mApi->messagesAddChatUser(chatId, user, fwdLimit);
}

qint64 Telegram::messagesDeleteChatUser(qint32 chatId, const InputUser &user) {
    return mApi->messagesDeleteChatUser(chatId, user);
}

qint64 Telegram::messagesCreateChat(const QList<InputUser> &users, const QString &chatTopic) {
    return mApi->messagesCreateChat(users, chatTopic);
}

qint64 Telegram::updatesGetState() {
    return mApi->updatesGetState();
}

qint64 Telegram::updatesGetDifference(qint32 pts, qint32 date, qint32 qts) {
    return mApi->updatesGetDifference(pts, date, qts);
}

qint64 Telegram::uploadGetFile(const InputFileLocation &location, qint32 fileSize, qint32 dcNum, const QByteArray &key, const QByteArray &iv) {
    return mFileHandler->uploadGetFile(location, fileSize, dcNum, key, iv);
}

qint64 Telegram::uploadCancelFile(qint64 fileId) {
    return mFileHandler->uploadCancelFile(fileId);
}
