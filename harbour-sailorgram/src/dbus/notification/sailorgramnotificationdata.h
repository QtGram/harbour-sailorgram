#ifndef SAILORGRAMNOTIFICATIONDATA_H
#define SAILORGRAMNOTIFICATIONDATA_H

#include <QObject>
#include <telegramengine.h>

class SailorgramNotificationData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramEngine* engine READ engine CONSTANT FINAL)

    public:
        explicit SailorgramNotificationData(const QString &peerhex, TelegramEngine* engine, QObject *parent = 0);
        TelegramEngine* engine() const;

    private:
        QString _peerhex;
        TelegramEngine* _engine;
};

#endif // SAILORGRAMNOTIFICATIONDATA_H
