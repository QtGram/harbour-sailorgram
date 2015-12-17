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

#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include "uploadfileengine.h"
#include "telegram/types/inputpeer.h"
#include "telegram/types/inputmedia.h"
#include "telegram/types/inputchatphoto.h"
#include "telegram/types/inputencryptedchat.h"
#include <QSharedPointer>

class FileOperation
{
public:
    enum OpType {
        sendMedia,
        editChatPhoto,
        uploadProfilePhoto,
        sendEncryptedFile
    };

    typedef QSharedPointer<FileOperation> Ptr;

    FileOperation(OpType opType) :
        mPeer(InputPeer::typeInputPeerEmpty),
        mInputMedia(InputMedia::typeInputMediaEmpty),
        mRandomId(0),
        mReplyToMsgId(0),
        mInputChatPhoto(InputChatPhoto::typeInputChatPhotoEmpty),
        mGeoPoint(InputGeoPoint::typeInputGeoPointEmpty),
        mCrop(InputPhotoCrop::typeInputPhotoCropAuto),
        mType(opType) {}

    InputPeer peer() const { return mPeer; }
    void setInputPeer(const InputPeer &peer) { mPeer = peer; }
    InputMedia inputMedia() const { return mInputMedia; }
    void setInputMedia(const InputMedia &inputMedia) { mInputMedia = inputMedia; }
    qint64 randomId() const { return mRandomId; }
    void setRandomId(qint64 randomId) { mRandomId = randomId; }
    qint32 replyToMsgId() const { return mReplyToMsgId; }
    void setReplyToMsgId(const qint32 &replyToMsgId) { mReplyToMsgId = replyToMsgId; }
    qint32 chatId() const { return mChatId; }
    void setChatId(qint32 chatId) { mChatId = chatId; }
    InputChatPhoto inputChatPhoto() const { return mInputChatPhoto; }
    void setInputChatPhoto(const InputChatPhoto &inputChatPhoto) { mInputChatPhoto = inputChatPhoto; }
    QString caption() const { return mCaption; }
    void setCaption(QString caption) { mCaption = caption; }
    InputGeoPoint geoPoint() const { return mGeoPoint; }
    void setGeoPoint(const InputGeoPoint &geoPoint) { mGeoPoint = geoPoint; }
    InputPhotoCrop crop() const { return mCrop; }
    void setCrop(const InputPhotoCrop &crop) { mCrop = crop; }
    OpType opType() const { return mType; }


    void setInputEncryptedChat(const InputEncryptedChat &inputEncryptedChat) { mInputEncryptedChat = inputEncryptedChat; }
    void setDecryptedMessage(const DecryptedMessage &decryptedMessage) { mDecryptedMessage = decryptedMessage; }
    InputEncryptedChat inputEncryptedChat() const { return mInputEncryptedChat; }
    DecryptedMessage decryptedMessage() const { return mDecryptedMessage; }
    QByteArray key() const { return mKey; }
    QByteArray iv() const { return mIv; }

    void initializeKeyAndIv() {
        char keyData[32];
        Utils::randomBytes(keyData, 32);
        mKey = QByteArray(keyData, 32);
        char ivData[32];
        Utils::randomBytes(ivData, 32);
        mIv = QByteArray(ivData, 32);
    }

private:
    // sendMedia operation attributes
    InputPeer mPeer;
    InputMedia mInputMedia;
    qint64 mRandomId;
    qint32 mReplyToMsgId;
    // editChatPhoto operation attributes
    qint32 mChatId;
    InputChatPhoto mInputChatPhoto;
    // uploadProfilePhoto operation attributes
    QString mCaption;
    InputGeoPoint mGeoPoint;
    InputPhotoCrop mCrop;
    OpType mType;

    InputEncryptedChat mInputEncryptedChat;
    DecryptedMessage mDecryptedMessage;
    //key and initialization vector for encrypted files
    QByteArray mKey; // 32 bytes
    QByteArray mIv; // 32 bytes
};

#endif // FILEOPERATION_H
