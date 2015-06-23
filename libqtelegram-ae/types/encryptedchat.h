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

#ifndef ENCRYPTEDCHAT_H
#define ENCRYPTEDCHAT_H

#include <QByteArray>

class EncryptedChat
{
public:

    enum EncryptedChatType {
       typeEncryptedChatWaiting = 0x3bf703dc,
       typeEncryptedChatRequested = 0xc878527e,
       typeEncryptedChatEmpty = 0xab7ec0a0,
       typeEncryptedChatDiscarded = 0x13d6dd27,
       typeEncryptedChat = 0xfa56ce36
    };

    EncryptedChat(EncryptedChatType classType = typeEncryptedChatEmpty) :
        mId(0),
        mGA(""),
        mKeyFingerprint(0),
        mDate(0),
        mAccessHash(0),
        mAdminId(0),
        mGAOrB(""),
        mParticipantId(0),
        mClassType(classType) {}

    void setId(qint32 id) {
        mId = id;
    }
    qint32 id() const {
        return mId;
    }
    void setAccessHash(qint64 accessHash) {
        mAccessHash = accessHash;
    }
    qint64 accessHash() const {
        return mAccessHash;
    }
    void setDate(qint32 date) {
        mDate = date;
    }
    qint32 date() const {
        return mDate;
    }
    void setAdminId(qint32 adminId) {
        mAdminId = adminId;
    }
    qint32 adminId() const {
        return mAdminId;
    }
    void setParticipantId(qint32 participantId) {
        mParticipantId = participantId;
    }
    qint32 participantId() const {
        return mParticipantId;
    }
    void setGA(const QByteArray & gA) {
        mGA = gA;
    }
    QByteArray gA() const {
        return mGA;
    }
    void setGAOrB(const QByteArray & gAOrB) {
        mGAOrB = gAOrB;
    }
    QByteArray gAOrB() const {
        return mGAOrB;
    }
    void setKeyFingerprint(qint64 keyFingerprint) {
        mKeyFingerprint = keyFingerprint;
    }
    qint64 keyFingerprint() const {
        return mKeyFingerprint;
    }
    void setClassType(EncryptedChatType classType) {
        mClassType = classType;
    }
    EncryptedChatType classType() const {
        return mClassType;
    }

private:
    qint32 mId;
    QByteArray mGA;
    qint64 mKeyFingerprint;
    qint32 mDate;
    qint64 mAccessHash;
    qint32 mAdminId;
    QByteArray mGAOrB;
    qint32 mParticipantId;
    EncryptedChatType mClassType;
};
#endif // ENCRYPTEDCHAT_H
