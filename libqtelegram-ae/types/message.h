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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "peer.h"
#include "messageaction.h"
#include "messagemedia.h"

class Message
{
public:
    enum MessageType {
       typeMessageEmpty = 0x83e5de54,
       typeMessageService = 0x1d86f70e,
       typeMessage = 0xa7ab1991
    };

    Message() :
        m_id(0),
        m_ttl(0),
        m_toId(Peer::typePeerUser),
        m_unread(false),
        m_action(MessageAction::typeMessageActionEmpty),
        m_fromId(0),
        m_out(false),
        m_date(0),
        m_replyToMsgId(0),
        m_media(MessageMedia::typeMessageMediaEmpty),
        m_fwdDate(0),
        m_fwdFromId(0),
        m_message(""),
        m_classType(typeMessageEmpty) {}


    Message(MessageType classType) :
        m_id(0),
        m_ttl(0),
        m_toId(Peer::typePeerUser),
        m_unread(false),
        m_action(MessageAction::typeMessageActionEmpty),
        m_fromId(0),
        m_out(false),
        m_date(0),
        m_replyToMsgId(0),
        m_media(MessageMedia::typeMessageMediaEmpty),
        m_fwdDate(0),
        m_fwdFromId(0),
        m_message(""),
        m_classType(classType) {}

    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setTtl(qint32 ttl) {
        m_ttl = ttl;
    }
    qint32 ttl() const {
        return m_ttl;
    }
    void setFromId(qint32 fromId) {
        m_fromId = fromId;
    }
    qint32 fromId() const {
        return m_fromId;
    }
    void setReplyToMsgId(qint32 msgId) {
        m_replyToMsgId = msgId;
    }
    qint32 replyToMsgId() const {
        return m_replyToMsgId;
    }
    void setToId(const Peer &toId) {
        m_toId = toId;
    }
    Peer toId() const {
        return m_toId;
    }
    void setOut(bool out) {
        m_out = out;
    }
    bool out() const {
        return m_out;
    }
    void setUnread(bool unread) {
        m_unread = unread;
    }
    bool unread() const {
        return m_unread;
    }
    void setDate(qint32 date) {
        m_date = date;
    }
    qint32 date() const {
        return m_date;
    }
    void setAction(const MessageAction &action) {
        m_action = action;
    }
    MessageAction action() const {
        return m_action;
    }
    void setMessage(const QString & message) {
        m_message = message;
    }
    QString message() const {
        return m_message;
    }
    void setMedia(const MessageMedia &media) {
        m_media = media;
    }
    MessageMedia media() const {
        return m_media;
    }
    void setFwdFromId(qint32 fwdFromId) {
        m_fwdFromId = fwdFromId;
    }
    qint32 fwdFromId() const {
        return m_fwdFromId;
    }
    void setFwdDate(qint32 fwdDate) {
        m_fwdDate = fwdDate;
    }
    qint32 fwdDate() const {
        return m_fwdDate;
    }
    void setClassType(MessageType classType) {
        m_classType = classType;
    }
    MessageType classType() const {
        return m_classType;
    }

private:
    qint32 m_id;
    qint32 m_ttl;
    Peer m_toId;
    bool m_unread;
    MessageAction m_action;
    qint32 m_fromId;
    bool m_out;
    qint32 m_date;
    qint32 m_replyToMsgId;
    MessageMedia m_media;
    qint32 m_fwdDate;
    qint32 m_fwdFromId;
    QString m_message;
    MessageType m_classType;
};
#endif // MESSAGE_H
