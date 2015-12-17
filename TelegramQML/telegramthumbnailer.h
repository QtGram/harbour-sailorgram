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

#if __cplusplus >= 201103L
#define TG_THUMBNAILER_CPP11
#endif

#ifdef TG_THUMBNAILER_CPP11
typedef std::function<void ()> TelegramThumbnailer_Callback;
#else
class TelegramThumbnailer_Callback
{
public:
    QPointer<QObject> object;
    QString method;
    QVariantList args;
};
#endif

class TelegramThumbnailer : public QObject
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
    static QVariant call( QObject *obj, const QString & member, Qt::ConnectionType type, QVariantList vals);

private:
    QHash<QString, TelegramThumbnailer_Callback> requests;

    QThread *thread;
    TelegramThumbnailerCore *core;
};
