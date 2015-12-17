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

#ifndef UPLOADFILEENGINE_H
#define UPLOADFILEENGINE_H

#include <QObject>
#include <QSharedPointer>
#include <QFile>
#include <QCryptographicHash>
#include "file.h"
#include "telegram/types/inputmedia.h"

class UploadFileEngine : public File
{
    Q_OBJECT
public:

    enum FileType {
        Main,
        Thumbnail
    };

    typedef QSharedPointer<UploadFileEngine> Ptr;

    explicit UploadFileEngine(Session *session, CryptoUtils *crypto, FileType type, const QByteArray &buffer, QObject *parent = 0);
    explicit UploadFileEngine(Session *session, CryptoUtils *crypto, FileType type, const QString &filePath, QObject *parent = 0);
    ~UploadFileEngine();

    qint64 relatedFileId() const { return mRelatedFileId; }
    void setRelatedFileId(qint64 relatedFileId) { mRelatedFileId = relatedFileId; }
    QString name() const { return mName; }
    void setName(const QString &name) { mName = name; }
    FileType fileType() const { return mFileType; }
    qint32 nParts() const { return mNParts; }
    qint32 uploadedParts() const { return mUploadedParts; }

    bool hasMoreParts() const;
    QByteArray nextPart();
    void increaseUploadedParts();
    QByteArray md5();

Q_SIGNALS:
    void fileNotFound();
    void fileNotOpen();

private:
    CryptoUtils *mCrypto;
    FileType mFileType;
    qint64 mRelatedFileId; // if this is the main file, related file is thumbnail and viceversa
    QString mName;
    QByteArray mBytes;
    QString mFilePath;
    qint32 mNParts;
    qint32 mCurrentPart;
    qint32 mUploadedParts;
    QFile mIODevice;
    QCryptographicHash mKrypt;

    void openIODevice();
    void calculatePartsCount();
};

#endif // UPLOADFILEENGINE_H
