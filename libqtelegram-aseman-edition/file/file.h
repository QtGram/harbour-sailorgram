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

#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QLoggingCategory>
#include "util/constants.h"
#include "util/utils.h"
#include "core/session.h"

Q_DECLARE_LOGGING_CATEGORY(TG_FILE_FILEHANDLER)

class File : public QObject
{
    Q_OBJECT
public:
    explicit File(Session *session, qint64 length, QObject *parent = 0) :
        QObject(parent),
        mSession(session),
        mLength(length),
        mPartLength(BLOCK),
        mTimeOut(0),
        mEncrypted(false) {

        // generate random file id
        Utils::randomBytes(&mId, 8);

        // calculate part length
        qint32 tmp = ((mLength + 2999) / 3000);
        while (mPartLength < tmp) {
            mPartLength *= 2;
        }
        if (mPartLength > (512 << 10)) {
            qCCritical(TG_FILE_FILEHANDLER) << "Too big file. Max supported part size is" << ((512 << 10) + 1000);
            return;
        }
    }
    virtual ~File() {
        if (mSession) {
            mSession->release();
        }
    }

    void setSession(Session *session) { mSession = session; }
    Session *session() { return mSession; }
    qint64 id() const { return mId; }
    qint64 length() const { return mLength; }
    qint64 partLength() const { return mPartLength; }
    qint32 timeOut() const { return mTimeOut; }

    void setPartLength(qint64 l) { mPartLength = l; }
    void setEncrypted(bool encrypted) { mEncrypted = encrypted; }
    void setKey(const QByteArray &key) { memcpy(mKey, key.data(), 32); }
    void setIv(const QByteArray &iv) { memcpy(mIv, iv.data(), 32); }
    void setTimeOut(const qint32 &timeOut) { mTimeOut = timeOut; }

protected:
    Session *mSession;
    qint64 mId;
    qint64 mLength;
    qint64 mPartLength;
    qint32 mTimeOut;

    bool mEncrypted; // if true, this file upload belongs to a secret chat
    //key and initialization vector for encrypted files
    uchar mKey[32]; // 32 bytes
    uchar mIv[32];  // 32 bytes
};

#endif // FILE_H
