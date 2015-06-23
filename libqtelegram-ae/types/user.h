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

#ifndef USER_H
#define USER_H

#include <QObject>
#include "userprofilephoto.h"
#include "userstatus.h"

class User
{
public:

    enum UserType {
       typeUserSelf = 0x1c60e608,
       typeUserContact = 0xcab35e18,
       typeUserDeleted = 0xd6016d7a,
       typeUserForeign = 0x75cf7a8,
       typeUserEmpty = 0x200250ba,
       typeUserRequest = 0xd9ccc4ef
    };

    User(UserType classType = typeUserEmpty) :
        m_id(0),
        m_accessHash(0),
        m_phone(""),
        m_firstName(""),
        m_photo(UserProfilePhoto::typeUserProfilePhotoEmpty),
        m_status(UserStatus::typeUserStatusEmpty),
        m_lastName(""),
        m_classType(classType) {}

    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setFirstName(const QString &firstName) {
        m_firstName = firstName;
    }
    QString firstName() const {
        return m_firstName;
    }
    void setLastName(const QString &lastName) {
        m_lastName = lastName;
    }
    QString lastName() const {
        return m_lastName;
    }
    void setUsername(const QString &username) {
        mUsername = username;
    }
    QString username() const {
        return mUsername;
    }
    void setPhone(const QString &phone) {
        m_phone = phone;
    }
    QString phone() const {
        return m_phone;
    }
    void setPhoto(const UserProfilePhoto &photo) {
        m_photo = photo;
    }
    UserProfilePhoto photo() const {
        return m_photo;
    }
    void setStatus(const UserStatus &status) {
        m_status = status;
    }
    UserStatus status() const {
        return m_status;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setClassType(UserType classType) {
        m_classType = classType;
    }
    UserType classType() const {
        return m_classType;
    }

private:
    qint32 m_id;
    qint64 m_accessHash;
    QString m_phone;
    QString m_firstName;
    UserProfilePhoto m_photo;
    UserStatus m_status;
    QString m_lastName;
    QString mUsername;
    UserType m_classType;
};
#endif // USER_H
