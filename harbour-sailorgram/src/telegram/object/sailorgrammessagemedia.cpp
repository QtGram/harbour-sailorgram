#include "sailorgrammessagemedia.h"

SailorgramMessageMedia::SailorgramMessageMedia(QObject *parent) : QObject(parent), _engine(NULL), _messagemedia(NULL), _webpage(NULL), _geopoint(NULL)
{
    this->_filesize = 0;
    this->_fileduration = 0;
    this->_transferedsize = 0;
    this->_isvoicefile = false;
    this->_istransfered = false;
    this->_istransfering = false;
}

SailorgramMessageMedia::SailorgramMessageMedia(TelegramEngine *engine, QObject *parent) : QObject(parent), _engine(engine), _messagemedia(NULL), _webpage(NULL), _geopoint(NULL)
{
    this->_filesize = 0;
    this->_fileduration = 0;
    this->_transferedsize = 0;
    this->_isvoicefile = false;
    this->_istransfered = false;
    this->_istransfering = false;
}

QString SailorgramMessageMedia::fileName() const
{
    return this->_filename;
}

QString SailorgramMessageMedia::filePath() const
{
    return this->_filepath;
}

QString SailorgramMessageMedia::fileMimeType() const
{
    return this->_filemimetype;
}

qint32 SailorgramMessageMedia::fileSize() const
{
    return this->_filesize;
}

QString SailorgramMessageMedia::thumbnailPath() const
{
    return this->_thumbpath;
}

QString SailorgramMessageMedia::fileTitle() const
{
    return this->_filetitle;
}

QString SailorgramMessageMedia::filePerformer() const
{
    return this->_fileperformer;
}

qint32 SailorgramMessageMedia::fileDuration() const
{
    return this->_fileduration;
}

bool SailorgramMessageMedia::isVoiceFile() const
{
    return this->_isvoicefile;
}

SailorgramWebPageMedia *SailorgramMessageMedia::webPage() const
{
    return this->_webpage;
}

SailorgramGeoPointMedia *SailorgramMessageMedia::geoPoint() const
{
    return this->_geopoint;
}

bool SailorgramMessageMedia::isTransfered() const
{
    return this->_istransfered;
}

bool SailorgramMessageMedia::isTransfering() const
{
    return this->_istransfering;
}

float SailorgramMessageMedia::transferProgress() const
{
    if(this->_filesize <= 0)
        return 0.0;

    return static_cast<float>(this->_transferedsize) / static_cast<float>(this->_filesize);
}

MessageMediaObject *SailorgramMessageMedia::rawMedia() const
{
    return this->_messagemedia;
}

void SailorgramMessageMedia::setFileName(const QString &filename)
{
    if(this->_filename == filename)
        return;

    this->_filename = filename;
    emit fileNameChanged();
}

void SailorgramMessageMedia::setFilePath(const QString &filepath)
{
    if(this->_filepath == filepath)
        return;

    this->_filepath = filepath;
    emit filePathChanged();
}

void SailorgramMessageMedia::setFileMimeType(const QString &filemimetype)
{
    if(this->_filemimetype == filemimetype)
        return;

    this->_filemimetype = filemimetype;
    emit fileMimeTypeChanged();
}

void SailorgramMessageMedia::setFileSize(qint32 filesize)
{
    if(this->_filesize == filesize)
        return;

    this->_filesize = filesize;
    emit fileSizeChanged();
    emit transferProgressChanged();
}

void SailorgramMessageMedia::setThumbPath(const QString &thumbpath)
{
    if(this->_thumbpath == thumbpath)
        return;

    this->_thumbpath = thumbpath;
    emit thumbnailPathChanged();
}

void SailorgramMessageMedia::setFileTitle(const QString &filetitle)
{
    if(this->_filetitle == filetitle)
        return;

    this->_filetitle = filetitle;
    emit fileTitleChanged();
}

void SailorgramMessageMedia::setFilePerformer(const QString &fileperformer)
{
    if(this->_fileperformer == fileperformer)
        return;

    this->_fileperformer = fileperformer;
    emit filePerformerChanged();
}

void SailorgramMessageMedia::setFileDuration(qint32 fileduration)
{
    if(this->_fileduration == fileduration)
        return;

    this->_fileduration = fileduration;
    emit fileDurationChanged();
}

void SailorgramMessageMedia::setIsVoiceFile(bool b)
{
    if(this->_isvoicefile == b)
        return;

    this->_isvoicefile = b;
    emit isVoiceFileChanged();
}

void SailorgramMessageMedia::setIsTransfered(bool b)
{
    if(this->_istransfered == b)
        return;

    this->_istransfered = b;
    emit isTransferedChanged();

}

void SailorgramMessageMedia::setIsTransfering(bool b)
{
    if(this->_istransfering == b)
        return;

    this->_istransfering = b;
    emit isTransferingChanged();
}

void SailorgramMessageMedia::setTransferedSize(qint32 transferedsize)
{
    if(this->_transferedsize == transferedsize)
        return;

    this->_transferedsize = transferedsize;
    emit transferProgressChanged();
}

void SailorgramMessageMedia::setMediaObject(MessageMediaObject *messagemedia)
{
    if(this->_messagemedia == messagemedia)
        return;

    this->_messagemedia = messagemedia;
    emit rawMediaChanged();

    if(messagemedia->classType() == MessageMediaObject::TypeMessageMediaWebPage)
    {
        if(this->_webpage)
            this->_webpage->deleteLater();

        this->_webpage = new SailorgramWebPageMedia(messagemedia->webpage(), this);
        emit webPageChanged();
    }
    else if((messagemedia->classType() == MessageMediaObject::TypeMessageMediaGeo) || (messagemedia->classType() == MessageMediaObject::TypeMessageMediaVenue))
    {
        if(this->_geopoint)
            this->_geopoint->deleteLater();

        this->_geopoint = new SailorgramGeoPointMedia(messagemedia->geo(), messagemedia->address(), this);
        emit geoPointChanged();
    }
}

void SailorgramMessageMedia::download()
{
    emit downloadRequest(true);
}

void SailorgramMessageMedia::stop()
{
    emit downloadRequest(false);
}
