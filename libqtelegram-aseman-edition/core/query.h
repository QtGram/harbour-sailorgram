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

#ifndef QUERY_H
#define QUERY_H

#include "util/utils.h"
#include "inboundpkt.h"
#include "querymethods.h"
#include "dc.h"
#include <QTimerEvent>
#include <QVariant>

class Query : public QObject
{
    Q_OBJECT
public:
    explicit Query(QObject *parent = 0);
    ~Query();

    qint64 msgId() { return m_msgId; }
    void setMsgId(qint64 msgId) { m_msgId = msgId; }
    qint64 mainMsgId() { return m_mainMsgId; }
    void setMainMsgId(qint64 mainMsgId) { m_mainMsgId = mainMsgId; }
    qint32 dataLength() { return m_dataLength; }
    qint32 seqNo() { return m_seqNo; }
    void setSeqNo(qint32 seqNo) { m_seqNo = seqNo; }
    QueryMethods *methods() { return m_methods; }
    void setMethods(QueryMethods *methods) { m_methods = methods; }
    bool acked() { return m_acked; }
    void setAcked(bool acked) { m_acked = acked; }
    QString name() { return m_name; }
    void setName(const QString &name) { m_name = name; }
    QVariant extra() { return mExtra; }
    void setExtra(const QVariant &extra) { mExtra = extra; }
    void *data();
    void setData(void *data, qint32 dataLength);
    qint32 decreaseResends();

Q_SIGNALS:
    void timeout(Query *q);

protected:
    void timerEvent(QTimerEvent *event);

private:
    qint64 m_msgId;
    qint64 m_mainMsgId;
    qint32 m_dataLength;
    qint32 m_seqNo;
    void *m_data;
    QueryMethods *m_methods;
    bool m_acked;
    QVariant mExtra;
    qint32 m_resends;
    QString m_name;
};

#endif // QUERY_H
