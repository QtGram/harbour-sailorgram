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

#ifndef CHATPARTICIPANTS_H
#define CHATPARTICIPANTS_H

#include <QList>
#include "chatparticipant.h"

class ChatParticipants
{
public:

    enum ChatParticipantsType {
       typeChatParticipants = 0x7841b415,
       typeChatParticipantsForbidden = 0xfd2bb8a
    };

    ChatParticipants(ChatParticipantsType classType) :
        m_chatId(0),
        m_version(0),
        m_adminId(0),
        m_classType(classType) {}

    void setChatId(qint32 chatId) {
        m_chatId = chatId;
    }
    qint32 chatId() const {
        return m_chatId;
    }
    void setAdminId(qint32 adminId) {
        m_adminId = adminId;
    }
    qint32 adminId() const {
        return m_adminId;
    }
    void setParticipants(const QList<ChatParticipant> & participants) {
        m_participants = participants;
    }
    QList<ChatParticipant> participants() const {
        return m_participants;
    }
    void setVersion(qint32 version) {
        m_version = version;
    }
    qint32 version() const {
        return m_version;
    }
    void setClassType(ChatParticipantsType classType) {
        m_classType = classType;
    }
    ChatParticipantsType classType() const {
        return m_classType;
    }

private:
    QList<ChatParticipant> m_participants;
    qint32 m_chatId;
    qint32 m_version;
    qint32 m_adminId;
    ChatParticipantsType m_classType;
};
#endif // CHATPARTICIPANTS_H
