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

#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QMetaMethod>
#include "telegramthumbnailer.h"

TelegramThumbnailer::TelegramThumbnailer(QObject *parent) : QObject(parent)
{
    core = new TelegramThumbnailerCore();
    thread = new QThread(this);
    thread->start();

    core->moveToThread(thread);

    connect(core, &TelegramThumbnailerCore::thumbnailCreated, this, &TelegramThumbnailer::thumbnailCreated, Qt::QueuedConnection);
}

TelegramThumbnailer::~TelegramThumbnailer()
{
    thread->quit();
    thread->wait();

    thread->deleteLater();
    thread = 0;
    
    core->deleteLater();
    core = 0;
}

QString TelegramThumbnailer::getThumbFilename(const QString &filePath) const {
    QByteArray bytes(filePath.toStdString().c_str());
    QString filename = QString(QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex());
    return filename.append(".jpg");
}

QString TelegramThumbnailer::getThumbPath(const QString &thumbnailDirPath, const QString &filePath) const {
    QString filename = getThumbFilename(filePath);
    QDir temp(thumbnailDirPath);
    QString thumbPath = temp.filePath(filename);
    return thumbPath;
}

bool TelegramThumbnailer::hasThumbnail(const QString &thumbnailDirPath, const QString &filePath) const {
    QString thumbPath = getThumbPath(thumbnailDirPath, filePath);
    return hasThumbnail(thumbPath);
}

bool TelegramThumbnailer::hasThumbnail(const QString &thumbPath) const {
    QFile thumbnail(thumbPath);
    return thumbnail.exists();
}

void TelegramThumbnailer::createThumbnail(const QString &source, const QString &dest, TelegramThumbnailer_Callback callback) {
    requests.insert(source, callback);

    QMetaObject::invokeMethod(core, "createThumbnail", Qt::QueuedConnection,
            Q_ARG(QString,source), Q_ARG(QString,dest));
}

void TelegramThumbnailer::thumbnailCreated(QString path) {
    TelegramThumbnailer_Callback callback = requests.take(path);
    if (callback) {
        callback();
    }
}
