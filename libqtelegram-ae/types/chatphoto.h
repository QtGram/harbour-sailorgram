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

#ifndef CHATPHOTO_H
#define CHATPHOTO_H

#include "filelocation.h"

class ChatPhoto
{
public:

    enum ChatPhotoType {
       typeChatPhoto = 0x6153276a,
       typeChatPhotoEmpty = 0x37c1011c
    };

    ChatPhoto(ChatPhotoType classType) :
        m_photoBig(FileLocation::typeFileLocationUnavailable),
        m_photoSmall(FileLocation::typeFileLocationUnavailable),
        m_classType(classType) {}

    void setPhotoSmall(const FileLocation &photoSmall) {
        m_photoSmall = photoSmall;
    }
    FileLocation photoSmall() const {
        return m_photoSmall;
    }
    void setPhotoBig(const FileLocation &photoBig) {
        m_photoBig = photoBig;
    }
    FileLocation photoBig() const {
        return m_photoBig;
    }
    void setClassType(ChatPhotoType classType) {
        m_classType = classType;
    }
    ChatPhotoType classType() const {
        return m_classType;
    }

private:
    FileLocation m_photoBig;
    FileLocation m_photoSmall;
    ChatPhotoType m_classType;
};
#endif // CHATPHOTO_H
