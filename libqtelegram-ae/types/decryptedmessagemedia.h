/*
 * Copyright 2014 Canonical Ltd.
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
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
 */
#ifndef DECRYPTEDMESSAGEMEDIA_H
#define DECRYPTEDMESSAGEMEDIA_H

#include <QByteArray>

class DecryptedMessageMedia {

public:

    enum DecryptedMessageMediaType {
        typeDecryptedMessageMediaEmpty = 0x89f5c4a,
        typeDecryptedMessageMediaPhoto = 0x32798a8c,
        typeDecryptedMessageMediaVideo_layer8 = 0x4cee6ef3,
        typeDecryptedMessageMediaGeoPoint = 0x35480a59,
        typeDecryptedMessageMediaContact = 0x588a0a97,
        typeDecryptedMessageMediaDocument = 0xb095434b,
        typeDecryptedMessageMediaAudio_layer8 = 0x6080758f,
        typeDecryptedMessageMediaVideo = 0x524a415d,
        typeDecryptedMessageMediaAudio = 0x57e0a9cb
    };

    DecryptedMessageMedia(DecryptedMessageMediaType classType = typeDecryptedMessageMediaEmpty) :
        mClassType(classType),
        mThumbW(0),
        mThumbH(0),
        mDuration(0),
        mW(0),
        mH(0),
        mSize(0),
        mLatitude(0),
        mLongitude(0),
        mPhoneNumber(""),
        mFirstName(""),
        mLastName(""),
        mUserId(0),
        mFileName(""),
        mMimeType("") {
    }

    QByteArray thumb() const { return mThumb; }
    qint32 thumbW() const { return mThumbW; }
    qint32 thumbH() const { return mThumbH; }
    qint32 duration() const { return mDuration; }
    qint32 w() const { return mW; }
    qint32 h() const { return mH; }
    qint32 size() const { return mSize; }
    double latitude() const { return mLatitude; }
    double longitude() const { return mLongitude; }
    QByteArray key() const { return mKey; }
    QByteArray iv() const { return mIv; }
    QString phoneNumber() const { return mPhoneNumber; }
    QString firstName() const { return mFirstName; }
    QString lastName() const { return mLastName; }
    qint32 userId() const { return mUserId; }
    QString fileName() const { return mFileName; }
    QString mimeType() const  { return mMimeType; }
    DecryptedMessageMediaType classType() const { return mClassType; }
    void setThumb(QByteArray thumb) { mThumb = thumb; }
    void setThumbW(qint32 thumbW) { mThumbW = thumbW; }
    void setThumbH(qint32 thumbH) { mThumbH = thumbH; }
    void setDuration(qint32 duration) { mDuration = duration; }
    void setW(qint32 w) { mW = w; }
    void setH(qint32 h) { mH = h; }
    void setSize(qint32 size) { mSize = size; }
    void setLatitude(double latitude) { mLatitude = latitude; }
    void setLongitude(double longitude) { mLongitude = longitude; }
    void setKey(QByteArray key) { mKey = key; }
    void setIv(QByteArray iv) { mIv = iv; }
    void setPhoneNumber(const QString &phoneNumber) { mPhoneNumber = phoneNumber; }
    void setFirstName(const QString &firstName) { mFirstName = firstName; }
    void setLastName(const QString &lastName) { mLastName = lastName; }
    void setUserId(qint32 userId) { mUserId = userId; }
    void setFileName(const QString &fileName) { mFileName = fileName; }
    void setMimeType(const QString &mimeType) { mMimeType = mimeType; }
    void setClassType(DecryptedMessageMediaType classType) { mClassType = classType; }

private:
    DecryptedMessageMediaType mClassType;
    QByteArray mThumb;
    qint32 mThumbW;
    qint32 mThumbH;
    qint32 mDuration;
    qint32 mW;
    qint32 mH;
    qint32 mSize;
    double mLatitude;
    double mLongitude;
    QByteArray mKey;
    QByteArray mIv;
    QString mPhoneNumber;
    QString mFirstName;
    QString mLastName;
    qint32 mUserId;
    QString mFileName;
    QString mMimeType;
};

#endif // DECRYPTEDMESSAGEMEDIA_H
