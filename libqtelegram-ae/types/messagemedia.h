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

#ifndef MESSAGEMEDIA_H
#define MESSAGEMEDIA_H

#include "audio.h"
#include "document.h"
#include "geopoint.h"
#include "photo.h"
#include "video.h"
#include <QString>
#include <QByteArray>

class MessageMedia
{
public:

    enum MessageMediaType {
       typeMessageMediaDocument = 0x2fda2204,
       typeMessageMediaContact = 0x5e7d2f39,
       typeMessageMediaEmpty = 0x3ded6320,
       typeMessageMediaVideo = 0xa2d24290,
       typeMessageMediaUnsupported = 0x9f84f49e,
       typeMessageMediaAudio = 0xc6b68300,
       typeMessageMediaPhoto = 0xc8c45a2a,
       typeMessageMediaGeo = 0x56e0d474
    };

    MessageMedia(MessageMediaType classType) :
        m_audio(Audio::typeAudioEmpty),
        m_lastName(""),
        m_bytes(""),
        m_firstName(""),
        m_document(Document::typeDocumentEmpty),
        m_geo(GeoPoint::typeGeoPointEmpty),
        m_photo(Photo::typePhotoEmpty),
        m_phoneNumber(""),
        m_userId(0),
        m_video(Video::typeVideoEmpty),
        m_classType(classType) {}

    void setDocument(const Document &document) {
        m_document = document;
    }
    Document document() const {
        return m_document;
    }
    void setPhoneNumber(const QString & phoneNumber) {
        m_phoneNumber = phoneNumber;
    }
    QString phoneNumber() const {
        return m_phoneNumber;
    }
    void setFirstName(const QString & firstName) {
        m_firstName = firstName;
    }
    QString firstName() const {
        return m_firstName;
    }
    void setLastName(const QString & lastName) {
        m_lastName = lastName;
    }
    QString lastName() const {
        return m_lastName;
    }
    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setVideo(const Video &video) {
        m_video = video;
    }
    Video video() const {
        return m_video;
    }
    void setBytes(const QByteArray & bytes) {
        m_bytes = bytes;
    }
    QByteArray bytes() const {
        return m_bytes;
    }
    void setAudio(const Audio &audio) {
        m_audio = audio;
    }
    Audio audio() const {
        return m_audio;
    }
    void setPhoto(const Photo &photo) {
        m_photo = photo;
    }
    Photo photo() const {
        return m_photo;
    }
    void setGeo(const GeoPoint &geo) {
        m_geo = geo;
    }
    GeoPoint geo() const {
        return m_geo;
    }
    void setClassType(MessageMediaType classType) {
        m_classType = classType;
    }
    MessageMediaType classType() const {
        return m_classType;
    }

private:
    Audio m_audio;
    QString m_lastName;
    QByteArray m_bytes;
    QString m_firstName;
    Document m_document;
    GeoPoint m_geo;
    Photo m_photo;
    QString m_phoneNumber;
    qint32 m_userId;
    Video m_video;
    MessageMediaType m_classType;
};
#endif // MESSAGEMEDIA_H
