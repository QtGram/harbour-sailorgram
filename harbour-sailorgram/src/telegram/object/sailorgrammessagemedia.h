#ifndef SAILORGRAMMESSAGEMEDIA_H
#define SAILORGRAMMESSAGEMEDIA_H

#include <QObject>
#include <telegram/objects/messageobject.h>
#include <telegramengine.h>
#include "media/sailorgramwebpagemedia.h"
#include "media/sailorgramgeopointmedia.h"

class SailorgramMessageMedia : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString thumbnailPath READ thumbnailPath NOTIFY thumbnailPathChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QString fileMimeType READ fileMimeType NOTIFY fileMimeTypeChanged)
    Q_PROPERTY(qint32 fileSize READ fileSize NOTIFY fileSizeChanged)

    // Audio
    Q_PROPERTY(QString fileTitle READ fileTitle NOTIFY fileTitleChanged)
    Q_PROPERTY(QString filePerformer READ filePerformer NOTIFY filePerformerChanged)
    Q_PROPERTY(qint32 fileDuration READ fileDuration NOTIFY fileDurationChanged)
    Q_PROPERTY(bool isVoiceFile READ isVoiceFile NOTIFY isVoiceFileChanged)

    // WebPage
    Q_PROPERTY(SailorgramWebPageMedia* webPage READ webPage NOTIFY webPageChanged)

    // Geo, Venue
    Q_PROPERTY(SailorgramGeoPointMedia* geoPoint READ geoPoint NOTIFY geoPointChanged)

    // File Transfer
    Q_PROPERTY(bool isDownloadable READ isDownloadable NOTIFY isDownloadableChanged)
    Q_PROPERTY(bool isTransfered READ isTransfered NOTIFY isTransferedChanged)
    Q_PROPERTY(bool isTransfering READ isTransfering NOTIFY isTransferingChanged)
    Q_PROPERTY(float transferProgress READ transferProgress NOTIFY transferProgressChanged)

    // TelegramQML compatibility
    Q_PROPERTY(MessageMediaObject* rawMedia READ rawMedia NOTIFY rawMediaChanged)

    public:
        explicit SailorgramMessageMedia(QObject *parent = 0);
        SailorgramMessageMedia(TelegramEngine* engine, QObject* parent = 0);
        QString fileName() const;
        QString thumbnailPath() const;
        QString filePath() const;
        QString fileMimeType() const;
        qint32 fileSize() const;

    public: // Audio
        QString fileTitle() const;
        QString filePerformer() const;
        qint32 fileDuration() const;
        bool isVoiceFile() const;

    public: // WebPage
        SailorgramWebPageMedia* webPage() const;

    public: // Geo, Venue
        SailorgramGeoPointMedia* geoPoint() const;

    public: // File Transfer
        bool isDownloadable() const;
        bool isTransfered() const;
        bool isTransfering() const;
        float transferProgress() const;

    public: // TelegramQML compatibility
        MessageMediaObject* rawMedia() const;

    public: // C++ only setters
        void setFileName(const QString& filename);
        void setFilePath(const QString& filepath);
        void setFileMimeType(const QString& filemimetype);
        void setFileSize(qint32 filesize);
        void setThumbPath(const QString& thumbpath);
        void setFileTitle(const QString& filetitle);
        void setFilePerformer(const QString& fileperformer);
        void setFileDuration(qint32 fileduration);
        void setIsVoiceFile(bool b);
        void setIsDownloadable(bool b);
        void setIsTransfered(bool b);
        void setIsTransfering(bool b);
        void setTransferedSize(qint32 transferedsize);
        void setMediaObject(MessageMediaObject* messagemedia);

    public slots:
        void download();
        void stop();

    signals:
        void fileNameChanged();
        void filePathChanged();
        void fileMimeTypeChanged();
        void fileSizeChanged();
        void thumbnailPathChanged();
        void fileTitleChanged();
        void filePerformerChanged();
        void fileDurationChanged();
        void isVoiceFileChanged();
        void isDownloadableChanged();
        void isTransferedChanged();
        void isTransferingChanged();
        void transferProgressChanged();
        void webPageChanged();
        void geoPointChanged();
        void rawMediaChanged();
        void downloadRequest(bool start);

    private:
        TelegramEngine* _engine;
        MessageMediaObject* _messagemedia;
        SailorgramWebPageMedia* _webpage;
        SailorgramGeoPointMedia* _geopoint;
        QString _filename;
        QString _filepath;
        QString _filemimetype;
        QString _thumbpath;
        QString _filetitle;
        QString _fileperformer;
        qint32 _filesize;
        qint32 _fileduration;
        qint32 _transferedsize;
        bool _isvoicefile;
        bool _isdownloadable;
        bool _istransfered;
        bool _istransfering;
};

#endif // SAILORGRAMMESSAGEMEDIA_H
