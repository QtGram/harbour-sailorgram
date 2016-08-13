#include <QtQuick>
#include <sailfishapp.h>
#include <telegramqmlinitializer.h>
#include "dbus/screenblank.h"
#include "selector/audiorecorder.h"
#include "selector/thumbnailprovider.h"
#include "selector/filesmodel.h"
#include "telegram/sailorgraminitializer.h"
#include "sailorgram.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
    application->setApplicationName("harbour-sailorgram");
    application->setApplicationVersion("0.8");
    application->addLibraryPath(QString("%1/../share/%2/lib").arg(qApp->applicationDirPath(), qApp->applicationName()));

    QStringList args = application->arguments();
    bool daemonized = args.contains("-daemon");

    if(daemonized && !SailorGram::hasDaemonFile())
        return 0;

    QDBusConnection sessionbus = QDBusConnection::sessionBus();

    if(sessionbus.interface()->isServiceRegistered(SailorgramInterface::INTERFACE_NAME)) // Only a Single Instance is allowed
    {
        SailorgramInterface::sendWakeUp();

        if(application->hasPendingEvents())
            application->processEvents();

        return 0;
    }

    FilesModel::registerMetaTypes();
    TelegramQmlInitializer::init("harbour.sailorgram.TelegramQml");
    SailorgramInitializer::init("harbour.sailorgram.Telegram", 1, 0);

    qmlRegisterType<SailorGram>("harbour.sailorgram.SailorGram", 1, 0, "SailorGram");
    qmlRegisterType<ScreenBlank>("harbour.sailorgram.DBus", 1, 0, "ScreenBlank");
    qmlRegisterType<AudioRecorder>("harbour.sailorgram.Selector", 1, 0, "AudioRecorder");
    qmlRegisterType<FilesModel>("harbour.sailorgram.FilesModel", 1, 0, "FilesModel");
    qmlRegisterType<TranslationInfoItem>("harbour.sailorgram.Model", 1, 0, "TranslationItem");

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
