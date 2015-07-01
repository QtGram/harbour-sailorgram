#ifndef SAILORGRAM_H
#define SAILORGRAM_H

#include <QObject>
#include <QFile>
#include <QStandardPaths>
#include <telegramqml.h>
#include <objects/types.h>

class SailorGram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        explicit SailorGram(QObject *parent = 0);
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);

    signals:
        void telegramChanged();

    public slots:
        void moveMediaToDownloads(MessageMediaObject* messagemediaobject);

    private:
        TelegramQml* _telegram;
};

#endif // SAILORGRAM_H
