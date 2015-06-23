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

#ifndef IMPORTEDCONTACT_H
#define IMPORTEDCONTACT_H

#include <QtGlobal>

class ImportedContact
{
public:

    enum ImportedContactType {
       typeImportedContact = 0xd0028438
    };

    ImportedContact() :
        m_clientId(0),
        m_classType(typeImportedContact),
        m_userId(0) {}

    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setClientId(qint64 clientId) {
        m_clientId = clientId;
    }
    qint64 clientId() const {
        return m_clientId;
    }
    void setClassType(ImportedContactType classType) {
        m_classType = classType;
    }
    ImportedContactType classType() const {
        return m_classType;
    }

private:
    qint64 m_clientId;
    ImportedContactType m_classType;
    qint32 m_userId;
};
#endif // IMPORTEDCONTACT_H
