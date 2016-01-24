/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QtQuick>
#include <sailfishapp.h>
#include <telegramqmlinitializer.h>
#include "dbus/screenblank.h"
#include "selector/audiorecorder.h"
#include "selector/thumbnailprovider.h"
#include "selector/imagesmodel.h"
#include "model/dialogshistorymodel.h"
#include "sailorgram.h"

bool hasDaemon(const QStringList& args)
{
    foreach(QString arg, args)
    {
        if(arg == "-daemon")
            return true;
    }

    return false;
}

void setPluginsPath(QScopedPointer<QGuiApplication>& application)
{
    application->addLibraryPath(qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + "lib");
}

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
    application->setApplicationName("harbour-sailorgram");
    application->setApplicationVersion("0.7");
    setPluginsPath(application);

    QStringList args = application->arguments();
    bool daemonized = hasDaemon(args);

    if(daemonized && SailorGram::hasNoDaemonFile())
        return 0;

    QDBusConnection sessionbus = QDBusConnection::sessionBus();

    if(sessionbus.interface()->isServiceRegistered(SailorgramInterface::INTERFACE_NAME)) // Only a Single Instance is allowed
    {
        SailorgramInterface::sendWakeUp();

        if(application->hasPendingEvents())
            application->processEvents();

        return 0;
    }

    TelegramQmlInitializer::init("harbour.sailorgram.TelegramQml");

    qmlRegisterType<SailorGram>("harbour.sailorgram.SailorGram", 1, 0, "SailorGram");
    qmlRegisterType<ScreenBlank>("harbour.sailorgram.DBus", 1, 0, "ScreenBlank");
    qmlRegisterType<AudioRecorder>("harbour.sailorgram.Selector", 1, 0, "AudioRecorder");
    qmlRegisterType<ImagesModel>("harbour.sailorgram.ImagesModel", 1, 0, "ImagesModel");
    qmlRegisterType<DialogsHistoryModel>("harbour.sailorgram.DialogsHistoryModel", 1, 0, "DialogsHistoryModel");

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    QQmlEngine* engine = view->engine();
    QObject::connect(engine, SIGNAL(quit()), application.data(), SLOT(quit()));
    engine->addImageProvider(QStringLiteral("thumbnail"), new ThumbnailProvider);

    view->setSource(SailfishApp::pathTo("qml/harbour-sailorgram.qml"));

    if(daemonized)
        application->setQuitOnLastWindowClosed(false);
    else
        view->show();

    return application->exec();
}
