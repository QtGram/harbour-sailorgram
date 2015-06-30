#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QObject>
#include <QHash>
#include <QImage>
#include <QPair>
#include <QtDBus>

class Notifications : public QObject
{
    Q_OBJECT

    public:
        explicit Notifications(QObject *parent = 0);

    public slots:
        void send(const QString& summary, const QString& body, const QString &icon, bool feedback, bool temporary);
        void send(const QString& summary, const QString& body);
        void beep();
};

#endif // NOTIFICATIONS_H
