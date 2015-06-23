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

#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QString>
#include "photosize.h"

class WallPaper
{
public:

    enum WallPaperType {
       typeWallPaper = 0xccb03657,
       typeWallPaperSolid = 0x63117f24
    };

    WallPaper(WallPaperType classType) :
        m_bgColor(0),
        m_color(0),
        m_id(0),
        m_title(""),
        m_classType(classType) {}

    void setId(qint32 id) {
        m_id = id;
    }
    qint32 id() const {
        return m_id;
    }
    void setTitle(const QString & title) {
        m_title = title;
    }
    QString title() const {
        return m_title;
    }
    void setSizes(const QList<PhotoSize> & sizes) {
        m_sizes = sizes;
    }
    QList<PhotoSize> sizes() const {
        return m_sizes;
    }
    void setColor(qint32 color) {
        m_color = color;
    }
    qint32 color() const {
        return m_color;
    }
    void setBgColor(qint32 bgColor) {
        m_bgColor = bgColor;
    }
    qint32 bgColor() const {
        return m_bgColor;
    }
    void setClassType(WallPaperType classType) {
        m_classType = classType;
    }
    WallPaperType classType() const {
        return m_classType;
    }

private:
    qint32 m_bgColor;
    qint32 m_color;
    qint32 m_id;
    QString m_title;
    QList<PhotoSize> m_sizes;
    WallPaperType m_classType;
};
#endif // WALLPAPER_H
