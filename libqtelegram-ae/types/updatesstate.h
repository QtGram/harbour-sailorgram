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
 *
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 */

#ifndef UPDATESSTATE_H
#define UPDATESSTATE_H

#include <QtGlobal>

class UpdatesState
{
public:

    enum UpdatesStateType {
       typeUpdatesState = 0xa56c2a3e
    };

    UpdatesState() :
        m_seq(0),
        m_pts(0),
        m_qts(0),
        m_date(0),
        m_unreadCount(0),
        m_classType(typeUpdatesState) {}

    void setPts(qint32 pts) {
        m_pts = pts;
    }
    qint32 pts() const {
        return m_pts;
    }
    void setQts(qint32 qts) {
        m_qts = qts;
    }
    qint32 qts() const {
        return m_qts;
    }
    void setDate(qint32 date) {
        m_date = date;
    }
    qint32 date() const {
        return m_date;
    }
    void setSeq(qint32 seq) {
        m_seq = seq;
    }
    qint32 seq() const {
        return m_seq;
    }
    void setUnreadCount(qint32 unreadCount) {
        m_unreadCount = unreadCount;
    }
    qint32 unreadCount() const {
        return m_unreadCount;
    }
    void setClassType(UpdatesStateType classType) {
        m_classType = classType;
    }
    UpdatesStateType classType() const {
        return m_classType;
    }

private:
    qint32 m_seq;
    qint32 m_pts;
    qint32 m_qts;
    qint32 m_date;
    qint32 m_unreadCount;
    UpdatesStateType m_classType;
};
#endif // UPDATESSTATE_H
