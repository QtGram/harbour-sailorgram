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

#ifndef INPUTPHOTOCROP_H
#define INPUTPHOTOCROP_H
class InputPhotoCrop
{
public:

    enum InputPhotoCropType {
       typeInputPhotoCropAuto = 0xade6b004,
       typeInputPhotoCrop = 0xd9915325
    };

    InputPhotoCrop(InputPhotoCropType classType = typeInputPhotoCropAuto) :
        m_cropTop(0.0),
        m_cropLeft(0.0),
        m_cropWidth(0.0),
        m_classType(classType) {}

    void setCropLeft(double cropLeft) {
        m_cropLeft = cropLeft;
    }
    double cropLeft() const {
        return m_cropLeft;
    }
    void setCropTop(double cropTop) {
        m_cropTop = cropTop;
    }
    double cropTop() const {
        return m_cropTop;
    }
    void setCropWidth(double cropWidth) {
        m_cropWidth = cropWidth;
    }
    double cropWidth() const {
        return m_cropWidth;
    }
    void setClassType(InputPhotoCropType classType) {
        m_classType = classType;
    }
    InputPhotoCropType classType() const {
        return m_classType;
    }

private:
    double m_cropTop;
    double m_cropLeft;
    double m_cropWidth;
    InputPhotoCropType m_classType;
};
#endif // INPUTPHOTOCROP_H
