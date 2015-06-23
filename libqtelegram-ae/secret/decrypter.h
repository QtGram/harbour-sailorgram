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

#ifndef DECRYPTER_H
#define DECRYPTER_H

#include "core/inboundpkt.h"
#include <QByteArray>
#include "types/decryptedmessage.h"
#include "types/encryptedmessage.h"

#include <QSharedPointer>
#include <QPair>

Q_DECLARE_LOGGING_CATEGORY(TG_SECRET_DECRYPTER)

class Decrypter : public QObject, public InboundPkt
{
    Q_OBJECT
public:
    explicit Decrypter(Settings *settings);
    ~Decrypter();

    void setSecretChat(SecretChat *secretChat);

    DecryptedMessage decryptEncryptedData(qint64 randomId, const QByteArray &bytes);

Q_SIGNALS:
    void sequenceNumberGap(qint32 chatId, qint32 startSeqNo, qint32 endSeqNo);

protected:
    Settings *mSettings;
    SecretChat *mSecretChat;
    QSharedPointer<char> mBufferPtr;

    bool checkKeyFingerprints();
    QByteArray decryptEncryptedMessage();

    DecryptedMessage fetchDecryptedMessage();
    DecryptedMessageMedia fetchDecryptedMessageMedia();
    DecryptedMessageAction fetchDecryptedMessageAction();
    SendMessageAction fetchSendMessageAction();
};

#endif // DECRYPTER_H
