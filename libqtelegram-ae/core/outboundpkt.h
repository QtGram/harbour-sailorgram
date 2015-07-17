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

#ifndef OUTBOUNDPKT_H
#define OUTBOUNDPKT_H

#include <QObject>
#include <QByteArray>
#include <QLoggingCategory>
#include <bn.h>
#include "util/constants.h"
#include "settings.h"
#include "types/inputpeer.h"
#include "types/inputuser.h"
#include "types/inputcontact.h"
#include "types/messagesfilter.h"
#include "types/inputmedia.h"
#include "types/inputfile.h"
#include "types/inputphoto.h"
#include "types/inputgeopoint.h"
#include "types/inputvideo.h"
#include "types/inputaudio.h"
#include "types/inputdocument.h"
#include "types/inputfilelocation.h"
#include "types/inputchatphoto.h"
#include "types/inputphotocrop.h"
#include "types/inputnotifypeer.h"
#include "types/inputpeernotifysettings.h"
#include "types/inputgeochat.h"
#include "types/inputencryptedchat.h"
#include "types/inputencryptedfile.h"
#include "types/inputprivacykey.h"
#include "types/inputprivacyrule.h"
#include "types/accountdaysttl.h"

#include <QSharedPointer>

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_OUTBOUNDPKT)

class OutboundPkt
{
public:
    explicit OutboundPkt(Settings *settings);
    virtual ~OutboundPkt();

    void clearPacket();
    void forwardPtr(qint32 positions);
    void initConnection();

    void appendInts(const qint32 *what, qint32 len);
    void appendInt(qint32 x);
    void appendLong(qint64 x);
    void appendDouble(double d);
    void appendData(const void *data, qint32 len);
    void appendBytes(const QByteArray &byteArray);
    void appendBignum(BIGNUM *n);
    void appendCString(const char *str, qint32 len);
    void appendString(const char *str);
    void appendQString (const QString &string);
    void appendRandom(qint32 n);
    void appendBool(bool b);

    void appendInputPeer(const InputPeer &peer);
    void appendInputUser(const InputUser &user);
    void appendInputContact(const InputContact &contact);
    void appendMessagesFilter(const MessagesFilter &filter);
    void appendInputMedia(const InputMedia &media);
    void appendInputFileLocation(const InputFileLocation &location);
    void appendInputChatPhoto(const InputChatPhoto &photo);
    void appendInputNotifyPeer(const InputNotifyPeer &peer);
    void appendInputPeerNotifySettings(const InputPeerNotifySettings &settings);
    void appendInputFile(const InputFile &file);
    void appendInputGeoPoint(const InputGeoPoint &geo);
    void appendInputPhotoCrop(const InputPhotoCrop &crop);
    void appendInputPhoto(const InputPhoto &photo);
    void appendInputEncryptedChat(const InputEncryptedChat &inputEncryptedChat);
    void appendInputEncryptedFile(const InputEncryptedFile &inputEncryptedFile);
    void appendSendMessageAction(const SendMessageAction &action);
    void appendDocumentAttribute(const DocumentAttribute &attr);
    void appendInputPrivacyKey(const InputPrivacyKey &key);
    void appendInputPrivacyRule(const InputPrivacyRule &rule);
    void appendAccountDaysTTL(const AccountDaysTTL &ttl);

    qint32 length() const;
    qint32 *buffer();

protected:
    qint32 *m_packetPtr;
    QSharedPointer<qint32> __buffer;
    qint32 *m_packetBuffer;

    void appendInputAudio(const InputAudio &audio);
    void appendInputVideo(const InputVideo &video);
    void appendInputDocument(const InputDocument &document);
    void appendInputGeoChat(const InputGeoChat &geoChat);

private:
    Settings *mSettings;
};

#endif // OUTBOUNDPKT_H
