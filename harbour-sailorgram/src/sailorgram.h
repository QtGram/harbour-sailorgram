#ifndef SAILORGRAM_H
#define SAILORGRAM_H

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QStandardPaths>
#include <QImage>
#include <QMimeDatabase>
#include <telegramqml.h>
#include <telegram.h>
#include <objects/types.h>

class SailorGram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        explicit SailorGram(QObject *parent = 0);
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);

    private:
        void moveMediaTo(FileLocationObject* locationobj, const QString& destination);

    signals:
        void telegramChanged();

    public slots:
        void sleep();
        void wake();
        bool fileIsPhoto(const QString& filepath);
        QString fileName(const QString& filepath);
        QSize imageSize(const QString& filepath);
        FileLocationObject *mediaLocation(MessageMediaObject* messagemediaobject);
        void moveMediaToDownloads(MessageMediaObject* messagemediaobject);
        void moveMediaToGallery(MessageMediaObject* messagemediaobject);

    private:
        TelegramQml* _telegram;
        QMimeDatabase _mimedb;
};

#endif // SAILORGRAM_H
