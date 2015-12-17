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

#ifndef DCAUTH_H
#define DCAUTH_H

#include "dc.h"
#include "connection.h"
#include "util/asserter.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_DCAUTH)

class DCAuth : public Connection
{
    Q_OBJECT
public:
    explicit DCAuth(DC *dc, Settings *settings, CryptoUtils *crypto, QObject *parent = 0);
    ~DCAuth();

    void createAuthKey();
    DC *dc();

Q_SIGNALS:
    void dcReady(DC *dc);

private:
    struct UnencryptedMsgHeader{
        qint64 authKeyId;
        qint64 outMsgId;
        qint32 msgLength;
    };

    char m_nonce[16];
    char m_newNonce[32];
    char m_serverNonce[16];

    Settings *mSettings;
    CryptoUtils *mCrypto;
    DC *m_dc;

    // dc auth methodsDCAuth
    void sendReqPQPacket();
    void processResPQAnswer (const InboundPkt &inboundPkt);
    void processDHAnswer(InboundPkt &inboundPkt);
    void processAuthComplete(InboundPkt &inboundPkt);
    // generic send and receive method
    void rpcSendPacket(OutboundPkt &outboundPkt);

    void processConnected();

protected Q_SLOTS:
    void processRpcAnswer(QByteArray response);
};

#endif // DCAUTH_H
