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

#ifndef INPUTNOTIFYPEER_H
#define INPUTNOTIFYPEER_H

#include "inputpeer.h"
#include "inputgeochat.h"

class InputNotifyPeer
{
public:

    enum InputNotifyPeerType {
       typeInputNotifyChats = 0x4a95e84e,
       typeInputNotifyUsers = 0x193b4417,
       typeInputNotifyAll = 0xa429b886,
       typeInputNotifyPeer = 0xb8bc5b0c,
       typeInputNotifyGeoChatPeer = 0x4d8ddec8
    };

    InputNotifyPeer(InputNotifyPeerType classType) :
        m_peer(InputPeer::typeInputPeerEmpty),
        m_classType(classType) {}

    void setPeer(const InputPeer &peer) {
        m_peer = peer;
    }
    InputPeer peer() const {
        return m_peer;
    }
    void setGeoChatPeer(const InputGeoChat &geoChatPeer) {
        m_geoChatPeer = geoChatPeer;
    }
    InputGeoChat geoChatPeer() const {
        return m_geoChatPeer;
    }
    void setClassType(InputNotifyPeerType classType) {
        m_classType = classType;
    }
    InputNotifyPeerType classType() const {
        return m_classType;
    }

private:
    InputPeer m_peer;
    InputGeoChat m_geoChatPeer;
    InputNotifyPeerType m_classType;
};
#endif // INPUTNOTIFYPEER_H
