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
 */

#include "eventtimer.h"

EventTimer::EventTimer(qint64 msgId, qint32 timeout, QObject *parent) :
    QTimer(parent), mMsgId(msgId) {

    this->setSingleShot(true);
    this->setInterval(timeout);
    connect(this, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void EventTimer::onTimeout() {
    Q_EMIT timerTimeout(mMsgId);
}
