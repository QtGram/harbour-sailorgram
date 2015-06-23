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

#ifndef CHATLOCATED_H
#define CHATLOCATED_H

#include <QtGlobal>

class ChatLocated
{
public:

    enum ChatLocatedType {
       typeChatLocated = 0x3631cf4c
    };

    ChatLocated() :
        m_chatId(0),
        m_distance(0),
        m_classType(typeChatLocated) {}

    void setChatId(qint32 chatId) {
        m_chatId = chatId;
    }
    qint32 chatId() const {
        return m_chatId;
    }
    void setDistance(qint32 distance) {
        m_distance = distance;
    }
    qint32 distance() const {
        return m_distance;
    }
    void setClassType(ChatLocatedType classType) {
        m_classType = classType;
    }
    ChatLocatedType classType() const {
        return m_classType;
    }

private:
    qint32 m_chatId;
    qint32 m_distance;
    ChatLocatedType m_classType;
};
#endif // CHATLOCATED_H
