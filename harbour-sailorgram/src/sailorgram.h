#ifndef SAILORGRAM_H
#define SAILORGRAM_H

#include <QGuiApplication>
#include <QUrl>
#include <QFile>
#include <QStandardPaths>
#include <QImage>
#include <QMimeDatabase>
#include <telegramqml.h>
#include <userdata.h>
#include <objects/types.h>
#include "heartbeat.h"
#include "dbus/interface/sailorgraminterface.h"
#include "dbus/notification/notification.h"

class SailorGram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool keepRunning READ keepRunning WRITE setKeepRunning NOTIFY keepRunningChanged)
    Q_PROPERTY(bool daemonized READ daemonized NOTIFY daemonizedChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(QString emojiPath READ emojiPath CONSTANT FINAL)
    Q_PROPERTY(QString configPath READ configPath CONSTANT FINAL)
    Q_PROPERTY(QString publicKey READ  publicKey CONSTANT FINAL)
    Q_PROPERTY(DialogObject* foregroundDialog READ foregroundDialog WRITE setForegroundDialog NOTIFY foregroundDialogChanged)

    public:
        explicit SailorGram(QObject *parent = 0);
        bool autostart();
        bool keepRunning() const;
        bool daemonized() const;
        bool connected() const;
        int interval() const;
        QString emojiPath() const;
        QString configPath() const;
        QString publicKey() const;
        TelegramQml* telegram() const;
        DialogObject* foregroundDialog() const;
        void setInterval(int interval);
        void setTelegram(TelegramQml* telegram);
        void setForegroundDialog(DialogObject* dialog);
        void setKeepRunning(bool keep);
        void setAutostart(bool autostart);

    public:
        static bool hasNoDaemonFile();

    public slots:
        bool fileIsPhoto(const QString& filepath);
        bool fileIsVideo(const QString& filepath);
        void moveMediaToDownloads(MessageMediaObject* messagemediaobject);
        void moveMediaToGallery(MessageMediaObject* messagemediaobject);
        void notify(MessageObject* message, const QString& name, const QString &elaboratedbody);
        QString fileName(const QString& filepath);
        FileLocationObject *mediaLocation(MessageMediaObject* messagemediaobject);

    private:
        void moveMediaTo(FileLocationObject* locationobj, const QString& destination);
        void notify(const QString& summary, const QString& body, const QString &icon, qint32 peerid);
        void beep();

    private slots:
        void onApplicationStateChanged(Qt::ApplicationState state);
        void onNotificationClosed(uint);
        void onWakeUpRequested();
        void startHeartBeat();
        void wakeSleep();

    signals:
        void autostartChanged();
        void keepRunningChanged();
        void daemonizedChanged();
        void connectedChanged();
        void intervalChanged();
        void telegramChanged();
        void wakeUpRequested();
        void foregroundDialogChanged();
        void openDialogRequested(qint32 peerid);

    private:
        QHash<qint32, Notification*> _notifications;
        QMimeDatabase _mimedb;
        TelegramQml* _telegram;
        HeartBeat* _heartbeat;
        SailorgramInterface* _interface;
        DialogObject* _foregrounddialog;
        bool _daemonized;
        bool _autostart;

    private:
        static const QString NO_DAEMON_FILE;
        static const QString CONFIG_FOLDER;
        static const QString PUBLIC_KEY_FILE;
        static const QString EMOJI_FOLDER;
        static const QString APPLICATION_PRETTY_NAME;
};

#endif // SAILORGRAM_H
