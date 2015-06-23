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

#ifndef CONTACTSLINK_H
#define CONTACTSLINK_H

#include "contactlink.h"
#include "user.h"

class ContactsLink
{
public:

    enum ContactsLinkType {
       typeContactsLink = 0x3ace484c
    };

    ContactsLink() :
        m_foreignLink(ContactLink::typeContactLinkUnknown),
        m_myLink(ContactLink::typeContactLinkUnknown),
        m_user(User::typeUserEmpty),
        m_classType(typeContactsLink) {}

    ~ContactsLink() {}

    void setMyLink(ContactLink myLink) {
        m_myLink = myLink;
    }
    ContactLink myLink() const {
        return m_myLink;
    }
    void setForeignLink(ContactLink foreignLink) {
        m_foreignLink = foreignLink;
    }
    ContactLink foreignLink() const {
        return m_foreignLink;
    }
    void setUser(const User &user) {
        m_user = user;
    }
    User user() const {
        return m_user;
    }
    void setClassType(ContactsLinkType classType) {
        m_classType = classType;
    }
    ContactsLinkType classType() const {
        return m_classType;
    }

private:
    ContactLink m_foreignLink;
    ContactLink m_myLink;
    User m_user;
    ContactsLinkType m_classType;
};
#endif // CONTACTSLINK_H
