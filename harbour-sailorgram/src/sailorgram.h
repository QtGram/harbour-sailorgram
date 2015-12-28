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

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(QString emojiPath READ emojiPath CONSTANT FINAL)

    public:
        explicit SailorGram(QObject *parent = 0);
        bool connected() const;
        QString emojiPath() const;
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);

    private:
        void moveMediaTo(FileLocationObject* locationobj, const QString& destination);

    public slots:
        bool fileIsPhoto(const QString& filepath);
        bool fileIsVideo(const QString& filepath);
        QString fileName(const QString& filepath);
        FileLocationObject *mediaLocation(MessageMediaObject* messagemediaobject);
        void moveMediaToDownloads(MessageMediaObject* messagemediaobject);
        void moveMediaToGallery(MessageMediaObject* messagemediaobject);

    private slots:
        void checkConnectionState();

    private:
        TelegramQml* _telegram;
        QMimeDatabase _mimedb;

    signals:
        void connectedChanged();
        void telegramChanged();

    private:
        static const QString EMOJI_FOLDER;
};

#endif // SAILORGRAM_H
