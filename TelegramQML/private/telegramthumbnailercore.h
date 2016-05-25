/*
    Copyright (C) 2015 Canonical

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QObject>
#include "telegramqml_global.h"

class TELEGRAMQMLSHARED_EXPORT TelegramThumbnailerCore : public QObject
{
    Q_OBJECT

public:
    TelegramThumbnailerCore(QObject *parent = 0);
    ~TelegramThumbnailerCore();

public Q_SLOTS:
    void createThumbnail(QString source, QString dest);

protected:
    bool createVideoThumbnail(const QString &source, const QString &dest);
    bool createAudioThumbnail(const QString &source, const QString &dest);

private:
    void removeFiles(const QString &dir);

Q_SIGNALS:
    void thumbnailCreated(QString path);
};
