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

#ifndef ACKTIMER_H
#define ACKTIMER_H

#include <QTimer>

class EventTimer : public QTimer
{
    Q_OBJECT
public:
    explicit EventTimer(qint64 msgId, qint32 timeout, QObject *parent = 0);

Q_SIGNALS:
    void timerTimeout(qint64 msgId);

private:
    qint64 mMsgId;

private Q_SLOTS:
    void onTimeout();
};

#endif // ACKTIMER_H
