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

#ifndef INPUTPHOTO_H
#define INPUTPHOTO_H

#include <QtGlobal>

class InputPhoto
{
public:

    enum InputPhotoType {
       typeInputPhoto = 0xfb95c6c4,
       typeInputPhotoEmpty = 0x1cd7bf0d
    };

    InputPhoto(InputPhotoType classType) :
        m_id(0),
        m_accessHash(0),
        m_classType(classType) {}

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
    void setClassType(InputPhotoType classType) {
        m_classType = classType;
    }
    InputPhotoType classType() const {
        return m_classType;
    }

private:
    qint64 m_id;
    qint64 m_accessHash;
    InputPhotoType m_classType;
};
#endif // INPUTPHOTO_H
