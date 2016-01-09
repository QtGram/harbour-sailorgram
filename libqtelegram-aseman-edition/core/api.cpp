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

#define CHECK_SESSION \
    if(!mMainSession) \
        return 0;

#include "api.h"
#include "telegram/types/types.h"
#include "telegram/functions/functions.h"
#include "telegram/coretypes.h"

#include "util/tlvalues.h"
#include "dcprovider.h"
#include "secret/encrypter.h"

Q_LOGGING_CATEGORY(TG_CORE_API, "tg.core.api")

#define DEBUG_FUNCTION \
    qCDebug(TG_CORE_API) << __LINE__ << __FUNCTION__;

using namespace Tg;

using namespace Tg;

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

    authRequestPasswordRecoveryMethods.onAnswer = &Api::onAuthRequestPasswordRecoveryAnswer;

    authRecoverPasswordMethods.onAnswer = &Api::onAuthRecoverPasswordAnswer;

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

    accountAccountGetPrivacyMethods.onAnswer = &Api::onAccountGetPrivacyRules;

    accountAccountSetPrivacyMethods.onAnswer = &Api::onAccountSetPrivacyRules;

    accountDeleteAccountMethods.onAnswer = &Api::onAccountDeleteAccountAnswer;

    accountGetAccountTTLMethods.onAnswer = &Api::onAccountGetAccountTTLAnswer;

    accountSetAccountTTLMethods.onAnswer = &Api::onAccountSetAccountTTLAnswer;

    accountChangePhoneMethods.onAnswer = &Api::onAccountChangePhoneAnswer;

    accountSendChangePhoneCodeMethods.onAnswer = &Api::onAccountSentChangePhoneCode;

    accountUpdateDeviceLockedMethods.onAnswer = &Api::onAccountUpdateDeviceLockedAnswer;

    accountGetPasswordMethods.onAnswer = &Api::onAccountGetPasswordAnswer;

    accountGetAuthorizationsMethods.onAnswer = &Api::onAccountGetAuthorizationsAnswer;

    accountResetAuthorizationMethods.onAnswer = &Api::onAccountResetAuthorizationAnswer;

    accountGetPasswordSettingsMethods.onAnswer = &Api::onAccountGetPasswordSettingsAnswer;

    accountUpdatePasswordSettingsMethods.onAnswer = &Api::onAccountUpdatePasswordSettingsAnswer;

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

    messagesForwardMediaMethods.onAnswer = &Api::onMessagesForwardMediaAnswer;

    messagesSetTypingMethods.onAnswer = &Api::onMessagesSetTypingAnswer;

    messagesGetMessagesMethods.onAnswer = &Api::onMessagesGetMessagesAnswer;

    messagesGetDialogsMethods.onAnswer = &Api::onMessagesGetDialogsAnswer;

    messagesGetHistoryMethods.onAnswer = &Api::onMessagesGetHistoryAnswer;

    messagesSearchMethods.onAnswer = &Api::onMessagesSearchAnswer;

    messagesReadHistoryMethods.onAnswer = &Api::onMessagesReadHistoryAnswer;

    messagesReadMessageContentsMethods.onAnswer = &Api::onMessagesReadMessageContentsAnswer;

    messagesDeleteHistoryMethods.onAnswer = &Api::onMessagesDeleteHistoryAnswer;

    messagesDeleteMessagesMethods.onAnswer = &Api::onMessagesDeleteMessagesAnswer;

    messagesReceivedMessagesMethods.onAnswer = &Api::onMessagesReceivedMessagesAnswer;

    messagesForwardMessageMethods.onAnswer = &Api::onMessagesForwardMessageAnswer;

    messagesForwardMessagesMethods.onAnswer = &Api::onMessagesForwardMessagesAnswer;

    messagesSendBroadcastMethods.onAnswer = &Api::onMessagesSendBroadcastAnswer;

    messagesGetWebPagePreviewMethods.onAnswer = &Api::onMessagesGetWebPagePreviewAnswer;

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

    messagesExportChatInviteMethods.onAnswer = &Api::onMessagesExportChatInviteAnswer;

    messagesCheckChatInviteMethods.onAnswer = &Api::onMessagesCheckChatInviteAnswer;

    messagesImportChatInviteMethods.onAnswer = &Api::onMessagesImportChatInviteAnswer;

    messagesGetStickerSetMethods.onAnswer = &Api::onMessagesGetStickerSetAnswer;

    messagesInstallStickerSetMethods.onAnswer = &Api::onMessagesInstallStickerSetAnswer;

    messagesUninstallStickerSetMethods.onAnswer = &Api::onMessagesUninstallStickerSetAnswer;

    updatesGetStateMethods.onAnswer = &Api::onUpdatesGetStateAnswer;

    updatesGetDifferenceMethods.onAnswer = &Api::onUpdatesGetDifferenceAnswer;

    uploadSaveFilePartMethods.onAnswer = &Api::onUploadSaveFilePartAnswer;

    uploadSaveBigFilePartMethods.onAnswer = &Api::onUploadSaveBigFilePartAnswer;

    uploadGetFileMethods.onAnswer = &Api::onUploadGetFileAnswer;
    uploadGetFileMethods.onError = &Api::onUploadGetFileError;
}

Api::~Api() {
}

void Api::onError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT error(q->msgId(), errorCode, errorText, q->name() );
}

void Api::onHelpGetConfigAnswer(Query *q, InboundPkt &inboundPkt) {
    const Config &result = Functions::Help::getConfigResult(&inboundPkt);
    Q_EMIT config(q->msgId(), result);
}

qint64 Api::helpGetConfig() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    Functions::Help::getConfig(&p);
    return mMainSession->sendQuery(p, &helpGetConfigMethods, QVariant(), __FUNCTION__ );
}

void Api::onHelpGetInviteTextAnswer(Query *q, InboundPkt &inboundPkt) {
    const HelpInviteText &result = Functions::Help::getInviteTextResult(&inboundPkt);
    Q_EMIT helpGetInviteTextAnswer(q->msgId(), result.message());
}

qint64 Api::helpGetInviteText(const QString &langCode) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    Functions::Help::getInviteText(&p, langCode);
    return mMainSession->sendQuery(p, &helpGetInviteTextMethods, QVariant(), __FUNCTION__ );
}

void Api::onErrorRetry(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT errorRetry(q->msgId(), errorCode, errorText);
}

void Api::onAuthCheckPhoneAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthCheckedPhone &result = Functions::Auth::checkPhoneResult(&inboundPkt);
    if(result.error())
        Q_EMIT authCheckedPhoneError(q->msgId());
    else
        Q_EMIT authCheckedPhone(q->msgId(), result.phoneRegistered());
}

qint64 Api::authCheckPhone(const QString &phoneNumber) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    Functions::Auth::checkPhone(&p, phoneNumber);
    qint64 resultId = mMainSession->sendQuery(p, &authCheckPhoneMethods, QVariant(), __FUNCTION__ );
    Q_EMIT authCheckPhoneSent(resultId, phoneNumber);
    return resultId;
}

void Api::onAuthSendCodeAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthSentCode &result = Functions::Auth::sendCodeResult(&inboundPkt);
    if(result.error())
        Q_EMIT authSendCodeError(q->msgId());
    else
    {
        if (result.classType() == AuthSentCode::typeAuthSentCode) {
            Q_EMIT authSentCode(q->msgId(), result.phoneRegistered(), result.phoneCodeHash(), result.sendCallTimeout(), result.isPassword());
        } else {
            Q_EMIT authSentAppCode(q->msgId(), result.phoneRegistered(), result.phoneCodeHash(), result.sendCallTimeout(), result.isPassword());
        }
    }
}

qint64 Api::authSendCode(const QString &phoneNumber, qint32 smsType, qint32 apiId, const QString &apiHash, const QString &langCode) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::sendCode(&p, phoneNumber, smsType, apiId, apiHash, langCode);
    return mMainSession->sendQuery(p, &authSendCodeMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthSendSmsAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Auth::sendSmsResult(&inboundPkt);
    Q_EMIT authSendSmsResult(q->msgId(), result);
}

qint64 Api::authSendSms(const QString &phoneNumber, const QString &phoneCodeHash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::sendSms(&p, phoneNumber, phoneCodeHash);
    return mMainSession->sendQuery(p, &authSendSmsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthSendCallAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Auth::sendCallResult(&inboundPkt);
    Q_EMIT authSendCallResult(q->msgId(), result);
}

qint64 Api::authSendCall(const QString &phoneNumber, const QString &phoneCodeHash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::sendCall(&p, phoneNumber, phoneCodeHash);
    return mMainSession->sendQuery(p, &authSendCallMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthSignInError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT authSignInError(q->msgId(), errorCode, errorText);
}

void Api::onAuthSignInAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthAuthorization &result = Functions::Auth::signInResult(&inboundPkt);
    if(result.error())
        Q_EMIT authSignInError(q->msgId(), -1, "LIBQTELEGRAM_INTERNAL_ERROR");
    else
        Q_EMIT authSignInAuthorization(q->msgId(), result.expires(), result.user());
}

qint64 Api::authSignIn(const QString &phoneNumber, const QString &phoneCodeHash, const QString &phoneCode) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::signIn(&p, phoneNumber, phoneCodeHash, phoneCode);
    return mMainSession->sendQuery(p, &authSignInMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthSignUpError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT authSignUpError(q->msgId(), errorCode, errorText);
}

void Api::onAuthSignUpAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthAuthorization &result = Functions::Auth::signUpResult(&inboundPkt);
    Q_EMIT authSignUpAuthorization(q->msgId(), result.expires(), result.user());
}

qint64 Api::authSignUp(const QString &phoneNumber, const QString &phoneCodeHash, const QString &phoneCode, const QString &firstName, const QString &lastName) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::signUp(&p, phoneNumber, phoneCodeHash, phoneCode, firstName, lastName);
    return mMainSession->sendQuery(p, &authSignUpMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthLogOutAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Auth::logOutResult(&inboundPkt);
    Q_EMIT authLogOutResult(q->msgId(), result);
}

qint64 Api::authLogOut() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::logOut(&p);
    return mMainSession->sendQuery(p, &authLogOutMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthSendInvitesAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Auth::sendInvitesResult(&inboundPkt);
    Q_EMIT authSendInvitesResult(q->msgId(), result);
}

qint64 Api::authSendInvites(const QStringList &phoneNumbers, const QString &message) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::sendInvites(&p, phoneNumbers, message);
    return mMainSession->sendQuery(p, &authSendInvitesMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthResetAuthorizationsAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Auth::resetAuthorizationsResult(&inboundPkt);
    Q_EMIT authResetAuthorizationsResult(q->msgId(), result);
}

qint64 Api::authResetAuthorizations() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::resetAuthorizations(&p);
    return mMainSession->sendQuery(p, &authResetAuthorizationsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthImportAuthorizationAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthAuthorization &result = Functions::Auth::importAuthorizationResult(&inboundPkt);
    Q_EMIT authImportedAuthorization(q->msgId(), result.expires(), result.user());
}

qint64 Api::authImportAuthorization(qint32 id, const QByteArray &bytes) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    Functions::Auth::importAuthorization(&p, id, bytes);
    return mMainSession->sendQuery(p, &authImportAuthorizationMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthCheckPasswordAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthAuthorization &result = Functions::Auth::checkPasswordResult(&inboundPkt);
    Q_EMIT authCheckPasswordResult(q->msgId(), result.expires(), result.user());
}

qint64 Api::authCheckPassword(const QByteArray &passwordHash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::checkPassword(&p, passwordHash);
    return mMainSession->sendQuery(p, &authCheckPasswordMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthRequestPasswordRecoveryAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthPasswordRecovery &result = Functions::Auth::requestPasswordRecoveryResult(&inboundPkt);
    Q_EMIT authRequestPasswordRecoveryResult(q->msgId(), result);
}

qint64 Api::authRequestPasswordRecovery() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::requestPasswordRecovery(&p);
    return mMainSession->sendQuery(p, &authRequestPasswordRecoveryMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthRecoverPasswordAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthAuthorization &result = Functions::Auth::recoverPasswordResult(&inboundPkt);
    Q_EMIT authRecoverPasswordResult(q->msgId(), result);
}

qint64 Api::authRecoverPassword(const QString &code) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::recoverPassword(&p, code);
    return mMainSession->sendQuery(p, &authRecoverPasswordMethods, QVariant(), __FUNCTION__ );
}

void Api::onAuthExportAuthorizationAnswer(Query *q, InboundPkt &inboundPkt) {
    const AuthExportedAuthorization &result = Functions::Auth::exportAuthorizationResult(&inboundPkt);
    Q_EMIT authExportedAuthorization(q->msgId(), result.id(), result.bytes());
}

qint64 Api::authExportAuthorization(qint32 dcId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Auth::exportAuthorization(&p, dcId);
    return mMainSession->sendQuery(p, &authExportAuthorizationMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountRegisterDeviceAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::registerDeviceResult(&inboundPkt);
    Q_EMIT accountRegisterDeviceResult(q->msgId(), result);
}

qint64 Api::accountRegisterDevice(qint32 tokenType, const QString &token, const QString &deviceModel, const QString &systemVersion, const QString &appVersion, bool appSandbox, const QString &langCode) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::registerDevice(&p, tokenType, token, deviceModel, systemVersion, appVersion, appSandbox, langCode);
    return mMainSession->sendQuery(p, &accountRegisterDeviceMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUnregisterDeviceAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::unregisterDeviceResult(&inboundPkt);
    Q_EMIT accountUnregisterDeviceResult(q->msgId(), result);
}

qint64 Api::accountUnregisterDevice(qint32 tokenType, const QString &token) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::unregisterDevice(&p, tokenType, token);
    return mMainSession->sendQuery(p, &accountUnregisterDeviceMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdateNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::updateNotifySettingsResult(&inboundPkt);
    Q_EMIT accountUpdateNotifySettingsResult(q->msgId(), result);
}

qint64 Api::accountUpdateNotifySettings(const InputNotifyPeer &peer, const InputPeerNotifySettings &settings) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::updateNotifySettings(&p, peer, settings);
    return mMainSession->sendQuery(p, &accountUpdateNotifySettingsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    const PeerNotifySettings &result = Functions::Account::getNotifySettingsResult(&inboundPkt);
    Q_EMIT accountPeerNotifySettings(q->msgId(), result);
}

qint64 Api::accountGetNotifySettings(const InputNotifyPeer &peer) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getNotifySettings(&p, peer);
    return mMainSession->sendQuery(p, &accountGetNotifySettingsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountResetNotifySettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::resetNotifySettingsResult((&inboundPkt));
    Q_EMIT accountResetNotifySettingsResult(q->msgId(), result);
}

qint64 Api::accountResetNotifySettings() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::resetNotifySettings(&p);
    return mMainSession->sendQuery(p, &accountResetNotifySettingsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdateProfileAnswer(Query *q, InboundPkt &inboundPkt) {
    const User &result = Functions::Account::updateProfileResult(&inboundPkt);
    Q_EMIT accountUser(q->msgId(), result);
}

qint64 Api::accountUpdateProfile(const QString &firstName, const QString &lastName) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::updateProfile(&p, firstName, lastName);
    return mMainSession->sendQuery(p, &accountUpdateProfileMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdateStatusAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::updateStatusResult(&inboundPkt);
    Q_EMIT accountUpdateStatusResult(q->msgId(), result);
}

qint64 Api::accountUpdateStatus(bool offline) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::updateStatus(&p, offline);
    return mMainSession->sendQuery(p, &accountUpdateStatusMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetWallPapersAnswer(Query *q, InboundPkt &inboundPkt) {
    const QList<WallPaper> &result = Functions::Account::getWallPapersResult(&inboundPkt);
    Q_EMIT accountGetWallPapersResult(q->msgId(), result);
}

qint64 Api::accountGetWallPapers() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getWallPapers(&p);
    return mMainSession->sendQuery(p, &accountGetWallPapersMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountCheckUsernameAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::checkUsernameResult(&inboundPkt);
    Q_EMIT accountCheckUsernameResult(q->msgId(), result);
}

qint64 Api::accountCheckUsername(const QString &username) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::checkUsername(&p, username);
    return mMainSession->sendQuery(p, &accountCheckUsernameMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdateUsernameAnswer(Query *q, InboundPkt &inboundPkt) {
    const User &result = Functions::Account::updateUsernameResult(&inboundPkt);
    Q_EMIT accountUpdateUsernameResult(q->msgId(), result);
}

qint64 Api::accountUpdateUsername(const QString &username) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::updateUsername(&p, username);
    return mMainSession->sendQuery(p, &accountUpdateUsernameMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetPrivacyRules(Query *q, InboundPkt &inboundPkt) {
    const AccountPrivacyRules &result = Functions::Account::getPrivacyResult(&inboundPkt);
    Q_EMIT accountGetPrivacyRules(q->msgId(), result.rules(), result.users());
}

qint64 Api::accountGetPrivacy(const InputPrivacyKey &key) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getPrivacy(&p, key);
    return mMainSession->sendQuery(p, &accountAccountGetPrivacyMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountSetPrivacyRules(Query *q, InboundPkt &inboundPkt) {
    const AccountPrivacyRules &result = Functions::Account::setPrivacyResult(&inboundPkt);
    Q_EMIT accountSetPrivacyRules(q->msgId(), result.rules(), result.users());
}

qint64 Api::accountSetPrivacy(const InputPrivacyKey &key, const QList<InputPrivacyRule> &rules) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::setPrivacy(&p, key, rules);
    return mMainSession->sendQuery(p, &accountAccountSetPrivacyMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountDeleteAccountAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::deleteAccountResult(&inboundPkt);
    Q_EMIT accountDeleteAccountResult(q->msgId(), result);
}

qint64 Api::accountDeleteAccount(const QString &reason) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::deleteAccount(&p, reason);
    return mMainSession->sendQuery(p, &accountDeleteAccountMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetAccountTTLAnswer(Query *q, InboundPkt &inboundPkt) {
    const AccountDaysTTL &result = Functions::Account::getAccountTTLResult(&inboundPkt);
    Q_EMIT accountGetAccountTTLResult(q->msgId(), result);
}

qint64 Api::accountGetAccountTTL() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getAccountTTL(&p);
    return mMainSession->sendQuery(p, &accountGetAccountTTLMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountSetAccountTTLAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::setAccountTTLResult(&inboundPkt);
    Q_EMIT accountSetAccountTTLResult(q->msgId(), result);
}

qint64 Api::accountSetAccountTTL(const AccountDaysTTL &ttl) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::setAccountTTL(&p, ttl);
    return mMainSession->sendQuery(p, &accountSetAccountTTLMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountChangePhoneAnswer(Query *q, InboundPkt &inboundPkt) {
    const User &result = Functions::Account::changePhoneResult(&inboundPkt);
    Q_EMIT accountChangePhoneResult(q->msgId(), result);
}

qint64 Api::accountChangePhone(const QString &phone_number, const QString &phone_code_hash, const QString &phone_code) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::changePhone(&p, phone_number, phone_code_hash, phone_code);
    return mMainSession->sendQuery(p, &accountChangePhoneMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdateDeviceLockedAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::updateDeviceLockedResult(&inboundPkt);
    Q_EMIT accountUpdateDeviceLockedResult(q->msgId(), result);
}

qint64 Api::accountUpdateDeviceLocked(int period) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::updateDeviceLocked(&p, period);
    return mMainSession->sendQuery(p, &accountUpdateDeviceLockedMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountSentChangePhoneCode(Query *q, InboundPkt &inboundPkt) {
    const AccountSentChangePhoneCode &result = Functions::Account::sendChangePhoneCodeResult(&inboundPkt);
    Q_EMIT accountSentChangePhoneCode(q->msgId(), result.phoneCodeHash(), result.sendCallTimeout());
}

qint64 Api::accountSendChangePhoneCode(const QString &phone_number) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::sendChangePhoneCode(&p, phone_number);
    return mMainSession->sendQuery(p, &accountSendChangePhoneCodeMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetPasswordAnswer(Query *q, InboundPkt &inboundPkt) {
    const AccountPassword &result = Functions::Account::getPasswordResult(&inboundPkt);
    Q_EMIT accountGetPasswordResult(q->msgId(), result);
}

qint64 Api::accountGetPassword() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getPassword(&p);
    return mMainSession->sendQuery(p, &accountGetPasswordMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetAuthorizationsAnswer(Query *q, InboundPkt &inboundPkt) {
    const AccountAuthorizations &result = Functions::Account::getAuthorizationsResult(&inboundPkt);
    Q_EMIT accountGetAuthorizationsResult(q->msgId(), result);
}

qint64 Api::accountGetAuthorizations() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getAuthorizations(&p);
    return mMainSession->sendQuery(p, &accountGetAuthorizationsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountResetAuthorizationAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::resetAuthorizationResult(&inboundPkt);
    Q_EMIT accountResetAuthorizationResult(q->msgId(), result);
}

qint64 Api::accountResetAuthorization(qint64 hash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::resetAuthorization(&p, hash);
    return mMainSession->sendQuery(p, &accountResetAuthorizationMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountGetPasswordSettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    const AccountPasswordSettings &result = Functions::Account::getPasswordSettingsResult(&inboundPkt);
    Q_EMIT accountGetPasswordSettingsResult(q->msgId(), result);
}

qint64 Api::accountGetPasswordSettings(const QByteArray &currentPasswordHash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::getPasswordSettings(&p, currentPasswordHash);
    return mMainSession->sendQuery(p, &accountGetPasswordSettingsMethods, QVariant(), __FUNCTION__ );
}

void Api::onAccountUpdatePasswordSettingsAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Account::updatePasswordSettingsResult(&inboundPkt);
    Q_EMIT accountUpdatePasswordSettingsResult(q->msgId(), result);
}

qint64 Api::accountUpdatePasswordSettings(const QByteArray &currentPasswordHash, const AccountPasswordInputSettings &newSettings) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Account::updatePasswordSettings(&p, currentPasswordHash, newSettings);
    return mMainSession->sendQuery(p, &accountUpdatePasswordSettingsMethods, QVariant(), __FUNCTION__ );
}

void Api::onPhotosUploadProfilePhotoAnswer(Query *q, InboundPkt &inboundPkt) {
    const PhotosPhoto &result = Functions::Photos::uploadProfilePhotoResult(&inboundPkt);
    Q_EMIT photosPhoto(q->msgId(), result.photo(), result.users());
}

qint64 Api::photosUploadProfilePhoto(const InputFile &file, const QString &caption, const InputGeoPoint &geoPoint, const InputPhotoCrop &crop) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Photos::uploadProfilePhoto(&p, file, caption, geoPoint, crop);
    return mMainSession->sendQuery(p, &photosUploadProfilePhotoMethods, QVariant(), __FUNCTION__ );
}

void Api::onPhotosUpdateProfilePhotoAnswer(Query *q, InboundPkt &inboundPkt) {
    const UserProfilePhoto &result = Functions::Photos::updateProfilePhotoResult(&inboundPkt);
    Q_EMIT photosUserProfilePhoto(q->msgId(), result);
}

qint64 Api::photosUpdateProfilePhoto(const InputPhoto &id, const InputPhotoCrop &crop) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Photos::updateProfilePhoto(&p, id, crop);
    return mMainSession->sendQuery(p, &photosUpdateProfilePhotoMethods, QVariant(), __FUNCTION__ );
}

void Api::onUsersGetUsersAnswer(Query *q, InboundPkt &inboundPkt) {
    const QList<User> &result = Functions::Users::getUsersResult(&inboundPkt);
    Q_EMIT usersGetUsersResult(q->msgId(), result);
}

qint64 Api::usersGetUsers(const QList<InputUser> &users) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Users::getUsers(&p, users);
    return mMainSession->sendQuery(p, &usersGetUsersMethods, QVariant(), __FUNCTION__ );
}

void Api::onUsersGetFullUserAnswer(Query *q, InboundPkt &inboundPkt) {
    const UserFull &result = Functions::Users::getFullUserResult(&inboundPkt);
    Q_EMIT userFull(q->msgId(), result.user(), result.link(), result.profilePhoto(), result.notifySettings(), result.blocked(), result.realFirstName(), result.realLastName());
}

qint64 Api::usersGetFullUser(const InputUser &user) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Users::getFullUser(&p, user);
    return mMainSession->sendQuery(p, &usersGetFullUserMethods, QVariant(), __FUNCTION__ );
}

void Api::onPhotosGetUserPhotos(Query *q, InboundPkt &inboundPkt) {
    const PhotosPhotos &result = Functions::Photos::getUserPhotosResult(&inboundPkt);
    if (result.classType() == PhotosPhotos::typePhotosPhotosSlice) {
        Q_EMIT photosPhotosSlice(q->msgId(), result.count(), result.photos(), result.users());
    } else {
        Q_EMIT photosPhotos(q->msgId(), result.photos(), result.users());
    }
}

qint64 Api::photosGetUserPhotos(const InputUser &user, qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Photos::getUserPhotos(&p, user, offset, maxId, limit);
    return mMainSession->sendQuery(p, &photosGetUserPhotosMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsGetStatusesAnswer(Query *q, InboundPkt &inboundPkt) {
    const QList<ContactStatus> &result = Functions::Contacts::getStatusesResult(&inboundPkt);
    Q_EMIT contactsGetStatusesResult(q->msgId(), result);
}

qint64 Api::contactsGetStatuses() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::getStatuses(&p);
    return mMainSession->sendQuery(p, &contactsGetStatusesMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsGetContactsAnswer(Query *q, InboundPkt &inboundPkt) {
    const ContactsContacts &result = Functions::Contacts::getContactsResult(&inboundPkt);
    if (result.classType() == ContactsContacts::typeContactsContacts) {
        Q_EMIT contactsContacts(q->msgId(), result.contacts(), result.users());
    } else {
        Q_EMIT contactsContactsNotModified(q->msgId());
    }
}

qint64 Api::contactsGetContacts(const QString &hash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::getContacts(&p, hash);
    return mMainSession->sendQuery(p, &contactsGetContactsMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsImportContactsAnswer(Query *q, InboundPkt &inboundPkt) {
    const ContactsImportedContacts &result = Functions::Contacts::importContactsResult(&inboundPkt);
    Q_EMIT contactsImportedContacts(q->msgId(), result.imported(), result.retryContacts(), result.users());
}

qint64 Api::contactsImportContacts(const QList<InputContact> &contacts, bool replace) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::importContacts(&p, contacts, replace);
    return mMainSession->sendQuery(p, &contactsImportContactsMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsDeleteContactAnswer(Query *q, InboundPkt &inboundPkt) {
    const ContactsLink &result = Functions::Contacts::deleteContactResult(&inboundPkt);
    Q_EMIT contactsDeleteContactLink(q->msgId(), result.myLink(), result.foreignLink(), result.user());
}

qint64 Api::contactsDeleteContact(const InputUser &id) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::deleteContact(&p, id);
    return mMainSession->sendQuery(p, &contactsDeleteContactMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsDeleteContactsAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Contacts::deleteContactsResult(&inboundPkt);
    Q_EMIT contactsDeleteContactsResult(q->msgId(), result);
}

qint64 Api::contactsDeleteContacts(const QList<InputUser> &ids) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::deleteContacts(&p, ids);
    return mMainSession->sendQuery(p, &contactsDeleteContactsMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsSearchAnswer(Query *q, InboundPkt &inboundPkt) {
    const ContactsFound &result = Functions::Contacts::searchResult(&inboundPkt);
    Q_EMIT contactsFound(q->msgId(), result.results(), result.users());
}

qint64 Api::contactsSearch(const QString &q, qint32 limit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::search(&p, q, limit);
    return mMainSession->sendQuery(p, &contactsSearchMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsResolveUsernameAnswer(Query *q, InboundPkt &inboundPkt) {
    const User &result = Functions::Contacts::resolveUsernameResult(&inboundPkt);
    Q_EMIT contactsResolveUsernameResult(q->msgId(), result);
}

qint64 Api::contactsResolveUsername(const QString &username) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::resolveUsername(&p, username);
    return mMainSession->sendQuery(p, &contactsResolveUsernameMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsBlockAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Contacts::blockResult(&inboundPkt);
    Q_EMIT contactsBlockResult(q->msgId(), result);
}

qint64 Api::contactsBlock(const InputUser &id) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::block(&p, id);
    return mMainSession->sendQuery(p, &contactsBlockMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsUnblockAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Contacts::unblockResult(&inboundPkt);
    Q_EMIT contactsUnblockResult(q->msgId(), result);
}

qint64 Api::contactsUnblock(const InputUser &id) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::unblock(&p, id);
    return mMainSession->sendQuery(p, &contactsUnblockMethods, QVariant(), __FUNCTION__ );
}

void Api::onContactsGetBlockedAnswer(Query *q, InboundPkt &inboundPkt) {
    const ContactsBlocked &result = Functions::Contacts::getBlockedResult(&inboundPkt);
    if (result.classType() == ContactsBlocked::typeContactsBlockedSlice) {
        Q_EMIT contactsBlockedSlice(q->msgId(), result.count(), result.blocked(), result.users());
    } else {
        Q_EMIT contactsBlocked(q->msgId(), result.blocked(), result.users());
    }
}

qint64 Api::contactsGetBlocked(qint32 offset, qint32 limit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Contacts::getBlocked(&p, offset, limit);
    return mMainSession->sendQuery(p, &contactsGetBlockedMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSendMessageAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesSentMessage &result = Functions::Messages::sendMessageResult(&inboundPkt);
    if (result.classType() == MessagesSentMessage::typeMessagesSentMessageLink) {
        Q_EMIT messagesSentMessageLink(q->msgId(), result.id(), result.date(), result.media(), result.pts(), result.ptsCount(), result.seq(), result.links());
    } else {
        Q_EMIT messagesSentMessage(q->msgId(), result.id(), result.date(), result.media(), result.pts(), result.ptsCount());
    }
}

qint64 Api::messagesSendMessage(const InputPeer &peer, const QString &message, qint64 randomId, qint32 replyToMsgId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    int flag = 0;
    if(replyToMsgId)
        flag = flag | (1<<0);

    Functions::Messages::sendMessage(&p, flag, peer, replyToMsgId, message, randomId);
    return mMainSession->sendQuery(p, &messagesSendMessageMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSendMediaAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::sendMediaResult(&inboundPkt);
    Q_EMIT messagesSentMedia(q->msgId(), result);
}

qint64 Api::messagesSendMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    int flag = 0;
    if(replyToMsgId)
        flag = flag | (1<<0);

    Functions::Messages::sendMedia(&p, flag, peer, replyToMsgId, media, randomId);
    return mMainSession->sendQuery(p, &messagesSendMediaMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesForwardMediaAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::sendMediaResult(&inboundPkt);
    Q_EMIT messagesForwardedMedia(q->msgId(), result);
}

qint64 Api::messagesForwardMedia(const InputPeer &peer, const InputMedia &media, qint64 randomId, qint32 replyToMsgId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    int flag = 0;
    if(replyToMsgId)
        flag = flag | (1<<0);

    Functions::Messages::sendMedia(&p, flag, peer, replyToMsgId, media, randomId);
    return mMainSession->sendQuery(p, &messagesForwardMediaMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSetTypingAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Messages::setTypingResult(&inboundPkt);
    Q_EMIT messagesSetTypingResult(q->msgId(), result);
}

qint64 Api::messagesSetTyping(const InputPeer &peer, const SendMessageAction &action) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::setTyping(&p, peer, action);
    return mMainSession->sendQuery(p, &messagesSetTypingMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesMessages &result = Functions::Messages::getMessagesResult(&inboundPkt);
    if (result.classType() == MessagesMessages::typeMessagesMessagesSlice) {
        Q_EMIT messagesGetMessagesMessagesSlice(q->msgId(), result.count(), result.messages(), result.chats(), result.users());
    } else {
        Q_EMIT messagesGetMessagesMessages(q->msgId(), result.messages(), result.chats(), result.users());
    }
}

qint64 Api::messagesGetMessages(const QList<qint32> &ids) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getMessages(&p, ids);
    return mMainSession->sendQuery(p, &messagesGetMessagesMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetDialogsAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesDialogs &result = Functions::Messages::getDialogsResult(&inboundPkt);
    if (result.classType() == MessagesDialogs::typeMessagesDialogsSlice) {
        Q_EMIT messagesDialogsSlice(q->msgId(), result.count(), result.dialogs(), result.messages(), result.chats(), result.users());
    } else {
        Q_EMIT messagesDialogs(q->msgId(), result.dialogs(), result.messages(), result.chats(), result.users());
    }
}

qint64 Api::messagesGetDialogs(qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getDialogs(&p, offset, maxId, limit);
    return mMainSession->sendQuery(p, &messagesGetDialogsMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesMessages &result = Functions::Messages::getHistoryResult(&inboundPkt);
    if (result.classType() == MessagesMessages::typeMessagesMessagesSlice) {
        Q_EMIT messagesGetHistoryMessagesSlice(q->msgId(), result.count(), result.messages(), result.chats(), result.users());
    } else {
        Q_EMIT messagesGetHistoryMessages(q->msgId(), result.messages(), result.chats(), result.users());
    }
}

qint64 Api::messagesGetHistory(const InputPeer &peer, qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getHistory(&p, peer, offset, maxId, limit);
    return mMainSession->sendQuery(p, &messagesGetHistoryMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSearchAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesMessages &result = Functions::Messages::searchResult(&inboundPkt);
    if (result.classType() == MessagesMessages::typeMessagesMessagesSlice) {
        Q_EMIT messagesSearchMessagesSlice(q->msgId(), result.count(), result.messages(), result.chats(), result.users());
    } else {
        Q_EMIT messagesSearchMessages(q->msgId(), result.messages(), result.chats(), result.users());
    }
}

qint64 Api::messagesSearch(const InputPeer &peer, const QString &q, const MessagesFilter &filter, qint32 minDate, qint32 maxDate, qint32 offset, qint32 maxId, qint32 limit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::search(&p, peer, q, filter, minDate, maxDate, offset, maxId, limit);
    return mMainSession->sendQuery(p, &messagesSearchMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesReadHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesAffectedHistory &result = Functions::Messages::readHistoryResult(&inboundPkt);
    Q_EMIT messagesReadAffectedHistory(q->msgId(), result.pts(), result.ptsCount(), result.offset());
}

qint64 Api::messagesReadHistory(const InputPeer &peer, qint32 maxId, qint32 offset) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    if (mMainSession->initConnectionNeeded()) {
        p.initConnection();
        mMainSession->setInitConnectionNeeded(false);
    }
    Functions::Messages::readHistory(&p, peer, maxId, offset);
    return mMainSession->sendQuery(p, &messagesReadHistoryMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesReadMessageContentsAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesAffectedMessages &result = Functions::Messages::readMessageContentsResult(&inboundPkt);
    Q_EMIT messagesReadMessageContentsResult(q->msgId(), result);
}

qint64 Api::messagesReadMessageContents(const QList<qint32> &ids) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::readMessageContents(&p, ids);
    return mMainSession->sendQuery(p, &messagesReadMessageContentsMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesDeleteHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesAffectedHistory &result = Functions::Messages::deleteHistoryResult(&inboundPkt);
    Q_EMIT messagesDeleteAffectedHistory(q->msgId(), result.pts(), result.ptsCount(), result.offset());
}

qint64 Api::messagesDeleteHistory(const InputPeer &peer, qint32 offset) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::deleteHistory(&p, peer, offset);
    return mMainSession->sendQuery(p, &messagesDeleteHistoryMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesDeleteMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesAffectedMessages &result = Functions::Messages::deleteMessagesResult(&inboundPkt);
    Q_EMIT messagesDeleteMessagesResult(q->msgId(), result);
}

qint64 Api::messagesDeleteMessages(const QList<qint32> &ids) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::deleteMessages(&p, ids);
    return mMainSession->sendQuery(p, &messagesDeleteMessagesMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesReceivedMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    const QList<ReceivedNotifyMessage> &result = Functions::Messages::receivedMessagesResult(&inboundPkt);
    Q_EMIT messagesReceivedMessagesResult(q->msgId(), result);
}

qint64 Api::messagesReceivedMessages(qint32 maxId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::receivedMessages(&p, maxId);
    return mMainSession->sendQuery(p, &messagesReceivedMessagesMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesForwardMessageAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::forwardMessageResult(&inboundPkt);
    Q_EMIT messagesForwardedMessage(q->msgId(), result);
}

qint64 Api::messagesForwardMessage(const InputPeer &peer, qint32 id, qint64 randomId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::forwardMessage(&p, peer, id, randomId);
    return mMainSession->sendQuery(p, &messagesForwardMessageMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesForwardMessagesAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::forwardMessagesResult(&inboundPkt);
    Q_EMIT messagesForwardedMessages(q->msgId(), result);
}

qint64 Api::messagesForwardMessages(const InputPeer &peer, const QList<qint32> &ids, const QList<qint64> &randomIds) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::forwardMessages(&p, peer, ids, randomIds);
    return mMainSession->sendQuery(p, &messagesForwardMessagesMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSendBroadcastAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::sendBroadcastResult(&inboundPkt);
    Q_EMIT messagesSentBroadcast(q->msgId(), result);
}

qint64 Api::messagesSendBroadcast(const QList<InputUser> &contacts, const QList<qint64> &randomIds, const QString &message, const InputMedia &media) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::sendBroadcast(&p, contacts, randomIds, message, media);
    return mMainSession->sendQuery(p, &messagesSendBroadcastMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetWebPagePreviewAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessageMedia &result = Functions::Messages::getWebPagePreviewResult((&inboundPkt));
    Q_EMIT messagesGetWebPagePreviewResult(q->msgId(), result);
}

qint64 Api::messagesGetWebPagePreview(const QString &message) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getWebPagePreview(&p, message);
    return mMainSession->sendQuery(p, &messagesGetWebPagePreviewMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetChatsAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesChats &result = Functions::Messages::getChatsResult(&inboundPkt);
    Q_EMIT messagesChats(q->msgId(), result.chats());
}

qint64 Api::messagesGetChats(const QList<qint32> &chatIds) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getChats(&p, chatIds);
    return mMainSession->sendQuery(p, &messagesGetChatsMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetFullChatAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesChatFull &result = Functions::Messages::getFullChatResult(&inboundPkt);
    Q_EMIT messagesChatFull(q->msgId(), result.fullChat(), result.chats(), result.users());
}

qint64 Api::messagesGetFullChat(qint32 chatId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getFullChat(&p, chatId);
    return mMainSession->sendQuery(p, &messagesGetFullChatMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesEditChatTitleAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::editChatTitleResult(&inboundPkt);
    Q_EMIT messagesEditedChatTitle(q->msgId(), result);
}

qint64 Api::messagesEditChatTitle(qint32 chatId, const QString &title) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::editChatTitle(&p, chatId, title);
    return mMainSession->sendQuery(p, &messagesEditChatTitleMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesEditChatPhotoAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::editChatPhotoResult(&inboundPkt);
    Q_EMIT messagesEditedChatPhoto(q->msgId(), result);
}

qint64 Api::messagesEditChatPhoto(qint32 chatId, const InputChatPhoto &photo) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::editChatPhoto(&p, chatId, photo);
    return mMainSession->sendQuery(p, &messagesEditChatPhotoMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesAddChatUserAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::addChatUserResult(&inboundPkt);
    Q_EMIT messagesAddedChatUser(q->msgId(), result);
}

qint64 Api::messagesAddChatUser(qint32 chatId, const InputUser &user, qint32 fwdLimit) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::addChatUser(&p, chatId, user, fwdLimit);
    return mMainSession->sendQuery(p, &messagesAddChatUserMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesDeleteChatUserAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::deleteChatUserResult(&inboundPkt);
    Q_EMIT messagesDeletedChat(q->msgId(), result);
}

qint64 Api::messagesDeleteChatUser(qint32 chatId, const InputUser &user) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::deleteChatUser(&p, chatId, user);
    return mMainSession->sendQuery(p, &messagesDeleteChatUserMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesCreateChatAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::createChatResult(&inboundPkt);
    Q_EMIT messagesCreatedChat(q->msgId(), result);
}

qint64 Api::messagesCreateChat(const QList<InputUser> &users, const QString &title) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::createChat(&p, users, title);
    return mMainSession->sendQuery(p, &messagesCreateChatMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetDhConfigAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesDhConfig &result = Functions::Messages::getDhConfigResult(&inboundPkt);
    if (result.classType() == MessagesDhConfig::typeMessagesDhConfig)
        Q_EMIT messagesDhConfig(q->msgId(), result.g(), result.p(), result.version(), result.random());
    else
        Q_EMIT messagesDhConfigNotModified(q->msgId(), result.random());
}

qint64 Api::messagesGetDhConfig(qint32 version, qint32 randomLength) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getDhConfig(&p, version, randomLength);
    return mMainSession->sendQuery(p, &messagesGetDhConfigMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesRequestEncryptionAnswer(Query *q, InboundPkt &inboundPkt) {
    const EncryptedChat &result = Functions::Messages::requestEncryptionResult(&inboundPkt);
    Q_EMIT messagesRequestEncryptionEncryptedChat(q->msgId(), result);
}

qint64 Api::messagesRequestEncryption(const InputUser &user, qint32 randomId, QByteArray g_a) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::requestEncryption(&p, user, randomId, g_a);
    return mMainSession->sendQuery(p, &messagesRequestEncryptionMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesAcceptEncryptionAnswer(Query *q, InboundPkt &inboundPkt) {
    const EncryptedChat &result = Functions::Messages::acceptEncryptionResult(&inboundPkt);
    Q_EMIT messagesAcceptEncryptionEncryptedChat(q->msgId(), result);
}

qint64 Api::messagesAcceptEncryption(const InputEncryptedChat &inputEncryptedChat, QByteArray g_b, qint64 keyFingerprint) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::acceptEncryption(&p, inputEncryptedChat, g_b, keyFingerprint);
    return mMainSession->sendQuery(p, &messagesAcceptEncryptionMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesDiscardEncryptionAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Messages::discardEncryptionResult(&inboundPkt);
    Q_EMIT messagesDiscardEncryptionResult(q->msgId(), result);
}

qint64 Api::messagesDiscardEncryption(qint32 chatId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::discardEncryption(&p, chatId);
    return mMainSession->sendQuery(p, &messagesDiscardEncryptionMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSetEncryptedTypingAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Messages::setEncryptedTypingResult(&inboundPkt);
    Q_EMIT messagesSetEncryptedTypingResult(q->msgId(), result);
}

qint64 Api::messagesSetEncryptedTyping(const InputEncryptedChat &inputEncryptedChat, bool typing) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::setEncryptedTyping(&p, inputEncryptedChat, typing);
    return mMainSession->sendQuery(p, &messagesSetEncryptedTypingMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesReadEncryptedHistoryAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Messages::readEncryptedHistoryResult(&inboundPkt);
    Q_EMIT messagesReadEncryptedHistoryResult(q->msgId(), result);
}

qint64 Api::messagesReadEncryptedHistory(const InputEncryptedChat &inputEncryptedChat, qint32 maxDate){
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::readEncryptedHistory(&p, inputEncryptedChat, maxDate);
    return mMainSession->sendQuery(p, &messagesReadEncryptedHistoryMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSendEncryptedAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesSentEncryptedMessage &result = Functions::Messages::sendEncryptedResult(&inboundPkt);
    if (result.classType() == MessagesSentEncryptedMessage::typeMessagesSentEncryptedFile) {
        Q_EMIT messagesSendEncryptedSentEncryptedFile(q->msgId(), result.date(), result.file());
    } else {
        Q_EMIT messagesSendEncryptedSentEncryptedMessage(q->msgId(), result.date());
    }
}

qint64 Api::messagesSendEncrypted(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    // include invokeAfterMsgs with the sequence of previous messages to manage before this one
    qint32 n = previousMsgs.size();
    if (n > 0) {
        p.appendInt(CoreTypes::typeInvokeAfterMsgs);
        p.appendInt(CoreTypes::typeVector);
        p.appendInt(n);
        Q_FOREACH (qint64 msgId, previousMsgs) {
            p.appendLong(msgId);
        }
    }
    // send the message
    Functions::Messages::sendEncrypted(&p, inputEncryptedChat, randomId, data);
    return mMainSession->sendQuery(p, &messagesSendEncryptedMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSendEncryptedFileAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesSentEncryptedMessage &result = Functions::Messages::sendEncryptedFileResult(&inboundPkt);
    if (result.classType() == MessagesSentEncryptedMessage::typeMessagesSentEncryptedFile) {
        Q_EMIT messagesSendEncryptedFileSentEncryptedFile(q->msgId(), result.date(), result.file());
    } else {
        Q_EMIT messagesSendEncryptedFileSentEncryptedMessage(q->msgId(), result.date());
    }
}

qint64 Api::messagesSendEncryptedFile(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data,
                                      const InputEncryptedFile &inputEncryptedFile) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    // include invokeAfterMsgs with the sequence of previous messages to manage before this one
    qint32 n = previousMsgs.size();
    if (n > 0) {
        p.appendInt(CoreTypes::typeInvokeAfterMsgs);
        p.appendInt(CoreTypes::typeVector);
        p.appendInt(n);
        Q_FOREACH (qint64 msgId, previousMsgs) {
            p.appendLong(msgId);
        }
    }
    // send the file message
    Functions::Messages::sendEncryptedFile(&p, inputEncryptedChat, randomId, data, inputEncryptedFile);
    return mMainSession->sendQuery(p, &messagesSendEncryptedFileMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesSendEncryptedServiceAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesSentEncryptedMessage &result = Functions::Messages::sendEncryptedServiceResult(&inboundPkt);
    if (result.classType() == MessagesSentEncryptedMessage::typeMessagesSentEncryptedFile) {
        Q_EMIT messagesSendEncryptedServiceSentEncryptedFile(q->msgId(), result.date(), result.file());
    } else {
        Q_EMIT messagesSendEncryptedServiceSentEncryptedMessage(q->msgId(), result.date());
    }
}

qint64 Api::messagesSendEncryptedService(const QList<qint64> &previousMsgs, const InputEncryptedChat &inputEncryptedChat, qint64 randomId, QByteArray data) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    // include invokeAfterMsgs with the sequence of previous messages to manage before this one
    qint32 n = previousMsgs.size();
    if (n > 0) {
        p.appendInt(CoreTypes::typeInvokeAfterMsgs);
        p.appendInt(CoreTypes::typeVector);
        p.appendInt(n);
        Q_FOREACH (qint64 msgId, previousMsgs) {
            p.appendLong(msgId);
        }
    }
    // send the encrypted service message
    Functions::Messages::sendEncryptedService(&p, inputEncryptedChat, randomId, data);
    return mMainSession->sendQuery(p, &messagesSendEncryptedServiceMethods, QVariant(), __FUNCTION__ );
}

// ### messages.receivedQueue
void Api::onMessagesReceivedQueueAnswer(Query *q, InboundPkt &inboundPkt) {
    const QList<qint64> &result = Functions::Messages::receivedQueueResult(&inboundPkt);
    Q_EMIT messagesReceivedQueueResult(q->msgId(), result);
}

qint64 Api::messagesReceivedQueue(qint32 maxQts) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::receivedQueue(&p, maxQts);
    return mMainSession->sendQuery(p, &messagesReceivedQueueMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetStickersAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesStickers &result = Functions::Messages::getStickersResult(&inboundPkt);
    Q_EMIT messagesGetStickersResult(q->msgId(), result);
}

qint64 Api::messagesGetStickers(const QString &emoticon, const QString &hash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getStickers(&p, emoticon, hash);
    return mMainSession->sendQuery(p, &messagesGetStickersMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetAllStickersAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesAllStickers &result = Functions::Messages::getAllStickersResult(&inboundPkt);
    Q_EMIT messagesGetAllStickersResult(q->msgId(), result);
}

qint64 Api::messagesGetAllStickers(const QString &hash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getAllStickers(&p, hash);
    return mMainSession->sendQuery(p, &messagesGetAllStickersMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesGetStickerSetAnswer(Query *q, InboundPkt &inboundPkt) {
    const MessagesStickerSet &result = Functions::Messages::getStickerSetResult(&inboundPkt);
    Q_EMIT messagesGetStickerSetResult(q->msgId(), result);
}

qint64 Api::messagesGetStickerSet(const InputStickerSet &stickerset) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::getStickerSet(&p, stickerset);
    return mMainSession->sendQuery(p, &messagesGetStickerSetMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesInstallStickerSetAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Messages::installStickerSetResult(&inboundPkt);
    Q_EMIT messagesInstallStickerSetResult(q->msgId(), result);
}

qint64 Api::messagesInstallStickerSet(const InputStickerSet &stickerset) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::installStickerSet(&p, stickerset);
    return mMainSession->sendQuery(p, &messagesInstallStickerSetMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesUninstallStickerSetAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Messages::uninstallStickerSetResult(&inboundPkt);
    Q_EMIT messagesUninstallStickerSetResult(q->msgId(), result);
}

qint64 Api::messagesUninstallStickerSet(const InputStickerSet &stickerset) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::uninstallStickerSet(&p, stickerset);
    return mMainSession->sendQuery(p, &messagesUninstallStickerSetMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesExportChatInviteAnswer(Query *q, InboundPkt &inboundPkt) {
    const ExportedChatInvite &result = Functions::Messages::exportChatInviteResult(&inboundPkt);
    Q_EMIT messagesExportChatInviteResult(q->msgId(), result);
}

qint64 Api::messagesExportChatInvite(qint32 chatId) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::exportChatInvite(&p, chatId);
    return mMainSession->sendQuery(p, &messagesExportChatInviteMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesCheckChatInviteAnswer(Query *q, InboundPkt &inboundPkt) {
    const ChatInvite &result = Functions::Messages::checkChatInviteResult(&inboundPkt);
    Q_EMIT messagesCheckChatInviteResult(q->msgId(), result);
}

qint64 Api::messagesCheckChatInvite(const QString &hash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::checkChatInvite(&p, hash);
    return mMainSession->sendQuery(p, &messagesCheckChatInviteMethods, QVariant(), __FUNCTION__ );
}

void Api::onMessagesImportChatInviteAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesType &result = Functions::Messages::importChatInviteResult(&inboundPkt);
    Q_EMIT messagesImportChatInviteResult(q->msgId(), result);
}

qint64 Api::messagesImportChatInvite(const QString &hash) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Messages::importChatInvite(&p, hash);
    return mMainSession->sendQuery(p, &messagesImportChatInviteMethods, QVariant(), __FUNCTION__ );
}

void Api::onUpdatesGetStateAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesState &result = Functions::Updates::getStateResult(&inboundPkt);
    Q_EMIT updatesState(q->msgId(), result.pts(), result.qts(), result.date(), result.seq(), result.unreadCount());
}

qint64 Api::updatesGetState() {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Updates::getState(&p);
    return mMainSession->sendQuery(p, &updatesGetStateMethods, QVariant(), __FUNCTION__ );
}

void Api::onUpdatesGetDifferenceAnswer(Query *q, InboundPkt &inboundPkt) {
    const UpdatesDifference &result = Functions::Updates::getDifferenceResult(&inboundPkt);
    switch(static_cast<int>(result.classType()))
    {
    case UpdatesDifference::typeUpdatesDifferenceEmpty:
        Q_EMIT updatesDifferenceEmpty(q->msgId(), result.date(), result.seq());
        break;

    case UpdatesDifference::typeUpdatesDifferenceSlice:
        Q_EMIT updatesDifferenceSlice(q->msgId(), result.newMessages(), result.newEncryptedMessages(), result.otherUpdates(), result.chats(), result.users(), result.state());
        break;

    case UpdatesDifference::typeUpdatesDifference:
        Q_EMIT updatesDifference(q->msgId(), result.newMessages(), result.newEncryptedMessages(), result.otherUpdates(), result.chats(), result.users(), result.state());
        break;
    }
}

qint64 Api::updatesGetDifference(qint32 pts, qint32 date, qint32 qts) {
    CHECK_SESSION
    DEBUG_FUNCTION
    OutboundPkt p(mSettings);
    Functions::Updates::getDifference(&p, pts, date, qts);
    return mMainSession->sendQuery(p, &updatesGetDifferenceMethods, QVariant(), __FUNCTION__ );
}

void Api::onUploadSaveFilePartAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Upload::saveFilePartResult(&inboundPkt);
    Q_EMIT uploadSaveFilePartResult(q->msgId(), q->extra().toLongLong(), result);
}


qint64 Api::uploadSaveFilePart(Session *session, qint64 fileId, qint32 filePart, const QByteArray &bytes) {
    Q_ASSERT(session);
    OutboundPkt p(mSettings);
    if (session->initConnectionNeeded()) {
        p.initConnection();
        session->setInitConnectionNeeded(false);
    }
    Functions::Upload::saveFilePart(&p, fileId, filePart, bytes);
    QVariant extra = fileId;
    return session->sendQuery(p, &uploadSaveFilePartMethods, extra);
}

void Api::onUploadSaveBigFilePartAnswer(Query *q, InboundPkt &inboundPkt) {
    const bool result = Functions::Upload::saveBigFilePartResult(&inboundPkt);
    Q_EMIT uploadSaveBigFilePartResult(q->msgId(), q->extra().toLongLong(), result);
}

qint64 Api::uploadSaveBigFilePart(Session *session, qint64 fileId, qint32 filePart, qint32 fileTotalParts, const QByteArray &bytes) {
    Q_ASSERT(session);
    OutboundPkt p(mSettings);
    if (session->initConnectionNeeded()) {
        p.initConnection();
        session->setInitConnectionNeeded(false);
    }
    Functions::Upload::saveBigFilePart(&p, fileId, filePart, fileTotalParts, bytes);
    QVariant extra = fileId;
    return session->sendQuery(p, &uploadSaveBigFilePartMethods, extra);
}

void Api::onUploadGetFileError(Query *q, qint32 errorCode, const QString &errorText) {
    Q_EMIT uploadFileError(q->msgId(), errorCode, errorText);
}

void Api::onUploadGetFileAnswer(Query *q, InboundPkt &inboundPkt) {
    const UploadFile &result = Functions::Upload::getFileResult(&inboundPkt);
    Q_EMIT uploadFile(q->msgId(), result.type(), result.mtime(), result.bytes());
}

qint64 Api::uploadGetFile(Session *session, const InputFileLocation &location, qint32 offset, qint32 limit) {
    Q_ASSERT(session);
    OutboundPkt p(mSettings);
    if (session->initConnectionNeeded()) {
        p.initConnection();
        session->setInitConnectionNeeded(false);
    }
    Functions::Upload::getFile(&p, location, offset, limit);
    return session->sendQuery(p, &uploadGetFileMethods, QVariant(), __FUNCTION__ );
}
