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

#ifndef GEOCHATMESSAGE_H
#define GEOCHATMESSAGE_H

#include "messageaction.h"
#include "messagemedia.h"

class GeoChatMessage
{
public:

    enum GeoChatMessageType {
       typeGeoChatMessage = 0x4505f8e1,
       typeGeoChatMessageService = 0xd34fa24e,
       typeGeoChatMessageEmpty = 0x60311a9b
    };

    GeoChatMessage(GeoChatMessageType classType) :
        m_id(0),
        m_action(MessageAction::typeMessageActionEmpty),
        m_fromId(0),
        m_date(0),
        m_media(MessageMedia::typeMessageMediaEmpty),
        m_chatId(0),
        m_message(""),
        m_classType(classType) {}

    void setChatId(qint32 chatId) {
        m_chatId = chatId;
    }
    qint32 chatId() const {
        return m_chatId;
    }
    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setFromId(qint32 fromId) {
        m_fromId = fromId;
    }
    qint32 fromId() const {
        return m_fromId;
    }
    void setDate(qint32 date) {
        m_date = date;
    }
    qint32 date() const {
        return m_date;
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
    void setAction(const MessageAction &action) {
        m_action = action;
    }
    MessageAction action() const {
        return m_action;
    }
    void setClassType(GeoChatMessageType classType) {
        m_classType = classType;
    }
    GeoChatMessageType classType() const {
        return m_classType;
    }

private:
    qint32 m_id;
    MessageAction m_action;
    qint32 m_fromId;
    qint32 m_date;
    MessageMedia m_media;
    qint32 m_chatId;
    QString m_message;
    GeoChatMessageType m_classType;
};
#endif // GEOCHATMESSAGE_H
