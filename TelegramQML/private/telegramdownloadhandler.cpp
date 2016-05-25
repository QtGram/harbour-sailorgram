#define TARGET_VALUE(PROPERTY) \
    if(p->location) return p->location->PROPERTY(); \
    else return 0;
#define DEFINE_DIS \
    QPointer<TelegramDownloadHandler> dis = this;

#include "telegramsharedpointer.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"
#include "telegramengine.h"
#include "private/telegramthumbnailer.h"
#include "private/telegramfilelocation.h"
#include "private/telegramdownloadhandler.h"

#include <QFileInfo>
#include <QPointer>
#include <telegram/objects/typeobjects.h>

class TelegramDownloadHandlerPrivate
{
public:
    QPointer<TelegramEngine> engine;

    QString real_thumbnail;
    TelegramThumbnailer *thumbnailer;

    TelegramSharedPointer<TelegramTypeQObject> sourceRoot;
    QPointer<TelegramTypeQObject> source;
    QPointer<QObject> target;
    QPointer<TelegramFileLocation> location;
    QPointer<TelegramFileLocation> thumb_location;
    int targetType;
    bool thumbnailChecked;

    static QHash<TelegramEngine*, QHash<QByteArray, TelegramFileLocation*> > locations;
};

QHash<TelegramEngine*, QHash<QByteArray, TelegramFileLocation*> > TelegramDownloadHandlerPrivate::locations;

TelegramDownloadHandler::TelegramDownloadHandler(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramDownloadHandlerPrivate;
    p->thumbnailer = 0;
    p->thumbnailChecked = false;
    p->targetType = TypeTargetUnknown;
}

void TelegramDownloadHandler::setSource(TelegramTypeQObject *source)
{
    if(p->source == source)
        return;

    p->source = source;
    p->sourceRoot = TelegramTools::objectRoot(p->source);

    retry();
    Q_EMIT targetChanged();
}

TelegramTypeQObject *TelegramDownloadHandler::source() const
{
    return p->source;
}

TelegramEngine *TelegramDownloadHandler::engine() const
{
    return p->engine;
}

void TelegramDownloadHandler::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    p->engine = engine;
    retry();
    Q_EMIT engineChanged();
}

TelegramFileLocation *TelegramDownloadHandler::target() const
{
    return p->location;
}

int TelegramDownloadHandler::targetType() const
{
    return p->targetType;
}

QSizeF TelegramDownloadHandler::imageSize() const
{
    if(p->location)
        return p->location->imageSize();
    else
        return QSizeF();
}

QSizeF TelegramDownloadHandler::thumbnailSize() const
{
    if(p->thumb_location)
        return p->thumb_location->imageSize();
    else
        return QSizeF();
}

qint32 TelegramDownloadHandler::size() const
{
    TARGET_VALUE(size);
}

qint32 TelegramDownloadHandler::downloadedSize() const
{
    TARGET_VALUE(downloadedSize);
}

qint32 TelegramDownloadHandler::downloadTotal() const
{
    TARGET_VALUE(downloadTotal);
}

bool TelegramDownloadHandler::downloading() const
{
    TARGET_VALUE(downloading);
}

QString TelegramDownloadHandler::destination() const
{
    if(p->location)
        return p->location->destination();
    else
        return QString::null;
}

QString TelegramDownloadHandler::thumbnail()
{
    checkRealThumbnail();
    if(!p->real_thumbnail.isEmpty())
        return p->real_thumbnail;
    else
    if(p->thumb_location)
        return p->thumb_location->destination();
    else
        return QString::null;
}

QStringList TelegramDownloadHandler::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine)
                         << FUNCTION_NAME(source);
}

bool TelegramDownloadHandler::download()
{
    if(p->location)
        return p->location->download();
    else
        return false;
}

bool TelegramDownloadHandler::check()
{
    if(p->location)
        return p->location->check();
    else
        return false;
}

void TelegramDownloadHandler::stop()
{
    if(p->location)
        p->location->stop();
}

TelegramFileLocation *TelegramDownloadHandler::findTarget(QObject *source, int *targetType, QObject **targetPointer)
{
    if(!p->engine || !source)
        return 0;

    QObject *object = 0;
    const ObjectType objectType = findObjectType(source);
    switch(static_cast<int>(objectType))
    {
    case TypeObjectMessage:
    {
        MessageObject *msg = static_cast<MessageObject*>(source);
        if(msg->media()->classType() != MessageMediaObject::TypeMessageMediaEmpty)
            object = msg->media();
        else
        if(msg->action()->classType() != MessageActionObject::TypeMessageActionEmpty)
            object = msg->action();
    }
        break;

    case TypeObjectDialog:
    {
        DialogObject *dlg = static_cast<DialogObject*>(source);
        object = dlg->peer();
    }
        break;

    case TypeObjectPeer:
    {
        TelegramSharedDataManager *tsdm = p->engine->sharedData();
        PeerObject *peer = static_cast<PeerObject*>(source);
        if(tsdm)
        {
            QByteArray key = TelegramTools::identifier(peer->core());
            switch(peer->classType())
            {
            case PeerObject::TypePeerChannel:
            case PeerObject::TypePeerChat:
                object = tsdm->getChat(key);
                break;

            case PeerObject::TypePeerUser:
                object = tsdm->getUser(key);
                break;
            }
        }
    }
        break;

    case TypeObjectInputPeer:
    {
        TelegramSharedDataManager *tsdm = p->engine->sharedData();
        InputPeerObject *inputPeer = static_cast<InputPeerObject*>(source);
        Peer peer = TelegramTools::inputPeerPeer(inputPeer->core());
        if(tsdm)
        {
            QByteArray key = TelegramTools::identifier(peer);
            switch(peer.classType())
            {
            case Peer::typePeerChannel:
            case Peer::typePeerChat:
                object = tsdm->getChat(key);
                break;

            case Peer::typePeerUser:
                object = tsdm->getUser(key);
                break;
            }
        }
    }
        break;

    case TypeObjectUser:
        object = static_cast<UserObject*>(source)->photo();
        break;

    case TypeObjectChat:
        object = static_cast<ChatObject*>(source)->photo();
        break;

    case TypeObjectFileLocation:
        return locationOf(static_cast<FileLocationObject*>(source));
        break;

    case TypeObjectMessageAction:
    {
        MessageActionObject *act = static_cast<MessageActionObject*>(source);
        if(act->classType() == MessageActionObject::TypeMessageActionChatEditPhoto)
        {
            object = act->photo();
            if(targetType) *targetType = TypeTargetActionChatPhoto;
            if(targetPointer) *targetPointer = object;
        }
    }
        break;

    case TypeObjectMessageMedia:
    {
        MessageMediaObject *media = static_cast<MessageMediaObject*>(source);
        if(media->classType() == MessageMediaObject::TypeMessageMediaDocument)
            object = media->document();
        else
        if(media->classType() == MessageMediaObject::TypeMessageMediaPhoto)
            object = media->photo();
    }
        break;

    case TypeObjectDocument:
        p->thumb_location = locationOf( static_cast<DocumentObject*>(source)->thumb() );
        return locationOf( static_cast<DocumentObject*>(source) );
        break;

    case TypeObjectPhoto:
    {
        p->thumb_location = locationOf(static_cast<PhotoObject*>(source), true);
        return locationOf( static_cast<PhotoObject*>(source) );
    }
        break;

    case TypeObjectPhotoSize:
        return locationOf(static_cast<PhotoSizeObject*>(source));
        break;

    case TypeObjectUserProfilePhoto:
        object = static_cast<UserProfilePhotoObject*>(source)->photoBig();
        p->thumb_location = findTarget( static_cast<UserProfilePhotoObject*>(source)->photoSmall() );
        if(targetType) *targetType = TypeTargetUserPhoto;
        if(targetPointer) *targetPointer = object;
        break;

    case TypeObjectChatPhoto:
        object = static_cast<ChatPhotoObject*>(source)->photoBig();
        p->thumb_location = findTarget( static_cast<ChatPhotoObject*>(source)->photoSmall() );
        if(targetType) *targetType = TypeTargetChatPhoto;
        if(targetPointer) *targetPointer = object;
        break;

    case TypeObjectEmpty:
        object = 0;
        if(targetType) *targetType = TypeTargetUnknown;
        if(targetPointer) *targetPointer = object;
        break;
    }

    return findTarget(object, targetType, targetPointer);
}

TelegramDownloadHandler::ObjectType TelegramDownloadHandler::findObjectType(QObject *obj)
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
    if(qobject_cast<InputPeerObject*>(obj))
        objectType = TypeObjectInputPeer;
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
    if(qobject_cast<DocumentObject*>(obj))
        objectType = TypeObjectDocument;
    else
    if(qobject_cast<PhotoObject*>(obj))
        objectType = TypeObjectPhoto;
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

TelegramFileLocation *TelegramDownloadHandler::locationOf(FileLocationObject *obj)
{
    if(!p->engine)
        return 0;

    const QByteArray &hash = obj->core().getHash();
    TelegramFileLocation *tfl = TelegramDownloadHandlerPrivate::locations.value(p->engine).value(hash);
    if(tfl)
        return tfl;

    tfl = new TelegramFileLocation(p->engine);
    tfl->setLocalId(obj->localId());
    tfl->setVolumeId(obj->volumeId());
    tfl->setSecret(obj->secret());
    tfl->setDcId(obj->dcId());
    tfl->setClassType(InputFileLocationObject::TypeInputFileLocation);

    registerLocation(tfl, hash);
    return tfl;
}

TelegramFileLocation *TelegramDownloadHandler::locationOf(DocumentObject *obj)
{
    if(!p->engine)
        return 0;

    const QByteArray &hash = obj->core().getHash();
    TelegramFileLocation *tfl = TelegramDownloadHandlerPrivate::locations.value(p->engine).value(hash);
    if(tfl)
        return tfl;

    tfl = new TelegramFileLocation(p->engine);
    tfl->setId(obj->id());
    tfl->setAccessHash(obj->accessHash());
    tfl->setSize(obj->size());
    tfl->setDcId(obj->dcId());
    tfl->setClassType(InputFileLocationObject::TypeInputDocumentFileLocation);

    QList<DocumentAttribute> attrs = obj->attributes();
    Q_FOREACH(const DocumentAttribute &attr, attrs)
    {
        switch(attr.classType())
        {
        case DocumentAttribute::typeDocumentAttributeAnimated:
            break;
        case DocumentAttribute::typeDocumentAttributeAudio:
            break;
        case DocumentAttribute::typeDocumentAttributeFilename:
            break;
        case DocumentAttribute::typeDocumentAttributeImageSize:
            tfl->setImageSize(QSizeF(attr.w(), attr.h()));
            break;
        case DocumentAttribute::typeDocumentAttributeSticker:
            break;
        case DocumentAttribute::typeDocumentAttributeVideo:
            tfl->setImageSize(QSizeF(attr.w(), attr.h()));
            break;
        }
    }

    registerLocation(tfl, hash);
    return tfl;
}

TelegramFileLocation *TelegramDownloadHandler::locationOf(PhotoObject *obj, bool thumbnail)
{
    if(!p->engine)
        return 0;

    QList<PhotoSize> sizes = obj->sizes();
    if(sizes.isEmpty())
        return 0;

    PhotoSize res = sizes.takeFirst();
    Q_FOREACH(const PhotoSize &size, sizes)
    {
        const qint64 res_size = res.w()*res.h();
        const qint64 siz_size = size.w()*size.h();
        if(thumbnail && siz_size<res_size)
            res = size;
        else
        if(!thumbnail && siz_size>res_size)
            res = size;
    }

    PhotoSizeObject sizeObject(res);
    return locationOf(&sizeObject);
}

TelegramFileLocation *TelegramDownloadHandler::locationOf(PhotoSizeObject *obj)
{
    TelegramFileLocation *res = locationOf(obj->location());
    if(res)
    {
        res->setSize(obj->size());
        res->setImageSize(QSizeF(obj->w(), obj->h()));
    }

    return res;
}

void TelegramDownloadHandler::registerLocation(TelegramFileLocation *loc, const QByteArray &hash)
{
    TelegramDownloadHandlerPrivate::locations[p->engine][hash] = loc;
}

void TelegramDownloadHandler::retry()
{
    p->location = 0;
    p->thumb_location = 0;
    p->targetType = TypeTargetUnknown;
    p->real_thumbnail.clear();

    if(!p->engine || !p->source)
    {
        Q_EMIT targetChanged();
        Q_EMIT targetTypeChanged();
        Q_EMIT destinationChanged();
        Q_EMIT thumbnailChanged();
        return;
    }

    if(p->location)
    {
        disconnect(p->location.data(), &TelegramFileLocation::downloadedSizeChanged, this, &TelegramDownloadHandler::downloadedSizeChanged);
        disconnect(p->location.data(), &TelegramFileLocation::downloadTotalChanged, this, &TelegramDownloadHandler::downloadTotalChanged);
        disconnect(p->location.data(), &TelegramFileLocation::downloadingChanged, this, &TelegramDownloadHandler::downloadingChanged);
        disconnect(p->location.data(), &TelegramFileLocation::sizeChanged, this, &TelegramDownloadHandler::sizeChanged);
        disconnect(p->location.data(), &TelegramFileLocation::destinationChanged, this, &TelegramDownloadHandler::destinationChanged);
        disconnect(p->location.data(), &TelegramFileLocation::destinationChanged, this, &TelegramDownloadHandler::checkRealThumbnail);
        disconnect(p->location.data(), &TelegramFileLocation::errorChanged, this, &TelegramDownloadHandler::error_changed);
        disconnect(p->location.data(), &TelegramFileLocation::imageSizeChanged, this, &TelegramDownloadHandler::imageSizeChanged);
    }
    if(p->thumb_location)
    {
        disconnect(p->thumb_location.data(), &TelegramFileLocation::destinationChanged, this, &TelegramDownloadHandler::thumbnailChanged);
        disconnect(p->thumb_location.data(), &TelegramFileLocation::imageSizeChanged, this, &TelegramDownloadHandler::thumbnailSizeChanged);
        disconnect(p->thumb_location.data(), &TelegramFileLocation::errorChanged, this, &TelegramDownloadHandler::error_changed);
    }

    int targetType = TypeTargetUnknown;
    QObject *targetObject = 0;
    p->location = findTarget(p->source, &targetType, &targetObject);
    p->targetType = targetType;
    p->target = targetObject;

    if(p->location)
    {
        connect(p->location.data(), &TelegramFileLocation::downloadedSizeChanged, this, &TelegramDownloadHandler::downloadedSizeChanged);
        connect(p->location.data(), &TelegramFileLocation::downloadTotalChanged, this, &TelegramDownloadHandler::downloadTotalChanged);
        connect(p->location.data(), &TelegramFileLocation::downloadingChanged, this, &TelegramDownloadHandler::downloadingChanged);
        connect(p->location.data(), &TelegramFileLocation::sizeChanged, this, &TelegramDownloadHandler::sizeChanged);
        connect(p->location.data(), &TelegramFileLocation::destinationChanged, this, &TelegramDownloadHandler::destinationChanged);
        connect(p->location.data(), &TelegramFileLocation::destinationChanged, this, &TelegramDownloadHandler::checkRealThumbnail);
        connect(p->location.data(), &TelegramFileLocation::errorChanged, this, &TelegramDownloadHandler::error_changed);
        connect(p->location.data(), &TelegramFileLocation::imageSizeChanged, this, &TelegramDownloadHandler::imageSizeChanged);
    }
    if(p->thumb_location)
    {
        connect(p->thumb_location.data(), &TelegramFileLocation::destinationChanged, this, &TelegramDownloadHandler::thumbnailChanged);
        connect(p->thumb_location.data(), &TelegramFileLocation::imageSizeChanged, this, &TelegramDownloadHandler::thumbnailSizeChanged);
        connect(p->thumb_location.data(), &TelegramFileLocation::errorChanged, this, &TelegramDownloadHandler::error_changed);
        p->thumb_location->download();
    }

    Q_EMIT targetChanged();
    Q_EMIT targetTypeChanged();
    Q_EMIT destinationChanged();
    Q_EMIT thumbnailChanged();
    Q_EMIT imageSizeChanged();
    Q_EMIT thumbnailSizeChanged();
    Q_EMIT downloadingChanged();
    Q_EMIT downloadedSizeChanged();
    Q_EMIT downloadTotalChanged();
}

void TelegramDownloadHandler::error_changed()
{
    TelegramFileLocation *loc = qobject_cast<TelegramFileLocation*>(sender());
    if(!loc)
        return;
    if(loc != p->thumb_location && loc != p->location)
        return;

    setError(loc->errorText(), loc->errorCode());
}

void TelegramDownloadHandler::checkRealThumbnail()
{
    const QString &dest = destination();
    if(dest.isEmpty() || !p->real_thumbnail.isEmpty())
        return;

    const QString &thumbPath = dest + "_thumb";
    if(QFileInfo::exists(thumbPath))
    {
        p->real_thumbnail = thumbPath;
        Q_EMIT thumbnailChanged();
        return;
    }
    if(p->thumbnailChecked)
        return;

    if(!p->thumbnailer)
        p->thumbnailer = new TelegramThumbnailer(this);

    p->thumbnailChecked = true;
    p->thumbnailer->createThumbnail(dest, thumbPath, [this, thumbPath](){
        p->real_thumbnail = (QFileInfo(thumbPath).exists()? thumbPath : "");
        Q_EMIT thumbnailChanged();
    });
}

TelegramDownloadHandler::~TelegramDownloadHandler()
{
    delete p;
}

