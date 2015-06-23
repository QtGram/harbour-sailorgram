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

#ifndef ENCRYPTER_H
#define ENCRYPTER_H

#include "core/outboundpkt.h"
#include "types/decryptedmessage.h"
#include "secret/secretchat.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(TG_SECRET_ENCRYPTER)

class Encrypter : public OutboundPkt
{
public:
    explicit Encrypter(Settings *settings);

    void setSecretChat(SecretChat *secretChat);

    QByteArray generateEncryptedData(const DecryptedMessage &decryptedMessage);

protected:
    qint32 *mEncrExtra;
    qint32 *mEncrPtr;
    qint32 *mEncrStart;
    qint32 *mEncrEnd;
    SecretChat *mSecretChat;

    void startEncryption();
    void endEncryption();
    qint32 *encryptDecryptedMessage();
    QByteArray getGeneratedBytes();

    void appendDecryptedMessage(const DecryptedMessage &decryptedMessage);
    void appendDecryptedMessageMedia(const DecryptedMessageMedia &media);
    void appendDecryptedMessageAction(const DecryptedMessageAction &action);
    void appendSendMessageAction(const SendMessageAction &sendMessageAction);
};

#endif // ENCRYPTER_H
