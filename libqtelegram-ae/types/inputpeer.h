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

#ifndef INPUTPEER_H
#define INPUTPEER_H

#include <QtGlobal>

class InputPeer
{
public:

    enum InputPeerType {
       typeInputPeerForeign = 0x9b447325,
       typeInputPeerSelf = 0x7da07ec9,
       typeInputPeerContact = 0x1023dbe8,
       typeInputPeerEmpty = 0x7f3b18ea,
       typeInputPeerChat = 0x179be863
    };

    InputPeer(InputPeerType classType = typeInputPeerEmpty) :
        m_chatId(0),
        m_userId(0),
        m_accessHash(0),
        m_classType(classType) {}

    qint32 id() {
        return (m_classType == typeInputPeerChat) ? m_chatId : m_userId;
    }
    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setChatId(qint32 chatId) {
        m_chatId = chatId;
    }
    qint32 chatId() const {
        return m_chatId;
    }
    void setClassType(InputPeerType classType) {
        m_classType = classType;
    }
    InputPeerType classType() const {
        return m_classType;
    }

private:
    qint32 m_chatId;
    qint32 m_userId;
    qint64 m_accessHash;
    InputPeerType m_classType;
};

#endif // INPUTPEER_H
