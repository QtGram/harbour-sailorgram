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

#ifndef MESSAGESFILTER_H
#define MESSAGESFILTER_H

class MessagesFilter
{
public:

    enum MessagesFilterType {
       typeInputMessagesFilterPhotos = 0x9609a51c,
       typeInputMessagesFilterVideo = 0x9fc00e65,
       typeInputMessagesFilterDocument = 0x9eddf188,
       typeInputMessagesFilterEmpty = 0x57e2f66c,
       typeInputMessagesFilterPhotoVideo = 0x56e9f0e4,
       typeInputMessagesFilterAudio = 0xcfc87522
    };

    MessagesFilter(MessagesFilterType classType) :
        m_classType(classType) {}

    void setClassType(MessagesFilterType classType) {
        m_classType = classType;
    }
    MessagesFilterType classType() const {
        return m_classType;
    }

private:
    MessagesFilterType m_classType;
};
#endif // MESSAGESFILTER_H
