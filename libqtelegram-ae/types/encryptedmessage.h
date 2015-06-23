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

#ifndef ENCRYPTEDMESSAGE_H
#define ENCRYPTEDMESSAGE_H

#include <QByteArray>
#include "encryptedfile.h"

class EncryptedMessage
{
public:

    enum EncryptedMessageType {
       typeEncryptedMessageService = 0x23734b06,
       typeEncryptedMessage = 0xed18c118
    };

    EncryptedMessage(EncryptedMessageType classType = typeEncryptedMessage) :
        mChatId(0),
        mDate(0),
        mRandomId(0),
        mFile(EncryptedFile::typeEncryptedFileEmpty),
        mBytes(""),
        mClassType(classType) {}

    void setRandomId(qint64 randomId) {
        mRandomId = randomId;
    }
    qint64 randomId() const {
        return mRandomId;
    }
    void setChatId(qint32 chatId) {
        mChatId = chatId;
    }
    qint32 chatId() const {
        return mChatId;
    }
    void setDate(qint32 date) {
        mDate = date;
    }
    qint32 date() const {
        return mDate;
    }
    void setBytes(const QByteArray &bytes) {
        mBytes = bytes;
    }
    QByteArray bytes() const {
        return mBytes;
    }
    void setFile(const EncryptedFile &file) {
        mFile = file;
    }
    EncryptedFile file() const {
        return mFile;
    }
    void setClassType(EncryptedMessageType classType) {
        mClassType = classType;
    }
    EncryptedMessageType classType() const {
        return mClassType;
    }

private:
    qint32 mChatId;
    qint32 mDate;
    qint64 mRandomId;
    EncryptedFile mFile;
    QByteArray mBytes;
    EncryptedMessageType mClassType;
};
#endif // ENCRYPTEDMESSAGE_H
