// This file generated by libqtelegram-code-generator.
// You can download it from: https://github.com/Aseman-Land/libqtelegram-code-generator
// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN

#include "encryptedfile.h"
#include "core/inboundpkt.h"
#include "core/outboundpkt.h"
#include "../coretypes.h"

EncryptedFile::EncryptedFile(EncryptedFileType classType, InboundPkt *in) :
    m_accessHash(0),
    m_dcId(0),
    m_id(0),
    m_keyFingerprint(0),
    m_size(0),
    m_classType(classType)
{
    if(in) fetch(in);
}

EncryptedFile::EncryptedFile(InboundPkt *in) :
    m_accessHash(0),
    m_dcId(0),
    m_id(0),
    m_keyFingerprint(0),
    m_size(0),
    m_classType(typeEncryptedFileEmpty)
{
    fetch(in);
}

EncryptedFile::~EncryptedFile() {
}

void EncryptedFile::setAccessHash(qint64 accessHash) {
    m_accessHash = accessHash;
}

qint64 EncryptedFile::accessHash() const {
    return m_accessHash;
}

void EncryptedFile::setDcId(qint32 dcId) {
    m_dcId = dcId;
}

qint32 EncryptedFile::dcId() const {
    return m_dcId;
}

void EncryptedFile::setId(qint64 id) {
    m_id = id;
}

qint64 EncryptedFile::id() const {
    return m_id;
}

void EncryptedFile::setKeyFingerprint(qint32 keyFingerprint) {
    m_keyFingerprint = keyFingerprint;
}

qint32 EncryptedFile::keyFingerprint() const {
    return m_keyFingerprint;
}

void EncryptedFile::setSize(qint32 size) {
    m_size = size;
}

qint32 EncryptedFile::size() const {
    return m_size;
}

bool EncryptedFile::operator ==(const EncryptedFile &b) {
    return m_accessHash == b.m_accessHash &&
           m_dcId == b.m_dcId &&
           m_id == b.m_id &&
           m_keyFingerprint == b.m_keyFingerprint &&
           m_size == b.m_size;
}

void EncryptedFile::setClassType(EncryptedFile::EncryptedFileType classType) {
    m_classType = classType;
}

EncryptedFile::EncryptedFileType EncryptedFile::classType() const {
    return m_classType;
}

bool EncryptedFile::fetch(InboundPkt *in) {
    LQTG_FETCH_LOG;
    int x = in->fetchInt();
    switch(x) {
    case typeEncryptedFileEmpty: {
        m_classType = static_cast<EncryptedFileType>(x);
        return true;
    }
        break;
    
    case typeEncryptedFile: {
        m_id = in->fetchLong();
        m_accessHash = in->fetchLong();
        m_size = in->fetchInt();
        m_dcId = in->fetchInt();
        m_keyFingerprint = in->fetchInt();
        m_classType = static_cast<EncryptedFileType>(x);
        return true;
    }
        break;
    
    default:
        LQTG_FETCH_ASSERT;
        return false;
    }
}

bool EncryptedFile::push(OutboundPkt *out) const {
    out->appendInt(m_classType);
    switch(m_classType) {
    case typeEncryptedFileEmpty: {
        return true;
    }
        break;
    
    case typeEncryptedFile: {
        out->appendLong(m_id);
        out->appendLong(m_accessHash);
        out->appendInt(m_size);
        out->appendInt(m_dcId);
        out->appendInt(m_keyFingerprint);
        return true;
    }
        break;
    
    default:
        return false;
    }
}

