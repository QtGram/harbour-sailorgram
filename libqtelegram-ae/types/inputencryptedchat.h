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

#ifndef INPUTENCRYPTEDCHAT_H
#define INPUTENCRYPTEDCHAT_H

#include <QtGlobal>

class InputEncryptedChat
{
public:

    enum InputEncryptedChatType {
       typeInputEncryptedChat = 0xf141b5e1
    };

    InputEncryptedChat() :
        m_accessHash(0),
        m_classType(typeInputEncryptedChat),
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
    void setClassType(InputEncryptedChatType classType) {
        m_classType = classType;
    }
    InputEncryptedChatType classType() const {
        return m_classType;
    }

private:
    qint64 m_accessHash;
    InputEncryptedChatType m_classType;
    qint32 m_chatId;
};
#endif // INPUTENCRYPTEDCHAT_H
