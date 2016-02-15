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

#ifndef SESSION_H
#define SESSION_H

#include "dc.h"
#include "connection.h"
#include "query.h"
#include "telegram/types/updatestype.h"
#include <QMap>
#include <QLoggingCategory>

#include "eventtimer.h"

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_SESSION)



class Session : public Connection
{
    Q_OBJECT
public:
    explicit Session(DC *dc, Settings *settings, CryptoUtils *crypto, QObject *parent = 0);
    ~Session();

    DC *dc();
    qint64 sendQuery(OutboundPkt &outboundPkt, QueryMethods *methods, const QVariant &extra = QVariant(), const QString &name = QString());

    inline qint64 sessionId() { return m_sessionId; }
    inline bool initConnectionNeeded() { return m_initConnectionNeeded; }
    inline void setInitConnectionNeeded(bool initConnectionNeeded) { m_initConnectionNeeded = initConnectionNeeded; }
    inline qint64 clientLastMsgId() { return m_clientLastMsgId; }
    inline qint32 seqNo() { return m_seqNo; }
    inline void setSeqNo(qint32 seqNo) { m_seqNo = seqNo; }

    void release();
    void close();

Q_SIGNALS:
    void sessionReady(DC *dc);
    void sessionReleased(qint64 sessionId);
    void sessionClosed(qint64 sessionId);

    void resultReceived(Query *q, InboundPkt &inboundPkt);
    void errorReceived(Query *q, qint32 errorCode, QString errorText);
    void updateMessageId(qint64 badMsgId, qint64 newMsgId);

    void updatesTooLong();
    void updateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShort(const Update &update, qint32 date);
    void updatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    void updates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

private:
    struct EncryptedMsg {
        // unencrypted header
        qint64 authKeyId;
        char msgKey[16];
        // encrypted part, starts with encrypted header
        qint64 serverSalt;
        qint64 sessionId;
        // first message follows
        qint64 msgId;
        qint32 seqNo;
        qint32 msgLen;   // divisible by 4
        qint32 message[MAX_MESSAGE_INTS];
    };

    Settings *mSettings;
    CryptoUtils *mCrypto;

    // session members
    qint64 m_sessionId;
    qint64 m_serverSalt;
    qint32 mTimeDifference; // difference between client and server time
    static qint64 m_clientLastMsgId;
    qint32 m_seqNo;

    DC *m_dc;
    QMap<qint64, EventTimer *> m_pendingAcks; // once received response, its msgId is here stored as pending to be acknowledged
    QMap<qint64, Query *> m_pendingQueries; // map with the msgIds of the queries sent but not received its ack yet.

    // ack responses
    void addToPendingAcks(qint64 msgId);
    void ackAll();
    void sendAcks(const QList<qint64> &msgIds);


    // Additional flags:
    // flag to see if initConnection() method must be sent before next output rpc query
    // (see https://core.telegram.org/api/invoking#saving-client-info)
    // This is needed at first operation against a new dc (and at beginning)
    // (the flag is set to true when just connected to dc until initConnection() operation is executed against this server)
    bool m_initConnectionNeeded;

    // connected signal management
    void processConnected();

    // process and transform response message
    void processRpcMessage(InboundPkt &inboundPkt);

    // execute rpc answer operation requested
    void rpcExecuteAnswer(InboundPkt &inboundPkt, qint64 msgId);

    void workUpdate(InboundPkt &inboundPkt, qint64 msgId);
    void workContainer (InboundPkt &inboundPkt, qint64 msgId);
    void workNewSessionCreated(InboundPkt &inboundPkt, qint64 msgId);
    void workMsgsAck(InboundPkt &inboundPkt, qint64 msgId);
    void workRpcResult(InboundPkt &inboundPkt, qint64 msgId);
    void workUpdateShort(InboundPkt &inboundPkt, qint64 msgId);
    void workUpdates(InboundPkt &inboundPkt, qint64 msgId);
    void workUpdatesCombined(InboundPkt &inboundPkt, qint64 msgId);
    void workUpdateShortMessage(InboundPkt &inboundPkt, qint64 msgId);
    void workUpdateShortChatMessage(InboundPkt &inboundPkt, qint64 msgId);
    void workPacked(InboundPkt &inboundPkt, qint64 msgId);
    void workBadServerSalt(InboundPkt &inboundPkt, qint64 msgId);
    void workPong(InboundPkt &inboundPkt, qint64 msgId);
    void workDetailedInfo(InboundPkt &inboundPkt, qint64 msgId);
    void workNewDetailedInfo(InboundPkt &inboundPkt, qint64 msgId);
    void workUpdatesTooLong(InboundPkt &inboundPkt, qint64 msgId);
    void workBadMsgNotification(InboundPkt &inboundPkt, qint64 msgId);

    // encrypt message and send
    EncryptedMsg *initEncryptedMessage(qint32 useful);
    qint64 encryptSendMessage(qint32 *msg, qint32 msgInts, qint32 useful);
    qint32 aesEncryptMessage(EncryptedMsg *encMsg);
    bool rpcSendMessage(void *data, qint32 len);
    qint64 generatePlainNextMsgId();
    qint64 generateNextMsgId();

    // query results
    void queryOnResult(InboundPkt &inboundPkt, qint64 msgId);
    void queryOnError(InboundPkt &inboundPkt, qint64 msgId);

    qint64 recomposeAndSendQuery(Query *q);

protected Q_SLOTS:
    void processRpcAnswer(QByteArray response);

private Q_SLOTS:
    void resendQuery(Query *q);
    void onDisconnected();
    void ack(qint64 msgId);
};

#endif // SESSION_H
