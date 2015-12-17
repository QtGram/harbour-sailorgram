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

#ifndef INBOUNDPKT_H
#define INBOUNDPKT_H

#include <QObject>
#include <QLoggingCategory>

#include <openssl/bn.h>

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_INBOUNDPKT)

class InboundPkt
{
public:
    explicit InboundPkt(char* buffer, qint32 len);
    virtual ~InboundPkt();

    virtual const char *buffer() const;
    virtual qint32 length() const;
    virtual void setInPtr(qint32 *inPtr);
    virtual void setInEnd(qint32 *inEnd);
    virtual qint32 *inPtr();
    virtual qint32 *inEnd();
    virtual void forwardInPtr(qint32 positions);

    virtual qint32 prefetchInt();
    virtual qint32 fetchInt();
    virtual bool fetchBool();
    virtual qint32 *fetchInts(qint32 length);
    virtual double fetchDouble();
    virtual qint64 fetchLong();
    virtual qint32 prefetchStrlen();
    virtual char *fetchStr(qint32 length);
    virtual QByteArray fetchBytes();
    virtual QString fetchQString();
    virtual qint32 fetchBignum (BIGNUM *x);
    virtual qint32 fetchDate();

protected:
    char *m_buffer;
    qint32 m_length;

    qint32 *m_inPtr;
    qint32 *m_inEnd;
};

#endif // INBOUNDPKT_H
