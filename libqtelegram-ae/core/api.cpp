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

#include "api.h"

#include "util/tlvalues.h"
#include "dcprovider.h"
#include "secret/encrypter.h"

Api::Api(Session *session, Settings *settings, CryptoUtils *crypto, QObject *parent) :
    AbstractApi(session, settings, crypto, parent),
    mSettings(settings),
    mCrypto(crypto)
{
    helpGetConfigMethods.onAnswer = &Api::onHelpGetConfigAnswer;
    helpGetConfigMethods.onError = &Api::onError; // this is not needed, because is the default one. Hereafter it won't be set explicitly

    helpGetInviteTextMethods.onAnswer = &Api::onHelpGetInviteTextAnswer;
    helpGetInviteTextMethods.onError = &Api::onErrorRetry;

    authCheckPhoneMethods.onAnswer = &Api::onAuthCheckPhoneAnswer;
    authCheckPhoneMethods.onError = &Api::onErrorRetry;

    authSendCodeMethods.onAnswer = &Api::onAuthSendCodeAnswer;

    authSendSmsMethods.onAnswer = &Api::onAuthSendSmsAnswer;

    authSendCallMethods.onAnswer = &Api::onAuthSendCallAnswer;

    authSignInMethods.onAnswer = &Api::onAuthSignInAnswer;
    authSignInMethods.onError = &Api::onAuthSignInError;

    authSignUpMethods.onAnswer = &Api::onAuthSignUpAnswer;
    authSignUpMethods.onError = &Api::onAuthSignUpError;

    authLogOutMethods.onAnswer = &Api::onAuthLogOutAnswer;

    authSendInvitesMethods.onAnswer = &Api::onAuthSendInvitesAnswer;

    authResetAuthorizationsMethods.onAnswer = &Api::onAuthResetAuthorizationsAnswer;

    authImportAuthorizationMethods.onAnswer = &Api::onAuthImportAuthorizationAnswer;

    authCheckPasswordMethods.onAnswer = &Api::onAuthCheckPasswordAnswer;

    authExportAuthorizationMethods.onAnswer = &Api::onAuthExportAuthorizationAnswer;

    accountRegisterDeviceMethods.onAnswer = &Api::onAccountRegisterDeviceAnswer;

    accountUnregisterDeviceMethods.onAnswer = &Api::onAccountUnregisterDeviceAnswer;

    accountUpdateNotifySettingsMethods.onAnswer = &Api::onAccountUpdateNotifySettingsAnswer;

    accountGetNotifySettingsMethods.onAnswer = &Api::onAccountGetNotifySettingsAnswer;

    accountResetNotifySettingsMethods.onAnswer = &Api::onAccountResetNotifySettingsAnswer;

    accountUpdateProfileMethods.onAnswer = &Api::onAccountUpdateProfileAnswer;

    accountUpdateStatusMethods.onAnswer = &Api::onAccountUpdateStatusAnswer;

    accountGetWallPapersMethods.onAnswer = &Api::onAccountGetWallPapersAnswer;

    accountCheckUsernameMethods.onAnswer = &Api::onAccountCheckUsernameAnswer;

    accountUpdateUsernameMethods.onAnswer = &Api::onAccountUpdateUsernameAnswer;

    accountAccountGetPrivacyMethods.onAnswer = &Api::onAccountPrivacyRules;

    accountAccountSetPrivacyMethods.onAnswer = &Api::onAccountPrivacyRules;

    accountDeleteAccountMethods.onAnswer = &Api::onAccountDeleteAccountAnswer;

    accountGetAccountTTLMethods.onAnswer = &Api::onAccountGetAccountTTLAnswer;

    accountSetAccountTTLMethods.onAnswer = &Api::onAccountSetAccountTTLAnswer;

    accountChangePhoneMethods.onAnswer = &Api::onAccountChangePhoneAnswer;

    accountSendChangePhoneCodeMethods.onAnswer = &Api::onAccountSentChangePhoneCode;

    accountUpdateDeviceLockedMethods.onAnswer = &Api::onAccountUpdateDeviceLockedAnswer;

    accountGetPasswordMethods.onAnswer = &Api::onAccountGetPasswordAnswer;

    accountSetPasswordMethods.onAnswer = &Api::onAccountSetPasswordAnswer;

    photosUploadProfilePhotoMethods.onAnswer = &Api::onPhotosUploadProfilePhotoAnswer;

    photosUpdateProfilePhotoMethods.onAnswer = &Api::onPhotosUpdateProfilePhotoAnswer;

    usersGetUsersMethods.onAnswer = &Api::onUsersGetUsersAnswer;

    usersGetFullUserMethods.onAnswer = &Api::onUsersGetFullUserAnswer;

    photosGetUserPhotosMethods.onAnswer = &Api::onPhotosGetUserPhotos;

    contactsGetStatusesMethods.onAnswer = &Api::onContactsGetStatusesAnswer;

    contactsGetContactsMethods.onAnswer = &Api::onContactsGetContactsAnswer;

    contactsImportContactsMethods.onAnswer = &Api::onContactsImportContactsAnswer;
    contactsImportContactsMethods.onError = &Api::onErrorRetry;

    contactsDeleteContactMethods.onAnswer = &Api::onContactsDeleteContactAnswer;

    contactsDeleteContactsMethods.onAnswer = &Api::onContactsDeleteContactsAnswer;

    contactsSearchMethods.onAnswer = &Api::onContactsSearchAnswer;

    contactsResolveUsernameMethods.onAnswer = &Api::onContactsResolveUsernameAnswer;

    contactsBlockMethods.onAnswer = &Api::onContactsBlockAnswer;

    contactsUnblockMethods.onAnswer = &Api::onContactsUnblockAnswer;

    contactsGetBlockedMethods.onAnswer = &Api::onContactsGetBlockedAnswer;

    messagesSendMessageMethods.onAnswer = &Api::onMessagesSendMessageAnswer;

    messagesSendMediaMethods.onAnswer = &Api::onMessagesSendMediaAnswer;

    messagesSetTypingMethods.onAnswer = &Api::onMessagesSetTypingAnswer;

    messagesGetMessagesMethods.onAnswer = &Api::onMessagesGetMessagesAnswer;

    messagesGetDialogsMethods.onAnswer = &Api::onMessagesGetDialogsAnswer;

    messagesGetHistoryMethods.onAnswer = &Api::onMessagesGetHistoryAnswer;

    messagesSearchMethods.onAnswer = &Api::onMessagesSearchAnswer;

    messagesReadHistoryMethods.onAnswer = &Api::onMessagesReadHistoryAnswer;

    messagesReadMessageContentsMethods.onAnswer = &Api::onMessagesReadMessageContentsAnswer;

    messagesDeleteHistoryMethods.onAnswer = &Api::onMessagesDeleteHistoryAnswer;

    messagesDeleteMessagesMethods.onAnswer = &Api::onMessagesDeleteMessagesAnswer;

    messagesRestoreMessagesMethods.onAnswer = &Api::onMessagesRestoreMessagesAnswer;

    messagesReceivedMessagesMethods.onAnswer = &Api::onMessagesReceivedMessagesAnswer;

    messagesForwardMessageMethods.onAnswer = &Api::onMessagesForwardMessageAnswer;

    messagesForwardMessagesMethods.onAnswer = &Api::onMessagesForwardMessagesAnswer;

    messagesSendBroadcastMethods.onAnswer = &Api::onMessagesSendBroadcastAnswer;

    messagesGetChatsMethods.onAnswer = &Api::onMessagesGetChatsAnswer;

    messagesGetFullChatMethods.onAnswer = &Api::onMessagesGetFullChatAnswer;

    messagesEditChatTitleMethods.onAnswer = &Api::onMessagesEditChatTitleAnswer;

    messagesEditChatPhotoMethods.onAnswer = &Api::onMessagesEditChatPhotoAnswer;

    messagesAddChatUserMethods.onAnswer = &Api::onMessagesAddChatUserAnswer;

    messagesDeleteChatUserMethods.onAnswer = &Api::onMessagesDeleteChatUserAnswer;

    messagesCreateChatMethods.onAnswer = &Api::onMessagesCreateChatAnswer;

    messagesGetDhConfigMethods.onAnswer = &Api::onMessagesGetDhConfigAnswer;

    messagesRequestEncryptionMethods.onAnswer = &Api::onMessagesRequestEncryptionAnswer;

    messagesDiscardEncryptionMethods.onAnswer = &Api::onMessagesDiscardEncryptionAnswer;

    messagesAcceptEncryptionMethods.onAnswer = &Api::onMessagesAcceptEncryptionAnswer;

    messagesSetEncryptedTypingMethods.onAnswer = &Api::onMessagesSetEncryptedTypingAnswer;

    messagesReadEncryptedHistoryMethods.onAnswer = &Api::onMessagesReadEncryptedHistoryAnswer;

    messagesSendEncryptedMethods.onAnswer = &Api::onMessagesSendEncryptedAnswer;

    messagesSendEncryptedFileMethods.onAnswer = &Api::onMessagesSendEncryptedFileAnswer;

    messagesSendEncryptedServiceMethods.onAnswer = &Api::onMessagesSendEncryptedServiceAnswer;

    messagesReceivedQueueMethods.onAnswer = &Api::onMessagesReceivedQueueAnswer;

    messagesGetStickersMethods.onAnswer = &Api::onMessagesGetStickersAnswer;

    messagesGetAllStickersMethods.onAnswer = &Api::onMessagesGetAllStickersAnswer;

    updatesGetStateMethods.onAnswer = &Api::onUpdatesGetStateAnswer;

    updatesGetDifferenceMethods.onAnswer = &Api::onUpdatesGetDifferenceAnswer;

    uploadSaveFilePartMethods.onAnswer = &Api::onUploadSaveFilePartAnswer;

    uploadSaveBigFilePartMethods.onAnswer = &Api::onUploadSaveBigFilePartAnswer;

    uploadGetFileMethods.onAnswer = &Api::onUploadGetFileAnswer;
    uploadGetFileMethods.onError = &Api::onUploadGetFileError;
}

void Api::onError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT error(q->msgId(), errorCode, errorText, q->name() );
}

//
// REGISTRATION / AUTHORIZATION
//
// ### help.getConfig()
void Api::onHelpGetConfigAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Config);
    Config cfg;
    cfg.setDate(inboundPkt.fetchInt());
    cfg.setExpires(inboundPkt.fetchInt());
    cfg.setTestMode(inboundPkt.fetchBool());
    cfg.setThisDc(inboundPkt.fetchInt());

    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<DcOption> dcOptions;
    for (qint32 i = 0; i < n; i++) {
        dcOptions.append(inboundPkt.fetchDcOption());
    }
    cfg.setDcOptions(dcOptions);

    cfg.setChatSizeMax(inboundPkt.fetchInt());
    cfg.setBroadcastSizeMax(inboundPkt.fetchInt());
    cfg.setOnlineUpdatePeriodMs(inboundPkt.fetchInt());
    cfg.setOfflineBlurTimeoutMs(inboundPkt.fetchInt());
    cfg.setOfflineIdleTimeoutMs(inboundPkt.fetchInt());
    cfg.setOnlineCloudTimeoutMs(inboundPkt.fetchInt());
    cfg.setNotifyCloudDelayMs(inboundPkt.fetchInt());
    cfg.setNotifyDefaultDelayMs(inboundPkt.fetchInt());
    cfg.setChatBigSize(inboundPkt.fetchInt());

    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 nf = inboundPkt.fetchInt();
    QList<DisabledFeature> disabledFeatures;
    for (qint32 i = 0; i < nf; i++) {
        disabledFeatures.append(inboundPkt.fetchDisabledFeature());
    }
    cfg.setDisabledFeatures(disabledFeatures);

    Q_EMIT config(q->msgId(), cfg);
}

qint64 Api::helpGetConfig() {
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_HelpGetConfig);
    return mMainSession->sendQuery(p, &helpGetConfigMethods, QVariant(), __FUNCTION__ );
}

void Api::onHelpGetInviteTextAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_HelpInviteText);
    QString textInvite = inboundPkt.fetchQString();
    Q_EMIT helpGetInviteTextAnswer(q->msgId(), textInvite);
}

qint64 Api::helpGetInviteText(const QString &langCode) {
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_HelpGetInviteText);
    p.appendQString(langCode);
    return mMainSession->sendQuery(p, &helpGetInviteTextMethods, QVariant(), __FUNCTION__ );
}

// ### auth.checkPhone()
void Api::onErrorRetry(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT errorRetry(q->msgId(), errorCode, errorText);
}

void Api::onAuthCheckPhoneAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AuthCheckedPhone);
    bool phoneRegistered = inboundPkt.fetchBool();
    Q_EMIT authCheckedPhone(q->msgId(), phoneRegistered);
}

qint64 Api::authCheckPhone(const QString &phoneNumber) {
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_AuthCheckPhone);
    p.appendQString(phoneNumber);
    qint64 resultId = mMainSession->sendQuery(p, &authCheckPhoneMethods, QVariant(), __FUNCTION__ );
    Q_EMIT authCheckPhoneSent(resultId, phoneNumber);
    return resultId;
}

// ### auth.sendCode()
void Api::onAuthSendCodeAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_AuthSentCode || x == (qint32)TL_AuthSentAppCode);
    bool phoneRegistered = inboundPkt.fetchInt();
    QString phoneCodeHash = inboundPkt.fetchQString();
    qint32 sendCallTimeout = inboundPkt.fetchInt();
    bool isPassword = inboundPkt.fetchBool();
    if (x == (qint32)TL_AuthSendCode) {
        Q_EMIT authSentCode(q->msgId(), phoneRegistered, phoneCodeHash, sendCallTimeout, isPassword);
    } else {
        Q_EMIT authSentAppCode(q->msgId(), phoneRegistered, phoneCodeHash, sendCallTimeout, isPassword);
    }
}

qint64 Api::authSendCode(const QString &phoneNumber, qint32 smsType, qint32 apiId, const QString &apiHash, const QString &langCode) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthSendCode);
    p.appendQString(phoneNumber);
    p.appendInt(smsType);
    p.appendInt(apiId);
    p.appendQString(apiHash);
    p.appendQString(langCode);
    return mMainSession->sendQuery(p, &authSendCodeMethods, QVariant(), __FUNCTION__ );
}

// ### auth.sendSms()
void Api::onAuthSendSmsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT authSendSmsResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::authSendSms(const QString &phoneNumber, const QString &phoneCodeHash) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthSendSms);
    p.appendQString(phoneNumber);
    p.appendQString(phoneCodeHash);
    return mMainSession->sendQuery(p, &authSendSmsMethods, QVariant(), __FUNCTION__ );
}

// ## auth.sendCall()
void Api::onAuthSendCallAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT authSendCallResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::authSendCall(const QString &phoneNumber, const QString &phoneCodeHash) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthSendCall);
    p.appendQString(phoneNumber);
    p.appendQString(phoneCodeHash);
    return mMainSession->sendQuery(p, &authSendCallMethods, QVariant(), __FUNCTION__ );
}

// ### auth.signIn()
void Api::onAuthSignInError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT authSignInError(q->msgId(), errorCode, errorText);
}

void Api::onAuthSignInAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AuthAuthorization);
    qint32 expires = inboundPkt.fetchInt();
    User user = inboundPkt.fetchUser();
    Q_EMIT authSignInAuthorization(q->msgId(), expires, user);
}

qint64 Api::authSignIn(const QString &phoneNumber, const QString &phoneCodeHash, const QString &phoneCode) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthSignIn);
    p.appendQString(phoneNumber);
    p.appendQString(phoneCodeHash);
    p.appendQString(phoneCode);
    return mMainSession->sendQuery(p, &authSignInMethods, QVariant(), __FUNCTION__ );
}

// ### auth.signUp()
void Api::onAuthSignUpError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT authSignUpError(q->msgId(), errorCode, errorText);
}

void Api::onAuthSignUpAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AuthAuthorization);
    qint32 expires = inboundPkt.fetchInt();
    User user = inboundPkt.fetchUser();
    Q_EMIT authSignUpAuthorization(q->msgId(), expires, user);
}

qint64 Api::authSignUp(const QString &phoneNumber, const QString &phoneCodeHash, const QString &phoneCode, const QString &firstName, const QString &lastName) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthSignUp);
    p.appendQString(phoneNumber);
    p.appendQString(phoneCodeHash);
    p.appendQString(phoneCode);
    p.appendQString(firstName);
    p.appendQString(lastName);
    return mMainSession->sendQuery(p, &authSignUpMethods, QVariant(), __FUNCTION__ );
}

// ### auth.logOut
void Api::onAuthLogOutAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT authLogOutResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::authLogOut() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthLogOut);
    return mMainSession->sendQuery(p, &authLogOutMethods, QVariant(), __FUNCTION__ );
}

// ### auth.sendInvites
void Api::onAuthSendInvitesAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT authSendInvitesResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::authSendInvites(const QStringList &phoneNumbers, const QString &message) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthSendInvites);
    p.appendInt(TL_Vector);
    p.appendInt(phoneNumbers.size());
    Q_FOREACH (QString phoneNumber, phoneNumbers) {
        p.appendQString(phoneNumber);
    }
    p.appendQString(message);
    return mMainSession->sendQuery(p, &authSendInvitesMethods, QVariant(), __FUNCTION__ );
}

// ### auth.resetAuthorization
void Api::onAuthResetAuthorizationsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT authResetAuthorizationsResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::authResetAuthorizations() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthResetAuthorizations);
    return mMainSession->sendQuery(p, &authResetAuthorizationsMethods, QVariant(), __FUNCTION__ );
}

// ### auth.importAuthorization
void Api::onAuthImportAuthorizationAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AuthAuthorization);
    qint32 expires = inboundPkt.fetchInt();
    User user = inboundPkt.fetchUser();
    Q_EMIT authImportedAuthorization(q->msgId(), expires, user);
}

qint64 Api::authImportAuthorization(qint32 id, const QByteArray &bytes) {
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_AuthImportAuthorization);
    p.appendInt(id);
    p.appendBytes(bytes);
    return mMainSession->sendQuery(p, &authImportAuthorizationMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthCheckPasswordAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AuthAuthorization);
    qint32 expires = inboundPkt.fetchInt();
    User user = inboundPkt.fetchUser();
    Q_EMIT authCheckPasswordResult(q->msgId(), expires, user);
}

qint64 Api::authCheckPassword(const QByteArray &passwordHash) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthCheckPassword);
    p.appendBytes(passwordHash);
    return mMainSession->sendQuery(p, &authCheckPasswordMethods, QVariant(), __FUNCTION__ );
}

// ### auth.exportAuthorization
void Api::onAuthExportAuthorizationAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AuthExportedAuthorization);
    qint32 id = inboundPkt.fetchInt();
    QByteArray bytes = inboundPkt.fetchBytes();
    Q_EMIT authExportedAuthorization(q->msgId(), id, bytes);
}

qint64 Api::authExportAuthorization(qint32 dcId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AuthExportAuthorization);
    p.appendInt(dcId);
    return mMainSession->sendQuery(p, &authExportAuthorizationMethods, QVariant(), __FUNCTION__ );
}

//
// NOTIFICATIONS / SETTINGS
//
// ### account.registerDevice
void Api::onAccountRegisterDeviceAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountRegisterDeviceResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountRegisterDevice(qint32 tokenType, const QString &token, const QString &deviceModel, const QString &systemVersion, const QString &appVersion, bool appSandbox, const QString &langCode) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountRegisterDevice);
    p.appendInt(tokenType);
    p.appendQString(token);
    p.appendQString(deviceModel);
    p.appendQString(systemVersion);
    p.appendQString(appVersion);
    p.appendBool(appSandbox);
    p.appendQString(langCode);
    return mMainSession->sendQuery(p, &accountRegisterDeviceMethods, QVariant(), __FUNCTION__ );
}

// ### account.unregisterDevice
void Api::onAccountUnregisterDeviceAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountUnregisterDeviceResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountUnregisterDevice(qint32 tokenType, const QString &token) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountUnregisterDevice);
    p.appendInt(tokenType);
    p.appendQString(token);
    return mMainSession->sendQuery(p, &accountUnregisterDeviceMethods, QVariant(), __FUNCTION__ );
}

// ### account.updateNotifySettings
void Api::onAccountUpdateNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountUpdateNotifySettingsResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountUpdateNotifySettings(const InputNotifyPeer &peer, const InputPeerNotifySettings &settings) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountUpdateNotifySettings);
    p.appendInputNotifyPeer(peer);
    p.appendInputPeerNotifySettings(settings);
    return mMainSession->sendQuery(p, &accountUpdateNotifySettingsMethods, QVariant(), __FUNCTION__ );
}

// ### account.getNotifySettings
void Api::onAccountGetNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountPeerNotifySettings(q->msgId(), inboundPkt.fetchPeerNotifySetting());
}

qint64 Api::accountGetNotifySettings(const InputNotifyPeer &peer) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountGetNotifySettings);
    p.appendInputNotifyPeer(peer);
    return mMainSession->sendQuery(p, &accountGetNotifySettingsMethods, QVariant(), __FUNCTION__ );
}

// ### account.resetNotifySettings
void Api::onAccountResetNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountResetNotifySettingsResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountResetNotifySettings() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountResetNotifySettings);
    return mMainSession->sendQuery(p, &accountResetNotifySettingsMethods, QVariant(), __FUNCTION__ );
}

// ### account.updateProfile
void Api::onAccountUpdateProfileAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountUser(q->msgId(), inboundPkt.fetchUser());
}

qint64 Api::accountUpdateProfile(const QString &firstName, const QString &lastName) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountUpdateProfile);
    p.appendQString(firstName);
    p.appendQString(lastName);
    return mMainSession->sendQuery(p, &accountUpdateProfileMethods, QVariant(), __FUNCTION__ );
}

// ### account.updateStatus
void Api::onAccountUpdateStatusAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountUpdateStatusResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountUpdateStatus(bool offline) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountUpdateStatus);
    p.appendBool(offline);
    return mMainSession->sendQuery(p, &accountUpdateStatusMethods, QVariant(), __FUNCTION__ );
}

// ### account.getWallPapers
void Api::onAccountGetWallPapersAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<WallPaper> wallpapers;
    for (qint32 i = 0; i < n; i++) {
        wallpapers.append(inboundPkt.fetchWallPaper());
    }
    Q_EMIT accountGetWallPapersResult(q->msgId(), wallpapers);
}

qint64 Api::accountGetWallPapers() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountGetWallPapers);
    return mMainSession->sendQuery(p, &accountGetWallPapersMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountCheckUsernameAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountCheckUsernameResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountCheckUsername(const QString &username) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountCheckUsername);
    p.appendQString(username);
    return mMainSession->sendQuery(p, &accountCheckUsernameMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdateUsernameAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountUpdateUsernameResult(q->msgId(), inboundPkt.fetchUser());
}

qint64 Api::accountUpdateUsername(const QString &username) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountUpdateUsername);
    p.appendQString(username);
    return mMainSession->sendQuery(p, &accountUpdateUsernameMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountPrivacyRules(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AccountPrivacyRules);
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 nr = inboundPkt.fetchInt();
    QList<PrivacyRule> rules;
    for (qint32 i = 0; i < nr; i++) {
        rules.append(inboundPkt.fetchPrivacyRule());
    }

    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 nu = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < nu; i++) {
        users.append(inboundPkt.fetchUser());
    }

    Q_EMIT accountPrivacyRules(q->msgId(), rules, users);
}

qint64 Api::accountGetPrivacy(const InputPrivacyKey &key) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountGetPrivacy);
    p.appendInputPrivacyKey(key);

    return mMainSession->sendQuery(p, &accountAccountGetPrivacyMethods, QVariant(), __FUNCTION__ );
}

qint64 Api::accountSetPrivacy(const InputPrivacyKey &key, const QList<InputPrivacyRule> &rules) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountSetPrivacy);
    p.appendInputPrivacyKey(key);

    p.appendInt(static_cast<qint32>(TL_Vector));
    p.appendInt(rules.size());
    Q_FOREACH (InputPrivacyRule rule, rules)
        p.appendInputPrivacyRule(rule);

    return mMainSession->sendQuery(p, &accountAccountSetPrivacyMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountDeleteAccountAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountDeleteAccountResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountDeleteAccount(const QString &reason) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountDeleteAccount);
    p.appendQString(reason);

    return mMainSession->sendQuery(p, &accountDeleteAccountMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetAccountTTLAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountGetAccountTTLResult(q->msgId(), inboundPkt.fetchAccountDaysTTL());
}

qint64 Api::accountGetAccountTTL()
{
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountGetAccountTTL);

    return mMainSession->sendQuery(p, &accountGetAccountTTLMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountSetAccountTTLAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountSetAccountTTLResult(q->msgId(), inboundPkt.fetchBool());
}

void Api::onAccountChangePhoneAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountChangePhoneResult(q->msgId(), inboundPkt.fetchUser());
}

void Api::onAccountUpdateDeviceLockedAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountUpdateDeviceLockedResult(q->msgId(), inboundPkt.fetchBool());
}

void Api::onAccountSentChangePhoneCode(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_AccountSentChangePhoneCode);
    Q_EMIT accountSentChangePhoneCode(q->msgId(), inboundPkt.fetchQString(), inboundPkt.fetchInt());
}

qint64 Api::accountSetAccountTTL(const AccountDaysTTL &ttl) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountSetAccountTTL);
    p.appendAccountDaysTTL(ttl);
    return mMainSession->sendQuery(p, &accountSetAccountTTLMethods, QVariant(), __FUNCTION__ );
}

qint64 Api::accountSendChangePhoneCode(const QString &phone_number) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountSendChangePhoneCode);
    p.appendQString(phone_number);
    return mMainSession->sendQuery(p, &accountSendChangePhoneCodeMethods, QVariant(), __FUNCTION__ );

}

qint64 Api::accountChangePhone(const QString &phone_number, const QString &phone_code_hash, const QString &phone_code) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountChangePhone);
    p.appendQString(phone_number);
    p.appendQString(phone_code_hash);
    p.appendQString(phone_code);
    return mMainSession->sendQuery(p, &accountChangePhoneMethods, QVariant(), __FUNCTION__ );
}

qint64 Api::accountUpdateDeviceLocked(int period) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountUpdateDeviceLocked);
    p.appendInt(period);
    return mMainSession->sendQuery(p, &accountUpdateDeviceLockedMethods, QVariant(), __FUNCTION__ );

}

void Api::onAccountGetPasswordAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountGetPasswordResult(q->msgId(), inboundPkt.fetchAccountPassword());
}

qint64 Api::accountGetPassword() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountGetPassword);
    return mMainSession->sendQuery(p, &accountGetPasswordMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountSetPasswordAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT accountSetPasswordResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::accountSetPassword(const QByteArray &currentPasswordHash, const QByteArray &newSalt, const QByteArray &newPasswordHash, const QString &hint) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_AccountSetPassword);
    p.appendBytes(currentPasswordHash);
    p.appendBytes(newSalt);
    p.appendBytes(newPasswordHash);
    p.appendQString(hint);
    return mMainSession->sendQuery(p, &accountSetPasswordMethods, QVariant(), __FUNCTION__ );
}

// ### photos.uploadProfilePhoto
void Api::onPhotosUploadProfilePhotoAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_PhotosPhoto);
    Photo photo = inboundPkt.fetchPhoto();
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    Q_EMIT photosPhoto(q->msgId(), photo, users);
}

qint64 Api::photosUploadProfilePhoto(const InputFile &file, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_PhotosUploadProfilePhoto);
    p.appendInputFile(file);
    p.appendQString(caption);
    p.appendInputGeoPoint(geoPoint);
    p.appendInputPhotoCrop(crop);
    return mMainSession->sendQuery(p, &photosUploadProfilePhotoMethods, QVariant(), __FUNCTION__ );
}

// ### photos.updateProfilePhoto
void Api::onPhotosUpdateProfilePhotoAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT photosUserProfilePhoto(q->msgId(), inboundPkt.fetchUserProfilePhoto());
}

qint64 Api::photosUpdateProfilePhoto(const InputPhoto &id, const InputPhotoCrop &crop) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_PhotosUpdateProfilePhoto);
    p.appendInputPhoto(id);
    p.appendInputPhotoCrop(crop);
    return mMainSession->sendQuery(p, &photosUpdateProfilePhotoMethods, QVariant(), __FUNCTION__ );
}

//
// USERS
//
// ### users.getUsers
void Api::onUsersGetUsersAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    Q_EMIT usersGetUsersResult(q->msgId(), users);
}

qint64 Api::usersGetUsers(const QList<InputUser> &users) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_UsersGetUsers);
    p.appendInt(TL_Vector);
    p.appendInt(users.size());
    Q_FOREACH (InputUser user, users) {
        p.appendInputUser(user);
    }
    return mMainSession->sendQuery(p, &usersGetUsersMethods, QVariant(), __FUNCTION__ );
}

// ### users.getFullUser
void Api::onUsersGetFullUserAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_UserFull);
    User user = inboundPkt.fetchUser();
    ContactsLink link = inboundPkt.fetchContactsLink();
    Photo photo = inboundPkt.fetchPhoto();
    PeerNotifySettings notifySettings = inboundPkt.fetchPeerNotifySetting();
    bool blocked = inboundPkt.fetchBool();
    QString realFirstName = inboundPkt.fetchQString();
    QString realLastName = inboundPkt.fetchQString();
    Q_EMIT userFull(q->msgId(), user, link, photo, notifySettings, blocked, realFirstName, realLastName);
}

qint64 Api::usersGetFullUser(const InputUser &user) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_UsersGetFullUser);
    p.appendInputUser(user);
    return mMainSession->sendQuery(p, &usersGetFullUserMethods, QVariant(), __FUNCTION__ );
}

// ### photos.getUserPhotos
void Api::onPhotosGetUserPhotos(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_PhotosPhotos || x == (qint32)TL_PhotosPhotosSlice);
    qint32 count = -1;
    if (x == (qint32)TL_PhotosPhotosSlice) {
        count = inboundPkt.fetchInt();
    }
    //photos
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Photo> photos;
    for (qint32 i = 0; i < n; i++) {
        photos.append(inboundPkt.fetchPhoto());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    if (x == (qint32)TL_PhotosPhotosSlice) {
        Q_EMIT photosPhotosSlice(q->msgId(), count, photos, users);
    } else {
        Q_EMIT photosPhotos(q->msgId(), photos, users);
    }
}

qint64 Api::photosGetUserPhotos(const InputUser &user, qint32 offset, qint32 maxId, qint32 limit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_PhotosGetUserPhotos);
    p.appendInputUser(user);
    p.appendInt(offset);
    p.appendInt(maxId);
    p.appendInt(limit);
    return mMainSession->sendQuery(p, &photosGetUserPhotosMethods, QVariant(), __FUNCTION__ );
}

//
// CONTACTS
//
// ### contacts.getStatuses
void Api::onContactsGetStatusesAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<ContactStatus> statuses;
    for (qint32 i = 0; i < n; i++) {
        statuses.append(inboundPkt.fetchContactStatus());
    }
    Q_EMIT contactsGetStatusesResult(q->msgId(), statuses);
}

qint64 Api::contactsGetStatuses() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsGetStatuses);
    return mMainSession->sendQuery(p, &contactsGetStatusesMethods, QVariant(), __FUNCTION__ );
}

// ### contacts.getContacts
void Api::onContactsGetContactsAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_ContactsContacts || x == (qint32)TL_ContactsContactsNotModified);
    if (x == (qint32)TL_ContactsContacts) {
        //contacts
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        qint32 n = inboundPkt.fetchInt();
        QList<Contact> contacts;
        for (qint32 i = 0; i < n; i++) {
            contacts.append(inboundPkt.fetchContact());
        }
        //users
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        QList<User> users;
        for (qint32 i = 0; i < n; i++) {
            users.append(inboundPkt.fetchUser());
        }
        Q_EMIT contactsContacts(q->msgId(), contacts, users);
    } else {
        Q_EMIT contactsContactsNotModified(q->msgId());
    }
}

qint64 Api::contactsGetContacts(const QString &hash) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsGetContacts);
    p.appendQString(hash);
    return mMainSession->sendQuery(p, &contactsGetContactsMethods, QVariant(), __FUNCTION__ );
}

// ### contacts.importContacts
void Api::onContactsImportContactsAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_ContactsImportedContacts);
    // imported contacts
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<ImportedContact> imported;
    for (qint32 i = 0; i < n; i++) {
        imported.append(inboundPkt.fetchImportedContact());
    }
    // retry contacts
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<qint64> retryContacts;
    for (qint32 i = 0; i < n; i++) {
        retryContacts.append(inboundPkt.fetchLong());
    }
    // users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    Q_EMIT contactsImportedContacts(q->msgId(), imported, retryContacts, users);
}

qint64 Api::contactsImportContacts(const QList<InputContact> &contacts, bool replace) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsImportContacts);
    p.appendInt(TL_Vector);
    p.appendInt(contacts.length());
    Q_FOREACH (InputContact contact, contacts) {
        p.appendInputContact(contact);
    }
    p.appendBool(replace);
    return mMainSession->sendQuery(p, &contactsImportContactsMethods, QVariant(), __FUNCTION__ );
}

// ### contacts.deleteContact
void Api::onContactsDeleteContactAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_ContactsLink);
    ContactLink myLink = inboundPkt.fetchContactLink();
    ContactLink foreignLink = inboundPkt.fetchContactLink();
    User user = inboundPkt.fetchUser();
    Q_EMIT contactsDeleteContactLink(q->msgId(), myLink, foreignLink, user);
}

qint64 Api::contactsDeleteContact(const InputUser &id) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsDeleteContact);
    p.appendInputUser(id);
    return mMainSession->sendQuery(p, &contactsDeleteContactMethods, QVariant(), __FUNCTION__ );
}

// ### contacts.deleteContacts
void Api::onContactsDeleteContactsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT contactsDeleteContactsResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::contactsDeleteContacts(const QList<InputUser> &ids) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsDeleteContacts);
    p.appendInt(TL_Vector);
    p.appendInt(ids.length());
    Q_FOREACH (InputUser id, ids) {
        p.appendInputUser(id);
    }
    return mMainSession->sendQuery(p, &contactsDeleteContactsMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsSearchAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_ContactsFound);
    // contact founds
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<ContactFound> founds;
    for (qint32 i = 0; i < n; i++) {
        founds.append(inboundPkt.fetchContactFound());
    }
    // users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    Q_EMIT contactsFound(q->msgId(), founds, users);
}

qint64 Api::contactsSearch(const QString &q, qint32 limit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsSearch);
    p.appendQString(q);
    p.appendInt(limit);
    return mMainSession->sendQuery(p, &contactsSearchMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsResolveUsernameAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT contactsResolveUsernameResult(q->msgId(), inboundPkt.fetchUser());
}

qint64 Api::contactsResolveUsername(const QString &username) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsResolveUsername);
    p.appendQString(username);
    return mMainSession->sendQuery(p, &contactsResolveUsernameMethods, QVariant(), __FUNCTION__ );
}

//
// BLACKLIST
//
// ### contacts.block
void Api::onContactsBlockAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT contactsBlockResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::contactsBlock(const InputUser &id) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsBlock);
    p.appendInputUser(id);
    return mMainSession->sendQuery(p, &contactsBlockMethods, QVariant(), __FUNCTION__ );
}

// ### contacts.unblock
void Api::onContactsUnblockAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT contactsUnblockResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::contactsUnblock(const InputUser &id) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsUnblock);
    p.appendInputUser(id);
    return mMainSession->sendQuery(p, &contactsUnblockMethods, QVariant(), __FUNCTION__ );
}

// ### contacts.getBlocked
void Api::onContactsGetBlockedAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_ContactsBlocked || x == (qint32)TL_ContactsBlockedSlice);
    qint32 count = -1;
    if (x == (qint32)TL_ContactsBlockedSlice) {
        count = inboundPkt.fetchInt();
    }
    // blocked
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<ContactBlocked> blocked;
    for (qint32 i = 0; i < n; i++) {
        blocked.append(inboundPkt.fetchContactBlocked());
    }
    // users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    if (x == (qint32)TL_ContactsBlockedSlice) {
        Q_EMIT contactsBlockedSlice(q->msgId(), count, blocked, users);
    } else {
        Q_EMIT contactsBlocked(q->msgId(), blocked, users);
    }
}

qint64 Api::contactsGetBlocked(qint32 offset, qint32 limit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_ContactsGetBlocked);
    p.appendInt(offset);
    p.appendInt(limit);
    return mMainSession->sendQuery(p, &contactsGetBlockedMethods, QVariant(), __FUNCTION__ );
}

//
// MESSAGES
//
// ### messages.sendMessage
void Api::onMessagesSendMessageAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesSentMessage || x == (qint32)TL_MessagesSentMessageLink);
    qint32 id = inboundPkt.fetchInt();
    qint32 date = inboundPkt.fetchInt();
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesSentMessageLink) {
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        qint32 n = inboundPkt.fetchInt();
        QList<ContactsLink> links;
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesSentMessageLink(q->msgId(), id, date, pts, pts_count, seq, links);
    } else {
        Q_EMIT messagesSentMessage(q->msgId(), id, date, pts, pts_count);
    }
}

qint64 Api::messagesSendMessage(const InputPeer &peer, const QString &message, qint64 randomId, qint32 replyToMsgId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesSendMessage);
    p.appendInputPeer(peer);
    p.appendInt(replyToMsgId);
    p.appendQString(message);
    p.appendLong(randomId);
    return mMainSession->sendQuery(p, &messagesSendMessageMethods, QVariant(), __FUNCTION__ );
}

// ### messages.sendMedia
void Api::onMessagesSendMediaAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesSentMediaStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesSentMediaStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesSendMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesSendMedia);
    p.appendInputPeer(peer);
    p.appendInt(replyToMsgId);
    p.appendInputMedia(media);
    p.appendLong(randomId);
    return mMainSession->sendQuery(p, &messagesSendMediaMethods, QVariant(), __FUNCTION__ );
}

// ### messages.setTyping
void Api::onMessagesSetTypingAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesSetTypingResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::messagesSetTyping(const InputPeer &peer, const SendMessageAction &action) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesSetTyping);
    p.appendInputPeer(peer);
    p.appendSendMessageAction(action);
    return mMainSession->sendQuery(p, &messagesSetTypingMethods, QVariant(), __FUNCTION__ );
}

// ### messages.getMessages
void Api::onMessagesGetMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesMessages || x == (qint32)TL_MessagesMessagesSlice);
    // slice count
    qint32 count = -1;
    if (x == (qint32)TL_MessagesMessagesSlice) {
        count = inboundPkt.fetchInt();
    }
    //messages
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Message> messages;
    for (qint32 i = 0; i < n; i++) {
        messages.append(inboundPkt.fetchMessage());
    }
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    if (x == (qint32)TL_MessagesMessagesSlice) {
        Q_EMIT messagesGetMessagesMessagesSlice(q->msgId(), count, messages, chats, users);
    } else {
        Q_EMIT messagesGetMessagesMessages(q->msgId(), messages, chats, users);
    }
}

qint64 Api::messagesGetMessages(const QList<qint32> &ids) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetMessages);
    p.appendInt(TL_Vector);
    p.appendInt(ids.length());
    Q_FOREACH (qint32 id, ids) {
        p.appendInt(id);
    }
    return mMainSession->sendQuery(p, &messagesGetMessagesMethods, QVariant(), __FUNCTION__ );
}

// ### messages.getDialogs
void Api::onMessagesGetDialogsAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesDialogs || x == (qint32)TL_MessagesDialogsSlice);
    // slice count
    qint32 count = -1;
    if (x == (qint32)TL_MessagesDialogsSlice) {
        count = inboundPkt.fetchInt();
    }
    //dialogs
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Dialog> dialogs;
    for (qint32 i = 0; i < n; i++) {
        dialogs.append(inboundPkt.fetchDialog());
    }
    //messages
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Message> messages;
    for (qint32 i = 0; i < n; i++) {
        messages.append(inboundPkt.fetchMessage());
    }
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    if (x == (qint32)TL_MessagesDialogsSlice) {
        Q_EMIT messagesDialogsSlice(q->msgId(), count, dialogs, messages, chats, users);
    } else {
        Q_EMIT messagesDialogs(q->msgId(), dialogs, messages, chats, users);
    }
}

qint64 Api::messagesGetDialogs(qint32 offset, qint32 maxId, qint32 limit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetDialogs);
    p.appendInt(offset);
    p.appendInt(maxId);
    p.appendInt(limit);
    return mMainSession->sendQuery(p, &messagesGetDialogsMethods, QVariant(), __FUNCTION__ );
}

// ### messages.getHistory
void Api::onMessagesGetHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesMessages || x == (qint32)TL_MessagesMessagesSlice);
    // slice count
    qint32 count = -1;
    if (x == (qint32)TL_MessagesMessagesSlice) {
        count = inboundPkt.fetchInt();
    }
    //messages
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Message> messages;
    for (qint32 i = 0; i < n; i++) {
        messages.append(inboundPkt.fetchMessage());
    }
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    if (x == (qint32)TL_MessagesMessagesSlice) {
        Q_EMIT messagesGetHistoryMessagesSlice(q->msgId(), count, messages, chats, users);
    } else {
        Q_EMIT messagesGetHistoryMessages(q->msgId(), messages, chats, users);
    }
}

qint64 Api::messagesGetHistory(const InputPeer &peer, qint32 offset, qint32 maxId, qint32 limit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetHistory);
    p.appendInputPeer(peer);
    p.appendInt(offset);
    p.appendInt(maxId);
    p.appendInt(limit);
    return mMainSession->sendQuery(p, &messagesGetHistoryMethods, QVariant(), __FUNCTION__ );
}

// ### messages.search
void Api::onMessagesSearchAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesMessages || x == (qint32)TL_MessagesMessagesSlice);
    // slice count
    qint32 count = -1;
    if (x == (qint32)TL_MessagesMessagesSlice) {
        count = inboundPkt.fetchInt();
    }
    //messages
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Message> messages;
    for (qint32 i = 0; i < n; i++) {
        messages.append(inboundPkt.fetchMessage());
    }
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    if (x == (qint32)TL_MessagesMessagesSlice) {
        Q_EMIT messagesSearchMessagesSlice(q->msgId(), count, messages, chats, users);
    } else {
        Q_EMIT messagesSearchMessages(q->msgId(), messages, chats, users);
    }
}

qint64 Api::messagesSearch(const InputPeer &peer, const QString &q, const MessagesFilter &filter, qint32 minDate, qint32 maxDate, qint32 offset, qint32 maxId, qint32 limit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesSearch);
    p.appendInputPeer(peer);
    p.appendQString(q);
    p.appendMessagesFilter(filter);
    p.appendInt(minDate);
    p.appendInt(maxDate);
    p.appendInt(offset);
    p.appendInt(maxId);
    p.appendInt(limit);
    return mMainSession->sendQuery(p, &messagesSearchMethods, QVariant(), __FUNCTION__ );
}

// ### messages.readHistory
void Api::onMessagesReadHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_MessagesAffectedHistory);
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    qint32 offset = inboundPkt.fetchInt();
    Q_EMIT messagesReadAffectedHistory(q->msgId(), pts, pts_count, offset);
}

qint64 Api::messagesReadHistory(const InputPeer &peer, qint32 maxId, qint32 offset) {
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_MessagesReadHistory);
    p.appendInputPeer(peer);
    p.appendInt(maxId);
    p.appendInt(offset);
    return mMainSession->sendQuery(p, &messagesReadHistoryMethods, QVariant(), __FUNCTION__ );
}

// ### messages.readMessageContents
void Api::onMessagesReadMessageContentsAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesReadMessageContentsResult(q->msgId(), inboundPkt.fetchAffectedMessages());
}

qint64 Api::messagesReadMessageContents(const QList<qint32> &ids) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesReadMessageContents);
    p.appendInt(TL_Vector);
    p.appendInt(ids.length());
    Q_FOREACH (qint32 id, ids) {
        p.appendInt(id);
    }
    return mMainSession->sendQuery(p, &messagesReadMessageContentsMethods, QVariant(), __FUNCTION__ );
}


// ### messages.deleteHistory
void Api::onMessagesDeleteHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_MessagesAffectedHistory);
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    qint32 offset = inboundPkt.fetchInt();
    Q_EMIT messagesDeleteAffectedHistory(q->msgId(), pts, pts_count, offset);
}

qint64 Api::messagesDeleteHistory(const InputPeer &peer, qint32 offset) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesDeleteHistory);
    p.appendInputPeer(peer);
    p.appendInt(offset);
    return mMainSession->sendQuery(p, &messagesDeleteHistoryMethods, QVariant(), __FUNCTION__ );
}

// ### messages.deleteMessages
void Api::onMessagesDeleteMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesDeleteMessagesResult(q->msgId(), inboundPkt.fetchAffectedMessages());
}

qint64 Api::messagesDeleteMessages(const QList<qint32> &ids) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesDeleteMessages);
    p.appendInt(TL_Vector);
    p.appendInt(ids.length());
    Q_FOREACH (qint32 id, ids) {
        p.appendInt(id);
    }
    return mMainSession->sendQuery(p, &messagesDeleteMessagesMethods, QVariant(), __FUNCTION__ );
}

// ### messages.restoreMessages
void Api::onMessagesRestoreMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<qint32> restoredIds;
    for (qint32 i = 0; i < n; i++) {
        restoredIds.append(inboundPkt.fetchInt());
    }
    Q_EMIT messagesRestoreMessagesResult(q->msgId(), restoredIds);
}

qint64 Api::messagesRestoreMessages(const QList<qint32> &ids) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesRestoreMessages);
    p.appendInt(TL_Vector);
    p.appendInt(ids.length());
    Q_FOREACH (qint32 id, ids) {
        p.appendInt(id);
    }
    return mMainSession->sendQuery(p, &messagesRestoreMessagesMethods, QVariant(), __FUNCTION__ );
}

// ### messages.receivedMessages
void Api::onMessagesReceivedMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<qint32> receivedIds;
    for (qint32 i = 0; i < n; i++) {
        receivedIds.append(inboundPkt.fetchInt());
    }
    Q_EMIT messagesReceivedMessagesResult(q->msgId(), receivedIds);
}

qint64 Api::messagesReceivedMessages(qint32 maxId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesReceivedMessages);
    p.appendInt(maxId);
    return mMainSession->sendQuery(p, &messagesReceivedMessagesMethods, QVariant(), __FUNCTION__ );
}

// ### messages.forwardMessage
void Api::onMessagesForwardMessageAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesForwardMsgStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesForwardMsgStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesForwardMessage(const InputPeer &peer, qint32 id, qint64 randomId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesForwardMessage);
    p.appendInputPeer(peer);
    p.appendInt(id);
    p.appendLong(randomId);
    return mMainSession->sendQuery(p, &messagesForwardMessageMethods, QVariant(), __FUNCTION__ );
}

// ### messages.forwardMessages
void Api::onMessagesForwardMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessages || x == (qint32)TL_MessagesStatedMessagesLinks);
    //messages
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Message> messages;
    for (qint32 i = 0; i < n; i++) {
        messages.append(inboundPkt.fetchMessage());
    }
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessagesLinks) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesForwardMsgsStatedMessagesLinks(q->msgId(), messages, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesForwardMsgsStatedMessages(q->msgId(), messages, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesForwardMessages(const InputPeer &peer, const QList<qint32> &ids, const QList<qint32> &randomIds) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesForwardMessages);
    p.appendInputPeer(peer);
    p.appendInt(TL_Vector);
    p.appendInt(ids.length());
    Q_FOREACH (qint32 id, ids) {
        p.appendInt(id);
    }
    p.appendInt(TL_Vector);
    p.appendInt(randomIds.length());
    Q_FOREACH (qint32 id, randomIds) {
        p.appendInt(id);
    }
    return mMainSession->sendQuery(p, &messagesForwardMessagesMethods, QVariant(), __FUNCTION__ );
}

// ### messages.sendBroadcast
void Api::onMessagesSendBroadcastAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessages || x == (qint32)TL_MessagesStatedMessagesLinks);
    //messages
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Message> messages;
    for (qint32 i = 0; i < n; i++) {
        messages.append(inboundPkt.fetchMessage());
    }
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessagesLinks) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesSendBroadcastStatedMessagesLinks(q->msgId(), messages, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesSendBroadcastStatedMessages(q->msgId(), messages, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesSendBroadcast(const QList<InputUser> &contacts, const QString &message, const InputMedia &media) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesSendBroadcast);
    p.appendInt(TL_Vector);
    p.appendInt(contacts.length());
    Q_FOREACH (InputUser user, contacts) {
        p.appendInputUser(user);
    }
    p.appendQString(message);
    p.appendInputMedia(media);
    return mMainSession->sendQuery(p, &messagesSendBroadcastMethods, QVariant(), __FUNCTION__ );
}

//
// CHATS
//
// ### messages.getChats
void Api::onMessagesGetChatsAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_MessagesChats);
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    Q_EMIT messagesChats(q->msgId(), chats);
}

qint64 Api::messagesGetChats(const QList<qint32> &chatIds) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetChats);
    p.appendInt(TL_Vector);
    p.appendInt(chatIds.length());
    Q_FOREACH (qint32 id, chatIds) {
        p.appendInt(id);
    }
    return mMainSession->sendQuery(p, &messagesGetChatsMethods, QVariant(), __FUNCTION__ );
}

// ### messages.getFullChat
void Api::onMessagesGetFullChatAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_MessagesChatFull);
    ChatFull fullChat = inboundPkt.fetchChatFull();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    Q_EMIT messagesChatFull(q->msgId(), fullChat, chats, users);
}

qint64 Api::messagesGetFullChat(qint32 chatId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetFullChat);
    p.appendInt(chatId);
    return mMainSession->sendQuery(p, &messagesGetFullChatMethods, QVariant(), __FUNCTION__ );
}

// ### messages.editChatTitle
void Api::onMessagesEditChatTitleAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesEditChatTitleStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesEditChatTitleStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesEditChatTitle(qint32 chatId, const QString &title) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesEditChatTitle);
    p.appendInt(chatId);
    p.appendQString(title);
    return mMainSession->sendQuery(p, &messagesEditChatTitleMethods, QVariant(), __FUNCTION__ );
}

// ### messages.editChatPhoto
void Api::onMessagesEditChatPhotoAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesEditChatPhotoStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesEditChatPhotoStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesEditChatPhoto(qint32 chatId, const InputChatPhoto &photo) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesEditChatPhoto);
    p.appendInt(chatId);
    p.appendInputChatPhoto(photo);
    return mMainSession->sendQuery(p, &messagesEditChatPhotoMethods, QVariant(), __FUNCTION__ );
}

// ### messages.addChatUser
void Api::onMessagesAddChatUserAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesAddChatUserStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesAddChatUserStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesAddChatUser(qint32 chatId, const InputUser &user, qint32 fwdLimit) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesAddChatUser);
    p.appendInt(chatId);
    p.appendInputUser(user);
    p.appendInt(fwdLimit);
    return mMainSession->sendQuery(p, &messagesAddChatUserMethods, QVariant(), __FUNCTION__ );
}

// ### messages.deleteChatUser
void Api::onMessagesDeleteChatUserAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesDeleteChatUserStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesDeleteChatUserStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesDeleteChatUser(qint32 chatId, const InputUser &user) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesDeleteChatUser);
    p.appendInt(chatId);
    p.appendInputUser(user);
    return mMainSession->sendQuery(p, &messagesDeleteChatUserMethods, QVariant(), __FUNCTION__ );
}

// ### messages.createChat
void Api::onMessagesCreateChatAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesStatedMessage || x == (qint32)TL_MessagesStatedMessageLink);
    Message message = inboundPkt.fetchMessage();
    //chats
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<Chat> chats;
    for (qint32 i = 0; i < n; i++) {
        chats.append(inboundPkt.fetchChat());
    }
    //users
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    n = inboundPkt.fetchInt();
    QList<User> users;
    for (qint32 i = 0; i < n; i++) {
        users.append(inboundPkt.fetchUser());
    }
    //links
    qint32 pts = inboundPkt.fetchInt();
    qint32 pts_count = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesStatedMessageLink) {
        QList<ContactsLink> links;
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        for (qint32 i = 0; i < n; i++) {
            links.append(inboundPkt.fetchContactsLink());
        }
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT messagesCreateChatStatedMessageLink(q->msgId(), message, chats, users, links, pts, pts_count, seq);
    } else {
        Q_EMIT messagesCreateChatStatedMessage(q->msgId(), message, chats, users, pts, pts_count);
    }
}

qint64 Api::messagesCreateChat(const QList<InputUser> &users, const QString &title) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesCreateChat);
    p.appendInt(TL_Vector);
    p.appendInt(users.length());
    Q_FOREACH (InputUser user, users) {
        p.appendInputUser(user);
    }
    p.appendQString(title);
    return mMainSession->sendQuery(p, &messagesCreateChatMethods, QVariant(), __FUNCTION__ );
}

//
// SECRET CHATS
//
// ### messages.getDhConfig
void Api::onMessagesGetDhConfigAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesDhConfig || x == (qint32)TL_MessagesDhConfigNotModified);
    if (x == (qint32)TL_MessagesDhConfig) {
        qint32 g = inboundPkt.fetchInt();
        QByteArray p = inboundPkt.fetchBytes();
        qint32 version = inboundPkt.fetchInt();
        QByteArray random = inboundPkt.fetchBytes();
        Q_EMIT messagesDhConfig(q->msgId(), g, p, version, random);
    } else {
        QByteArray random = inboundPkt.fetchBytes();
        Q_EMIT messagesDhConfigNotModified(q->msgId(), random);
    }
}

qint64 Api::messagesGetDhConfig(qint32 version, qint32 randomLength) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetDhConfig);
    p.appendInt(version);
    p.appendInt(randomLength);
    return mMainSession->sendQuery(p, &messagesGetDhConfigMethods, QVariant(), __FUNCTION__ );
}

// ### messages.requestEncryption
void Api::onMessagesRequestEncryptionAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesRequestEncryptionEncryptedChat(q->msgId(), inboundPkt.fetchEncryptedChat());
}

qint64 Api::messagesRequestEncryption(const InputUser &user, qint32 randomId, QByteArray g_a) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesRequestEncryption);
    p.appendInputUser(user);
    p.appendInt(randomId);
    p.appendBytes(g_a);
    return mMainSession->sendQuery(p, &messagesRequestEncryptionMethods, QVariant(), __FUNCTION__ );
}

// ### message.acceptEncryption
void Api::onMessagesAcceptEncryptionAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesAcceptEncryptionEncryptedChat(q->msgId(), inboundPkt.fetchEncryptedChat());
}

qint64 Api::messagesAcceptEncryption(const InputEncryptedChat &inputEncryptedChat, QByteArray g_b, qint64 keyFingerprint) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesAcceptEncryption);
    p.appendInputEncryptedChat(inputEncryptedChat);
    p.appendBytes(g_b);
    p.appendLong(keyFingerprint);
    return mMainSession->sendQuery(p, &messagesAcceptEncryptionMethods, QVariant(), __FUNCTION__ );
}

// ### messages.discardEncryption
void Api::onMessagesDiscardEncryptionAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesDiscardEncryptionResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::messagesDiscardEncryption(qint32 chatId) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesDiscardEncryption);
    p.appendInt(chatId);
    return mMainSession->sendQuery(p, &messagesDiscardEncryptionMethods, QVariant(), __FUNCTION__ );
}

// ### messages.setEncryptedTyping
void Api::onMessagesSetEncryptedTypingAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesSetEncryptedTypingResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::messagesSetEncryptedTyping(const InputEncryptedChat &inputEncryptedChat, bool typing) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesSetEncryptedTyping);
    p.appendInputEncryptedChat(inputEncryptedChat);
    p.appendBool(typing);
    return mMainSession->sendQuery(p, &messagesSetEncryptedTypingMethods, QVariant(), __FUNCTION__ );
}

// ### messages.readEncryptedHistory
void Api::onMessagesReadEncryptedHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT messagesReadEncryptedHistoryResult(q->msgId(), inboundPkt.fetchBool());
}

qint64 Api::messagesReadEncryptedHistory(const InputEncryptedChat &inputEncryptedChat, qint32 maxDate){
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesReadEncryptedHistory);
    p.appendInputEncryptedChat(inputEncryptedChat);
    p.appendInt(maxDate);
    return mMainSession->sendQuery(p, &messagesReadEncryptedHistoryMethods, QVariant(), __FUNCTION__ );
}

// ### messages.sendEncrypted
void Api::onMessagesSendEncryptedAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesSentEncryptedMessage || x == (qint32)TL_MessagesSentEncryptedFile);
    qint32 date = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesSentEncryptedFile) {
        EncryptedFile file = inboundPkt.fetchEncryptedFile();
        Q_EMIT messagesSendEncryptedSentEncryptedFile(q->msgId(), date, file);
    } else {
        Q_EMIT messagesSendEncryptedSentEncryptedMessage(q->msgId(), date);
    }
}

qint64 Api::messagesSendEncrypted(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data) {
    OutboundPkt p(mSettings);
    // include invokeAfterMsgs with the sequence of previous messages to manage before this one
    qint32 n = previousMsgs.size();
    if (n > 0) {
        p.appendInt(TL_InvokeAfterMsgs);
        p.appendInt(TL_Vector);
        p.appendInt(n);
        Q_FOREACH (qint64 msgId, previousMsgs) {
            p.appendLong(msgId);
        }
    }
    // send the message
    p.appendInt(TL_MessagesSendEncrypted);
    p.appendInputEncryptedChat(inputEncryptedChat);
    p.appendLong(randomId);
    p.appendBytes(data);
    return mMainSession->sendQuery(p, &messagesSendEncryptedMethods, QVariant(), __FUNCTION__ );
}

// ### messages.sendEncryptedFile
void Api::onMessagesSendEncryptedFileAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesSentEncryptedMessage || x == (qint32)TL_MessagesSentEncryptedFile);
    qint32 date = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesSentEncryptedFile) {
        EncryptedFile file = inboundPkt.fetchEncryptedFile();
        Q_EMIT messagesSendEncryptedFileSentEncryptedFile(q->msgId(), date, file);
    } else {
        Q_EMIT messagesSendEncryptedFileSentEncryptedMessage(q->msgId(), date);
    }
}

qint64 Api::messagesSendEncryptedFile(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data,
                                      const InputEncryptedFile &inputEncryptedFile) {
    OutboundPkt p(mSettings);
    // include invokeAfterMsgs with the sequence of previous messages to manage before this one
    qint32 n = previousMsgs.size();
    if (n > 0) {
        p.appendInt(TL_InvokeAfterMsgs);
        p.appendInt(TL_Vector);
        p.appendInt(n);
        Q_FOREACH (qint64 msgId, previousMsgs) {
            p.appendLong(msgId);
        }
    }
    // send the file message
    p.appendInt(TL_MessagesSendEncryptedFile);
    p.appendInputEncryptedChat(inputEncryptedChat);
    p.appendLong(randomId);
    p.appendBytes(data);
    p.appendInputEncryptedFile(inputEncryptedFile);
    return mMainSession->sendQuery(p, &messagesSendEncryptedFileMethods, QVariant(), __FUNCTION__ );
}

// ### messages.sendEncryptedService
void Api::onMessagesSendEncryptedServiceAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_MessagesSentEncryptedMessage || x == (qint32)TL_MessagesSentEncryptedFile);
    qint32 date = inboundPkt.fetchInt();
    if (x == (qint32)TL_MessagesSentEncryptedFile) {
        EncryptedFile file = inboundPkt.fetchEncryptedFile();
        Q_EMIT messagesSendEncryptedServiceSentEncryptedFile(q->msgId(), date, file);
    } else {
        Q_EMIT messagesSendEncryptedServiceSentEncryptedMessage(q->msgId(), date);
    }
}

qint64 Api::messagesSendEncryptedService(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data) {
    OutboundPkt p(mSettings);
    // include invokeAfterMsgs with the sequence of previous messages to manage before this one
    qint32 n = previousMsgs.size();
    if (n > 0) {
        p.appendInt(TL_InvokeAfterMsgs);
        p.appendInt(TL_Vector);
        p.appendInt(n);
        Q_FOREACH (qint64 msgId, previousMsgs) {
            p.appendLong(msgId);
        }
    }
    // send the encrypted service message
    p.appendInt(TL_MessagesSendEncryptedService);
    p.appendInputEncryptedChat(inputEncryptedChat);
    p.appendLong(randomId);
    p.appendBytes(data);
    return mMainSession->sendQuery(p, &messagesSendEncryptedServiceMethods, QVariant(), __FUNCTION__ );
}

// ### messages.receivedQueue
void Api::onMessagesReceivedQueueAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
    qint32 n = inboundPkt.fetchInt();
    QList<qint64> randomIds;
    for (qint32 i = 0; i < n; i++) {
        randomIds.append(inboundPkt.fetchLong());
    }
    Q_EMIT messagesReceivedQueueResult(q->msgId(), randomIds);
}

qint64 Api::messagesReceivedQueue(qint32 maxQts) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesReceivedQueue);
    p.appendInt(maxQts);
    return mMainSession->sendQuery(p, &messagesReceivedQueueMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetStickersAnswer(Query *q, InboundPkt &inboundPkt) {
    qint64 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)MessagesStickers::typeMessagesStickers ||
           x == (qint32)MessagesStickers::typeMessagesStickersNotModified);

    MessagesStickers stickers( static_cast<MessagesStickers::MessagesStickersType>(x) );
    switch(x)
    {
    case MessagesStickers::typeMessagesStickers:
    {
        stickers.setHash(inboundPkt.fetchQString());
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        qint32 n = inboundPkt.fetchInt();
        QList<Document> documents;
        for (qint32 i = 0; i < n; i++) {
            documents.append(inboundPkt.fetchDocument());
        }
        stickers.setDocuments(documents);
    }
        break;
    }

    Q_EMIT messagesGetStickersResult(q->msgId(), stickers);
}

qint64 Api::messagesGetStickers(const QString &emoticon, const QString &hash) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetStickers);
    p.appendQString(emoticon);
    p.appendQString(hash);
    return mMainSession->sendQuery(p, &messagesGetStickersMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetAllStickersAnswer(Query *q, InboundPkt &inboundPkt) {
    qint64 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)MessagesAllStickers::typeMessagesAllStickers ||
           x == (qint32)MessagesAllStickers::typeMessagesAllStickersNotModified);

    MessagesAllStickers stickers( static_cast<MessagesAllStickers::MessagesAllStickersType>(x) );
    switch(x)
    {
    case MessagesAllStickers::typeMessagesAllStickers:
    {
        stickers.setHash(inboundPkt.fetchQString());
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        qint32 ns = inboundPkt.fetchInt();
        QList<StickerPack> packs;
        for (qint32 i = 0; i < ns; i++) {
            packs.append(inboundPkt.fetchStickerPack());
        }
        stickers.setPacks(packs);

        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        qint32 nd = inboundPkt.fetchInt();
        QList<Document> documents;
        for (qint32 i = 0; i < nd; i++) {
            documents.append(inboundPkt.fetchDocument());
        }
        stickers.setDocuments(documents);
    }
        break;
    }

    Q_EMIT messagesGetAllStickersResult(q->msgId(), stickers);
}

qint64 Api::messagesGetAllStickers(const QString &hash) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_MessagesGetAllStickers);
    p.appendQString(hash);
    return mMainSession->sendQuery(p, &messagesGetAllStickersMethods, QVariant(), __FUNCTION__ );
}

//
// UPDATES
//
// ### updates.getState
void Api::onUpdatesGetStateAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_UpdatesState);
    qint32 pts = inboundPkt.fetchInt();
    qint32 qts = inboundPkt.fetchInt();
    qint32 date = inboundPkt.fetchInt();
    qint32 seq = inboundPkt.fetchInt();
    qint32 unreadCount = inboundPkt.fetchInt();
    Q_EMIT updatesState(q->msgId(), pts, qts, date, seq, unreadCount);
}

qint64 Api::updatesGetState() {
    OutboundPkt p(mSettings);
    p.appendInt(TL_UpdatesGetState);
    return mMainSession->sendQuery(p, &updatesGetStateMethods, QVariant(), __FUNCTION__ );
}

// ### updates.getDifference
void Api::onUpdatesGetDifferenceAnswer(Query *q, InboundPkt &inboundPkt) {
    qint32 x = inboundPkt.fetchInt();
    ASSERT(x == (qint32)TL_UpdatesDifferenceEmpty || x == (qint32)TL_UpdatesDifferenceSlice || x == (qint32)TL_UpdatesDifference);
    if (x == (qint32)TL_UpdatesDifferenceEmpty) {
        qint32 date = inboundPkt.fetchInt();
        qint32 seq = inboundPkt.fetchInt();
        Q_EMIT updatesDifferenceEmpty(q->msgId(), date, seq);
    } else {
        //newMessages
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        qint32 n = inboundPkt.fetchInt();
        QList<Message> newMessages;
        for (qint32 i = 0; i < n; i++) {
            newMessages.append(inboundPkt.fetchMessage());
        }
        //newEncryptedMessages
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        QList<EncryptedMessage> encMessages;
        for (qint32 i = 0; i < n; i++) {
            encMessages.append(inboundPkt.fetchEncryptedMessage());
        }
        //otherUpdates
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        QList<Update> otherUpdates;
        for (qint32 i = 0; i < n; i++) {
            otherUpdates.append(inboundPkt.fetchUpdate());
        }
        //chats
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        QList<Chat> chats;
        for (qint32 i = 0; i < n; i++) {
            chats.append(inboundPkt.fetchChat());
        }
        //users
        ASSERT(inboundPkt.fetchInt() == (qint32)TL_Vector);
        n = inboundPkt.fetchInt();
        QList<User> users;
        for (qint32 i = 0; i < n; i++) {
            users.append(inboundPkt.fetchUser());
        }
        UpdatesState state = inboundPkt.fetchUpdatesState();
        if (x == (qint32)TL_UpdatesDifferenceSlice) {
            Q_EMIT updatesDifferenceSlice(q->msgId(), newMessages, encMessages, otherUpdates, chats, users, state);
        } else {
            Q_EMIT updatesDifference(q->msgId(), newMessages, encMessages, otherUpdates, chats, users, state);
        }
    }
}

qint64 Api::updatesGetDifference(qint32 pts, qint32 date, qint32 qts) {
    OutboundPkt p(mSettings);
    p.appendInt(TL_UpdatesGetDifference);
    p.appendInt(pts);
    p.appendInt(date);
    p.appendInt(qts);
    return mMainSession->sendQuery(p, &updatesGetDifferenceMethods, QVariant(), __FUNCTION__ );
}

//
// FILES
//
// ### upload.saveFilePart
void Api::onUploadSaveFilePartAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT uploadSaveFilePartResult(q->msgId(), q->extra().toLongLong(), inboundPkt.fetchBool());
}


qint64 Api::uploadSaveFilePart(Session *session, qint64 fileId, qint32 filePart, const QByteArray &bytes) {
    Q_ASSERT(session);
    OutboundPkt p(mSettings);
    if (session->initConnectionNeeded()) {
        p.initConnection();
        session->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_UploadSaveFilePart);
    p.appendLong(fileId);
    p.appendInt(filePart);
    p.appendBytes(bytes);
    QVariant extra = fileId;
    return session->sendQuery(p, &uploadSaveFilePartMethods, extra);
}

// ### upload.saveBigFilePart
void Api::onUploadSaveBigFilePartAnswer(Query *q, InboundPkt &inboundPkt) {
    Q_EMIT uploadSaveBigFilePartResult(q->msgId(), q->extra().toLongLong(), inboundPkt.fetchBool());
}

qint64 Api::uploadSaveBigFilePart(Session *session, qint64 fileId, qint32 filePart, qint32 fileTotalParts, const QByteArray &bytes) {
    Q_ASSERT(session);
    OutboundPkt p(mSettings);
    if (session->initConnectionNeeded()) {
        p.initConnection();
        session->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_UploadSaveBigFilePart);
    p.appendLong(fileId);
    p.appendInt(filePart);
    p.appendInt(fileTotalParts);
    p.appendBytes(bytes);
    QVariant extra = fileId;
    return session->sendQuery(p, &uploadSaveBigFilePartMethods, extra);
}

// ### upload.getFile
void Api::onUploadGetFileError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT uploadFileError(q->msgId(), errorCode, errorText);
}

void Api::onUploadGetFileAnswer(Query *q, InboundPkt &inboundPkt) {
    ASSERT(inboundPkt.fetchInt() == (qint32)TL_UploadFile);
    StorageFileType storageType = inboundPkt.fetchStorageFileType();
    qint32 mtime = inboundPkt.fetchInt();
    QByteArray bytes = inboundPkt.fetchBytes();
    Q_EMIT uploadFile(q->msgId(), storageType, mtime, bytes);
}

qint64 Api::uploadGetFile(Session *session, const InputFileLocation &location, qint32 offset, qint32 limit) {
    Q_ASSERT(session);
    OutboundPkt p(mSettings);
    if (session->initConnectionNeeded()) {
        p.initConnection();
        session->setInitConnectionNeeded(false);
    }
    p.appendInt(TL_UploadGetFile);
    p.appendInputFileLocation(location);
    p.appendInt(offset);
    p.appendInt(limit);
    return session->sendQuery(p, &uploadGetFileMethods, QVariant(), __FUNCTION__ );
}
