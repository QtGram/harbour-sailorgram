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

#ifndef MESSAGEACTION_H
#define MESSAGEACTION_H

#include "photo.h"

class MessageAction
{
public:

    enum MessageActionType {
       typeMessageActionEmpty = 0xb6aef7b0,
       typeMessageActionChatDeletePhoto = 0x95e3fbef,
       typeMessageActionChatCreate = 0xa6638b9a,
       typeMessageActionChatEditTitle = 0xb5a1ce5a,
       typeMessageActionChatEditPhoto = 0x7fcb13a8,
       typeMessageActionGeoChatCreate = 0x6f038ebc,
       typeMessageActionChatDeleteUser = 0xb2ae9b0c,
       typeMessageActionChatAddUser = 0x5e3cfc4b,
       typeMessageActionGeoChatCheckin = 0xc7d53de
    };

    MessageAction(MessageActionType classType = typeMessageActionEmpty) :
        m_address(""),
        m_userId(0),
        m_photo(Photo::typePhotoEmpty),
        m_title(""),
        m_classType(classType) {}

    void setTitle(const QString & title) {
        m_title = title;
    }
    QString title() const {
        return m_title;
    }
    void setUsers(const QList<qint32> & users) {
        m_users = users;
    }
    QList<qint32> users() const {
        return m_users;
    }
    void setPhoto(const Photo &photo) {
        m_photo = photo;
    }
    Photo photo() const {
        return m_photo;
    }
    void setAddress(const QString & address) {
        m_address = address;
    }
    QString address() const {
        return m_address;
    }
    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setClassType(MessageActionType classType) {
        m_classType = classType;
    }
    MessageActionType classType() const {
        return m_classType;
    }

private:
    QString m_address;
    qint32 m_userId;
    Photo m_photo;
    QString m_title;
    QList<qint32> m_users;
    MessageActionType m_classType;
};
#endif // MESSAGEACTION_H
