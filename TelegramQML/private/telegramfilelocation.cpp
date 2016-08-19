#define DEFINE_DIS \
    QPointer<TelegramFileLocation> dis = this;

#include "telegramfilelocation.h"
#include "telegramengine.h"
#include "telegramuploadhandler.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <QDir>

class TelegramFileLocationPrivate
{
public:
    qint64 downloadFileId;
    QPointer<QFile> file;

    qint32 dcId;
    qint32 size;
    QSizeF imageSize;
    qint32 downloadedSize;
    qint32 downloadTotal;
    InputFileLocationObject *location;
    QPointer<TelegramEngine> engine;
    bool downloading;
    QString destination;
};

TelegramFileLocation::TelegramFileLocation(TelegramEngine *engine) :
    TqObject(engine)
{
    p = new TelegramFileLocationPrivate;
    p->dcId = 0;
    p->size = 0;
    p->downloadFileId = 0;
    p->downloadedSize = 0;
    p->downloadTotal = 0;
    p->downloading = false;
    p->engine = engine;

    p->location = new InputFileLocationObject(this);

    connect(p->location, &InputFileLocationObject::accessHashChanged, this, &TelegramFileLocation::accessHashChanged);
    connect(p->location, &InputFileLocationObject::localIdChanged   , this, &TelegramFileLocation::localIdChanged   );
    connect(p->location, &InputFileLocationObject::secretChanged    , this, &TelegramFileLocation::secretChanged    );
    connect(p->location, &InputFileLocationObject::volumeIdChanged  , this, &TelegramFileLocation::volumeIdChanged  );
    connect(p->location, &InputFileLocationObject::idChanged        , this, &TelegramFileLocation::idChanged        );
    connect(p->location, &InputFileLocationObject::classTypeChanged , this, &TelegramFileLocation::classTypeChanged );
    connect(p->location, &InputFileLocationObject::coreChanged      , this, &TelegramFileLocation::locationChanged  );

    connect(p->engine.data(), &TelegramEngine::destroyed, this, &TelegramFileLocation::deleteLater);
}

qint32 TelegramFileLocation::dcId() const
{
    return p->dcId;
}

void TelegramFileLocation::setDcId(const qint32 &dcId)
{
    if(p->dcId == dcId)
        return;

    p->dcId = dcId;
    Q_EMIT dcIdChanged();
}

qint32 TelegramFileLocation::localId() const
{
    return p->location->localId();
}

void TelegramFileLocation::setLocalId(const qint32 &localId)
{
    p->location->setLocalId(localId);
}

qint64 TelegramFileLocation::secret() const
{
    return p->location->secret();
}

void TelegramFileLocation::setSecret(const qint64 &secret)
{
    p->location->setSecret(secret);
}

qint64 TelegramFileLocation::volumeId() const
{
    return p->location->volumeId();
}

void TelegramFileLocation::setVolumeId(const qint64 &volumeId)
{
    p->location->setVolumeId(volumeId);
}

qint64 TelegramFileLocation::accessHash() const
{
    return p->location->accessHash();
}

void TelegramFileLocation::setAccessHash(const qint64 &accessHash)
{
    p->location->setAccessHash(accessHash);
}

qint64 TelegramFileLocation::id() const
{
    return p->location->id();
}

void TelegramFileLocation::setId(const qint64 &id)
{
    p->location->setId(id);
}

qint32 TelegramFileLocation::size() const
{
    return p->size;
}

void TelegramFileLocation::setSize(const qint32 &size)
{
    if(p->size == size)
        return;

    p->size = size;
    Q_EMIT sizeChanged();
}

QSizeF TelegramFileLocation::imageSize() const
{
    return p->imageSize;
}

void TelegramFileLocation::setImageSize(const QSizeF &imageSize)
{
    if(p->imageSize == imageSize)
        return;

    p->imageSize = imageSize;
    Q_EMIT imageSizeChanged();
}

qint32 TelegramFileLocation::downloadedSize() const
{
    return p->downloadedSize;
}

void TelegramFileLocation::setDownloadedSize(qint32 size)
{
    if(p->downloadedSize == size)
        return;

    p->downloadedSize = size;
    Q_EMIT downloadedSizeChanged();
}

qint32 TelegramFileLocation::downloadTotal() const
{
    return p->downloadTotal;
}

bool TelegramFileLocation::downloading() const
{
    return p->downloading;
}

void TelegramFileLocation::setDownloading(bool downloading)
{
    if(p->downloading == downloading)
        return;

    p->downloading = downloading;
    Q_EMIT downloadingChanged();
}

void TelegramFileLocation::setDownloadTotal(qint32 total)
{
    if(p->downloadTotal == total)
        return;

    p->downloadTotal = total;
    Q_EMIT downloadTotalChanged();
}

TelegramEngine *TelegramFileLocation::engine() const
{
    return p->engine;
}

int TelegramFileLocation::classType() const
{
    return p->location->classType();
}

void TelegramFileLocation::setClassType(int classType)
{
    p->location->setClassType(classType);
}

InputFileLocationObject *TelegramFileLocation::location() const
{
    return p->location;
}

QString TelegramFileLocation::destination() const
{
    return p->destination;
}

QStringList TelegramFileLocation::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine);
}

void TelegramFileLocation::setDestination(const QString &destination)
{
    p->destination = destination;
    Q_EMIT destinationChanged();
}

QString TelegramFileLocation::getLocation(bool *uploading) const
{
    if(uploading) *uploading = false;
    if(p->engine->configDirectory().isEmpty() || p->engine->phoneNumber().isEmpty() || !p->location)
        return QString();

    QList<TelegramUploadHandler*> uploadHandlers = TelegramUploadHandler::getItems(p->engine, 0);
    Q_FOREACH(TelegramUploadHandler *handler, uploadHandlers)
    {
        MessageObject *msg = handler->result();
        if(!msg)
            continue;

        if(msg->media()->document()->id() == p->location->id() ||
           msg->media()->photo()->id() == p->location->volumeId())
        {
            if(uploading) *uploading = true;
            return QUrl(handler->file()).toLocalFile();
        }
    }

    const QString profilePath = p->engine->configDirectory() + "/" + p->engine->phoneNumber() + "/downloads/";
    QDir().mkpath(profilePath);

    return profilePath + p->location->core().getHash().toHex();
}

bool TelegramFileLocation::download()
{
    if(!p->engine || !p->engine->telegram())
        return false;
    if(p->downloading || p->downloadFileId)
        return true;

    bool uploading = false;
    const QString location = getLocation(&uploading);

    if(QFileInfo::exists(location))
    {
        const qint64 fsize = QFileInfo(location).size();
        if(!uploading && (fsize==0 || (size() && fsize != size())))
        {
            QFile::remove(location);
        }
        else
        {
            setDownloadTotal(size());
            setDownloadedSize(downloadTotal());
            setDestination(location);
            Q_EMIT finished();
            return true;
        }
    }

    p->file = new QFile(location, this);
    if(!p->file->open(QFile::WriteOnly))
    {
        delete p->file;
        return false;
    }

    setDownloadTotal(size());
    setDownloading(true);

    DEFINE_DIS;
    Telegram *tg = p->engine->telegram();

    p->downloadFileId = tg->uploadGetFile(p->location->core(), p->size, p->dcId, [this, dis](TG_UPLOAD_GET_FILE_CUSTOM_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis || !p->file)
            return;

        if(!error.null) {
            setError(error.errorText, error.errorCode);
            setDownloadedSize(0);
            setDownloadTotal(0);
            setDownloading(false);
            p->file->close();
            p->file->remove();
            delete p->file;
            p->downloadFileId = 0;
            Q_EMIT finished();
            return;
        }

        switch(static_cast<int>(result.classType()))
        {
        case UploadGetFile::typeUploadGetFileEmpty:
        case UploadGetFile::typeUploadGetFileCanceled:
            setDownloadedSize(0);
            setDownloadTotal(0);
            setDownloading(false);
            p->file->close();
            p->file->remove();
            delete p->file;
            p->downloadFileId = 0;
            Q_EMIT finished();
            break;

        case UploadGetFile::typeUploadGetFileFinished:
            setDownloadTotal(result.total());
            setDownloadedSize(downloadTotal());
            setDownloading(false);
            p->file->write(result.bytes());
            p->file->close();
            setDestination(p->file->fileName());
            delete p->file;
            p->downloadFileId = 0;
            Q_EMIT finished();
            break;

        case UploadGetFile::typeUploadGetFileProgress:
            p->file->write(result.bytes());
            setDownloadTotal(result.total());
            setDownloadedSize(result.downloaded());
            break;
        }
    });

    return true;
}

bool TelegramFileLocation::check()
{
    if(!p->engine || !p->engine->telegram())
        return false;
    if(p->downloading)
        return false;

    bool uploading = false;
    const QString location = getLocation(&uploading);
    if(QFileInfo::exists(location))
    {
        if(size() && !uploading && QFileInfo(location).size() != size())
            return false;
        else
        {
            setDownloadTotal(size());
            setDownloadedSize(downloadTotal());
            setDestination(location);
            Q_EMIT finished();
            return true;
        }
    }
    else
        return false;
}

void TelegramFileLocation::stop()
{
    if(!p->engine || !p->engine->telegram())
        return;
    if(!p->downloading)
        return;

    Telegram *tg = p->engine->telegram();
    tg->uploadCancelFile(p->downloadFileId);
    p->downloadFileId = 0;
    setDownloadedSize(0);
    setDownloadTotal(0);
    setDownloading(false);
    if(p->file) {
        p->file->close();
        p->file->remove();
        delete p->file;
    }
    Q_EMIT finished();
}

TelegramFileLocation::~TelegramFileLocation()
{
    delete p;
}
