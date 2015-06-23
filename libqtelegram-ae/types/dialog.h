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

#ifndef DIALOG_H
#define DIALOG_H

#include "peernotifysettings.h"
#include "peer.h"

class Dialog
{
public:

    enum DialogType {
       typeDialog = 0xc1dd804a
    };

    Dialog() :
        m_peer(Peer::typePeerUser),
        m_notifySettings(PeerNotifySettings::typePeerNotifySettingsEmpty),
        m_topMessage(0),
        m_readInboxMaxId(0),
        m_unreadCount(0),
        m_classType(typeDialog) {}

    virtual ~Dialog() {}

    void setPeer(const Peer &peer) {
        m_peer = peer;
    }
    Peer peer() const {
        return m_peer;
    }
    void setTopMessage(qint32 topMessage) {
        m_topMessage = topMessage;
    }
    qint32 topMessage() const {
        return m_topMessage;
    }
    void setReadInboxMaxId(qint32 readInboxMaxId) {
        m_readInboxMaxId = readInboxMaxId;
    }
    qint32 readInboxMaxId() const {
        return m_readInboxMaxId;
    }
    virtual void setUnreadCount(qint32 unreadCount) {
        m_unreadCount = unreadCount;
    }
    qint32 unreadCount() const {
        return m_unreadCount;
    }
    void setNotifySettings(const PeerNotifySettings &notifySettings) {
        m_notifySettings = notifySettings;
    }
    PeerNotifySettings notifySettings() const {
        return m_notifySettings;
    }
    void setClassType(DialogType classType) {
        m_classType = classType;
    }
    DialogType classType() const {
        return m_classType;
    }

private:
    Peer m_peer;
    PeerNotifySettings m_notifySettings;
    qint32 m_topMessage;
    qint32 m_readInboxMaxId;
    qint32 m_unreadCount;
    DialogType m_classType;
};
#endif // DIALOG_H
