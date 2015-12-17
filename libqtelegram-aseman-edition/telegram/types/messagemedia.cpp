// This file generated by libqtelegram-code-generator.
// You can download it from: https://github.com/Aseman-Land/libqtelegram-code-generator
// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN

#include "messagemedia.h"
#include "core/inboundpkt.h"
#include "core/outboundpkt.h"
#include "../coretypes.h"

MessageMedia::MessageMedia(MessageMediaType classType, InboundPkt *in) :
    m_userId(0),
    m_classType(classType)
{
    if(in) fetch(in);
}

MessageMedia::MessageMedia(InboundPkt *in) :
    m_userId(0),
    m_classType(typeMessageMediaEmpty)
{
    fetch(in);
}

MessageMedia::~MessageMedia() {
}

void MessageMedia::setAddress(const QString &address) {
    m_address = address;
}

QString MessageMedia::address() const {
    return m_address;
}

void MessageMedia::setAudio(const Audio &audio) {
    m_audio = audio;
}

Audio MessageMedia::audio() const {
    return m_audio;
}

void MessageMedia::setCaption(const QString &caption) {
    m_caption = caption;
}

QString MessageMedia::caption() const {
    return m_caption;
}

void MessageMedia::setDocument(const Document &document) {
    m_document = document;
}

Document MessageMedia::document() const {
    return m_document;
}

void MessageMedia::setFirstName(const QString &firstName) {
    m_firstName = firstName;
}

QString MessageMedia::firstName() const {
    return m_firstName;
}

void MessageMedia::setGeo(const GeoPoint &geo) {
    m_geo = geo;
}

GeoPoint MessageMedia::geo() const {
    return m_geo;
}

void MessageMedia::setLastName(const QString &lastName) {
    m_lastName = lastName;
}

QString MessageMedia::lastName() const {
    return m_lastName;
}

void MessageMedia::setPhoneNumber(const QString &phoneNumber) {
    m_phoneNumber = phoneNumber;
}

QString MessageMedia::phoneNumber() const {
    return m_phoneNumber;
}

void MessageMedia::setPhoto(const Photo &photo) {
    m_photo = photo;
}

Photo MessageMedia::photo() const {
    return m_photo;
}

void MessageMedia::setProvider(const QString &provider) {
    m_provider = provider;
}

QString MessageMedia::provider() const {
    return m_provider;
}

void MessageMedia::setTitle(const QString &title) {
    m_title = title;
}

QString MessageMedia::title() const {
    return m_title;
}

void MessageMedia::setUserId(qint32 userId) {
    m_userId = userId;
}

qint32 MessageMedia::userId() const {
    return m_userId;
}

void MessageMedia::setVenueId(const QString &venueId) {
    m_venueId = venueId;
}

QString MessageMedia::venueId() const {
    return m_venueId;
}

void MessageMedia::setVideo(const Video &video) {
    m_video = video;
}

Video MessageMedia::video() const {
    return m_video;
}

void MessageMedia::setWebpage(const WebPage &webpage) {
    m_webpage = webpage;
}

WebPage MessageMedia::webpage() const {
    return m_webpage;
}

bool MessageMedia::operator ==(const MessageMedia &b) {
    return m_address == b.m_address &&
           m_audio == b.m_audio &&
           m_caption == b.m_caption &&
           m_document == b.m_document &&
           m_firstName == b.m_firstName &&
           m_geo == b.m_geo &&
           m_lastName == b.m_lastName &&
           m_phoneNumber == b.m_phoneNumber &&
           m_photo == b.m_photo &&
           m_provider == b.m_provider &&
           m_title == b.m_title &&
           m_userId == b.m_userId &&
           m_venueId == b.m_venueId &&
           m_video == b.m_video &&
           m_webpage == b.m_webpage;
}

void MessageMedia::setClassType(MessageMedia::MessageMediaType classType) {
    m_classType = classType;
}

MessageMedia::MessageMediaType MessageMedia::classType() const {
    return m_classType;
}

bool MessageMedia::fetch(InboundPkt *in) {
    LQTG_FETCH_LOG;
    int x = in->fetchInt();
    switch(x) {
    case typeMessageMediaEmpty: {
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaPhoto: {
        m_photo.fetch(in);
        m_caption = in->fetchQString();
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaVideo: {
        m_video.fetch(in);
        m_caption = in->fetchQString();
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaGeo: {
        m_geo.fetch(in);
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaContact: {
        m_phoneNumber = in->fetchQString();
        m_firstName = in->fetchQString();
        m_lastName = in->fetchQString();
        m_userId = in->fetchInt();
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaUnsupported: {
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaDocument: {
        m_document.fetch(in);
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaAudio: {
        m_audio.fetch(in);
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaWebPage: {
        m_webpage.fetch(in);
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    case typeMessageMediaVenue: {
        m_geo.fetch(in);
        m_title = in->fetchQString();
        m_address = in->fetchQString();
        m_provider = in->fetchQString();
        m_venueId = in->fetchQString();
        m_classType = static_cast<MessageMediaType>(x);
        return true;
    }
        break;
    
    default:
        LQTG_FETCH_ASSERT;
        return false;
    }
}

bool MessageMedia::push(OutboundPkt *out) const {
    out->appendInt(m_classType);
    switch(m_classType) {
    case typeMessageMediaEmpty: {
        return true;
    }
        break;
    
    case typeMessageMediaPhoto: {
        m_photo.push(out);
        out->appendQString(m_caption);
        return true;
    }
        break;
    
    case typeMessageMediaVideo: {
        m_video.push(out);
        out->appendQString(m_caption);
        return true;
    }
        break;
    
    case typeMessageMediaGeo: {
        m_geo.push(out);
        return true;
    }
        break;
    
    case typeMessageMediaContact: {
        out->appendQString(m_phoneNumber);
        out->appendQString(m_firstName);
        out->appendQString(m_lastName);
        out->appendInt(m_userId);
        return true;
    }
        break;
    
    case typeMessageMediaUnsupported: {
        return true;
    }
        break;
    
    case typeMessageMediaDocument: {
        m_document.push(out);
        return true;
    }
        break;
    
    case typeMessageMediaAudio: {
        m_audio.push(out);
        return true;
    }
        break;
    
    case typeMessageMediaWebPage: {
        m_webpage.push(out);
        return true;
    }
        break;
    
    case typeMessageMediaVenue: {
        m_geo.push(out);
        out->appendQString(m_title);
        out->appendQString(m_address);
        out->appendQString(m_provider);
        out->appendQString(m_venueId);
        return true;
    }
        break;
    
    default:
        return false;
    }
}

