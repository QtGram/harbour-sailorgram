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

#ifndef CHATPARTICIPANT_H
#define CHATPARTICIPANT_H

#include <QtGlobal>

class ChatParticipant
{
public:

    enum ChatParticipantType {
       typeChatParticipant = 0xc8d7493e
    };

    ChatParticipant() :
        m_userId(0),
        m_date(0),
        m_inviterId(0),
        m_classType(typeChatParticipant) {}

    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setInviterId(qint32 inviterId) {
        m_inviterId = inviterId;
    }
    qint32 inviterId() const {
        return m_inviterId;
    }
    void setDate(qint32 date) {
        m_date = date;
    }
    qint32 date() const {
        return m_date;
    }
    void setClassType(ChatParticipantType classType) {
        m_classType = classType;
    }
    ChatParticipantType classType() const {
        return m_classType;
    }

private:
    qint32 m_userId;
    qint32 m_date;
    qint32 m_inviterId;
    ChatParticipantType m_classType;
};
#endif // CHATPARTICIPANT_H
