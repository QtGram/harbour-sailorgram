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

#ifndef DOWNLOADFILE_H
#define DOWNLOADFILE_H

#include <QString>
#include <QByteArray>
#include <QSharedPointer>
#include "util/constants.h"
#include "telegram/types/inputfilelocation.h"
#include "file.h"
#include "core/session.h"

class DownloadFile : public File
{
    Q_OBJECT
public:

    typedef QSharedPointer<DownloadFile> Ptr;

    DownloadFile(Session *session, InputFileLocation fileLocation, qint32 expectedSize, QObject *parent = 0);
    ~DownloadFile();

    void appendBytes(const QByteArray &bytes);
    QByteArray bytes() const;
    qint32 offset() const;
    void setOffset(qint32 offset);
    void setMtime(qint32 mtime);
    qint32 mtime() const;
    InputFileLocation fileLocation() const;
    qint32 expectedSize() const ;
    qint32 partId() const;
    void incrementPartId();
    bool encrypted() const;
    uchar *key();
    uchar *iv();

private:
    InputFileLocation mFileLocation;
    QString mName;
    QByteArray mBytes;
    qint32 mOffset;
    qint32 mMtime;
    qint32 mExpectedSize;
    qint32 mPartId;
};

#endif // DOWNLOADFILE_H
