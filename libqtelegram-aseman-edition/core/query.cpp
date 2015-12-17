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

#include "query.h"

Query::Query(QObject *parent) :
    QObject(parent), m_dataLength(0), m_data(0), m_methods(0), m_resends(MAX_QUERY_RESENDS) {
}

Query::~Query() {
    Utils::freeSecure(m_data, (4 * m_dataLength));
}

void *Query::data() {
    return m_data;
}

void Query::setData(void *data, qint32 dataLength) {
    if (m_data && m_dataLength > 0) {
        Utils::freeSecure(m_data, 4 * m_dataLength);
    }
    m_dataLength = dataLength;
    m_data = (char *)Utils::talloc(4 * dataLength);
    memcpy(m_data, data, 4 * dataLength);
}

qint32 Query::decreaseResends() {
    return --m_resends;
}

void Query::timerEvent(QTimerEvent *event) {
    if (!m_acked) {
        Q_EMIT timeout(this);
    } else {
        killTimer(event->timerId());
    }
}
