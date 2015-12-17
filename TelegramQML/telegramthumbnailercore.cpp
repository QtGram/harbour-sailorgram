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

#include "telegramthumbnailercore.h"

#include <QFileInfo>
#include <QProcess>
#include <QStringList>
#include <QImage>
#include <QCoreApplication>

#ifdef UBUNTU_PHONE
#include <stdexcept>
#include <QDebug>
#include <QSize>
#include <QSharedPointer>
#include "thumbnailer-qt.h"
#endif

// https://core.telegram.org/constructor/decryptedMessageMediaPhoto
// Content of thumbnail file (JPEGfile, quality 55, set in a square 90x90)
const int THUMB_QUAILTY = 55;
const int THUMB_SIZE    = 90;

TelegramThumbnailerCore::TelegramThumbnailerCore(QObject *parent) : QObject(parent) {
}

TelegramThumbnailerCore::~TelegramThumbnailerCore() {
}

#ifdef UBUNTU_PHONE
void TelegramThumbnailerCore::createThumbnail(QString source, QString dest) {
    try {
        QSize size(THUMB_SIZE, THUMB_SIZE);
        unity::thumbnailer::qt::Thumbnailer thumbnailer;

        QSharedPointer<unity::thumbnailer::qt::Request> request =
                thumbnailer.getThumbnail(source, size);
        // We're on a generic thumbnailer thread, safe to call blocking method.
        request->waitForFinished();
        if (request->isValid()) {
            QImage image = request->image();
            image.save(dest, "JPEG", THUMB_QUAILTY);

            Q_EMIT thumbnailCreated(source);
            return;
        }
    } catch (const std::runtime_error &e) {
        qCritical() << "Failed to generate thumbnail!" << e.what();
    } catch (...) {
        qCritical() << "Failed to generate thumbnail!";
    }
    // Failed to create thumbnail, empty thumb to avoid calling back in here.
    QImage image(THUMB_SIZE, THUMB_SIZE, QImage::Format_ARGB32);
    image.fill(0);
    image.save(dest, "JPEG", THUMB_QUAILTY);
    qWarning() << "created thumbnail placeholder";

    Q_EMIT thumbnailCreated(source);
}
#endif

#ifndef UBUNTU_PHONE
void TelegramThumbnailerCore::createThumbnail(QString source, QString dest) {
    QString ffmpegPath;
#if defined(Q_OS_WIN)
    ffmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
#elif defined(Q_OS_MAC)
    ffmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg";
#else
    if (!QFileInfo::exists(ffmpegPath)) {
        if(QFileInfo::exists("/usr/bin/avconv"))
            ffmpegPath = "/usr/bin/avconv";
        else
            ffmpegPath = "ffmpeg";
    }
#endif

    QStringList args;
    args << "-itsoffset" << "-4";
    args << "-i" << source;
    args << "-vcodec" << "mjpeg";
    args << "-vframes" << "1";
    args << "-an";
    args << "-f" << "rawvideo";
    args << dest;
    args << "-y";

    QProcess prc;
    prc.start(ffmpegPath, args);
    prc.waitForStarted();
    prc.waitForFinished();

    if (prc.exitCode() == 0) {
        Q_EMIT thumbnailCreated(source);
    } else {
        QImage image;
        image.save(dest, "JPEG");
        Q_EMIT thumbnailCreated(source);
    }
}
#endif
