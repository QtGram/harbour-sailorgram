#ifndef TELEGRAMCALENDAR_H
#define TELEGRAMCALENDAR_H

#include <QtQml>
#include <QObject>

class TelegramCalendar : public QObject
{
    Q_OBJECT

    public:
        explicit TelegramCalendar(QObject *parent = 0);
        Q_INVOKABLE QDateTime fromTime_t(qint32 timestamp);
        Q_INVOKABLE QString timeToString(qint32 timestamp);

    public:
        static QObject* initialize(QQmlEngine *, QJSEngine *);
};

#endif // TELEGRAMCALENDAR_H
