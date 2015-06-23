#include "telegramcalendar.h"

TelegramCalendar::TelegramCalendar(QObject *parent): QObject(parent)
{
}

QDateTime TelegramCalendar::fromTime_t(qint32 timestamp)
{
    return QDateTime::fromTime_t(timestamp);
}

QString TelegramCalendar::timeToString(qint32 timestamp)
{
    QDateTime datetime = QDateTime::fromTime_t(timestamp);

    if(QDate::currentDate() == datetime.date())
        return datetime.toString("HH:mm");
    else if(datetime.date().daysTo(QDate::currentDate()) < 7)
        return datetime.toString("ddd HH:mm");
    else if(datetime.date().year() == QDate::currentDate().year())
        return datetime.toString("dd MMM");

    return datetime.toString("dd MMM yy");
}

QObject *TelegramCalendar::initialize(QQmlEngine *, QJSEngine *)
{
    return new TelegramCalendar();
}
