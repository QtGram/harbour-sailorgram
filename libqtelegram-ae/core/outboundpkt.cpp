/*
 * Copyright 2013 Vitaly Valtman
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

#include "outboundpkt.h"


#include "util/utils.h"
#include "util/tlvalues.h"

Q_LOGGING_CATEGORY(TG_CORE_OUTBOUNDPKT, "tg.core.outboundpkt")

template <typename T_>
void do_delete(T_ buf[]) {
    delete[] buf;
}

OutboundPkt::OutboundPkt(Settings *settings) :
    mSettings(settings)
{
    __buffer = QSharedPointer<qint32>(new qint32[PACKET_BUFFER_SIZE], do_delete<qint32>);
    m_packetBuffer = __buffer.data() + 16;
    clearPacket();
}

OutboundPkt::~OutboundPkt() {
    __buffer.clear();
}

qint32 OutboundPkt::length() const {
    return m_packetPtr - m_packetBuffer;
}

qint32 *OutboundPkt::buffer() {
    return m_packetBuffer;
}

void OutboundPkt::clearPacket() {
    m_packetPtr = m_packetBuffer;
}

void OutboundPkt::forwardPtr(qint32 positions) {
    m_packetPtr += positions;
}

void OutboundPkt::appendBignum(BIGNUM *n) {
    qint32 l = Utils::serializeBignum(n, (char *)m_packetPtr, (PACKET_BUFFER_SIZE - (m_packetPtr - m_packetBuffer)) * 4);
    Q_ASSERT(l > 0);
    m_packetPtr += l >> 2;
}

void OutboundPkt::appendLong(qint64 x) {
    Q_ASSERT(m_packetPtr + 2 <= m_packetBuffer + PACKET_BUFFER_SIZE);
    qCDebug(TG_CORE_OUTBOUNDPKT) << "appendLong()" << QString::number(x) << " (" << "0x" + QString::number(x,16) << ")";;
    *(qint64 *)m_packetPtr = x;
    m_packetPtr += 2;
}

void OutboundPkt::appendBytes(const QByteArray &byteArray) {
    appendCString(byteArray.data(), byteArray.size());
}

void OutboundPkt::appendData(const void *data, qint32 len) {
    Q_ASSERT(len >= 0 && len < (1 << 24) && !(len & 3));
    Q_ASSERT((char *) m_packetPtr + len + 8 < (char *) (m_packetBuffer + PACKET_BUFFER_SIZE));
    memcpy (m_packetPtr, data, len);
    m_packetPtr += len >> 2;
}

void OutboundPkt::appendInts (const qint32 *what, qint32 len) {
    Q_ASSERT(m_packetPtr + len <= m_packetBuffer + PACKET_BUFFER_SIZE);
    memcpy (m_packetPtr, what, len * 4);
    m_packetPtr += len;
}

void OutboundPkt::appendInt (qint32 x) {
    Q_ASSERT(m_packetPtr + 1 <= m_packetBuffer + PACKET_BUFFER_SIZE);
    qCDebug(TG_CORE_OUTBOUNDPKT) << "appendInt()" << QString::number(x) << " (" << "0x" + QString::number(x,16) << ")";;
    *m_packetPtr++ = x;
}

void OutboundPkt::appendDouble(double d) {
    Q_ASSERT(m_packetPtr + 2 <= m_packetBuffer + PACKET_BUFFER_SIZE);
    qCDebug(TG_CORE_OUTBOUNDPKT) << "appendInt()" << QString::number(d);
    *(double *)m_packetPtr = d;
    m_packetPtr += 2;
}

void OutboundPkt::appendCString(const char *str, qint32 len) {
    Q_ASSERT(len >= 0 && len < (1 << 24));
    Q_ASSERT((char *) m_packetPtr + len + 8 < (char *) (m_packetBuffer + PACKET_BUFFER_SIZE));
    char *dest = (char *) m_packetPtr;
    if (len < 254) {
        *dest++ = len;
    } else {
        *m_packetPtr = (len << 8) + 0xfe;
        dest += 4;
    }
    memcpy(dest, str, len);
    dest += len;
    while ((qint64) dest & 3) {
        *dest++ = 0;
    }
    m_packetPtr = (qint32 *) dest;
}

void OutboundPkt::appendString (const char *str) {
    appendCString(str, strlen(str));
}

void OutboundPkt::appendQString (const QString &string) {
//    qCDebug(TG_CORE_OUTBOUNDPKT) << "appendQString()" << string;
    QByteArray bytes = string.toUtf8();
    appendCString(bytes.data(), bytes.length());
}

void OutboundPkt::appendRandom(qint32 n) {
    Q_ASSERT(n <= 32);
    static char buf[32];
    Utils::randomBytes(buf, n);
    appendCString(buf, n);
}

void OutboundPkt::appendBool(bool b) {
    qCDebug(TG_CORE_OUTBOUNDPKT) << "appendBool()" << b;
    b ? appendInt(TL_BoolTrue) : appendInt(TL_BoolFalse);
}

void OutboundPkt::appendInputPeer(const InputPeer &peer) {
    appendInt((qint32)peer.classType());
    if (peer.classType() == InputPeer::typeInputPeerContact || peer.classType() == InputPeer::typeInputPeerForeign) {
        appendInt(peer.userId());
        if (peer.classType() == InputPeer::typeInputPeerForeign) {
            appendLong(peer.accessHash());
        }
    } else if (peer.classType() == InputPeer::typeInputPeerChat) {
        appendInt(peer.chatId());
    }
}

void OutboundPkt::appendInputUser(const InputUser &user) {
    appendInt((qint32)user.classType());
    if (user.classType() == InputUser::typeInputUserContact || user.classType() == InputUser::typeInputUserForeign) {
        appendInt(user.userId());
    }
    if (user.classType() == InputUser::typeInputUserForeign) {
        appendLong(user.accessHash());
    }
}

void OutboundPkt::appendInputContact(const InputContact &contact) {
    appendInt((qint32)contact.classType());
    appendLong(contact.clientId());
    appendQString(contact.phone());
    appendQString(contact.firstName());
    appendQString(contact.lastName());
}

void OutboundPkt::appendMessagesFilter(const MessagesFilter &filter) {
    appendInt(filter.classType());
}

void OutboundPkt::appendInputMedia(const InputMedia &media) {
    InputMedia::InputMediaType x = media.classType();
    appendInt(x);
    switch (x) {
    case InputMedia::typeInputMediaEmpty:
        break;
    case InputMedia::typeInputMediaUploadedPhoto:
        appendInputFile(media.file());
        break;
    case InputMedia::typeInputMediaPhoto:
        appendInputPhoto(media.photoId());
        break;
    case InputMedia::typeInputMediaGeoPoint:
        appendInputGeoPoint(media.geoPoint());
        break;
    case InputMedia::typeInputMediaContact:
        appendQString(media.phoneNumber());
        appendQString(media.firstName());
        appendQString(media.lastName());
        break;
    case InputMedia::typeInputMediaUploadedVideo:
        appendInputFile(media.file());
        appendInt(media.duration());
        appendInt(media.w());
        appendInt(media.h());
        appendQString(media.mimeType());
        break;
    case InputMedia::typeInputMediaUploadedThumbVideo:
        appendInputFile(media.file());
        appendInputFile(media.thumb());
        appendInt(media.duration());
        appendInt(media.w());
        appendInt(media.h());
        appendQString(media.mimeType());
        break;
    case InputMedia::typeInputMediaVideo:
        appendInputVideo(media.videoId());
        break;
    case InputMedia::typeInputMediaUploadedAudio:
        appendInputFile(media.file());
        appendInt(media.duration());
        appendQString(media.mimeType());
        break;
    case InputMedia::typeInputMediaAudio:
        appendInputAudio(media.audioId());
        break;
    case InputMedia::typeInputMediaUploadedDocument:
        appendInputFile(media.file());
        appendQString(media.mimeType());
    {
        QList<DocumentAttribute> attrs = media.attributes();
        appendInt(static_cast<qint32>(TL_Vector));
        appendInt(attrs.size());
        Q_FOREACH (DocumentAttribute attr, attrs)
            appendDocumentAttribute(attr);
    }
        break;
    case InputMedia::typeInputMediaUploadedThumbDocument:
        appendInputFile(media.file());
        appendInputFile(media.thumb());
        appendQString(media.mimeType());
    {
        QList<DocumentAttribute> attrs = media.attributes();
        appendInt(static_cast<qint32>(TL_Vector));
        appendInt(attrs.size());
        Q_FOREACH (DocumentAttribute attr, attrs)
            appendDocumentAttribute(attr);
    }
        break;
    case InputMedia::typeInputMediaDocument:
        appendInputDocument(media.documentId());
        break;
    }
}

void OutboundPkt::appendInputFile(const InputFile &file) {
    appendInt(file.classType());
    appendLong(file.id());
    appendInt(file.parts());
    appendQString(file.name());
    if (file.classType() == InputFile::typeInputFile) {
        appendQString(file.md5Checksum());
    }
}

void OutboundPkt::appendInputAudio(const InputAudio &audio) {
    appendInt(audio.classType());
    if (audio.classType() == InputAudio::typeInputAudio) {
        appendLong(audio.id());
        appendLong(audio.accessHash());
    }
}

void OutboundPkt::appendInputVideo(const InputVideo &video) {
    appendInt(video.classType());
    if (video.classType() == InputVideo::typeInputVideo) {
        appendLong(video.id());
        appendLong(video.accessHash());
    }
}

void OutboundPkt::appendInputDocument(const InputDocument &document) {
    appendInt(document.classType());
    if (document.classType() == InputDocument::typeInputDocument) {
        appendLong(document.id());
        appendLong(document.accessHash());
    }
}

void OutboundPkt::appendInputPhoto(const InputPhoto &photo) {
    appendInt(photo.classType());
    if (photo.classType() == InputPhoto::typeInputPhoto) {
        appendLong(photo.id());
        appendLong(photo.accessHash());
    }
}

void OutboundPkt::appendInputGeoPoint(const InputGeoPoint &geo) {
    appendInt(geo.classType());
    if (geo.classType() == InputGeoPoint::typeInputGeoPoint) {
        appendDouble(geo.lat());
        appendDouble(geo.longitude());
    }
}

void OutboundPkt::appendInputFileLocation(const InputFileLocation &location) {
    appendInt(location.classType());
    if (location.classType() == InputFileLocation::typeInputFileLocation) {
        appendLong(location.volumeId());
        appendInt(location.localId());
        appendLong(location.secret());
    } else {
        appendLong(location.id());
        appendLong(location.accessHash());
    }
}

void OutboundPkt::appendInputChatPhoto(const InputChatPhoto &photo) {
    appendInt(photo.classType());
    if (photo.classType() != InputChatPhoto::typeInputChatPhotoEmpty) {
        if (photo.classType() == InputChatPhoto::typeInputChatUploadedPhoto) {
            appendInputFile(photo.file());
        } else {
            appendInputPhoto(photo.id());
        }
        appendInputPhotoCrop(photo.crop());
    }
}

void OutboundPkt::appendInputPhotoCrop(const InputPhotoCrop &crop) {
    appendInt(crop.classType());
    if (crop.classType() == InputPhotoCrop::typeInputPhotoCrop) {
        appendDouble(crop.cropLeft());
        appendDouble(crop.cropTop());
        appendDouble(crop.cropWidth());
    }
}

void OutboundPkt::appendInputNotifyPeer(const InputNotifyPeer &peer) {
    appendInt(peer.classType());
    if (peer.classType() == InputNotifyPeer::typeInputNotifyPeer) {
        appendInputPeer(peer.peer());
    } else if (peer.classType() == InputNotifyPeer::typeInputNotifyGeoChatPeer) {
        appendInputGeoChat(peer.geoChatPeer());
    }
}

void OutboundPkt::appendInputGeoChat(const InputGeoChat &geoChat) {
    appendInt(geoChat.classType());
    appendInt(geoChat.chatId());
    appendLong(geoChat.accessHash());
}

void OutboundPkt::appendInputPeerNotifySettings(const InputPeerNotifySettings &settings) {
    appendInt(settings.classType());
    appendInt(settings.muteUntil());
    appendQString(settings.sound());
    appendBool(settings.showPreviews());
    appendInt(settings.eventsMask());
}

void OutboundPkt::appendInputEncryptedChat(const InputEncryptedChat &inputEncryptedChat) {
    appendInt(inputEncryptedChat.classType());
    appendInt(inputEncryptedChat.chatId());
    appendLong(inputEncryptedChat.accessHash());
}

void OutboundPkt::appendInputEncryptedFile(const InputEncryptedFile &file) {
    appendInt(file.classType());
    if (file.classType() != InputEncryptedFile::typeInputEncryptedFileEmpty) {
        appendLong(file.id());
        if (file.classType() == InputEncryptedFile::typeInputEncryptedFile) {
            appendLong(file.accessHash());
        } else {
            appendInt(file.parts());
            if (file.classType() == InputEncryptedFile::typeInputEncryptedFileUploaded) {
                /*intentionally left commented. If in future wanted to send md5 checksum this line
                 should be used */
                //appendQString(file.md5Checksum());
                //TODO while not sending file md5, we must send that parameter as empty string
                appendQString("");
            }
            appendInt(file.keyFingerprint());
        }
    }
}

void OutboundPkt::appendSendMessageAction(const SendMessageAction &action) {
    appendInt(action.classType());
}

void OutboundPkt::appendDocumentAttribute(const DocumentAttribute &attr)
{
    appendInt(attr.classType());
    switch(static_cast<int>(attr.classType()))
    {
    case DocumentAttribute::typeAttributeAudio:
        appendInt(attr.duration());
        break;
    case DocumentAttribute::typeAttributeFilename:
        appendQString(attr.filename());
        break;
    case DocumentAttribute::typeAttributeImageSize:
        appendInt(attr.w());
        appendInt(attr.h());
        break;
    case DocumentAttribute::typeAttributeSticker:
        appendQString(attr.alt());
        break;
    case DocumentAttribute::typeAttributeVideo:
        appendInt(attr.duration());
        appendInt(attr.w());
        appendInt(attr.h());
        break;
    }
}

void OutboundPkt::appendInputPrivacyKey(const InputPrivacyKey &key)
{
    appendInt(key.classType());
}

void OutboundPkt::appendInputPrivacyRule(const InputPrivacyRule &rule)
{
    appendInt(rule.classType());

    QList<qint32> users = rule.users();
    appendInt(static_cast<qint32>(TL_Vector));
    appendInt(users.size());
    Q_FOREACH (qint32 user, users)
        appendInt(user);
}

void OutboundPkt::appendAccountDaysTTL(const AccountDaysTTL &ttl)
{
    appendInt(ttl.classType());
    appendInt(ttl.days());
}

void OutboundPkt::initConnection() {
    appendInt(TL_InvokeWithLayer25);
    appendInt(LAYER);
    appendInt(TL_InitConnection);
    appendInt(mSettings->appId());
    appendQString(Utils::getDeviceModel());
    appendQString(Utils::getSystemVersion());
    appendQString(Utils::getAppVersion());
    appendQString(mSettings->langCode());
}
