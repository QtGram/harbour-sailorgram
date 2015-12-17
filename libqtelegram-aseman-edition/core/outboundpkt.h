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
#include <QSharedPointer>

#include <openssl/bn.h>

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_OUTBOUNDPKT)

class Settings;
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

    qint32 length() const;
    qint32 *buffer();

protected:
    qint32 *m_packetPtr;
    QSharedPointer<qint32> __buffer;
    qint32 *m_packetBuffer;

private:
    Settings *mSettings;
};

#endif // OUTBOUNDPKT_H
