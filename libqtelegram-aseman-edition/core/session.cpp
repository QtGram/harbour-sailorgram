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

#include "session.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <QDateTime>
#include "util/tlvalues.h"
#include "telegram/coretypes.h"

Q_LOGGING_CATEGORY(TG_CORE_SESSION, "tg.core.session")

qint64 Session::m_clientLastMsgId = 0;


Session::Session(DC *dc, Settings *settings, CryptoUtils *crypto, QObject *parent) :
    Connection(dc->host(), dc->port(), parent),
    mSettings(settings),
    mCrypto(crypto),
    m_sessionId(0),
    m_serverSalt(0),
    mTimeDifference(0),
    m_seqNo(0),
    m_dc(dc),
    m_initConnectionNeeded(true) {
    // ensure dc has, at least, the shared key created
    Q_ASSERT(dc->state() >= DC::authKeyCreated);
    // copy calculated values for timeDifference and serverSalt when created shared key.
    // This copy is needed because we could have several sessions against same dc with different values
    mTimeDifference = m_dc->timeDifference();
    m_serverSalt = m_dc->serverSalt();
    // create session id
    RAND_pseudo_bytes((uchar *) &m_sessionId, 8);
    qCDebug(TG_CORE_SESSION) << "created session with id" << QString::number(m_sessionId, 16);
}

Session::~Session() {
    qCDebug(TG_CORE_SESSION) << "destroyed session with id" << QString::number(m_sessionId, 16);
}

void Session::close() {
    if (this->state() != QAbstractSocket::UnconnectedState) {
        connect(this, &QAbstractSocket::disconnected, this, &Session::onDisconnected);
        this->disconnectFromHost();
    } else {
        Q_EMIT sessionClosed(m_sessionId);
    }
}

void Session::onDisconnected() {
    if(error() == QAbstractSocket::RemoteHostClosedError) {
        return; // Trying to reconnect...
    }

    Q_EMIT sessionClosed(m_sessionId);
}

void Session::release() {
    Q_EMIT sessionReleased(m_sessionId);
}

DC *Session::dc() {
    return m_dc;
}

qint64 Session::generatePlainNextMsgId() {
    return (qint64) ((QDateTime::currentDateTime().toTime_t() - mTimeDifference) * (1LL << 32)) & -4;
}

qint64 Session::generateNextMsgId() {
    qint64 nextId = generatePlainNextMsgId();
    if (nextId <= m_clientLastMsgId) {
        nextId = m_clientLastMsgId += 4;
    } else {
        m_clientLastMsgId = nextId;
    }
    return nextId;
}

void Session::processConnected() {
    // ack all pending server response messages. This happens when socket has been connected
    // previously and after reconnection some acks are pending to be sent.
    ackAll();
    Q_EMIT sessionReady(m_dc);
}

void Session::processRpcAnswer(QByteArray response) {
    qint32 len = response.length();

    qCDebug(TG_CORE_SESSION) << "connection #" << socketDescriptor() << "received rpc answer with" << len << "content bytes by session" << QString::number(m_sessionId, 16);

    InboundPkt p(response.data(), len);
    processRpcMessage(p);
}

void Session::processRpcMessage(InboundPkt &inboundPkt) {

    EncryptedMsg *enc = (EncryptedMsg *)inboundPkt.buffer();
    qint32 len = inboundPkt.length();

    const qint32 MINSZ = offsetof (EncryptedMsg, message);
    const qint32 UNENCSZ = offsetof (EncryptedMsg, serverSalt);
    qCDebug(TG_CORE_SESSION) << "processRpcMessage(), len=" << len;

    if(len < MINSZ || (len & 15) != (UNENCSZ & 15))
        return;

    Q_ASSERT(m_dc->authKeyId());
    mAsserter.check(enc->authKeyId == m_dc->authKeyId());
    //msg_key is used to compute AES key and to decrypt the received message
    mCrypto->initAESAuth(m_dc->authKey() + 8, enc->msgKey, AES_DECRYPT);
    qint32 l = mCrypto->padAESDecrypt((char *)&enc->serverSalt, len - UNENCSZ, (char *)&enc->serverSalt, len - UNENCSZ);
    Q_UNUSED(l);
    Q_ASSERT(l == len - UNENCSZ);
    if( !(!(enc->msgLen & 3) && enc->msgLen > 0 && enc->msgLen <= len - MINSZ && len - MINSZ - enc->msgLen <= 12) )
        return;

    //check msg_key is indeed equal to SHA1 of the plaintext obtained after decription (without final padding bytes).
    static uchar sha1Buffer[20];
    SHA1((uchar *)&enc->serverSalt, enc->msgLen + (MINSZ - UNENCSZ), sha1Buffer);
    if(memcmp (&enc->msgKey, sha1Buffer + 4, 16))
        return;

    if (m_dc->serverSalt() != enc->serverSalt) {
        m_dc->setServerSalt(enc->serverSalt);
    }

    // check time synchronization
    qint32 serverTime = enc->msgId >> 32LL;
    qint32 clientTime = QDateTime::currentDateTime().toTime_t() - mTimeDifference;
    if (clientTime <= serverTime - 30 || clientTime >= serverTime + 300) {
        qCDebug(TG_CORE_SESSION) << "salt =" << enc->serverSalt << ", sessionId =" << QString::number(enc->sessionId, 16) << ", msgId =" << QString::number(enc->msgId, 16) << ", seqNo =" << enc->seqNo << ", serverTime =" << serverTime << ", clientTime =" << clientTime;
        QString alert("Received message has too large difference between client and server dates - ");
        if (clientTime <= serverTime -30) {
            alert.append("the message has a date at least 30 seconds later in time than current date");
        } else {
            alert.append("the message was sent at least 300 seconds ago");
        }
        qCWarning(TG_CORE_SESSION) << alert;
    }

    inboundPkt.setInPtr(enc->message);
    inboundPkt.setInEnd(inboundPkt.inPtr() + (enc->msgLen / 4));

    qCDebug(TG_CORE_SESSION) << "received message id" << QString::number(enc->msgId, 16);

    Q_ASSERT(l >= (MINSZ - UNENCSZ) + 8);

    if (enc->msgId & 1) {
        addToPendingAcks(enc->msgId);
    }

    mAsserter.check(m_sessionId == enc->sessionId);
    rpcExecuteAnswer(inboundPkt, enc->msgId);
    mAsserter.check(inboundPkt.inPtr() == inboundPkt.inEnd());
}

void Session::rpcExecuteAnswer(InboundPkt &inboundPkt, qint64 msgId) {
    qint32 op = inboundPkt.prefetchInt();
    qCDebug(TG_CORE_SESSION) << "rpcExecuteAnswer(), op =" << QString::number(op, 16);
    switch (op) {
    case TL_MsgContainer:
        workContainer(inboundPkt, msgId);
        return;
    case TL_NewSessionCreated:
        workNewSessionCreated(inboundPkt, msgId);
        return;
    case TL_MsgsAck:
        workMsgsAck(inboundPkt, msgId);
        return;
    case TL_RpcResult:
        workRpcResult(inboundPkt, msgId);
        return;
    case UpdatesType::typeUpdateShort:
        workUpdateShort(inboundPkt, msgId);
        return;
    case UpdatesType::typeUpdatesCombined:
        workUpdatesCombined(inboundPkt, msgId);
    case UpdatesType::typeUpdates:
        workUpdates(inboundPkt, msgId);
        return;
    case UpdatesType::typeUpdateShortMessage:
        workUpdateShortMessage(inboundPkt, msgId);
        return;
    case UpdatesType::typeUpdateShortChatMessage:
        workUpdateShortChatMessage(inboundPkt, msgId);
        return;
    case TL_GZipPacked:
        workPacked(inboundPkt, msgId);
        return;
    case TL_BadServerSalt:
        workBadServerSalt(inboundPkt, msgId);
        return;
    case TL_Pong:
        workPong(inboundPkt, msgId);
        return;
    case TL_MsgDetailedInfo:
        workDetailedInfo(inboundPkt, msgId);
        return;
    case TL_MsgNewDetailedInfo:
        workNewDetailedInfo(inboundPkt, msgId);
        return;
    case UpdatesType::typeUpdatesTooLong:
        workUpdatesTooLong(inboundPkt, msgId);
        return;
    case TL_BadMsgNotification:
        workBadMsgNotification(inboundPkt, msgId);
        return;
    }
    qCWarning(TG_CORE_SESSION) << "Unknown rpc response message";
    inboundPkt.setInPtr(inboundPkt.inEnd());
}

void Session::workContainer (InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workContainer: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == TL_MsgContainer);
    qint32 n = inboundPkt.fetchInt();
    for (qint32 i = 0; i < n; i++) { // message
        qint64 id = inboundPkt.fetchLong (); // msg_id
        if (id & 1) {
           addToPendingAcks(id);
        }
        inboundPkt.fetchInt (); // seq_no
        qint32 bytes = inboundPkt.fetchInt ();
        qint32 *t = inboundPkt.inEnd();
        inboundPkt.setInEnd(inboundPkt.inPtr() + (bytes / 4));
        rpcExecuteAnswer(inboundPkt, id);
        Q_ASSERT (inboundPkt.inPtr() == inboundPkt.inEnd());
        inboundPkt.setInEnd(t);
    }
}

void Session::workNewSessionCreated(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workNewSessionCreated: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_NewSessionCreated);
    inboundPkt.fetchLong(); // first_msg_id; //XXX set is as m_clientLastMsgId??
    inboundPkt.fetchLong (); // unique_id
    m_dc->setServerSalt(inboundPkt.fetchLong()); // server_salt
}

void Session::workMsgsAck(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workMsgsAck: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_MsgsAck);
    mAsserter.check(inboundPkt.fetchInt () == (qint32)CoreTypes::typeVector);
    qint32 n = inboundPkt.fetchInt();
    for (qint32 i = 0; i < n; i++) {
        qint64 id = inboundPkt.fetchLong ();
        Query *q = m_pendingQueries.value(id);
        if(!q)
            return;

        Q_ASSERT(q);
        q->setAcked(true);
    }
}

void Session::workRpcResult(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workRpcResult: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_RpcResult);
    qint64 id = inboundPkt.fetchLong();
    qint32 op = inboundPkt.prefetchInt();
    if (op == (qint32)TL_RpcError) {
        queryOnError(inboundPkt, id);
    } else {
        queryOnResult(inboundPkt, id);
    }
}

void Session::workUpdateShort(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workUpdateShort: msgId =" << QString::number(msgId, 16);
    UpdatesType upd(&inboundPkt);
    Q_EMIT updates(upd);
}

void Session::workUpdatesCombined(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workUpdatesCombined: msgId =" << QString::number(msgId, 16);
    UpdatesType upd(&inboundPkt);
    Q_EMIT updates(upd);
}

void Session::workUpdates(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workUpdates: msgId =" << QString::number(msgId, 16);
    UpdatesType upd(&inboundPkt);
    Q_EMIT updates(upd);
}

void Session::workUpdateShortMessage(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workUpdateShortMessage: msgId =" << QString::number(msgId, 16);
    Q_UNUSED(msgId)
    UpdatesType upd(&inboundPkt);
    Q_EMIT updates(upd);
}

void Session::workUpdateShortChatMessage(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workUpdateShortChatMessage: msgId =" << QString::number(msgId, 16);
    Q_UNUSED(msgId)
    UpdatesType upd(&inboundPkt);
    Q_EMIT updates(upd);
}

void Session::workPacked(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workPacked: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_GZipPacked);
    static qint32 buf[MAX_PACKED_SIZE >> 2];
    qint32 l = inboundPkt.prefetchStrlen();
    char *s = inboundPkt.fetchStr(l);

    qint32 totalOut = Utils::tinflate(s, l, buf, MAX_PACKED_SIZE);
    qint32 *inPtr = inboundPkt.inPtr();
    qint32 *inEnd = inboundPkt.inEnd();
    inboundPkt.setInPtr(buf);
    inboundPkt.setInEnd(inboundPkt.inPtr() + totalOut / 4);
    qCDebug(TG_CORE_SESSION) << "Unzipped data";
    rpcExecuteAnswer(inboundPkt, msgId);
    inboundPkt.setInPtr(inPtr); //TODO Not sure about this operations of setting inPtr and inEnd after execute answer completion
    inboundPkt.setInEnd(inEnd);
}

void Session::workBadServerSalt(InboundPkt &inboundPkt, qint64 msgId) {
    Q_UNUSED(msgId)
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_BadServerSalt);
    qint64 badMsgId = inboundPkt.fetchLong();
    qint32 badMsgSeqNo = inboundPkt.fetchInt();
    qint32 errorCode = inboundPkt.fetchInt();
    qCDebug(TG_CORE_SESSION) << "workBadServerSalt: badMsgId =" << QString::number(badMsgId, 16)
            << ", badMsgSeqNo =" << badMsgSeqNo << ", errorCode =" << errorCode;
    m_dc->setServerSalt(inboundPkt.fetchLong()); // new server_salt
    Query *q = m_pendingQueries.take(badMsgId);
    qint64 newMsgId = recomposeAndSendQuery(q);
    if (newMsgId != 0) {
        Q_EMIT updateMessageId(badMsgId, newMsgId);
    }
}

void Session::workPong(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workPong: msgId =" << QString::number(msgId, 16);
    mAsserter.check (inboundPkt.fetchInt() == (qint32)TL_Pong);
    inboundPkt.fetchLong(); // msg_id
    inboundPkt.fetchLong(); // ping_id
}

void Session::workDetailedInfo(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workDetailedInfo: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_MsgDetailedInfo);
    inboundPkt.fetchLong(); // msg_id
    inboundPkt.fetchLong(); // answer_msg_id
    inboundPkt.fetchInt(); // bytes
    inboundPkt.fetchInt(); // status
}

void Session::workNewDetailedInfo(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workNewDetailedInfo: msgId =" << QString::number(msgId, 16);
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_MsgNewDetailedInfo);
    inboundPkt.fetchLong(); // answer_msg_id
    inboundPkt.fetchInt(); // bytes
    inboundPkt.fetchInt(); // status
}

void Session::workUpdatesTooLong(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "workUpdatesTooLong: msgId =" << QString::number(msgId, 16);
    UpdatesType upd(&inboundPkt);
    Q_EMIT updates(upd);
}

void Session::workBadMsgNotification(InboundPkt &inboundPkt, qint64 msgId) {
    mAsserter.check(inboundPkt.fetchInt() == (qint32)TL_BadMsgNotification);
    qint64 badMsgId = inboundPkt.fetchLong();
    qint32 badMsgSeqNo = inboundPkt.fetchInt();
    qint32 errorCode = inboundPkt.fetchInt();
    qCWarning(TG_CORE_SESSION) << "workBadMsgNotification: badMsgId =" << QString::number(badMsgId, 16) <<
            ", badMsgSeqNo =" << badMsgSeqNo << ", errorCode =" << errorCode;
    switch (errorCode) {
    case 16:
    case 17:
    case 19:
    case 32:
    case 33:
    case 64:
        // update time sync difference and reset msgIds counter
        qint32 serverTime = msgId >> 32LL;
        mTimeDifference = QDateTime::currentDateTime().toTime_t() - serverTime;

        qint64 nextId = generatePlainNextMsgId();
        if (!m_pendingQueries.contains(nextId)) {
            m_clientLastMsgId = 0;
        }

        // read removing from pending queries, recompose and send the last query
        Query *q = m_pendingQueries.take(badMsgId);
        qint64 newMsgId = recomposeAndSendQuery(q);
        if (newMsgId != 0) {
            Q_EMIT updateMessageId(badMsgId, newMsgId);
        }
        break;
    }
}

Session::EncryptedMsg *Session::initEncryptedMessage(qint32 useful) {
    EncryptedMsg *encMsg = new EncryptedMsg;
    Q_ASSERT(m_dc->authKeyId());
    encMsg->authKeyId = m_dc->authKeyId();
    Q_ASSERT(m_dc->serverSalt());
    encMsg->serverSalt = m_dc->serverSalt();
    Q_ASSERT(m_sessionId);
    encMsg->sessionId = m_sessionId;
    encMsg->msgId = generateNextMsgId();
    encMsg->seqNo = m_seqNo;
    if (useful) {
        encMsg->seqNo |= 1;
    }
    m_seqNo += 2;
    return encMsg;
}

qint32 Session::aesEncryptMessage (EncryptedMsg *encMsg) {
    uchar sha1Buffer[20];
    const qint32 MINSZ = offsetof (EncryptedMsg, message);
    const qint32 UNENCSZ = offsetof (EncryptedMsg, serverSalt);
    qint32 encLen = (MINSZ - UNENCSZ) + encMsg->msgLen;
    Q_ASSERT (encMsg->msgLen >= 0 && encMsg->msgLen <= MAX_MESSAGE_INTS * 4 - 16 && !(encMsg->msgLen & 3));
    SHA1 ((uchar *) &encMsg->serverSalt, encLen, sha1Buffer);
    qCDebug(TG_CORE_SESSION) << "sending message with sha1" << QString::number(*(qint32 *)sha1Buffer, 8);

    memcpy (encMsg->msgKey, sha1Buffer + 4, 16);
    mCrypto->initAESAuth(m_dc->authKey(), encMsg->msgKey, AES_ENCRYPT);
    return mCrypto->padAESEncrypt((char *) &encMsg->serverSalt, encLen, (char *) &encMsg->serverSalt, MAX_MESSAGE_INTS * 4 + (MINSZ - UNENCSZ));
}

qint64 Session::encryptSendMessage(qint32 *msg, qint32 msgInts, qint32 useful) {
    const qint32 UNENCSZ = offsetof (EncryptedMsg, serverSalt);
    if (msgInts <= 0 || msgInts > MAX_MESSAGE_INTS - 4) {
      return -1;
    }
    EncryptedMsg *encMsg = initEncryptedMessage(useful);
    if (msg) {
      memcpy (encMsg->message, msg, msgInts * 4);
      encMsg->msgLen = msgInts * 4;
    } else if ((encMsg->msgLen & 0x80000003) || encMsg->msgLen > MAX_MESSAGE_INTS * 4 - 16) {
      delete encMsg;
      return -1;
    }

    qint32 l = aesEncryptMessage(encMsg);
    Q_ASSERT(l > 0);

    if(!rpcSendMessage(encMsg, l + UNENCSZ))
        return -1;

    delete encMsg;
    return m_clientLastMsgId;
}

bool Session::rpcSendMessage(void *data, qint32 len) {
    qCDebug(TG_CORE_SESSION) << "rpcSendMessage()," << len;

    qint32 written;
    Q_UNUSED(written);

    mAsserter.check(len > 0 && !(len & 0xfc000003));
    qint32 totalLen = len >> 2;

    if (totalLen < 0x7f) {
        written = writeOut(&totalLen, 1);
        if(written != 1)
            return false;
    } else {
        totalLen = (totalLen << 8) | 0x7f;
        written = writeOut(&totalLen, 4);
        if(written != 4)
            return false;
    }

    written = writeOut(data, len);
    if(written != len)
        return false;

    return true;
}


//### query management
qint64 Session::sendQuery(OutboundPkt &outboundPkt, QueryMethods *methods, const QVariant &extra, const QString &name) {
    Q_ASSERT (m_sessionId);
    Q_ASSERT (m_dc->authKeyId());
    qint32 *data = outboundPkt.buffer();
    qint32 ints = outboundPkt.length();

    // prepend init connection header to outboundPkt if initConnectionNeeded
    // Note: 'wrap' needs to be declare outside of if to persist until encriptSendMessage is completed
    OutboundPkt wrap(mSettings);
    if (m_initConnectionNeeded) {
        wrap.initConnection();
        wrap.appendOutboundPkt(outboundPkt);
        data = wrap.buffer();
        ints = wrap.length();
        m_initConnectionNeeded = false;
    }

    qCDebug(TG_CORE_SESSION) << "Sending query of size" << 4 * ints << "to DC" << m_dc->id() << "at" << peerName() << ":" << peerPort() << "by session" << QString::number(m_sessionId, 16);

    Query *q = new Query(this);
    q->setData(data, ints);
    q->setMsgId(encryptSendMessage(data, ints, 1));
    q->setMainMsgId(q->msgId());
    q->setSeqNo(m_seqNo - 1);
    qCDebug(TG_CORE_SESSION) << "msgId is" << QString::number(q->msgId(), 16);
    q->setMethods(methods);
    q->setAcked(false);
    q->setExtra(extra);
    q->setName(name);

    if (mSettings->resendQueries()) {
        connect(q, &Query::timeout, this, &Session::resendQuery, Qt::UniqueConnection);
        q->startTimer(QUERY_TIMEOUT);
    }

    m_pendingQueries.insert(q->msgId(), q);

    return q->msgId();
}

qint64 Session::recomposeAndSendQuery(Query *q) {
    if(!q)
        return 0;

    qCDebug(TG_CORE_SESSION) << "Resending query with previous msgId" << QString::number(q->msgId(), 16);
    q->setMsgId(encryptSendMessage((qint32 *)q->data(), q->dataLength(), 1));
    q->setSeqNo(m_seqNo - 1);
    qCDebug(TG_CORE_SESSION) << "new msgId is" << QString::number(q->msgId(), 16);
    q->setAcked(false);
    m_pendingQueries.insert(q->msgId(), q);
    return q->msgId();
}

void Session::resendQuery(Query *q) {
    Q_ASSERT(q);
    //avoid resending if resend numbers is less than zero
    if (q->decreaseResends() < 0) {
        qCDebug(TG_CORE_SESSION) << "Max resend numbers reached for query with msgId" << QString::number(q->msgId(), 16) << ",query discarded";
        m_pendingQueries.remove(q->msgId());
        delete q;
    } else {
        qCDebug(TG_CORE_SESSION) << "Resending query with msgId" << QString::number(q->msgId(), 16);
        OutboundPkt p(mSettings);
        p.appendInt(TL_MsgContainer);
        p.appendInt(1);
        p.appendLong(q->msgId());
        p.appendInt(q->seqNo());
        p.appendInt(4 * q->dataLength());
        p.appendInts((qint32 *)q->data(), q->dataLength());
        encryptSendMessage(p.buffer(), p.length(), 0);
    }
}

void Session::queryOnResult(InboundPkt &inboundPkt, qint64 msgId) {
    qCDebug(TG_CORE_SESSION) << "result for query" << QString::number(msgId, 16);
    qint32 op = inboundPkt.prefetchInt();
    qint32 *inPtr = 0;
    qint32 *inEnd = 0;
    if (op == (qint32)TL_GZipPacked) {
        inboundPkt.fetchInt();
        qint32 l = inboundPkt.prefetchStrlen();
        char *s = inboundPkt.fetchStr(l);
        static qint32 packedBuffer[MAX_PACKED_SIZE / 4];
        qint32 totalOut = Utils::tinflate (s, l, packedBuffer, MAX_PACKED_SIZE);
        inPtr = inboundPkt.inPtr();
        inEnd = inboundPkt.inEnd();
        inboundPkt.setInPtr(packedBuffer);
        inboundPkt.setInEnd(inboundPkt.inPtr() + totalOut / 4);
        qCDebug(TG_CORE_SESSION) << "unzipped data";
    }

    Query *q = m_pendingQueries.take(msgId);
    if (!q) {
        qCWarning(TG_CORE_SESSION) << "No such query";
        inboundPkt.setInPtr(inboundPkt.inEnd());
    } else {
        qCDebug(TG_CORE_SESSION) << "acked query with msgId" << QString::number(msgId, 16) << ",pendingQueries:" << m_pendingQueries.size();
        q->setAcked(true);
        Q_EMIT resultReceived(q, inboundPkt);
    }

    if (inPtr) {
        inboundPkt.setInPtr(inPtr);
        inboundPkt.setInEnd(inEnd);
    }
}

void Session::queryOnError(InboundPkt &inboundPkt, qint64 msgId) {
    mAsserter.check(inboundPkt.fetchInt() == TL_RpcError);
    qint32 errorCode = inboundPkt.fetchInt();
    QString errorText = inboundPkt.fetchQString();
    qCDebug(TG_CORE_SESSION) << "error for query" << QString::number(msgId, 16) << " :" << errorCode << " :" << errorText;

    Query *q = m_pendingQueries.take(msgId);
    if (!q) {
        qCWarning(TG_CORE_SESSION) << "No such query";
    } else {
        q->setAcked(true);
        Q_EMIT errorReceived(q, errorCode, errorText);
    }
}


void Session::addToPendingAcks(qint64 msgId) {
    EventTimer *t = new EventTimer(msgId, ACK_TIMEOUT, this);
    connect(t, &EventTimer::timerTimeout, this, &Session::ack);
    t->start(); //timeout of 60 secs
    m_pendingAcks[msgId] = t;
    if (m_pendingAcks.size() > MAX_PENDING_ACKS) {
        ackAll();
    }
}

void Session::ack(qint64 msgId) {
    QList<qint64> idsList;
    idsList.append(msgId);
    sendAcks(idsList);
}

void Session::ackAll() {
    sendAcks(m_pendingAcks.keys());
}

void Session::sendAcks(const QList<qint64> &msgIds) {
    // verify that socket is connected. Don't send acks if state is other than connected.
    // In case the socket is not connected, pending acks will be sent when socket is
    // connected back again.
    if (state() != QAbstractSocket::ConnectedState) {
        return;
    }
    OutboundPkt p(mSettings);
    p.appendInt(TL_MsgsAck);
    p.appendInt(CoreTypes::typeVector);
    int n = msgIds.length();
    p.appendInt(n);
    Q_FOREACH (qint64 msgId, msgIds) {
        p.appendLong(msgId);
        qCDebug(TG_CORE_SESSION) << "Ack for msgId" << msgId;
        EventTimer* t = m_pendingAcks.take(msgId);
        if (t) {
            t->stop();
            delete t;
        }
    }
    qint64 sentAcksId = encryptSendMessage(p.buffer(), p.length(), 0);
    qCDebug(TG_CORE_SESSION) << "Sent Acks with id:" << QString::number(sentAcksId, 16);
}
