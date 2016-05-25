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

#include <functional>

#include <QObject>
#include <QThread>
#include <QHash>
#include <QPointer>

#include "telegramthumbnailercore.h"
#include "telegramqml_global.h"

typedef std::function<void ()> TelegramThumbnailer_Callback;

class TELEGRAMQMLSHARED_EXPORT TelegramThumbnailer : public QObject
{
    Q_OBJECT

public:
    TelegramThumbnailer(QObject *parent = 0);
    ~TelegramThumbnailer();

    QString getThumbFilename(const QString &filePath) const;
    QString getThumbPath(const QString &thumbnailDirPath, const QString &filePath) const;
    bool hasThumbnail(const QString &thumbnailDirPath, const QString &filePath) const;
    bool hasThumbnail(const QString &thumbPath) const;
    void createThumbnail(const QString &source, const QString &dest, TelegramThumbnailer_Callback callback);

private Q_SLOTS:
    void thumbnailCreated(QString path);

private:
    QHash<QString, TelegramThumbnailer_Callback> requests;

    QThread *thread;
    TelegramThumbnailerCore *core;
};
