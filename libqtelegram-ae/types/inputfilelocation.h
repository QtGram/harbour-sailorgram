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

#ifndef INPUTFILELOCATION_H
#define INPUTFILELOCATION_H

#include <QtGlobal>

class InputFileLocation
{
public:

    enum InputFileLocationType {
       typeInputVideoFileLocation = 0x3d0364ec,
       typeInputEncryptedFileLocation = 0xf5235d55,
       typeInputFileLocation = 0x14637196,
       typeInputAudioFileLocation = 0x74dc404d,
       typeInputDocumentFileLocation = 0x4e45abe9
    };

    InputFileLocation(InputFileLocationType classType = typeInputFileLocation) :
        m_id(0),
        m_secret(0),
        m_accessHash(0),
        m_volumeId(0),
        m_classType(classType),
        m_localId(0) {}

    void setId(qint64 id) {
        m_id = id;
    }
    qint64 id() const {
        return m_id;
    }
    void setAccessHash(qint64 accessHash) {
        m_accessHash = accessHash;
    }
    qint64 accessHash() const {
        return m_accessHash;
    }
    void setVolumeId(qint64 volumeId) {
        m_volumeId = volumeId;
    }
    qint64 volumeId() const {
        return m_volumeId;
    }
    void setLocalId(qint32 localId) {
        m_localId = localId;
    }
    qint32 localId() const {
        return m_localId;
    }
    void setSecret(qint64 secret) {
        m_secret = secret;
    }
    qint64 secret() const {
        return m_secret;
    }
    void setClassType(InputFileLocationType classType) {
        m_classType = classType;
    }
    InputFileLocationType classType() const {
        return m_classType;
    }

private:
    qint64 m_id;
    qint64 m_secret;
    qint64 m_accessHash;
    qint64 m_volumeId;
    InputFileLocationType m_classType;
    qint32 m_localId;
};
#endif // INPUTFILELOCATION_H
