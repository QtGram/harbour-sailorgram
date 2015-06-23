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

#ifndef CONTACT_H
#define CONTACT_H

#include <QtGlobal>

class Contact
{
public:

    enum ContactType {
       typeContact = 0xf911c994
    };

    Contact() :
        m_userId(0),
        m_mutual(false),
        m_classType(typeContact) {}

    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setMutual(bool mutual) {
        m_mutual = mutual;
    }
    bool mutual() const {
        return m_mutual;
    }
    void setClassType(ContactType classType) {
        m_classType = classType;
    }
    ContactType classType() const {
        return m_classType;
    }

private:
    qint32 m_userId;
    bool m_mutual;
    ContactType m_classType;
};
#endif // CONTACT_H
