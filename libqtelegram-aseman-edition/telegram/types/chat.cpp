// This file generated by libqtelegram-code-generator.
// You can download it from: https://github.com/Aseman-Land/libqtelegram-code-generator
// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN

#include "chat.h"
#include "core/inboundpkt.h"
#include "core/outboundpkt.h"
#include "../coretypes.h"

Chat::Chat(ChatType classType, InboundPkt *in) :
    m_accessHash(0),
    m_checkedIn(false),
    m_date(0),
    m_id(0),
    m_left(false),
    m_participantsCount(0),
    m_version(0),
    m_classType(classType)
{
    if(in) fetch(in);
}

Chat::Chat(InboundPkt *in) :
    m_accessHash(0),
    m_checkedIn(false),
    m_date(0),
    m_id(0),
    m_left(false),
    m_participantsCount(0),
    m_version(0),
    m_classType(typeChatEmpty)
{
    fetch(in);
}

Chat::~Chat() {
}

void Chat::setAccessHash(qint64 accessHash) {
    m_accessHash = accessHash;
}

qint64 Chat::accessHash() const {
    return m_accessHash;
}

void Chat::setAddress(const QString &address) {
    m_address = address;
}

QString Chat::address() const {
    return m_address;
}

void Chat::setCheckedIn(bool checkedIn) {
    m_checkedIn = checkedIn;
}

bool Chat::checkedIn() const {
    return m_checkedIn;
}

void Chat::setDate(qint32 date) {
    m_date = date;
}

qint32 Chat::date() const {
    return m_date;
}

void Chat::setGeo(const GeoPoint &geo) {
    m_geo = geo;
}

GeoPoint Chat::geo() const {
    return m_geo;
}

void Chat::setId(qint32 id) {
    m_id = id;
}

qint32 Chat::id() const {
    return m_id;
}

void Chat::setLeft(bool left) {
    m_left = left;
}

bool Chat::left() const {
    return m_left;
}

void Chat::setParticipantsCount(qint32 participantsCount) {
    m_participantsCount = participantsCount;
}

qint32 Chat::participantsCount() const {
    return m_participantsCount;
}

void Chat::setPhoto(const ChatPhoto &photo) {
    m_photo = photo;
}

ChatPhoto Chat::photo() const {
    return m_photo;
}

void Chat::setTitle(const QString &title) {
    m_title = title;
}

QString Chat::title() const {
    return m_title;
}

void Chat::setVenue(const QString &venue) {
    m_venue = venue;
}

QString Chat::venue() const {
    return m_venue;
}

void Chat::setVersion(qint32 version) {
    m_version = version;
}

qint32 Chat::version() const {
    return m_version;
}

bool Chat::operator ==(const Chat &b) {
    return m_accessHash == b.m_accessHash &&
           m_address == b.m_address &&
           m_checkedIn == b.m_checkedIn &&
           m_date == b.m_date &&
           m_geo == b.m_geo &&
           m_id == b.m_id &&
           m_left == b.m_left &&
           m_participantsCount == b.m_participantsCount &&
           m_photo == b.m_photo &&
           m_title == b.m_title &&
           m_venue == b.m_venue &&
           m_version == b.m_version;
}

void Chat::setClassType(Chat::ChatType classType) {
    m_classType = classType;
}

Chat::ChatType Chat::classType() const {
    return m_classType;
}

bool Chat::fetch(InboundPkt *in) {
    LQTG_FETCH_LOG;
    int x = in->fetchInt();
    switch(x) {
    case typeChatEmpty: {
        m_id = in->fetchInt();
        m_classType = static_cast<ChatType>(x);
        return true;
    }
        break;
    
    case typeChat: {
        m_id = in->fetchInt();
        m_title = in->fetchQString();
        m_photo.fetch(in);
        m_participantsCount = in->fetchInt();
        m_date = in->fetchInt();
        m_left = in->fetchBool();
        m_version = in->fetchInt();
        m_classType = static_cast<ChatType>(x);
        return true;
    }
        break;
    
    case typeChatForbidden: {
        m_id = in->fetchInt();
        m_title = in->fetchQString();
        m_date = in->fetchInt();
        m_classType = static_cast<ChatType>(x);
        return true;
    }
        break;
    
    case typeGeoChat: {
        m_id = in->fetchInt();
        m_accessHash = in->fetchLong();
        m_title = in->fetchQString();
        m_address = in->fetchQString();
        m_venue = in->fetchQString();
        m_geo.fetch(in);
        m_photo.fetch(in);
        m_participantsCount = in->fetchInt();
        m_date = in->fetchInt();
        m_checkedIn = in->fetchBool();
        m_version = in->fetchInt();
        m_classType = static_cast<ChatType>(x);
        return true;
    }
        break;
    
    default:
        LQTG_FETCH_ASSERT;
        return false;
    }
}

bool Chat::push(OutboundPkt *out) const {
    out->appendInt(m_classType);
    switch(m_classType) {
    case typeChatEmpty: {
        out->appendInt(m_id);
        return true;
    }
        break;
    
    case typeChat: {
        out->appendInt(m_id);
        out->appendQString(m_title);
        m_photo.push(out);
        out->appendInt(m_participantsCount);
        out->appendInt(m_date);
        out->appendBool(m_left);
        out->appendInt(m_version);
        return true;
    }
        break;
    
    case typeChatForbidden: {
        out->appendInt(m_id);
        out->appendQString(m_title);
        out->appendInt(m_date);
        return true;
    }
        break;
    
    case typeGeoChat: {
        out->appendInt(m_id);
        out->appendLong(m_accessHash);
        out->appendQString(m_title);
        out->appendQString(m_address);
        out->appendQString(m_venue);
        m_geo.push(out);
        m_photo.push(out);
        out->appendInt(m_participantsCount);
        out->appendInt(m_date);
        out->appendBool(m_checkedIn);
        out->appendInt(m_version);
        return true;
    }
        break;
    
    default:
        return false;
    }
}

