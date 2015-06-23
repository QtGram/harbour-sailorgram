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

#ifndef NOTIFYPEER_H
#define NOTIFYPEER_H

#include "peer.h"

class NotifyPeer
{
public:

    enum NotifyPeerType {
       typeNotifyAll = 0x74d07c60,
       typeNotifyPeer = 0x9fd40bd8,
       typeNotifyUsers = 0xb4c83b4c,
       typeNotifyChats = 0xc007cec3
    };

    NotifyPeer(NotifyPeerType classType) :
        m_peer(Peer::typePeerUser),
        m_classType(classType) {}

    void setPeer(const Peer &peer) {
        m_peer = peer;
    }
    Peer peer() const {
        return m_peer;
    }
    void setClassType(NotifyPeerType classType) {
        m_classType = classType;
    }
    NotifyPeerType classType() const {
        return m_classType;
    }

private:
    Peer m_peer;
    NotifyPeerType m_classType;
};
#endif // NOTIFYPEER_H
