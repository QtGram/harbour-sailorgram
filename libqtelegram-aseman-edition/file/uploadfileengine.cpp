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

#include "uploadfileengine.h"

#include <QFileInfo>

UploadFileEngine::UploadFileEngine(Session *session, CryptoUtils *crypto, UploadFileEngine::FileType fileType, const QByteArray &buffer, QObject *parent) :
    File(session, buffer.size(), parent),
    mCrypto(crypto),
    mFileType(fileType),
    mRelatedFileId(0),
    mName(""),
    mBytes(buffer),
    mFilePath(""),
    mNParts(0),
    mCurrentPart(0),
    mUploadedParts(0),
    mKrypt(QCryptographicHash::Md5)
{
    calculatePartsCount();
}

UploadFileEngine::UploadFileEngine(Session *session, CryptoUtils *crypto, UploadFileEngine::FileType fileType, const QString &filePath, QObject *parent) :
    File(session, QFileInfo(filePath).size(), parent),
    mCrypto(crypto),
    mFileType(fileType),
    mRelatedFileId(0),
    mName(QFileInfo(filePath).fileName()),
    mFilePath(filePath),
    mNParts(0),
    mCurrentPart(0),
    mUploadedParts(0),
    mIODevice(filePath),
    mKrypt(QCryptographicHash::Md5)
{
    openIODevice();
    calculatePartsCount();
}

UploadFileEngine::~UploadFileEngine() {
}

void UploadFileEngine::openIODevice() {
    if (mIODevice.open(QIODevice::ReadOnly)) {
        // get file size with file open
        mLength = mIODevice.size();
    } else {
        Q_EMIT fileNotFound();
    }
}

bool UploadFileEngine::hasMoreParts() const {
    return mCurrentPart < mNParts;
}

QByteArray UploadFileEngine::nextPart() {
    QByteArray partBytes(mPartLength, Qt::Uninitialized);

    //let's see if the file content is loaded or a filepath is set
    if (!mBytes.isEmpty()) {
        qint32 k = 0;
        if (mCurrentPart < mNParts -1) {
            // if not the last part
            qint32 n = (mCurrentPart + 1) * mPartLength;
            for (qint32 j = (mCurrentPart * mPartLength); j < n; j++) {
                partBytes[k++] = mBytes[j];
            }
        } else {
            // if last part
            qint32 lastPartSize = mLength % mPartLength;
            // if not the last part
            qint32 n = (mCurrentPart * mPartLength) + lastPartSize;
            for (qint32 j = (mCurrentPart * mPartLength); j < n; j++) {
                partBytes[k++] = mBytes[j];
            }
        }
    } else {
        // in this case we're reading an open file from a local path
        if (mIODevice.isOpen()) {
            mIODevice.seek(mCurrentPart * mPartLength); //set offset
            partBytes = mIODevice.read(mPartLength);
            if (!hasMoreParts()) {
                mIODevice.close();
            }
        } else {
            Q_EMIT fileNotOpen();
        }
    }
    mCurrentPart++;

    QByteArray processedBytes;
    if (mEncrypted) {
        processedBytes = mCrypto->encryptFilePart(partBytes, mKey, mIv);
    } else {
        processedBytes = partBytes;
    }

    // add the new data to cryptographic hash before return it
    mKrypt.addData(processedBytes, processedBytes.length());
    return processedBytes;
}

void UploadFileEngine::increaseUploadedParts() {
    mUploadedParts++;
}

QByteArray UploadFileEngine::md5() {
    return mKrypt.result();
}

void UploadFileEngine::calculatePartsCount() {
    // calculate number of parts to be uploaded
    mNParts = mLength / mPartLength;
    if (mNParts * mPartLength < mLength) mNParts++;
}
