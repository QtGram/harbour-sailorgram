#ifndef SAILORGRAM_H
#define SAILORGRAM_H

#include <QGuiApplication>
#include <QStandardPaths>
#include <QFile>
#include <QMimeDatabase>
#include <QNetworkConfigurationManager>
#include <telegramengine.h>
#include <telegram/objects/dialogobject.h>
#include <telegram/objects/messageobject.h>
#include "dbus/interface/sailorgraminterface.h"
#include "dbus/notification/notification.h"
#include "dbus/connectivitychecker.h"
#include "item/translationinfoitem.h"

class SailorGram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool globalMute READ globalMute WRITE setGlobalMute NOTIFY globalMuteChanged)
    Q_PROPERTY(bool keepRunning READ keepRunning WRITE setKeepRunning NOTIFY keepRunningChanged)
    Q_PROPERTY(bool daemonized READ daemonized NOTIFY daemonizedChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(QString emojiPath READ emojiPath CONSTANT FINAL)
    Q_PROPERTY(QString configPath READ configPath CONSTANT FINAL)
    Q_PROPERTY(QString publicKey READ publicKey CONSTANT FINAL)
    Q_PROPERTY(QString homeFolder READ homeFolder CONSTANT FINAL)
    Q_PROPERTY(QString sdcardFolder READ sdcardFolder CONSTANT FINAL)
    Q_PROPERTY(QString picturesFolder READ picturesFolder CONSTANT FINAL)
    Q_PROPERTY(QString androidStorage READ androidStorage CONSTANT FINAL)
    Q_PROPERTY(QString voiceRecordPath READ voiceRecordPath CONSTANT FINAL)
    Q_PROPERTY(QString currentPeerKey READ currentPeerKey WRITE setCurrentPeerKey NOTIFY currentPeerKeyChanged)
    Q_PROPERTY(QList<QObject *> translations READ translations CONSTANT FINAL)

    public:
        explicit SailorGram(QObject *parent = 0);
        bool autostart();
        bool globalMute() const;
        bool keepRunning() const;
        bool daemonized() const;
        bool connected() const;
        int interval() const;
        QString emojiPath() const;
        QString configPath() const;
        QString publicKey() const;
        QString homeFolder() const;
        QString sdcardFolder() const;
        QString picturesFolder() const;
        QString androidStorage() const;
        QString voiceRecordPath() const;
        const QString& currentPeerKey() const;
        TelegramEngine* engine() const;
        QList<QObject *> translations();
        void setEngine(TelegramEngine* engine);
        void setCurrentPeerKey(const QString& peerkey);
        void setKeepRunning(bool keep);
        void setGlobalMute(bool globalmute);
        void setAutostart(bool autostart);

    public:
        static bool hasDaemonFile();

    public slots:
        void moveMediaToDownloads(const QString &mediafile);
        void moveMediaToGallery(const QString &mediafile, quint32 mediatype);
        void notify(const QString& title, const QString& body, const QString &peerkey);
        void closeNotification(const QString& peerkey);

    private:
        QString mediaLocation(quint32 mediatype);
        void updatePendingState(const QString& peerkey);
        void moveMediaTo(const QString &mediafile, const QString& destination);
        void sendNotify(const QString& title, const QString& body, const QString &peerkey);
        void beep();

    private slots:
        void onApplicationStateChanged(Qt::ApplicationState state);
        void onNotificationClosed(uint);
        void onMessageUnreadedChanged();
        void onWakeUpRequested();
        void updateOnlineState();
        void delayedCloseNotification();

    signals:
        void globalMuteChanged();
        void autostartChanged();
        void keepRunningChanged();
        void daemonizedChanged();
        void connectedChanged();
        void engineChanged();
        void wakeUpRequested();
        void currentPeerKeyChanged();
        void openDialogRequested(QString peerkey);

    private:
        QHash<int, MessageObject*> _deletednotifications;
        QHash<QString, Notification*> _notifications;
        QHash<QString, MessageObject*> _topmessages;
        QMimeDatabase _mimedb;
        TelegramEngine* _engine;
        ConnectivityChecker* _connectivitychecker;
        QNetworkConfigurationManager* _netcfgmanager;
        SailorgramInterface* _interface;
        QString _currentpeerkey;
        int _connected;
        bool _daemonized;
        bool _globalmute;
        bool _autostart;

    private:
        static const QString DAEMON_FILE;
        static const QString CONFIG_FOLDER;
        static const QString PUBLIC_KEY_FILE;
        static const QString EMOJI_FOLDER;
        static const QString APPLICATION_PRETTY_NAME;
};

#endif // SAILORGRAM_H
