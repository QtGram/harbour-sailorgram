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
 */

#include "decrypter.h"
#include "util/constants.h"
#include "util/tlvalues.h"
#include "util/utils.h"
#include "core/settings.h"
#include <sha.h>
#include <aes.h>

Q_LOGGING_CATEGORY(TG_SECRET_DECRYPTER, "tg.secret.decrypter")

template <typename T_>
void do_delete(T_ buf[]) {
    delete[] buf;
}

Decrypter::Decrypter(Settings *settings) :
    InboundPkt(0, 0),
    mSettings(settings),
    mSecretChat(0) {
    mBufferPtr = QSharedPointer<char>(new char[PACKET_BUFFER_SIZE], do_delete<char>);
}

Decrypter::~Decrypter() {
    mBufferPtr.clear();
}

void Decrypter::setSecretChat(SecretChat *secretChat) {
    mSecretChat = secretChat;
}

DecryptedMessage Decrypter::decryptEncryptedData(qint64 randomId, const QByteArray &bytes) {

    m_length = bytes.length();

    if (m_length == 0) {
        return DecryptedMessage();
    }

    m_buffer = mBufferPtr.data();
    memcpy(m_buffer, bytes.data(), m_length);

    if (!((m_length & 15) == 8)) {
        qCWarning(TG_SECRET_DECRYPTER) << "Received packet doesn't satisfy length rule (!(length & 0xF) == 1000)";
        return DecryptedMessage();
    }

    m_inPtr = (qint32 *)m_buffer;
    m_inEnd = m_inPtr + (m_length / 4);

    bool processMessage = true;
    bool gap = false;
    qint32 deflatedOutSeqNo = -1;
    qint32 receivedOutSeqNoParam = -1;
    // message must be read completly regardless it belongs to a valid secret chat
    if (!mSecretChat) {
        qCDebug(TG_SECRET_DECRYPTER) << "Received messsage to unknown chat. Dropping";
        processMessage = false;
    } else {
        processMessage = checkKeyFingerprints();
    }

    if (mSecretChat && processMessage) {

        decryptEncryptedMessage();

        qint32 x = prefetchInt();

        if (x == TL_DecryptedMessageLayer) {
            fetchInt(); //needed to advance pointer after TL_DecryptedMessageLayer

            QByteArray randomBytes = fetchBytes();
            processMessage = randomBytes.length() > 15;

            qint32 layer = fetchInt();
            processMessage = layer >= 17;

            qint32 receivedInSeqNoParam = fetchInt();
            receivedOutSeqNoParam = fetchInt();
            qCDebug(TG_SECRET_DECRYPTER) << "Received in_seq_no=" << receivedInSeqNoParam << "(expected" << (mSecretChat->getOutSeqNoParam() - 2) << ")";
            qCDebug(TG_SECRET_DECRYPTER) << "Received out_seq_no=" << receivedOutSeqNoParam << "(expected" << mSecretChat->getInSeqNoParam() << ")";

            qint32 ourId = mSettings->ourId();

            if ((receivedOutSeqNoParam & 1) != 1 - (mSecretChat->adminId() == ourId) ||
                    (receivedInSeqNoParam & 1) != (mSecretChat->adminId() == ourId)) {
                qCWarning(TG_SECRET_DECRYPTER) << "Bad message admin";
                processMessage = false;
            }

            deflatedOutSeqNo = receivedOutSeqNoParam / 2;
            qint32 deflatedInSeqNo = receivedInSeqNoParam / 2;

            if (deflatedOutSeqNo != mSecretChat->inSeqNo()) {
                if (deflatedOutSeqNo < mSecretChat->inSeqNo()) {
                    qCWarning(TG_SECRET_DECRYPTER) << "Received out_seq_no (" << deflatedOutSeqNo
                                             << ") is lower than expected (" << mSecretChat->inSeqNo() << ")";
                    processMessage = false;
                } else if (deflatedOutSeqNo > mSecretChat->inSeqNo()) {
                    qCWarning(TG_SECRET_DECRYPTER) << "Found gap since received out_seq_no (" << deflatedOutSeqNo
                                             << ") is greater than expected (" << mSecretChat->inSeqNo()
                                             << "). Requesting intermediate messages to fill the gap";
                    processMessage = false;
                    // set gap flag to true, to store the incoming message into a gap waiting queue until solving the gap
                    gap = true;
                }
            }
            if (deflatedInSeqNo > mSecretChat->outSeqNo()) {
                qCWarning(TG_SECRET_DECRYPTER) << "deflated received in_seq_no is bigger than our's out seq no sent in last message (out_seq_no ="
                                         << deflatedInSeqNo << ", our out_seq_no =" << mSecretChat->outSeqNo();
                processMessage = false;
            }

            if (processMessage) {
                mSecretChat->setLayer(layer);
                mSecretChat->setInSeqNo(receivedOutSeqNoParam / 2 + 1);
                qCDebug(TG_SECRET_DECRYPTER) << "Updating in_seq_no; Current values -> out_seq_no:" << mSecretChat->outSeqNo()
                                       << ",in_seq_no:" << mSecretChat->inSeqNo();
            }
        }

        DecryptedMessage decryptedMessage = fetchDecryptedMessage();

        // from layer 17, decrypted message randomId must be equals to wrapping message one
        if (mSecretChat->layer() >= 17 && decryptedMessage.randomId() != randomId) {
            processMessage = false;
        }

        if (processMessage) {
            // process internal message
            // TODO Roberto: move this to another method
            if (decryptedMessage.classType() == DecryptedMessage::typeDecryptedMessageService ||
                    decryptedMessage.classType() == DecryptedMessage::typeDecryptedMessageService_level8) {
                DecryptedMessageAction action = decryptedMessage.action();
                switch (action.classType()) {
                case DecryptedMessageAction::typeDecryptedMessageActionNotifyLayer: {
                    mSecretChat->setLayer(action.layer());
                    qCDebug(TG_SECRET_DECRYPTER) << "Received layer" << action.layer() << "from peer";
                    break;
                }
                default:
                    ;
                    //TODO Roberto: implement rest of cases
                }
            } else {
                qCDebug(TG_SECRET_DECRYPTER) << "Received message";
            }

            // append to secret chat sequence
            mSecretChat->appendToSequence(decryptedMessage.randomId());

            return decryptedMessage;
        }

        if (gap) {
            // ask server for all the gap messages, including this same message
            Q_EMIT sequenceNumberGap(mSecretChat->chatId(), mSecretChat->getOutSeqNoParam(), receivedOutSeqNoParam);
        }
    }
    return DecryptedMessage();
}

bool Decrypter::checkKeyFingerprints() {
    bool result = false;
    if (mSecretChat) {
        qint64 keyFingerprint = fetchLong();
        qCDebug(TG_SECRET_DECRYPTER) << "Received keyFingerprint" << keyFingerprint;
        qCDebug(TG_SECRET_DECRYPTER) << "Stored keyFingerprint" << mSecretChat->keyFingerprint();
        if (keyFingerprint != mSecretChat->keyFingerprint()) {
            qCWarning(TG_SECRET_DECRYPTER) << "Encrypted message with bad keyFingerprint for chat" << mSecretChat->chatId();
        } else {
            qCDebug(TG_SECRET_DECRYPTER) << "keyFingerprints matches!!";
            result = true;
        }
    }
    return result;
}

QByteArray Decrypter::decryptEncryptedMessage() {

    qint32 *sharedKey = (qint32*)mSecretChat->sharedKey();
    qint32 *msgKey = m_inPtr;
    qint32 bufferLength = 4 * (m_inEnd - m_inPtr);

    fetchInts(4);
    static uchar sha1a_buffer[20];
    static uchar sha1b_buffer[20];
    static uchar sha1c_buffer[20];
    static uchar sha1d_buffer[20];
    static uchar key[32];
    static uchar iv[32];

    static uchar buf[64];
    memcpy(buf, msgKey, 16);
    memcpy(buf + 16, sharedKey, 32);
    SHA1(buf, 48, sha1a_buffer);

    memcpy(buf, sharedKey + 8, 16);
    memcpy(buf + 16, msgKey, 16);
    memcpy(buf + 32, sharedKey + 12, 16);
    SHA1(buf, 48, sha1b_buffer);

    memcpy(buf, sharedKey + 16, 32);
    memcpy(buf + 32, msgKey, 16);
    SHA1(buf, 48, sha1c_buffer);

    memcpy(buf, msgKey, 16);
    memcpy(buf + 16, sharedKey + 24, 32);
    SHA1(buf, 48, sha1d_buffer);

    memcpy(key, sha1a_buffer + 0, 8);
    memcpy(key + 8, sha1b_buffer + 8, 12);
    memcpy(key + 20, sha1c_buffer + 4, 12);

    memcpy(iv, sha1a_buffer + 8, 12);
    memcpy(iv + 12, sha1b_buffer + 0, 8);
    memcpy(iv + 20, sha1c_buffer + 16, 4);
    memcpy(iv + 24, sha1d_buffer + 0, 8);

    AES_KEY aesKey;
    AES_set_decrypt_key(key, 256, &aesKey);
    AES_ige_encrypt((uchar *)m_inPtr, (uchar *)m_inPtr, bufferLength, &aesKey, iv, 0);
    Utils::secureZeroMemory(&aesKey, 0, sizeof(aesKey));

    qint32 x = prefetchInt();
    if (x < 0 || (x & 3)) {
        qCWarning(TG_SECRET_DECRYPTER) << "Not valid value for internal data length" << x;
        return QByteArray();
    }
    ASSERT(x >= 0 && !(x & 3));
    SHA1((uchar *)m_inPtr, 4 + x, sha1a_buffer);

    if (memcmp(sha1a_buffer + 4, msgKey, 16)) {
        qCWarning(TG_SECRET_DECRYPTER) << "SHA1 mismatch";
        return QByteArray();
    }
    qCDebug(TG_SECRET_DECRYPTER) << "SHA1 checked and valid";

    qint32 plainDataLength = fetchInt();
    m_inEnd = m_inPtr + plainDataLength;
    qCDebug(TG_SECRET_DECRYPTER) << "decrypted data length" << plainDataLength;

    QByteArray plainData((char *)m_inPtr, plainDataLength);
    return plainData;
}

DecryptedMessage Decrypter::fetchDecryptedMessage() {
    qint32 x = fetchInt();
    ASSERT(x == (qint32)DecryptedMessage::typeDecryptedMessage_level8 ||
           x == (qint32)DecryptedMessage::typeDecryptedMessageService_level8 ||
           x == (qint32)DecryptedMessage::typeDecryptedMessage ||
           x == (qint32)DecryptedMessage::typeDecryptedMessageService);
    DecryptedMessage message((DecryptedMessage::DecryptedMessageType)x);
    message.setRandomId(fetchLong());

    switch (x) {
    case DecryptedMessage::typeDecryptedMessage_level8: {
        message.setRandomBytes(fetchBytes());
        message.setMessage(fetchQString());
        message.setMedia(fetchDecryptedMessageMedia());
        break;
    }
    case DecryptedMessage::typeDecryptedMessageService_level8: {
        message.setRandomBytes(fetchBytes());
        message.setAction(fetchDecryptedMessageAction());
        break;
    }
    case DecryptedMessage::typeDecryptedMessage: {
        message.setTtl(fetchInt());
        message.setMessage(fetchQString());
        message.setMedia(fetchDecryptedMessageMedia());
        break;
    }
    case DecryptedMessage::typeDecryptedMessageService: {
        message.setAction(fetchDecryptedMessageAction());
        break;
    }
    }
    return message;
}

DecryptedMessageMedia Decrypter::fetchDecryptedMessageMedia() {
    qint32 x = fetchInt();
    ASSERT(x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaEmpty ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaPhoto ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaVideo_layer8 ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaGeoPoint ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaContact ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaDocument ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaAudio_layer8 ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaVideo ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaAudio ||
           x == (qint32)DecryptedMessageMedia::typeDecryptedMessageMediaExternalDocument);
    DecryptedMessageMedia media((DecryptedMessageMedia::DecryptedMessageMediaType)x);

    switch(x) {
    case DecryptedMessageMedia::typeDecryptedMessageMediaEmpty: {
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaExternalDocument: {
        media.setId(fetchInt());
        media.setAccessHash(fetchLong());
        media.setDate(fetchLong());
        media.setMimeType(fetchQString());
        media.setSize(fetchInt());
        media.setThumb23(fetchPhotoSize());
        media.setDcId(fetchInt());

        ASSERT(fetchInt() == (qint32)TL_Vector);
        qint32 n = fetchInt();
        QList<DocumentAttribute> attrs;
        for (qint32 i = 0; i < n; i++)
            attrs.append(fetchDocumentAttribute());
        media.setAttributes(attrs);
        break;
    }

    case DecryptedMessageMedia::typeDecryptedMessageMediaPhoto: {
        media.setThumb(fetchBytes());
        media.setThumbW(fetchInt());
        media.setThumbH(fetchInt());
        media.setW(fetchInt());
        media.setH(fetchInt());
        media.setSize(fetchInt());
        media.setKey(fetchBytes());
        media.setIv(fetchBytes());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaVideo_layer8: {
        media.setThumb(fetchBytes());
        media.setThumbW(fetchInt());
        media.setThumbH(fetchInt());
        media.setDuration(fetchInt());
        media.setW(fetchInt());
        media.setH(fetchInt());
        media.setSize(fetchInt());
        media.setKey(fetchBytes());
        media.setIv(fetchBytes());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaGeoPoint: {
        media.setLatitude(fetchDouble());
        media.setLongitude(fetchDouble());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaContact: {
        media.setPhoneNumber(fetchQString());
        media.setFirstName(fetchQString());
        media.setLastName(fetchQString());
        media.setUserId(fetchInt());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaDocument: {
        media.setThumb(fetchBytes());
        media.setThumbW(fetchInt());
        media.setThumbH(fetchInt());
        media.setFileName(fetchQString());
        media.setMimeType(fetchQString());
        media.setSize(fetchInt());
        media.setKey(fetchBytes());
        media.setIv(fetchBytes());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaAudio_layer8: {
        media.setDuration(fetchInt());
        media.setSize(fetchInt());
        media.setKey(fetchBytes());
        media.setIv(fetchBytes());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaVideo: {
        media.setThumb(fetchBytes());
        media.setThumbW(fetchInt());
        media.setThumbH(fetchInt());
        media.setDuration(fetchInt());
        media.setMimeType(fetchQString());
        media.setW(fetchInt());
        media.setH(fetchInt());
        media.setSize(fetchInt());
        media.setKey(fetchBytes());
        media.setIv(fetchBytes());
        break;
    }
    case DecryptedMessageMedia::typeDecryptedMessageMediaAudio: {
        media.setDuration(fetchInt());
        media.setMimeType(fetchQString());
        media.setSize(fetchInt());
        media.setKey(fetchBytes());
        media.setIv(fetchBytes());
        break;
    }
    }
    return media;
}

DecryptedMessageAction Decrypter::fetchDecryptedMessageAction() {
    qint32 x = fetchInt();
    ASSERT(x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionSetMessageTTL ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionReadMessages ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionDeleteMessages ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionScreenshotMessages ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionFlushHistory ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionResend ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionNotifyLayer ||
           x == (qint32)DecryptedMessageAction::typeDecryptedMessageActionTyping);
    DecryptedMessageAction action((DecryptedMessageAction::DecryptedMessageActionType)x);

    switch (x) {
    case DecryptedMessageAction::typeDecryptedMessageActionSetMessageTTL: {
        action.setTtlSeconds(fetchInt());
        break;
    }
    case DecryptedMessageAction::typeDecryptedMessageActionReadMessages:
    case DecryptedMessageAction::typeDecryptedMessageActionDeleteMessages:
    case DecryptedMessageAction::typeDecryptedMessageActionScreenshotMessages: {
        ASSERT(fetchInt() == (qint32)TL_Vector);
        qint32 n = fetchInt();
        QList<qint64> randomIds;
        for (qint32 i = 0; i < n; i++) {
            randomIds.append(fetchLong());
        }
        action.setRandomIds(randomIds);
    }
    case DecryptedMessageAction::typeDecryptedMessageActionResend: {
        action.setStartSeqNo(fetchInt());
        action.setEndSeqNo(fetchInt());
        break;
    }
    case DecryptedMessageAction::typeDecryptedMessageActionNotifyLayer: {
        action.setLayer(fetchInt());
        break;
    }
    case DecryptedMessageAction::typeDecryptedMessageActionTyping: {
        action.setAction(fetchSendMessageAction());
        break;
    }
    }
    return action;
}

SendMessageAction Decrypter::fetchSendMessageAction() {
    qint32 x = fetchInt();
    ASSERT(x == (qint32)SendMessageAction::typeSendMessageTypingAction ||
           x == (qint32)SendMessageAction::typeSendMessageCancelAction ||
           x == (qint32)SendMessageAction::typeSendMessageRecordVideoAction ||
           x == (qint32)SendMessageAction::typeSendMessageUploadVideoAction ||
           x == (qint32)SendMessageAction::typeSendMessageRecordAudioAction ||
           x == (qint32)SendMessageAction::typeSendMessageUploadAudioAction ||
           x == (qint32)SendMessageAction::typeSendMessageUploadPhotoAction ||
           x == (qint32)SendMessageAction::typeSendMessageUploadDocumentAction ||
           x == (qint32)SendMessageAction::typeSendMessageGeoLocationAction ||
           x == (qint32)SendMessageAction::typeSendMessageChooseContactAction);
    SendMessageAction sendMessageAction((SendMessageAction::SendMessageActionType)x);
    return sendMessageAction;
}
