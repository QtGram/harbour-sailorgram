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

#ifndef USERSTATUS_H
#define USERSTATUS_H

#include <QtGlobal>

class UserStatus
{
public:

    enum UserStatusType {
       typeUserStatusOffline = 0x8c703f,
       typeUserStatusEmpty = 0x9d05049,
       typeUserStatusOnline = 0xedb93949,
       typeUserStatusRecently = 0xe26f42f1,
       typeUserStatusLastWeek = 0x7bf09fc,
       typeUserStatusLastMonth = 0x77ebc742
    };

    UserStatus(UserStatusType classType) :
        m_wasOnline(0),
        m_expires(0),
        m_classType(classType) {}

    void setWasOnline(qint32 wasOnline) {
        m_wasOnline = wasOnline;
    }
    qint32 wasOnline() const {
        return m_wasOnline;
    }
    void setExpires(qint32 expires) {
        m_expires = expires;
    }
    qint32 expires() const {
        return m_expires;
    }
    void setClassType(UserStatusType classType) {
        m_classType = classType;
    }
    UserStatusType classType() const {
        return m_classType;
    }

private:
    qint32 m_wasOnline;
    qint32 m_expires;
    UserStatusType m_classType;
};
#endif // USERSTATUS_H
