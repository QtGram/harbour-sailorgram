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

#ifndef PEER_H
#define PEER_H

#include <QtGlobal>

class Peer
{
public:

    enum PeerType {
       typePeerUser = 0x9db1bc6d,
       typePeerChat = 0xbad0e5bb
    };

    Peer(PeerType classType) :
        m_chatId(0),
        m_userId(0),
        m_classType(classType) {}

    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setChatId(qint32 chatId) {
        m_chatId = chatId;
    }
    qint32 chatId() const {
        return m_chatId;
    }
    void setClassType(PeerType classType) {
        m_classType = classType;
    }
    PeerType classType() const {
        return m_classType;
    }

private:
    qint32 m_chatId;
    qint32 m_userId;
    PeerType m_classType;
};
#endif // PEER_H
