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

#ifndef CHAT_H
#define CHAT_H

#include "chatphoto.h"
#include "geopoint.h"

class Chat
{
public:

    enum ChatType {
       typeChatEmpty = 0x9ba2d800,
       typeChatForbidden = 0xfb0ccc41,
       typeGeoChat = 0x75eaea5a,
       typeChat = 0x6e9c9bc7
    };

    Chat(ChatType classType) :
        m_participantsCount(0),
        m_id(0),
        m_version(0),
        m_venue(""),
        m_title(""),
        m_address(""),
        m_date(0),
        m_photo(ChatPhoto::typeChatPhotoEmpty),
        m_geo(GeoPoint::typeGeoPointEmpty),
        m_accessHash(0),
        m_checkedIn(false),
        m_left(false),
        m_classType(classType) {}

    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setTitle(const QString & title) {
        m_title = title;
    }
    QString title() const {
        return m_title;
    }
    void setDate(qint32 date) {
        m_date = date;
    }
    qint32 date() const {
        return m_date;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setAddress(const QString &address) {
        m_address = address;
    }
    QString address() const {
        return m_address;
    }
    void setVenue(const QString &venue) {
        m_venue = venue;
    }
    QString venue() const {
        return m_venue;
    }
    void setGeo(const GeoPoint &geo) {
        m_geo = geo;
    }
    GeoPoint geo() const {
        return m_geo;
    }
    void setPhoto(const ChatPhoto &photo) {
        m_photo = photo;
    }
    ChatPhoto photo() const {
        return m_photo;
    }
    void setParticipantsCount(qint32 participantsCount) {
        m_participantsCount = participantsCount;
    }
    qint32 participantsCount() const {
        return m_participantsCount;
    }
    void setCheckedIn(bool checkedIn) {
        m_checkedIn = checkedIn;
    }
    bool checkedIn() const {
        return m_checkedIn;
    }
    void setVersion(qint32 version) {
        m_version = version;
    }
    qint32 version() const {
        return m_version;
    }
    void setLeft(bool left) {
        m_left = left;
    }
    bool left() const {
        return m_left;
    }
    void setClassType(ChatType classType) {
        m_classType = classType;
    }
    ChatType classType() const {
        return m_classType;
    }

private:
    qint32 m_participantsCount;
    qint32 m_id;
    qint32 m_version;
    QString m_venue;
    QString m_title;
    QString m_address;
    qint32 m_date;
    ChatPhoto m_photo;
    GeoPoint m_geo;
    qint64 m_accessHash;
    bool m_checkedIn;
    bool m_left;
    ChatType m_classType;
};
#endif // CHAT_H
