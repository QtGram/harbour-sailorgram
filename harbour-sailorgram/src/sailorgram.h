#ifndef SAILORGRAM_H
#define SAILORGRAM_H

#include <QGuiApplication>
#include "dbus/interface/sailorgraminterface.h"
#include "dbus/notification/notification.h"
#include "item/translationinfoitem.h"

class SailorGram : public QObject
{
    Q_OBJECT

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
    Q_PROPERTY(QList<QObject *> translations READ translations CONSTANT FINAL)

    public:
        explicit SailorGram(QObject *parent = 0);
        bool autostart();
        bool keepRunning() const;
        bool daemonized() const;
        QString emojiPath() const;
        QString publicKey() const;
        QString homeFolder() const;
        QString sdcardFolder() const;
        QString picturesFolder() const;
        QString androidStorage() const;
        QString voiceRecordPath() const;
        QList<QObject *> translations();
        void setKeepRunning(bool keep);
        void setAutostart(bool autostart);

    public:
        static bool hasDaemonFile();

    private slots:
        void onApplicationStateChanged(Qt::ApplicationState state);
        void onWakeUpRequested();

    signals:
        void autostartChanged();
        void keepRunningChanged();
        void daemonizedChanged();
        void wakeUpRequested();
        void openDialogRequested(QByteArray peerkey);

    private:
        SailorgramInterface* _interface;
        bool _daemonized;
        bool _autostart;

    private:
        static const QString DAEMON_FILE;
        static const QString PUBLIC_KEY_FILE;
        static const QString EMOJI_FOLDER;
};

#endif // SAILORGRAM_H
