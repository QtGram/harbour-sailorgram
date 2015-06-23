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

#ifndef PHOTOSIZE_H
#define PHOTOSIZE_H

#include "filelocation.h"

class PhotoSize
{
public:

    enum PhotoSizeType {
       typePhotoCachedSize = 0xe9a734fa,
       typePhotoSizeEmpty = 0xe17e23c,
       typePhotoSize = 0x77bfb61b
    };

    PhotoSize(PhotoSizeType classType = typePhotoSizeEmpty) :
        m_type(""),
        m_bytes(""),
        m_location(FileLocation::typeFileLocationUnavailable),
        m_classType(classType),
        m_h(0),
        m_size(0),
        m_w(0) {}

    void setType(const QString & type) {
        m_type = type;
    }
    QString type() const {
        return m_type;
    }
    void setLocation(const FileLocation &location) {
        m_location = location;
    }
    FileLocation location() const {
        return m_location;
    }
    void setW(qint32 w) {
        m_w = w;
    }
    qint32 w() const {
        return m_w;
    }
    void setH(qint32 h) {
        m_h = h;
    }
    qint32 h() const {
        return m_h;
    }
    void setBytes(const QByteArray & bytes) {
        m_bytes = bytes;
    }
    QByteArray bytes() const {
        return m_bytes;
    }
    void setSize(qint32 size) {
        m_size = size;
    }
    qint32 size() const {
        return m_size;
    }
    void setClassType(PhotoSizeType classType) {
        m_classType = classType;
    }
    PhotoSizeType classType() const {
        return m_classType;
    }

private:
    QString m_type;
    QByteArray m_bytes;
    FileLocation m_location;
    PhotoSizeType m_classType;
    qint32 m_h;
    qint32 m_size;
    qint32 m_w;
};
#endif // PHOTOSIZE_H
