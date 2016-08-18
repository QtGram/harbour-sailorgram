#ifndef SAILORGRAMNOTIFICATIONS_H
#define SAILORGRAMNOTIFICATIONS_H

#include <QObject>
#include <QHash>
#include <telegramengine.h>
#include <telegramnotificationhandler.h>
#include "model/sailorgramdialogsmodel.h"
#include "../dbus/notification/notification.h"

class SailorgramNotifications : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(SailorgramDialogsModel* dialogs READ dialogs WRITE setDialogs NOTIFY dialogsChanged)
    Q_PROPERTY(SailorgramDialogItem* currentDialog READ currentDialog WRITE setCurrentDialog NOTIFY currentDialogChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int globalUnreadCount READ globalUnreadCount NOTIFY globalUnreadCountChanged)
    Q_PROPERTY(bool globalMute READ globalMute WRITE setGlobalMute NOTIFY globalMuteChanged)

    public:
        explicit SailorgramNotifications(QObject *parent = 0);
        TelegramEngine* engine() const;
        SailorgramDialogsModel* dialogs() const;
        SailorgramDialogItem* currentDialog() const;
        int unreadCount() const;
        int globalUnreadCount() const;
        bool globalMute() const;
        void setEngine(TelegramEngine* engine);
        void setDialogs(SailorgramDialogsModel* dialogs);
        void setCurrentDialog(SailorgramDialogItem* dialog);
        void setGlobalMute(bool b);

    private:
        void init();
        void insertUpdate(const Update& update);
        void messageReaded(const QByteArray& peerkey, qint32 maxid);
        void updatePendingState(const Message& message, const QByteArray& peerkey);
        void closeNotification(const QByteArray &peerkey);
        void notify(const Message &message, const QByteArray &peerkey, bool ischannel);
        void sendNotify(const Message &message, const QByteArray &peerkey, bool ischannel);
        void beep();
        QString notificationTitle(const Message &message, bool ischannel) const;
        QString notificationBody(const Message& message) const;

    public slots:
        void resetCurrentDialog();

    private slots:
        void connectSignals();
        void onUpdates(const UpdatesType& updates);
        void delayCloseNotification();
        void onNotificationClosed(uint);

    signals:
        void engineChanged();
        void dialogsChanged();
        void currentDialogChanged();
        void unreadCountChanged();
        void globalUnreadCountChanged();
        void globalMuteChanged();

    public:
        static const QString APPLICATION_PRETTY_NAME;

    private:
        TelegramEngine* _engine;
        SailorgramDialogsModel* _dialogs;
        SailorgramDialogItem* _currentdialog;
        TelegramNotificationHandler* _notificationhandler;
        QHash<QByteArray, Notification*> _notifications;
        QHash<QByteArray, qint32> _topmessageids;
        QHash<int, QByteArray> _deletednotifications;
        bool _globalmute;
};

#endif // SAILORGRAMNOTIFICATIONS_H
