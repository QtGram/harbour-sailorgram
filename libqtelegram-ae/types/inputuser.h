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

#ifndef INPUTUSER_H
#define INPUTUSER_H

#include <QtGlobal>

class InputUser
{
public:

    enum InputUserType {
       typeInputUserEmpty = 0xb98886cf,
       typeInputUserContact = 0x86e94f65,
       typeInputUserSelf = 0xf7c1b13f,
       typeInputUserForeign = 0x655e74ff
    };

    InputUser(InputUserType classType = typeInputUserEmpty) :        
        m_accessHash(0),
        m_classType(classType),
        m_userId(0) {}

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
    void setClassType(InputUserType classType) {
        m_classType = classType;
    }
    InputUserType classType() const {
        return m_classType;
    }

private:
    qint64 m_accessHash;
    InputUserType m_classType;
    qint32 m_userId;
};
#endif // INPUTUSER_H
