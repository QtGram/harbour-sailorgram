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

#ifndef STORAGEFILETYPE_H
#define STORAGEFILETYPE_H
class StorageFileType
{
public:

    enum StorageFileTypeType {
       typeStorageFileMov = 0x4b09ebbc,
       typeStorageFilePartial = 0x40bc6f52,
       typeStorageFileUnknown = 0xaa963b05,
       typeStorageFileJpeg = 0x7efe0e,
       typeStorageFilePdf = 0xae1e508d,
       typeStorageFileMp4 = 0xb3cea0e4,
       typeStorageFilePng = 0xa4f63c0,
       typeStorageFileGif = 0xcae1aadf,
       typeStorageFileMp3 = 0x528a0677,
       typeStorageFileWebp = 0x1081464c
    };

    StorageFileType() : m_classType(typeStorageFileUnknown) {}
    StorageFileType(StorageFileTypeType classType) : m_classType(classType) {}

    void setClassType(StorageFileTypeType classType) {
        m_classType = classType;
    }
    StorageFileTypeType classType() const {
        return m_classType;
    }

private:
    StorageFileTypeType m_classType;
};
#endif // STORAGEFILETYPE_H
