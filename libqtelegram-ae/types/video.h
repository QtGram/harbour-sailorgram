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

#ifndef VIDEO_H
#define VIDEO_H

#include "photosize.h"

class Video
{
public:

    enum VideoType {
       typeVideoEmpty = 0xc10658a8,
       typeVideo = 0x388fa391
    };

    Video(VideoType classType) :
        m_caption(""),
        m_mimeType(""),
        m_thumb(PhotoSize::typePhotoSizeEmpty),
        m_id(0),
        m_accessHash(0),
        m_classType(classType),
        m_dcId(0),
        m_date(0),
        m_duration(0),
        m_h(0),
        m_size(0),
        m_userId(0),
        m_w(0) {}

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
    void setDuration(qint32 duration) {
        m_duration = duration;
    }
    qint32 duration() const {
        return m_duration;
    }
    void setMimeType(const QString & mimeType) {
        m_mimeType = mimeType;
    }
    QString mimeType() const {
        return m_mimeType;
    }
    void setSize(qint32 size) {
        m_size = size;
    }
    qint32 size() const {
        return m_size;
    }
    void setThumb(const PhotoSize &thumb) {
        m_thumb = thumb;
    }
    PhotoSize thumb() const {
        return m_thumb;
    }
    void setDcId(qint32 dcId) {
        m_dcId = dcId;
    }
    qint32 dcId() const {
        return m_dcId;
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
    void setClassType(VideoType classType) {
        m_classType = classType;
    }
    VideoType classType() const {
        return m_classType;
    }

private:
    QString m_caption;
    QString m_mimeType;
    PhotoSize m_thumb;
    qint64 m_id;
    qint64 m_accessHash;
    VideoType m_classType;
    qint32 m_dcId;
    qint32 m_date;
    qint32 m_duration;
    qint32 m_h;
    qint32 m_size;
    qint32 m_userId;
    qint32 m_w;
};
#endif // VIDEO_H
