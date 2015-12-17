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

#include "inboundpkt.h"

#include <QDebug>
#include "util/utils.h"
#include "util/tlvalues.h"
#include "telegram/coretypes.h"

Q_LOGGING_CATEGORY(TG_CORE_INBOUNDPKT, "tg.core.inboundpkt")

InboundPkt::InboundPkt(char *buffer, qint32 length) :
    m_buffer(buffer),
    m_length(length),
    m_inPtr((qint32 *)m_buffer),
    m_inEnd(m_inPtr + (m_length / 4)) {
}

InboundPkt::~InboundPkt() {
}

const char *InboundPkt::buffer() const{
    return m_buffer;
}

qint32 InboundPkt::length() const {
    return m_length;
}

void InboundPkt::setInPtr(qint32 *inPtr) {
    this->m_inPtr = inPtr;
}

void InboundPkt::setInEnd(qint32 *inEnd) {
    this->m_inEnd = inEnd;
}

qint32 *InboundPkt::inPtr() {
    return m_inPtr;
}

qint32 *InboundPkt::inEnd() {
    return m_inEnd;
}

void InboundPkt::forwardInPtr(qint32 positions) {
    m_inPtr += positions;
}

qint32 InboundPkt::prefetchStrlen() {
    if (m_inPtr >= m_inEnd) {
        return -1;
    }
    unsigned l = *m_inPtr;
    if ((l & 0xff) < 0xfe) {
        l &= 0xff;
        return (m_inEnd >= m_inPtr + (l >> 2) + 1) ? (qint32)l : -1;
    } else if ((l & 0xff) == 0xfe) {
        l >>= 8;
        return (l >= 254 && m_inEnd >= m_inPtr + ((l + 7) >> 2)) ? (qint32)l : -1;
    } else {
        return -1;
    }
}

qint32 InboundPkt::prefetchInt() {
  Q_ASSERT(m_inPtr < m_inEnd);
  return *(m_inPtr);
}

qint32 InboundPkt::fetchInt() {
    Q_ASSERT(m_inPtr + 1 <= m_inEnd);
    qCDebug(TG_CORE_INBOUNDPKT) << "fetchInt()" << *m_inPtr << " (" << Utils::toHex(*m_inPtr) << ")";
    return *(m_inPtr ++);
}

bool InboundPkt::fetchBool() {
    Q_ASSERT(m_inPtr + 1 <= m_inEnd);
    ASSERT(*(m_inPtr) == (qint32)CoreTypes::typeBoolTrue || *(m_inPtr) == (qint32)CoreTypes::typeBoolFalse);
    qCDebug(TG_CORE_INBOUNDPKT) << "fetchBool()" << (*(m_inPtr) == (qint32)CoreTypes::typeBoolTrue) << " (" << Utils::toHex(*m_inPtr) << ")";
    return *(m_inPtr++) == (qint32)CoreTypes::typeBoolTrue;
}

qint64 InboundPkt::fetchLong() {
    Q_ASSERT(m_inPtr + 2 <= m_inEnd);
    qint64 r = *(qint64 *)m_inPtr;
    qCDebug(TG_CORE_INBOUNDPKT) << "fetchLong()" <<  r ;
    m_inPtr += 2;
    return r;
}

qint32 *InboundPkt::fetchInts(qint32 count) {
    Q_ASSERT(m_inPtr + count <= m_inEnd);
    qint32 *data = (qint32 *)Utils::talloc(4 * count);
    memcpy (data, m_inPtr, 4 * count);
    m_inPtr += count;
    return data;
}

double InboundPkt::fetchDouble() {
    Q_ASSERT(m_inPtr + 2 <= m_inEnd);
    double r = *(double *)m_inPtr;
    m_inPtr += 2;
    return r;
}

char *InboundPkt::fetchStr(qint32 len) {
    Q_ASSERT(len >= 0);
    qCDebug(TG_CORE_INBOUNDPKT) << "fetchStr(), len =" << len;
    char *str;
    if (len < 254) {
        str = (char *)m_inPtr + 1;
        m_inPtr += 1 + (len >> 2);
    } else {
        str = (char *)m_inPtr + 4;
        m_inPtr += (len + 7) >> 2;
    }
    return str;
}

QString InboundPkt::fetchQString() {
    qint32 l = prefetchStrlen();
    return QString::fromUtf8(fetchStr(l),l);
}

QByteArray InboundPkt::fetchBytes() {
    qint32 l = prefetchStrlen();
    char *bytes = fetchStr(l);
    // http://qt-project.org/doc/qt-4.8/qbytearray.html#fromRawData
    return QByteArray::fromRawData(bytes, l);
}

qint32 InboundPkt::fetchBignum (BIGNUM *x) {
    qint32 l = prefetchStrlen();
    if (l < 0) {
        return l;
    }
    const char *str = fetchStr (l);
    BIGNUM *bn = BN_bin2bn ((uchar *) str, l, x);
    Q_UNUSED(bn);
    Q_ASSERT(bn == x);
    return l;
}

qint32 InboundPkt::fetchDate() {
  qint32 p = fetchInt ();
  // TODO must we take account about last_date ??
  return p;
}
