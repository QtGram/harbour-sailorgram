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

#ifndef DCOPTION_H
#define DCOPTION_H

#include <QObject>

class DcOption
{
public:
    enum DcOptionType {
       typeDcOption = 0x2ec2a43c
    };

    DcOption() :
        m_hostname(""),
        m_id(0),
        m_ipAddress(""),
        m_port(0),
        m_classType(typeDcOption) {}

    ~DcOption() {}

    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setHostname(const QString &hostname) {
        m_hostname = hostname;
    }
    QString hostname() const {
        return m_hostname;
    }
    void setIpAddress(const QString &ipAddress) {
        m_ipAddress = ipAddress;
    }
    QString ipAddress() const {
        return m_ipAddress;
    }
    void setPort(qint32 port) {
        m_port = port;
    }
    qint32 port() const {
        return m_port;
    }
    void setClassType(DcOptionType classType) {
        m_classType = classType;
    }
    DcOptionType classType() const {
        return m_classType;
    }

private:
    QString m_hostname;
    qint32 m_id;
    QString m_ipAddress;
    qint32 m_port;
    DcOptionType m_classType;
};
#endif // DCOPTION_H
