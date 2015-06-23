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

#ifndef INPUTCHATPHOTO_H
#define INPUTCHATPHOTO_H

#include "inputphoto.h"
#include "inputfile.h"
#include "inputphotocrop.h"

class InputChatPhoto
{
public:

    enum InputChatPhotoType {
       typeInputChatPhoto = 0xb2e1bf08,
       typeInputChatUploadedPhoto = 0x94254732,
       typeInputChatPhotoEmpty = 0x1ca48f57
    };

    InputChatPhoto(InputChatPhotoType classType) :
        m_file(InputFile::typeInputFile),
        m_id(InputPhoto::typeInputPhotoEmpty),
        m_crop(InputPhotoCrop::typeInputPhotoCropAuto),
        m_classType(classType) {}

    void setId(const InputPhoto &id) {
        m_id = id;
    }
    InputPhoto id() const {
        return m_id;
    }
    void setCrop(const InputPhotoCrop &crop) {
        m_crop = crop;
    }
    InputPhotoCrop crop() const {
        return m_crop;
    }
    void setFile(const InputFile &file) {
        m_file = file;
    }
    InputFile file() const {
        return m_file;
    }
    void setClassType(InputChatPhotoType classType) {
        m_classType = classType;
    }
    InputChatPhotoType classType() const {
        return m_classType;
    }

private:
    InputFile m_file;
    InputPhoto m_id;
    InputPhotoCrop m_crop;
    InputChatPhotoType m_classType;
};
#endif // INPUTCHATPHOTO_H
