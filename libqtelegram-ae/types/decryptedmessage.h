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
#ifndef DECRYPTEDMESSAGE_H
#define DECRYPTEDMESSAGE_H

#include "decryptedmessageaction.h"
#include "decryptedmessagemedia.h"

class DecryptedMessage
{
public:

    enum DecryptedMessageType {
        typeDecryptedMessage_level8 = 0x1f814f1f,
        typeDecryptedMessageService_level8 = 0xaa48327d,
        typeDecryptedMessage = 0x204d3878,
        typeDecryptedMessageService = 0x73164160
    };

    DecryptedMessage(DecryptedMessageType classType = typeDecryptedMessage) :
        mRandomId(0),
        mTtl(0),
        mMessage(""),
        mClassType(classType) {}

    qint64 randomId() const { return mRandomId; }
    qint32 ttl() const { return mTtl; }
    QByteArray randomBytes() const { return mRandomBytes; }
    QString message() const { return mMessage; }
    DecryptedMessageMedia media() const { return mMedia; }
    DecryptedMessageAction action() const { return mAction; }
    DecryptedMessageType classType() const { return mClassType; }
    void setRandomId(qint64 randomId) { mRandomId = randomId; }
    void setTtl(qint32 ttl) { mTtl = ttl; }
    void setRandomBytes(const QByteArray &randomBytes) { mRandomBytes = randomBytes; }
    void setMessage(const QString &message) { mMessage = message; }
    void setMedia(const DecryptedMessageMedia &media) { mMedia = media; }
    void setAction(const DecryptedMessageAction &action) { mAction = action; }
    void setClassType(DecryptedMessageType classType) { mClassType = classType; }

private:
    qint64 mRandomId;
    qint32 mTtl;
    QByteArray mRandomBytes;
    QString mMessage;
    DecryptedMessageMedia mMedia;
    DecryptedMessageAction mAction;
    DecryptedMessageType mClassType;
};

#endif // DECRYPTEDMESSAGE_H
