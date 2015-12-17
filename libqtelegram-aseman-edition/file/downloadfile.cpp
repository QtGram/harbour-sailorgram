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

#include "downloadfile.h"

DownloadFile::DownloadFile(Session *session, InputFileLocation fileLocation, qint32 expectedSize, QObject *parent) :
    File(session, 0, parent),
    mFileLocation(fileLocation),
    mOffset(0),
    mMtime(0),
    mExpectedSize(expectedSize),
    mPartId(0) {
}

DownloadFile::~DownloadFile() {
}

QByteArray DownloadFile::bytes() const {
    return mBytes;
}

void DownloadFile::appendBytes(const QByteArray &bytes) {
    mBytes.append(bytes);
    mOffset += bytes.length();
}

qint32 DownloadFile::offset() const {
    return mOffset;
}

void DownloadFile::setOffset(qint32 offset) {
    mOffset = offset;
}

qint32 DownloadFile::mtime() const  {
    return mMtime;
}
void DownloadFile::setMtime(qint32 mtime) {
    mMtime = mtime;
}

InputFileLocation DownloadFile::fileLocation() const {
    return mFileLocation;
}

qint32 DownloadFile::expectedSize() const {
    return mExpectedSize;
}

qint32 DownloadFile::partId() const {
    return mPartId;
}

void DownloadFile::incrementPartId() {
    mPartId++;
}

bool DownloadFile::encrypted() const {
    return mEncrypted;
}

uchar *DownloadFile::key() {
    return mKey;
}

uchar *DownloadFile::iv() {
    return mIv;
}
