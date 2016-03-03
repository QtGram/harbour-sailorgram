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
#include "core/settings.h"
#include "util/constants.h"
#include "telegram/coretypes.h"

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

void OutboundPkt::appendOutboundPkt(OutboundPkt& other) {
    appendInts(other.buffer(), other.length());
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
    b ? appendInt(CoreTypes::typeBoolTrue) : appendInt(CoreTypes::typeBoolFalse);
}

void OutboundPkt::initConnection() {
    appendInt(CoreTypes::typeInvokeWithLayer);
    appendInt(CoreTypes::typeLayerVersion);
    appendInt(CoreTypes::typeInitConnection);
    appendInt(mSettings->appId());
    appendQString(Utils::getDeviceModel());
    appendQString(Utils::getSystemVersion());
    appendQString(Utils::getAppVersion());
    appendQString(mSettings->langCode());
}
