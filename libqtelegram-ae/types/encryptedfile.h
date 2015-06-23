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

#ifndef ENCRYPTEDFILE_H
#define ENCRYPTEDFILE_H

#include <QtGlobal>

class EncryptedFile
{
public:

    enum EncryptedFileType {
       typeEncryptedFileEmpty = 0xc21f497e,
       typeEncryptedFile = 0x4a70994c
    };

    EncryptedFile(EncryptedFileType classType = typeEncryptedFileEmpty) :
        mId(0),
        mAccessHash(0),
        mClassType(classType),
        mDcId(0),
        mKeyFingerprint(0),
        mSize(0) {}

    void setId(qint64 id) {
        mId = id;
    }
    qint64 id() const {
        return mId;
    }
    void setAccessHash(qint64 accessHash) {
        mAccessHash = accessHash;
    }
    qint64 accessHash() const {
        return mAccessHash;
    }
    void setSize(qint32 size) {
        mSize = size;
    }
    qint32 size() const {
        return mSize;
    }
    void setDcId(qint32 dcId) {
        mDcId = dcId;
    }
    qint32 dcId() const {
        return mDcId;
    }
    void setKeyFingerprint(qint32 keyFingerprint) {
        mKeyFingerprint = keyFingerprint;
    }
    qint32 keyFingerprint() const {
        return mKeyFingerprint;
    }
    void setClassType(EncryptedFileType classType) {
        mClassType = classType;
    }
    EncryptedFileType classType() const {
        return mClassType;
    }

private:
    qint64 mId;
    qint64 mAccessHash;
    EncryptedFileType mClassType;
    qint32 mDcId;
    qint32 mKeyFingerprint;
    qint32 mSize;
};
#endif // ENCRYPTEDFILE_H
