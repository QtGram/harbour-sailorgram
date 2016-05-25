/*
 * Copyright 2014 Canonical Ltd.
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
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
 */

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QString>

class Endpoint
{
public:
    Endpoint() : m_port(0), m_ipv6(false) {}
    explicit Endpoint(const QString &host, qint32 port) : m_host(host), m_port(port), m_ipv6(false) {}
    virtual ~Endpoint() {}

    QString host() const { return m_host; }
    qint32 port() const { return m_port; }
    void setHost(const QString &host) { m_host = host; }
    void setPort(qint32 port) { m_port = port; }
    bool ipv6() const { return m_ipv6; }
    void setIpv6(bool ipv6) { m_ipv6 = ipv6; }

protected:
    QString m_host;
    qint32 m_port;
    bool m_ipv6;
};

#endif // ENDPOINT_H




