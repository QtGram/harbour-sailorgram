#include "sailorgram.h"

SailorGram::SailorGram(QObject *parent): QObject(parent), _telegram(NULL)
{
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

void SailorGram::moveMediaToDownloads(MessageMediaObject *messagemediaobject)
{
    if(!this->_telegram)
        return;

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

    if(!locationobj)
        return;

    QString downloadfile = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QDir::separator() + locationobj->fileName();

    if(QFile::exists(downloadfile)) // Don't overwrite existing files
        return;

    QFile::copy(QUrl(locationobj->download()->location()).path(), downloadfile);
}
