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

#ifndef USERPROFILEPHOTO_H
#define USERPROFILEPHOTO_H

#include "filelocation.h"

class UserProfilePhoto
{
public:

    enum UserProfilePhotoType {
       typeUserProfilePhoto = 0xd559d8c8,
       typeUserProfilePhotoEmpty = 0x4f11bae1
    };

    UserProfilePhoto(UserProfilePhotoType classType) :
        m_photoId(0),
        m_photoBig(FileLocation::typeFileLocationUnavailable),
        m_photoSmall(FileLocation::typeFileLocationUnavailable) {
        m_classType = classType;
    }

    void setPhotoId(qint64 photoId) {
        m_photoId = photoId;
    }
    qint64 photoId() const {
        return m_photoId;
    }
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
    void setClassType(UserProfilePhotoType classType) {
        m_classType = classType;
    }
    UserProfilePhotoType classType() const {
        return m_classType;
    }

private:
    qint64 m_photoId;
    FileLocation m_photoBig;
    FileLocation m_photoSmall;
    UserProfilePhotoType m_classType;
};
#endif // USERPROFILEPHOTO_H
