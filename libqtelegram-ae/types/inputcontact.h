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

#ifndef INPUTCONTACT_H
#define INPUTCONTACT_H

#include <QString>

class InputContact
{
public:

    enum InputContactType {
       typeInputPhoneContact = 0xf392b7f4
    };

    InputContact() :
        m_firstName(""),
        m_phone(""),
        m_clientId(0),
        m_lastName(""),
        m_classType(typeInputPhoneContact) {}

    void setClientId(qint64 clientId) {
        m_clientId = clientId;
    }
    qint64 clientId() const {
        return m_clientId;
    }
    void setPhone(const QString & phone) {
        m_phone = phone;
    }
    QString phone() const {
        return m_phone;
    }
    void setFirstName(const QString & firstName) {
        m_firstName = firstName;
    }
    QString firstName() const {
        return m_firstName;
    }
    void setLastName(const QString & lastName) {
        m_lastName = lastName;
    }
    QString lastName() const {
        return m_lastName;
    }
    void setClassType(InputContactType classType) {
        m_classType = classType;
    }
    InputContactType classType() const {
        return m_classType;
    }

private:
    QString m_firstName;
    QString m_phone;
    qint64 m_clientId;
    QString m_lastName;
    InputContactType m_classType;
};
#endif // INPUTCONTACT_H
