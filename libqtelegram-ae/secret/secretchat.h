/*
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
#ifndef SECRETCHAT_H
#define SECRETCHAT_H

// max number of previous messages stored in sequence map
#define SEQUENCE_MAX_SIZE 10

#include <QObject>
#include <QMap>
#include <QPair>
#include <QLoggingCategory>
#include "types/inputuser.h"
#include "openssl/bn.h"
#include "types/encryptedchat.h"
#include "types/decryptedmessage.h"
#include "libqtelegram_global.h"
#include "util/constants.h"

Q_DECLARE_LOGGING_CATEGORY(TG_SECRET_SECRETCHAT)

class Settings;
class LIBQTELEGRAMSHARED_EXPORT SecretChat : public QObject
{
    Q_OBJECT
public:

    enum State {
        Init = 0,
        Requested,
        Accepted,
        Cancelled
    };

    typedef QMap<QPair<qint32,qint32>, qint64> Sequence;

    explicit SecretChat(Settings *settings, QObject *parent = 0);
    ~SecretChat();

    void createMyKey(const QByteArray &serverRandom);

    SecretChat::State state() const;
    qint32 chatId() const;
    InputUser requestedUser() const;
    qint64 accessHash() const;
    qint32 adminId() const;
    qint32 participantId() const;
    qint32 date() const;
    QByteArray gAOrB() const;
    BIGNUM *myKey();
    qint64 keyFingerprint();
    uchar *sharedKey();
    qint32 layer() const;
    qint32 inSeqNo() const;
    qint32 outSeqNo() const;

    void setState(SecretChat::State state);
    void setChatId(qint32 chatId);
    void setRequestedUser(const InputUser &requestedUser);
    void setAccessHash(qint64 accessHash);
    void setAdminId(qint32 adminId);
    void setParticipantId(qint32 participantId);
    void setDate(qint32 date);
    void setGAOrB(QByteArray gAOrB);
    void setMyKey(BIGNUM *myKey);
    void setKeyFingerprint(qint64 keyFingerprint);
    void setLayer(qint32 layer);
    void setInSeqNo(qint32 inSeqNo);
    void setOutSeqNo(qint32 outSeqNo);

    qint32 getInSeqNoParam();
    qint32 getOutSeqNoParam();

    void increaseOutSeqNo();
    void appendToSequence(qint64 msgId);
    QList<qint64> sequence();
    void setSequence(const QList<qint64> &sequence);

private:
    Settings *mSettings;
    State mState;
    qint32 mChatId;
    InputUser mRequestedUser;
    qint64 mAccessHash;
    qint32 mAdminId;
    qint32 mParticipantId;
    qint32 mDate;
    QByteArray mGAOrB;
    BIGNUM *mMyKey;
    qint64 mKeyFingerprint;
    uchar mSharedKey[SHARED_KEY_LENGTH];

    qint32 mLayer;
    qint32 mInSeqNo;
    qint32 mOutSeqNo;

    // list of (10 as max) previous processed message ids
    QList<qint64> mSequence;
};

#endif // SECRETCHAT_H
