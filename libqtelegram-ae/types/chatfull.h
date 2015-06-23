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

#ifndef CHATFULL_H
#define CHATFULL_H

#include "chatparticipants.h"
#include "photo.h"
#include "peernotifysettings.h"

class ChatFull
{
public:

    enum ChatFullType {
       typeChatFull = 0x630e61be
    };

    ChatFull() :
        m_participants(ChatParticipants::typeChatParticipants),
        m_notifySettings(PeerNotifySettings::typePeerNotifySettingsEmpty),
        m_chatPhoto(Photo::typePhotoEmpty),        
        m_classType(typeChatFull),
        m_id(0) {}

    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setParticipants(const ChatParticipants &participants) {
        m_participants = participants;
    }
    ChatParticipants participants() const {
        return m_participants;
    }
    void setChatPhoto(const Photo &chatPhoto) {
        m_chatPhoto = chatPhoto;
    }
    Photo chatPhoto() const {
        return m_chatPhoto;
    }
    void setNotifySettings(const PeerNotifySettings &notifySettings) {
        m_notifySettings = notifySettings;
    }
    PeerNotifySettings notifySettings() const {
        return m_notifySettings;
    }
    void setClassType(ChatFullType classType) {
        m_classType = classType;
    }
    ChatFullType classType() const {
        return m_classType;
    }

private:
    ChatParticipants m_participants;
    PeerNotifySettings m_notifySettings;
    Photo m_chatPhoto;
    ChatFullType m_classType;
    qint32 m_id;
};
#endif // CHATFULL_H
