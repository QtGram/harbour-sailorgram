#include "telegramfilehandler.h"
#include "telegramthumbnailer.h"
#include "telegramqml.h"
#include "objects/types.h"

#include <QPointer>
#include <QImageReader>
#include <QDebug>

class TelegramFileHandlerPrivate
{
public:
    QPointer<TelegramQml> telegram;
    QPointer<QObject> object;
    QPointer<QObject> target;
    QPointer<FileLocationObject> location;
    QPointer<FileLocationObject> thumb_location;
    QPointer<UploadObject> upload;

    int objectType;
    int targetType;

    int progressType;
    bool downloaded;

    QUrl defaultThumbnail;
    QUrl filePath;
    QUrl thumbPath;
};

TelegramFileHandler::TelegramFileHandler(QObject *parent) :
    QObject(parent)
{
    p = new TelegramFileHandlerPrivate;
    p->objectType = TypeObjectEmpty;
    p->progressType = TypeProgressEmpty;
    p->targetType = TypeTargetUnknown;
    p->downloaded = false;
}

void TelegramFileHandler::setTelegram(TelegramQml *tg)
{
    if(p->telegram == tg)
        return;

    if(p->telegram)
        disconnect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(refresh()));

    p->telegram = tg;
    if(p->telegram)
        connect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(refresh()), Qt::QueuedConnection);

    Q_EMIT telegramChanged();
    refresh();
}

TelegramQml *TelegramFileHandler::telegram() const
{
    return p->telegram;
}

void TelegramFileHandler::setTarget(QObject *obj)
{
    if(p->object == obj)
        return;

    p->object = obj;
    Q_EMIT targetChanged();

    refresh();
}

QObject *TelegramFileHandler::target() const
{
    return p->object;
}

int TelegramFileHandler::objectType() const
{
    return p->objectType;
}

int TelegramFileHandler::targetType() const
{
    return p->targetType;
}

int TelegramFileHandler::progressType() const
{
    return p->progressType;
}

bool TelegramFileHandler::downloaded() const
{
    return p->downloaded;
}

qint64 TelegramFileHandler::progressTotalByte() const
{
    if(p->upload)
        return p->upload->totalSize();
    else
    if(p->location)
        return p->location->download()->total();
    else
        return 0;
}

qint64 TelegramFileHandler::progressCurrentByte() const
{
    if(p->upload)
        return p->upload->uploaded();
    else
    if(p->location)
        return p->location->download()->downloaded();
    else
        return 0;
}

qreal TelegramFileHandler::progressPercent() const
{
    return 100.0*progressCurrentByte()/progressTotalByte();
}

void TelegramFileHandler::setDefaultThumbnail(const QUrl &url)
{
    if(p->defaultThumbnail == url)
        return;

    p->defaultThumbnail = url;

    Q_EMIT defaultThumbnailChanged();
    Q_EMIT thumbPathChanged();
}

QUrl TelegramFileHandler::defaultThumbnail() const
{
    return p->defaultThumbnail;
}

QUrl TelegramFileHandler::filePath() const
{
    return p->filePath;
}

QUrl TelegramFileHandler::thumbPath() const
{
    if(p->thumbPath.isEmpty())
        return p->defaultThumbnail;
    else
        return p->thumbPath;
}

bool TelegramFileHandler::isSticker() const
{
    if(!p->telegram || !p->target)
        return false;
    if(p->targetType != TypeTargetMediaDocument)
        return false;

    return p->telegram->documentIsSticker( static_cast<DocumentObject*>(p->target.data()) );
}

QSize TelegramFileHandler::imageSize() const
{
    QSize result;
    if(!p->telegram || !p->target || !p->location)
        return result;
    if(p->thumbPath == p->defaultThumbnail && p->filePath.isEmpty())
    {
        QImageReader image(p->thumbPath.toLocalFile());
        return image.size();
    }

    switch(p->targetType)
    {
    case TypeTargetMediaPhoto:
    {
        PhotoSizeObject *obj = qobject_cast<PhotoSizeObject*>(p->location->parent());
        if(obj)
            result = QSize(obj->w(), obj->h());
    }
        break;

    case TypeTargetMediaAudio:
        break;

    case TypeTargetMediaVideo:
    {
        VideoObject *vid = qobject_cast<VideoObject*>(p->target);
        if(vid)
            result = QSize(vid->w(), vid->h());
    }
        break;

    case TypeTargetMediaDocument:
    {
        DocumentObject *doc = qobject_cast<DocumentObject*>(p->target);
        if(doc)
            result = QSize(doc->thumb()->w(), doc->thumb()->h());
    }
        break;

    case TypeTargetMediaOther:
        break;
    case TypeTargetChatPhoto:
        break;
    case TypeTargetUserPhoto:
        break;
    case TypeTargetActionChatPhoto:
        break;
    }

    return result;
}

qint64 TelegramFileHandler::fileSize() const
{
    qint64 result = 0;
    if(!p->telegram || !p->target || !p->location)
        return result;

    switch(p->targetType)
    {
    case TypeTargetMediaPhoto:
    {
        PhotoSizeObject *obj = qobject_cast<PhotoSizeObject*>(p->location->parent());
        if(obj)
            result = obj->size();
    }
        break;

    case TypeTargetMediaAudio:
    {
        AudioObject *aud = qobject_cast<AudioObject*>(p->target);
        if(aud)
            result = aud->size();
    }
        break;

    case TypeTargetMediaVideo:
    {
        VideoObject *vid = qobject_cast<VideoObject*>(p->target);
        if(vid)
            result = vid->size();
    }
        break;

    case TypeTargetMediaDocument:
    {
        DocumentObject *doc = qobject_cast<DocumentObject*>(p->target);
        if(doc)
            result = doc->size();
    }
        break;

    case TypeTargetMediaOther:
        break;
    case TypeTargetChatPhoto:
        break;
    case TypeTargetUserPhoto:
        break;
    case TypeTargetActionChatPhoto:
        break;
    }

    return result;
}

QString TelegramFileHandler::fileName() const
{
    QString result = 0;
    if(!p->telegram || !p->target)
        return result;

    switch(p->targetType)
    {
    case TypeTargetMediaPhoto:
    case TypeTargetMediaAudio:
    case TypeTargetMediaVideo:
    case TypeTargetMediaOther:
    case TypeTargetChatPhoto:
    case TypeTargetUserPhoto:
    case TypeTargetActionChatPhoto:
        break;

    case TypeTargetMediaDocument:
    {
        DocumentObject *doc = qobject_cast<DocumentObject*>(p->target);
        if(doc)
            result = p->telegram->documentFileName(doc);
    }
        break;
    }

    if(result.isEmpty() && p->location)
        result = p->location->fileName();

    return result;
}

bool TelegramFileHandler::cancelProgress()
{
    if(!p->telegram)
        return false;

    switch(p->progressType)
    {
    case TypeProgressDownload:
        if(p->location)
            p->telegram->cancelDownload(p->location->download());
        else
            return false;
        break;

    case TypeProgressUpload:
        qDebug() << __FUNCTION__ << "This feature isn't supported yet.";
        return false;
        break;
    }

    return true;
}

bool TelegramFileHandler::download()
{
    if(!p->telegram || !p->location)
        return false;
    if(p->progressType != TypeProgressEmpty)
        return false;

    InputFileLocation::InputFileLocationType type;
    switch(p->targetType)
    {
    case TypeTargetMediaAudio:
        type = InputFileLocation::typeInputAudioFileLocation;
        break;
    case TypeTargetMediaVideo:
        type = InputFileLocation::typeInputVideoFileLocation;
        break;
    case TypeTargetMediaDocument:
        type = InputFileLocation::typeInputDocumentFileLocation;
        break;
    default:
        type = InputFileLocation::typeInputFileLocation;
        break;
    }

    p->telegram->getFile(p->location, type, fileSize());
    return true;
}

void TelegramFileHandler::refresh()
{
    if(!p->telegram/* || !p->telegram->authLoggedIn()*/)
        return;

    disconnectLocation(p->location);
    disconnectLocation(p->thumb_location);

    p->upload = 0;
    p->location = 0;
    p->thumb_location = 0;
    p->downloaded = false;
    p->progressType = TypeProgressEmpty;
    p->targetType = TypeTargetUnknown;
    p->objectType = TypeObjectEmpty;
    p->filePath = QUrl();
    p->thumbPath = QUrl();

    detectObjectType();

    int targetType = TypeTargetUnknown;
    QObject *targetObject = 0;
    p->location = analizeObject(p->object, &targetType, &targetObject);
    p->targetType = targetType;
    p->target = targetObject;

    connectLocation(p->location);
    connectLocation(p->thumb_location);

    if(p->object)
    {
        if(p->objectType == TypeObjectMessage)
        {
            UploadObject *upload = static_cast<MessageObject*>(p->object.data())->upload();
            if(upload->fileId())
            {
                disconnectUpload(p->upload);
                p->upload = upload;
                connectUpload(p->upload);

                p->progressType = TypeProgressUpload;
            }
        }
        if(p->progressType == TypeProgressEmpty && p->location && p->location->download()->fileId())
            p->progressType = TypeProgressDownload;
    }
    if(p->telegram)
    {
        if(p->location)
        {
            p->telegram->getFileJustCheck(p->location);
            p->location->download()->locationChanged();
            p->location->download()->downloadedChanged();
        }
        if(p->thumb_location)
        {
            p->telegram->getFile(p->thumb_location);
            p->thumb_location->download()->locationChanged();
        }
    }

    Q_EMIT targetTypeChanged();

    Q_EMIT progressTypeChanged();
    Q_EMIT downloadedChanged();
    Q_EMIT progressTotalByteChanged();
    Q_EMIT progressCurrentByteChanged();
    Q_EMIT progressPercentChanged();

    Q_EMIT filePathChanged();
    Q_EMIT thumbPathChanged();

    Q_EMIT isStickerChanged();
    Q_EMIT imageSizeChanged();
    Q_EMIT fileSizeChanged();
    Q_EMIT fileNameChanged();
}

void TelegramFileHandler::dwl_locationChanged()
{
    if(!p->telegram)
        return;

    DownloadObject *dl = qobject_cast<DownloadObject*>(sender());
    if(!dl)
        return;

    const QString &location = dl->location();
    QUrl result;
    if(!location.isEmpty())
        result = QUrl(location);

    if(p->location && p->location->download() == dl)
    {
        p->filePath = result;
        if(p->targetType == TypeTargetMediaVideo) {
#ifdef TG_THUMBNAILER_CPP11
            TelegramThumbnailer_Callback callBack = [this](){
                emitPathChanges();
            };
#else
            TelegramThumbnailer_Callback callBack;
            callBack.object = this;
            callBack.method = "emitPathChanges";
#endif
            p->thumbPath = p->telegram->videoThumbLocation(result.toLocalFile(), callBack);
        } else {
            emitPathChanges();
        }
    }
    else
    if(p->thumb_location && p->thumb_location->download() == dl )
    {
        if(p->targetType == TypeTargetMediaVideo && !p->filePath.isEmpty())
            return;

        p->thumbPath = result;
        Q_EMIT thumbPathChanged();
    }

    Q_EMIT imageSizeChanged();
}

void TelegramFileHandler::dwl_downloadedChanged()
{
    if(p->location && p->downloaded != p->location->download()->downloaded())
    {
        p->downloaded = p->location->download()->downloaded();
        Q_EMIT downloadedChanged();
    }

    Q_EMIT progressCurrentByteChanged();
    Q_EMIT progressPercentChanged();
}

void TelegramFileHandler::dwl_totalChanged()
{
    Q_EMIT progressCurrentByteChanged();
    Q_EMIT progressPercentChanged();
}

void TelegramFileHandler::dwl_fileIdChanged()
{
    DownloadObject *dl = qobject_cast<DownloadObject*>(sender());
    if(!dl)
        return;
    if(p->thumb_location && p->thumb_location->download() == dl)
        return;

    if(!dl->fileId() && p->progressType == TypeProgressDownload)
    {
        p->progressType = TypeProgressEmpty;
        Q_EMIT progressTypeChanged();
    }
    else
    if(dl->fileId())
    {
        p->progressType = TypeProgressDownload;
        Q_EMIT progressTypeChanged();
    }
}

void TelegramFileHandler::upl_locationChanged()
{
    UploadObject *ul = qobject_cast<UploadObject*>(sender());
    if(!ul)
        return;

    const QString &location = ul->location();
    QUrl result;
    if(!location.isEmpty())
        result = QUrl::fromLocalFile(location);

    p->filePath = result;
    Q_EMIT filePathChanged();
}

void TelegramFileHandler::upl_uploadedChanged()
{
    Q_EMIT progressCurrentByteChanged();
    Q_EMIT progressPercentChanged();
}

void TelegramFileHandler::upl_totalSizeChanged()
{
    Q_EMIT progressCurrentByteChanged();
    Q_EMIT progressPercentChanged();
}

void TelegramFileHandler::upl_fileIdChanged()
{
    UploadObject *ul = qobject_cast<UploadObject*>(sender());
    if(!ul)
        return;

    if(!ul->fileId() && p->progressType == TypeProgressUpload)
    {
        p->progressType = TypeProgressEmpty;
        Q_EMIT progressTypeChanged();
    }
    else
    if(ul->fileId())
    {
        p->progressType = TypeProgressUpload;
        Q_EMIT progressTypeChanged();
    }
}

void TelegramFileHandler::emitPathChanges()
{
    Q_EMIT filePathChanged();
    Q_EMIT thumbPathChanged();
}

/*! Recersive Function !*/
FileLocationObject *TelegramFileHandler::analizeObject(QObject *target, int *targetType, QObject **targetPointer)
{
    if(!p->telegram || !target)
        return 0;

    QObject *object = 0;
    const ObjectType objectType = detectObjectType(target);
    switch(objectType)
    {
    case TypeObjectMessage:
    {
        MessageObject *msg = static_cast<MessageObject*>(target);
        if(msg->media()->classType() != MessageMedia::typeMessageMediaEmpty)
            object = msg->media();
        else
        if(msg->action()->classType() != MessageAction::typeMessageActionEmpty)
            object = msg->action();
    }
        break;

    case TypeObjectDialog:
    {
        DialogObject *dlg = static_cast<DialogObject*>(target);
        object = dlg->peer();
    }
        break;

    case TypeObjectPeer:
    {
        PeerObject *peer = static_cast<PeerObject*>(target);
        if(peer->classType() == Peer::typePeerChat)
            object = p->telegram->chat(peer->chatId());
        else
            object = p->telegram->user(peer->userId());
    }
        break;

    case TypeObjectUser:
        object = static_cast<UserObject*>(target)->photo();
        break;

    case TypeObjectChat:
        object = static_cast<ChatObject*>(target)->photo();
        break;

    case TypeObjectFileLocation:
        return static_cast<FileLocationObject*>(target);
        break;

    case TypeObjectMessageAction:
    {
        MessageActionObject *act = static_cast<MessageActionObject*>(target);
        if(act->classType() == MessageAction::typeMessageActionChatEditPhoto)
        {
            object = act->photo();
            if(targetType) *targetType = TypeTargetActionChatPhoto;
            if(targetPointer) *targetPointer = object;
        }
    }
        break;

    case TypeObjectMessageMedia:
    {
        MessageMediaObject *media = static_cast<MessageMediaObject*>(target);
        if(media->classType() == MessageMedia::typeMessageMediaAudio)
            object = media->audio();
        else
        if(media->classType() == MessageMedia::typeMessageMediaDocument)
            object = media->document();
        else
        if(media->classType() == MessageMedia::typeMessageMediaVideo)
            object = media->video();
        else
        if(media->classType() == MessageMedia::typeMessageMediaPhoto)
            object = media->photo();
        else
        if(media->classType() == MessageMedia::typeMessageMediaGeo)
            object = media->geo();
        else
        if(media->classType() == MessageMedia::typeMessageMediaWebPage)
            object = media->webpage();
    }
        break;

    case TypeObjectAudio:
        object = p->telegram->locationOfAudio( static_cast<AudioObject*>(target) );
        if(targetType) *targetType = TypeTargetMediaAudio;
        if(targetPointer) *targetPointer = static_cast<AudioObject*>(target);
        break;

    case TypeObjectDocument:
        object = p->telegram->locationOfDocument( static_cast<DocumentObject*>(target) );
        p->thumb_location = analizeObject( static_cast<DocumentObject*>(target)->thumb() );
        if(targetType) *targetType = TypeTargetMediaDocument;
        if(targetPointer) *targetPointer = static_cast<DocumentObject*>(target);
        break;

    case TypeObjectVideo:
        object = p->telegram->locationOfVideo( static_cast<VideoObject*>(target) );
        p->thumb_location = analizeObject( static_cast<VideoObject*>(target)->thumb() );
        if(targetType) *targetType = TypeTargetMediaVideo;
        if(targetPointer) *targetPointer = static_cast<VideoObject*>(target);
        break;

    case TypeObjectWebPage:
    {
        PhotoObject* photo = static_cast<WebPageObject*>(target)->photo();
        object = p->telegram->locationOfPhoto(photo);
        p->thumb_location = p->telegram->locationOfThumbPhoto(photo);
        if(targetType) *targetType = TypeTargetMediaPhoto;
        if(targetPointer) *targetPointer = photo;
    }
        break;

    case TypeObjectGeoPoint:
        object = 0;
        if(targetType) *targetType = TypeTargetMediaGeoPoint;
        if(targetPointer) *targetPointer = object;
        break;

    case TypeObjectContact:
        object = 0;
        if(targetType) *targetType = TypeTargetMediaContact;
        if(targetPointer) *targetPointer = object;
        break;


    case TypeObjectPhoto:
    {
        object = p->telegram->locationOfPhoto( static_cast<PhotoObject*>(target) );
        p->thumb_location = p->telegram->locationOfThumbPhoto(static_cast<PhotoObject*>(target) );
        if(targetType) *targetType = TypeTargetMediaPhoto;
        if(targetPointer) *targetPointer = static_cast<PhotoObject*>(target);
    }
        break;

    case TypeObjectPhotoSizeList:
    {
        PhotoSizeList *list = static_cast<PhotoSizeList*>(target);
        int minIdx = 0,
            minSize = 0;
        int maxIdx = 0,
            maxSize = 0;
        for(int i=0; i<list->count(); i++)
        {
            PhotoSizeObject *size = list->at(i);
            if(minSize == 0)
                minSize = size->w();
            else
            if(size->w() <= minSize)
            {
                minIdx = i;
                minSize = size->w();
            }

            if(maxSize == 0)
                maxSize = size->w();
            else
            if(size->w() >= maxSize)
            {
                maxIdx = i;
                maxSize = size->w();
            }
        }

        if(list->count() != 0)
        {
            object = list->at(maxIdx);
            p->thumb_location = analizeObject( list->at(minIdx) );
        }
    }
        break;

    case TypeObjectPhotoSize:
        object = static_cast<PhotoSizeObject*>(target)->location();
        break;

    case TypeObjectUserProfilePhoto:
        object = static_cast<UserProfilePhotoObject*>(target)->photoBig();
        p->thumb_location = analizeObject( static_cast<UserProfilePhotoObject*>(target)->photoSmall() );
        if(targetType) *targetType = TypeTargetUserPhoto;
        if(targetPointer) *targetPointer = object;
        break;

    case TypeObjectChatPhoto:
        object = static_cast<ChatPhotoObject*>(target)->photoBig();
        p->thumb_location = analizeObject( static_cast<ChatPhotoObject*>(target)->photoSmall() );
        if(targetType) *targetType = TypeTargetChatPhoto;
        if(targetPointer) *targetPointer = object;
        break;

    case TypeObjectEmpty:
        object = 0;
        if(targetType) *targetType = TypeTargetUnknown;
        if(targetPointer) *targetPointer = object;
        break;
    }

    return analizeObject(object, targetType, targetPointer);
}

TelegramFileHandler::ObjectType TelegramFileHandler::detectObjectType(QObject *obj)
{
    ObjectType objectType = TypeObjectEmpty;
    if(!obj)
        return objectType;

    if(qobject_cast<MessageObject*>(obj))
        objectType = TypeObjectMessage;
    else
    if(qobject_cast<PeerObject*>(obj))
        objectType = TypeObjectPeer;
    else
    if(qobject_cast<DialogObject*>(obj))
        objectType = TypeObjectDialog;
    else
    if(qobject_cast<UserObject*>(obj))
        objectType = TypeObjectUser;
    else
    if(qobject_cast<ChatObject*>(obj))
        objectType = TypeObjectChat;
    else
    if(qobject_cast<FileLocationObject*>(obj))
        objectType = TypeObjectFileLocation;
    else
    if(qobject_cast<MessageActionObject*>(obj))
        objectType = TypeObjectMessageAction;
    else
    if(qobject_cast<MessageMediaObject*>(obj))
        objectType = TypeObjectMessageMedia;
    else
    if(qobject_cast<AudioObject*>(obj))
        objectType = TypeObjectAudio;
    else
    if(qobject_cast<DocumentObject*>(obj))
        objectType = TypeObjectDocument;
    else
    if(qobject_cast<VideoObject*>(obj))
        objectType = TypeObjectVideo;
    else
    if(qobject_cast<WebPageObject*>(obj))
        objectType = TypeObjectWebPage;
    else
    if(qobject_cast<GeoPointObject*>(obj))
        objectType = TypeObjectGeoPoint;
    else
    if(qobject_cast<PhotoObject*>(obj))
        objectType = TypeObjectPhoto;
    else
    if(qobject_cast<PhotoSizeList*>(obj))
        objectType = TypeObjectPhotoSizeList;
    else
    if(qobject_cast<PhotoSizeObject*>(obj))
        objectType = TypeObjectPhotoSize;
    else
    if(qobject_cast<UserProfilePhotoObject*>(obj))
        objectType = TypeObjectUserProfilePhoto;
    else
    if(qobject_cast<ChatPhotoObject*>(obj))
        objectType = TypeObjectChatPhoto;
    else
        objectType = TypeObjectEmpty;

    return objectType;
}

void TelegramFileHandler::detectObjectType()
{
    p->objectType = detectObjectType(p->object);
    Q_EMIT objectTypeChanged();
}

void TelegramFileHandler::connectLocation(FileLocationObject *lct)
{
    if(!lct)
        return;

    DownloadObject *dl = lct->download();
    connect(dl, SIGNAL(downloadedChanged()), this, SLOT(dwl_downloadedChanged()));
    connect(dl, SIGNAL(totalChanged())     , this, SLOT(dwl_totalChanged())     );
    connect(dl, SIGNAL(locationChanged())  , this, SLOT(dwl_locationChanged())  );
    connect(dl, SIGNAL(fileIdChanged())    , this, SLOT(dwl_fileIdChanged())  );
}

void TelegramFileHandler::disconnectLocation(FileLocationObject *lct)
{
    if(!lct)
        return;

    DownloadObject *dl = lct->download();
    disconnect(dl, SIGNAL(downloadedChanged()), this, SLOT(dwl_downloadedChanged()));
    disconnect(dl, SIGNAL(totalChanged())     , this, SLOT(dwl_totalChanged())     );
    disconnect(dl, SIGNAL(locationChanged())  , this, SLOT(dwl_locationChanged())  );
    disconnect(dl, SIGNAL(fileIdChanged())    , this, SLOT(dwl_fileIdChanged())    );
}

void TelegramFileHandler::connectUpload(UploadObject *ul)
{
    if(!ul)
        return;

    connect(ul, SIGNAL(locationChanged()) , this, SLOT(upl_locationChanged()) );
    connect(ul, SIGNAL(totalSizeChanged()), this, SLOT(upl_totalSizeChanged()));
    connect(ul, SIGNAL(uploadedChanged()) , this, SLOT(upl_uploadedChanged()) );
    connect(ul, SIGNAL(fileIdChanged())   , this, SLOT(upl_fileIdChanged())   );
}

void TelegramFileHandler::disconnectUpload(UploadObject *ul)
{
    if(!ul)
        return;

    disconnect(ul, SIGNAL(locationChanged()) , this, SLOT(upl_locationChanged()) );
    disconnect(ul, SIGNAL(totalSizeChanged()), this, SLOT(upl_totalSizeChanged()));
    disconnect(ul, SIGNAL(uploadedChanged()) , this, SLOT(upl_uploadedChanged()) );
    disconnect(ul, SIGNAL(fileIdChanged())   , this, SLOT(upl_fileIdChanged())   );
}

TelegramFileHandler::~TelegramFileHandler()
{
    delete p;
}
