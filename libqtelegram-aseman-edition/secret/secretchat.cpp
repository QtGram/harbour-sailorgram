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
 */

#include "secretchat.h"
#include "util/utils.h"
#include "core/settings.h"
#include "telegram/coretypes.h"

Q_LOGGING_CATEGORY(TG_SECRET_SECRETCHAT, "tg.secret.secretchat")

SecretChat::SecretChat(Settings *settings) :
    mSettings(settings),
    mState(Init),
    mChatId(0),
    mRequestedUser(InputUser::typeInputUserEmpty),
    mAccessHash(0),
    mAdminId(0),
    mParticipantId(0),
    mDate(0),
    mMyKey(0),
    mKeyFingerprint(0),
    mLayer(CoreTypes::typeLayerVersion),
    mInSeqNo(0),
    mOutSeqNo(0) {
}

SecretChat::~SecretChat() {
    if (mMyKey) BN_clear_free(mMyKey);
}

void SecretChat::createMyKey(const QByteArray &serverRandomBA) {
    // a more entrophy random number is generated here by using also a
    // client random number and operating so:
    // m_random := server_random XOR client_random
    uchar serverRandom[DH_CONFIG_SERVER_RANDOM_LENGTH];
    uchar clientRandom[DH_CONFIG_SERVER_RANDOM_LENGTH];
    memcpy(serverRandom, serverRandomBA.data(), DH_CONFIG_SERVER_RANDOM_LENGTH);
    Utils::randomBytes(clientRandom, DH_CONFIG_SERVER_RANDOM_LENGTH);
    for (int i=0; i < DH_CONFIG_SERVER_RANDOM_LENGTH; i++) {
        serverRandom[i] ^= clientRandom[i];
    }
    mMyKey = BN_bin2bn(serverRandom, DH_CONFIG_SERVER_RANDOM_LENGTH, 0);
    Utils::ensurePtr(mMyKey);
}

SecretChat::State SecretChat::state() const {
    return mState;
}

qint32 SecretChat::chatId() const {
    return mChatId;
}

InputUser SecretChat::requestedUser() const {
    return mRequestedUser;
}

qint64 SecretChat::accessHash() const {
    return mAccessHash;
}

qint32 SecretChat::adminId() const {
    return mAdminId;
}

qint32 SecretChat::participantId() const {
    return mParticipantId;
}

qint32 SecretChat::date() const {
    return mDate;
}

QByteArray SecretChat::gAOrB() const {
    return mGAOrB;
}

BIGNUM *SecretChat::myKey() {
    return mMyKey;
}

void SecretChat::setMyKey(BIGNUM *myKey) {
    BN_clear_free(mMyKey);
    mMyKey = BN_dup(myKey);
}

qint64 SecretChat::keyFingerprint() {
    if (!mKeyFingerprint) {
        mKeyFingerprint = Utils::getKeyFingerprint(mSharedKey);
    }
    return mKeyFingerprint;
}

uchar *SecretChat::sharedKey() {
    return mSharedKey;
}

qint32 SecretChat::outSeqNo() const {
    return mOutSeqNo;
}

qint32 SecretChat::inSeqNo() const {
    return mInSeqNo;
}

qint32 SecretChat::layer() const {
    return mLayer;
}

void SecretChat::setState(SecretChat::State state) {
    mState = state;
}

void SecretChat::setChatId(qint32 chatId) {
    mChatId = chatId;
}

void SecretChat::setRequestedUser(const InputUser &requestedUser) {
    mRequestedUser = requestedUser;
}

void SecretChat::setAccessHash(qint64 accessHash) {
    mAccessHash = accessHash;
}

void SecretChat::setAdminId(qint32 adminId) {
    mAdminId = adminId;
}

void SecretChat::setParticipantId(qint32 participantId) {
    mParticipantId = participantId;
}

void SecretChat::setDate(qint32 date) {
    mDate = date;
}

void SecretChat::setGAOrB(QByteArray gAOrB) {
    mGAOrB = gAOrB;
}

void SecretChat::setKeyFingerprint(qint64 keyFingerprint) {
    mKeyFingerprint = keyFingerprint;
}

void SecretChat::setInSeqNo(qint32 inSeqNo) {
    mInSeqNo = inSeqNo;
}

void SecretChat::setOutSeqNo(qint32 outSeqNo) {
    mOutSeqNo = outSeqNo;
}

void SecretChat::setLayer(qint32 layer) {
    mLayer = layer;
}

qint32 SecretChat::getInSeqNoParam() {
    qint32 ourId = mSettings->ourId();
    return 2 * mInSeqNo + ( mAdminId != ourId ? 1 : 0 );
}

qint32 SecretChat::getOutSeqNoParam() {
    qint32 ourId = mSettings->ourId();
    return 2 * mOutSeqNo + ( mAdminId == ourId ? 1 : 0 );
}

void SecretChat::increaseOutSeqNo() {
    mOutSeqNo++;
    qCDebug(TG_SECRET_SECRETCHAT) << "updated -> out_seq_no:" << mOutSeqNo << ",in_seq_no:" << mInSeqNo;
}

void SecretChat::appendToSequence(qint64 msgId) {
    mSequence.append(msgId);
    if (mSequence.size() > SEQUENCE_MAX_SIZE) {
        mSequence.removeFirst();
    }
    qCDebug(TG_SECRET_SECRETCHAT) << "Current previous messages sequence:" << endl << mSequence;
}

QList<qint64> SecretChat::sequence() {
    return mSequence;
}

void SecretChat::setSequence(const QList<qint64> &sequence) {
    mSequence = sequence;
}
