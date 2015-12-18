#include "sailorgram.h"

const QString SailorGram::EMOJI_FOLDER = "emoji";

SailorGram::SailorGram(QObject *parent): QObject(parent), _telegram(NULL)
{
}

QString SailorGram::emojiPath() const
{
    return qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + SailorGram::EMOJI_FOLDER + QDir::separator();
}

TelegramQml *SailorGram::telegram() const
{
    return this->_telegram;
}

void SailorGram::setTelegram(TelegramQml *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    emit telegramChanged();
}

void SailorGram::moveMediaTo(FileLocationObject *locationobj, const QString &destination)
{
    if(!locationobj)
        return;

    QString filename = locationobj->fileName();
    QUrl location(locationobj->download()->location());

    if(filename.isEmpty())
        filename = location.fileName();

    QString destpath = destination + QDir::separator() + filename;

    if(QFile::exists(destpath)) // Don't overwrite existing files
        return;

    QFile::copy(location.path(), destpath);
}

void SailorGram::sleep()
{
    this->_telegram->telegram()->sleep();
}

void SailorGram::wake()
{
    this->_telegram->telegram()->wake();
}

bool SailorGram::fileIsPhoto(const QString &filepath)
{
    if(filepath.isEmpty())
        return false;

    QUrl url(filepath);
    QMimeType mime = this->_mimedb.mimeTypeForFile(url.path());
    return mime.isValid() && (mime.name().split("/")[0] == "image");
}

bool SailorGram::fileIsVideo(const QString &filepath)
{
    if(filepath.isEmpty())
        return false;

    QUrl url(filepath);
    QMimeType mime = this->_mimedb.mimeTypeForFile(url.path());
    return mime.isValid() && (mime.name().split("/")[0] == "video");
}

QString SailorGram::fileName(const QString &filepath)
{
    QUrl url(filepath);
    return url.fileName();
}

QSize SailorGram::imageSize(const QString &filepath)
{
    QUrl url(filepath);
    QImage img(url.path());
    return img.size();
}

FileLocationObject* SailorGram::mediaLocation(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return NULL;

    FileLocationObject* locationobj = NULL;

    switch(messagemediaobject->classType())
    {
        case MessageMedia::typeMessageMediaAudio:
            locationobj = this->_telegram->locationOfAudio(messagemediaobject->audio());
            break;

        case MessageMedia::typeMessageMediaDocument:
            locationobj = this->_telegram->locationOfDocument(messagemediaobject->document());
            break;

        case MessageMedia::typeMessageMediaVideo:
            locationobj = this->_telegram->locationOfVideo(messagemediaobject->video());
            break;

        case MessageMedia::typeMessageMediaPhoto:
            locationobj = messagemediaobject->photo()->sizes()->last()->location();
            break;

        default:
            break;
    }

    return locationobj;
}

void SailorGram::moveMediaToDownloads(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return;

    FileLocationObject* locationobj = this->mediaLocation(messagemediaobject);
    this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

void SailorGram::moveMediaToGallery(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return;

    QString type;
    FileLocationObject* locationobj = this->mediaLocation(messagemediaobject);

    if(messagemediaobject->classType() == MessageMedia::typeMessageMediaDocument)
    {
        QString mime = messagemediaobject->document()->mimeType();
        type = mime.split("/")[0];
    }

    if((messagemediaobject->classType() == MessageMedia::typeMessageMediaVideo) || (type == "video"))
        this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    else if((messagemediaobject->classType() == MessageMedia::typeMessageMediaAudio) || (type == "audio"))
        this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    else if((messagemediaobject->classType() == MessageMedia::typeMessageMediaPhoto) || (type == "image"))
        this->moveMediaTo(locationobj, QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    else
        this->moveMediaToDownloads(messagemediaobject); // Fallback to Downloads folder
}
