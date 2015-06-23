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

#ifndef INPUTENCRYPTEDFILE_H
#define INPUTENCRYPTEDFILE_H

#include <QString>

class InputEncryptedFile
{
public:

    enum InputEncryptedFileType {
       typeInputEncryptedFileUploaded = 0x64bd0306,
       typeInputEncryptedFileBigUploaded = 0x2dc173c8,
       typeInputEncryptedFileEmpty = 0x1837c364,
       typeInputEncryptedFile = 0x5a17b5e5
    };

    InputEncryptedFile(InputEncryptedFileType classType) :
        m_id(0),
        m_accessHash(0),
        m_md5Checksum(""),
        m_classType(classType),
        m_keyFingerprint(0),
        m_parts(0) {}

    void setId(qint64 id) {
        m_id = id;
    }
    qint64 id() const {
        return m_id;
    }
    void setParts(qint32 parts) {
        m_parts = parts;
    }
    qint32 parts() const {
        return m_parts;
    }
    void setMd5Checksum(const QString & md5Checksum) {
        m_md5Checksum = md5Checksum;
    }
    QString md5Checksum() const {
        return m_md5Checksum;
    }
    void setKeyFingerprint(qint32 keyFingerprint) {
        m_keyFingerprint = keyFingerprint;
    }
    qint32 keyFingerprint() const {
        return m_keyFingerprint;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setClassType(InputEncryptedFileType classType) {
        m_classType = classType;
    }
    InputEncryptedFileType classType() const {
        return m_classType;
    }

private:
    qint64 m_id;
    qint64 m_accessHash;
    QString m_md5Checksum;
    InputEncryptedFileType m_classType;
    qint32 m_keyFingerprint;
    qint32 m_parts;
};
#endif // INPUTENCRYPTEDFILE_H
