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

#ifndef INPUTMEDIA_H
#define INPUTMEDIA_H

#include "inputaudio.h"
#include "inputphoto.h"
#include "inputdocument.h"
#include "inputvideo.h"
#include "inputfile.h"
#include "inputgeopoint.h"
#include "documentattribute.h"

class InputMedia
{
public:

    enum InputMediaType {
       typeInputMediaUploadedDocument = 0xffe76b78,
       typeInputMediaVideo = 0x7f023ae6,
       typeInputMediaUploadedThumbDocument = 0x41481486,
       typeInputMediaUploadedPhoto = 0x2dc53a7d,
       typeInputMediaDocument = 0xd184e841,
       typeInputMediaUploadedThumbVideo = 0x9912dabf,
       typeInputMediaUploadedVideo = 0x133ad6f6,
       typeInputMediaUploadedAudio = 0x4e498cab,
       typeInputMediaGeoPoint = 0xf9c44144,
       typeInputMediaPhoto = 0x8f2ab2ec,
       typeInputMediaEmpty = 0x9664f57f,
       typeInputMediaContact = 0xa6e45987,
       typeInputMediaAudio = 0x89938781
    };

    InputMedia(InputMediaType classType) :
        m_audioId(InputAudio::typeInputAudioEmpty),
        m_photoId(InputPhoto::typeInputPhotoEmpty),
        m_videoId(InputVideo::typeInputVideoEmpty),
        m_documentId(InputDocument::typeInputDocumentEmpty),
        m_firstName(""),
        m_mimeType(""),
        m_file(InputFile::typeInputFile),
        m_thumb(InputFile::typeInputFile),
        m_geoPoint(InputGeoPoint::typeInputGeoPointEmpty),
        m_duration(0),
        m_h(0),
        m_phoneNumber(""),
        m_w(0),
        m_lastName(""),
        m_classType(classType) {}

    void setFile(const InputFile &file) { m_file = file; }
    InputFile file() const { return m_file; }
    void setMimeType(const QString & mimeType) { m_mimeType = mimeType; }
    QString mimeType() const { return m_mimeType; }
    void setPhotoId(const InputPhoto &photoId) { m_photoId = photoId; }
    InputPhoto photoId() const { return m_photoId; }
    void setAudioId(const InputAudio &audioId) { m_audioId = audioId; }
    InputAudio audioId() const { return m_audioId; }
    void setVideoId(const InputVideo &videoId) { m_videoId = videoId; }
    InputVideo videoId() const { return m_videoId; }
    void setDocumentId(const InputDocument &documentId) { m_documentId = documentId; }
    InputDocument documentId() const { return m_documentId; }
    void setThumb(const InputFile &thumb) { m_thumb = thumb; }
    InputFile thumb() const { return m_thumb; }
    void setDuration(qint32 duration) { m_duration = duration; }
    qint32 duration() const { return m_duration; }
    void setW(qint32 w) { m_w = w; }
    qint32 w() const { return m_w; }
    void setH(qint32 h) { m_h = h; }
    qint32 h() const { return m_h; }
    void setGeoPoint(const InputGeoPoint &geoPoint) { m_geoPoint = geoPoint; }
    InputGeoPoint geoPoint() const { return m_geoPoint; }
    void setPhoneNumber(const QString & phoneNumber) { m_phoneNumber = phoneNumber; }
    QString phoneNumber() const { return m_phoneNumber; }
    void setFirstName(const QString & firstName) { m_firstName = firstName; }
    QString firstName() const { return m_firstName; }
    void setLastName(const QString & lastName) { m_lastName = lastName; }
    QString lastName() const { return m_lastName; }
    void setAttributes(QList<DocumentAttribute> attrs) { m_attributes = attrs; }
    QList<DocumentAttribute> attributes() const { return m_attributes; }
    void setClassType(InputMediaType classType) { m_classType = classType; }
    InputMediaType classType() const { return m_classType; }

private:
    InputAudio m_audioId;
    InputPhoto m_photoId;
    InputVideo m_videoId;
    InputDocument m_documentId;
    QList<DocumentAttribute> m_attributes;
    QString m_firstName;
    QString m_mimeType;
    InputFile m_file;
    InputFile m_thumb;
    InputGeoPoint m_geoPoint;
    qint32 m_duration;
    qint32 m_h;
    QString m_phoneNumber;
    qint32 m_w;
    QString m_lastName;
    InputMediaType m_classType;
};
#endif // INPUTMEDIA_H
