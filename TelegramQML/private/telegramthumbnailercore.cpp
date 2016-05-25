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
#include <QMimeDatabase>
#include <QMimeType>
#include <QImageWriter>
#include <QUuid>
#include <QDir>
#include <QDebug>

#ifdef UBUNTU_PHONE
#include <stdexcept>
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
    QMimeType t = QMimeDatabase().mimeTypeForFile(source);
    if(t.name().contains("audio"))
        createAudioThumbnail(source, dest);
    else
    if(t.name().contains("video"))
        createVideoThumbnail(source, dest);

    Q_EMIT thumbnailCreated(source);
}
#endif

bool TelegramThumbnailerCore::createVideoThumbnail(const QString &source, const QString &dest)
{
    QString command;
#if defined(Q_OS_WIN)
    command = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
#elif defined(Q_OS_MAC)
    command = QCoreApplication::applicationDirPath() + "/ffmpeg";
#else
    if (!QFileInfo::exists(command)) {
        if(QFileInfo::exists("/usr/bin/avconv"))
            command = "/usr/bin/avconv";
        else
            command = "ffmpeg";
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
    prc.start(command, args);
    prc.waitForStarted();
    prc.waitForFinished();

    if (prc.exitCode() != 0) {
        QImage image;
        image.save(dest, "JPEG");
        return false;
    }

    return true;
}

bool TelegramThumbnailerCore::createAudioThumbnail(const QString &source, const QString &dest)
{
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    Q_UNUSED(audio)
    Q_UNUSED(output)
    return false;
#endif

    const QString command = "eyeD3";
    const QString coverName = "FRONT_COVER";
    const QString uuid = QUuid::createUuid().toString();
    const QString tmpDir = QDir::tempPath() + "/" + uuid;

    QDir().mkpath(tmpDir);

    QStringList args;
    args << "--write-images=" + tmpDir;
    args << source;

    QProcess prc;
    prc.start(command, args);
    prc.waitForStarted();
    prc.waitForFinished();

    if(prc.exitCode() != 0)
    {
        removeFiles(tmpDir);
        return false;
    }

    QString file;
    const QStringList files = QDir(tmpDir).entryList(QDir::Files);
    Q_FOREACH(const QString &f, files)
        if(f.left(coverName.length()) == coverName)
        {
            file = tmpDir + "/" + f;
            break;
        }

    if(file.isEmpty())
    {
        removeFiles(tmpDir);
        return false;
    }

    QImageWriter writer(dest);
    writer.write(QImage(file));

    removeFiles(tmpDir);
    return true;
}

void TelegramThumbnailerCore::removeFiles(const QString &dir)
{
    const QStringList dirs = QDir(dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    Q_FOREACH(const QString &d, dirs)
        removeFiles(dir + "/" + d);

    const QStringList files = QDir(dir).entryList(QDir::Files);
    Q_FOREACH(const QString &f, files)
        QFile::remove(dir + "/" + f);

    QDir().rmdir(dir);
}
