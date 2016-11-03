#ifndef SAILORGRAM_H
#define SAILORGRAM_H

#include <QGuiApplication>
#include <objects/notifications/telegramnotifications.h>
#include <objects/notifications/notificationobject.h>
#include "dbus/interface/sailorgraminterface.h"
#include "dbus/notification/notification.h"
#include "item/translationinfoitem.h"

class SailorGram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Telegram* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool keepRunning READ keepRunning WRITE setKeepRunning NOTIFY keepRunningChanged)
    Q_PROPERTY(bool daemonized READ daemonized NOTIFY daemonizedChanged)
    Q_PROPERTY(QString emojiPath READ emojiPath CONSTANT FINAL)
    Q_PROPERTY(QString publicKey READ publicKey CONSTANT FINAL)
    Q_PROPERTY(QString homeFolder READ homeFolder CONSTANT FINAL)
    Q_PROPERTY(QString sdcardFolder READ sdcardFolder CONSTANT FINAL)
    Q_PROPERTY(QString picturesFolder READ picturesFolder CONSTANT FINAL)
    Q_PROPERTY(QString androidStorage READ androidStorage CONSTANT FINAL)
    Q_PROPERTY(QString voiceRecordPath READ voiceRecordPath CONSTANT FINAL)
    Q_PROPERTY(TelegramNotifications* notifications READ notifications CONSTANT FINAL)
    Q_PROPERTY(QList<QObject *> translations READ translations CONSTANT FINAL)

    public:
        explicit SailorGram(QObject *parent = 0);
        Telegram* telegram() const;
        bool autostart() const;
        bool keepRunning() const;
        bool daemonized() const;
        QString emojiPath() const;
        QString publicKey() const;
        QString homeFolder() const;
        QString sdcardFolder() const;
        QString picturesFolder() const;
        QString androidStorage() const;
        QString voiceRecordPath() const;
        TelegramNotifications* notifications() const;
        QList<QObject *> translations() const;
        void setTelegram(Telegram* telegram);
        void setKeepRunning(bool keep);
        void setAutostart(bool autostart);

    public:
        static bool hasDaemonFile();

    private:
        void beep() const;

    private slots:
        void notify(const NotificationObject *notificationobj);
        void closeNotification(Dialog* dialog);
        void onApplicationStateChanged(Qt::ApplicationState state);
        void onNotificationClosed(uint);
        void onWakeUpRequested();

    signals:
        void telegramChanged();
        void autostartChanged();
        void keepRunningChanged();
        void daemonizedChanged();
        void wakeUpRequested();
        void openDialogRequested(TLInt dialogid);

    private:
        QHash<TLInt, Notification*> _notificationsmap;
        TelegramNotifications* _notifications;
        SailorgramInterface* _interface;
        Telegram* _telegram;
        bool _daemonized;
        bool _autostart;

    private:
        static const QString DAEMON_FILE;
        static const QString PUBLIC_KEY_FILE;
        static const QString EMOJI_FOLDER;
        static const QString APPLICATION_PRETTY_NAME;
};

#endif // SAILORGRAM_H
