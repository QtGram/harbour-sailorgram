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

#ifndef DECRYPTEDMESSAGEACTION_H
#define DECRYPTEDMESSAGEACTION_H

#include <QList>
#include "telegram/types/sendmessageaction.h"

class DecryptedMessageAction
{
public:

    enum DecryptedMessageActionType {
        typeDecryptedMessageActionSetMessageTTL = 0xa1733aec,
        typeDecryptedMessageActionDeleteMessages = 0x65614304,
        typeDecryptedMessageActionReadMessages = 0xc4f40be,
        typeDecryptedMessageActionScreenshotMessages = 0x8ac1f475,
        typeDecryptedMessageActionFlushHistory = 0x6719e45c,
        typeDecryptedMessageActionResend = 0x511110b0,
        typeDecryptedMessageActionNotifyLayer = 0xf3048883,
        typeDecryptedMessageActionTyping = 0xccb27641
    };

    DecryptedMessageAction(DecryptedMessageActionType classType = typeDecryptedMessageActionNotifyLayer) :
        mLayer(0),
        mTtlSeconds(0),
        mStartSeqNo(0),
        mEndSeqNo(0),
        mClassType(classType) {}

    void setLayer(qint32 layer) {
        mLayer = layer;
    }
    qint32 layer() const {
        return mLayer;
    }
    void setTtlSeconds(qint32 ttlSeconds) {
        mTtlSeconds = ttlSeconds;
    }
    qint32 ttlSeconds() const {
        return mTtlSeconds;
    }
    void setStartSeqNo(qint32 startSeqNo) {
        mStartSeqNo =  startSeqNo;
    }
    qint32 startSeqNo() const {
        return mStartSeqNo;
    }
    void setEndSeqNo(qint32 endSeqNo) {
        mEndSeqNo = endSeqNo;
    }
    qint32 endSeqNo() const {
        return mEndSeqNo;
    }
    void setAction(const SendMessageAction &action) {
        mAction = action;
    }
    SendMessageAction action() const {
        return mAction;
    }
    void setRandomIds(const QList<qint64> & randomIds) {
        mRandomIds = randomIds;
    }
    QList<qint64> randomIds() const {
        return mRandomIds;
    }
    void setClassType(DecryptedMessageActionType classType) {
        mClassType = classType;
    }
    DecryptedMessageActionType classType() const {
        return mClassType;
    }

private:
    qint32 mLayer;
    QList<qint64> mRandomIds;
    qint32 mTtlSeconds;
    qint32 mStartSeqNo;
    qint32 mEndSeqNo;
    SendMessageAction mAction;
    DecryptedMessageActionType mClassType;
};
#endif // DECRYPTEDMESSAGEACTION_H
