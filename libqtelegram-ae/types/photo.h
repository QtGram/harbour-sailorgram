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

#ifndef PHOTO_H
#define PHOTO_H

#include "photosize.h"
#include "geopoint.h"

class Photo
{
public:

    enum PhotoType {
       typePhoto = 0x22b56751,
       typePhotoEmpty = 0x2331b22d
    };

    Photo(PhotoType classType = typePhotoEmpty) :
        m_id(0),
        m_caption(""),
        m_date(0),
        m_geo(GeoPoint::typeGeoPointEmpty),
        m_accessHash(0),
        m_userId(0),
        m_classType(classType) {}

    void setId(qint64 id) {
        m_id = id;
    }
    qint64 id() const {
        return m_id;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setDate(qint32 date) {
        m_date = date;
    }
    qint32 date() const {
        return m_date;
    }
    void setCaption(const QString & caption) {
        m_caption = caption;
    }
    QString caption() const {
        return m_caption;
    }
    void setGeo(const GeoPoint &geo) {
        m_geo = geo;
    }
    GeoPoint geo() const {
        return m_geo;
    }
    void setSizes(const QList<PhotoSize> & sizes) {
        m_sizes = sizes;
    }
    QList<PhotoSize> sizes() const {
        return m_sizes;
    }
    void setClassType(PhotoType classType) {
        m_classType = classType;
    }
    PhotoType classType() const {
        return m_classType;
    }

private:
    qint64 m_id;
    QString m_caption;
    qint32 m_date;
    QList<PhotoSize> m_sizes;
    GeoPoint m_geo;
    qint64 m_accessHash;
    qint32 m_userId;
    PhotoType m_classType;
};
#endif // PHOTO_H
