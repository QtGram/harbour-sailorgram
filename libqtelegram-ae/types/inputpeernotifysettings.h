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

#ifndef INPUTPEERNOTIFYSETTINGS_H
#define INPUTPEERNOTIFYSETTINGS_H

#include <QString>

class InputPeerNotifySettings
{
public:

    enum InputPeerNotifySettingsType {
       typeInputPeerNotifySettings = 0x46a2ce98
    };

    InputPeerNotifySettings() :
        m_muteUntil(0),
        m_eventsMask(0),
        m_sound(""),
        m_showPreviews(false),
        m_classType(typeInputPeerNotifySettings) {}

    void setMuteUntil(qint32 muteUntil) {
        m_muteUntil = muteUntil;
    }
    qint32 muteUntil() const {
        return m_muteUntil;
    }
    void setSound(const QString & sound) {
        m_sound = sound;
    }
    QString sound() const {
        return m_sound;
    }
    void setShowPreviews(bool showPreviews) {
        m_showPreviews = showPreviews;
    }
    bool showPreviews() const {
        return m_showPreviews;
    }
    void setEventsMask(qint32 eventsMask) {
        m_eventsMask = eventsMask;
    }
    qint32 eventsMask() const {
        return m_eventsMask;
    }
    void setClassType(InputPeerNotifySettingsType classType) {
        m_classType = classType;
    }
    InputPeerNotifySettingsType classType() const {
        return m_classType;
    }

private:
    qint32 m_muteUntil;
    qint32 m_eventsMask;
    QString m_sound;
    bool m_showPreviews;
    InputPeerNotifySettingsType m_classType;
};
#endif // INPUTPEERNOTIFYSETTINGS_H
