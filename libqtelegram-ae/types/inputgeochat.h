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

#ifndef INPUTGEOCHAT_H
#define INPUTGEOCHAT_H

#include <QtGlobal>

class InputGeoChat
{
public:

    enum InputGeoChatType {
       typeInputGeoChat = 0x74d456fa
    };

    InputGeoChat() :
        m_accessHash(0),
        m_classType(typeInputGeoChat),
        m_chatId(0) {}

    void setChatId(qint32 chatId) {
        m_chatId = chatId;
    }
    qint32 chatId() const {
        return m_chatId;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setClassType(InputGeoChatType classType) {
        m_classType = classType;
    }
    InputGeoChatType classType() const {
        return m_classType;
    }

private:
    qint64 m_accessHash;
    InputGeoChatType m_classType;
    qint32 m_chatId;
};
#endif // INPUTGEOCHAT_H
