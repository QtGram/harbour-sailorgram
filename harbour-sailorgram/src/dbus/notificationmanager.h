#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QGuiApplication>
#include <QObject>
#include <QHash>
#include <QImage>
#include <QPair>
#include <QtDBus>
#include <telegramqml.h>
#include <userdata.h>
#include <objects/types.h>

class NotificationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(DialogObject* foregroundDialog READ foregroundDialog WRITE setForegroundDialog NOTIFY foregroundDialogChanged)

    public:
        explicit NotificationManager(QObject *parent = 0);
        TelegramQml* telegram() const;
        DialogObject* foregroundDialog() const;
        void setTelegram(TelegramQml* telegram);
        void setForegroundDialog(DialogObject* dialog);

    private:
        QString completeName(UserObject* user) const;
        void send(const QString& summary, const QString& body, const QString &icon, bool feedback, bool temporary);
        void send(const QString& summary, const QString& body);
        void beep();

    public slots:
        void notify(MessageObject* message);

    signals:
        void telegramChanged();
        void foregroundDialogChanged();

    private:
        TelegramQml* _telegram;
        DialogObject* _foregrounddialog;

    private:
        static const QString APPLICATION_PRETTY_NAME;
};

#endif // NOTIFICATION_MANAGER_H
