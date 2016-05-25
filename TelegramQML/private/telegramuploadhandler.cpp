#define DEFINE_DIS \
    QPointer<TelegramUploadHandler> dis = this;

#define PROPERTY_SET_TRY(NAME) \
    if(p->NAME == NAME) return; \
    p->NAME = NAME; \
    Q_EMIT NAME##Changed();

#define PROPERTY_GET_TRY(NAME) \
    return p->NAME;

#include "telegramuploadhandler.h"
#include "telegramsharedpointer.h"
#include "telegramtools.h"
#include "telegramengine.h"
#include "telegramshareddatamanager.h"
#include "telegramthumbnailer.h"
#include "telegramenums.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QPointer>
#include <QDateTime>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QUrl>
#include <QImageReader>

class TelegramUploadHandlerPrivate
{
public:
    QPointer<TelegramEngine> engine;
    TelegramSharedPointer<InputPeerObject> currentPeer;
    QString text;
    QString file;
    bool silent;
    bool noWebpage;
    int sendFileType;
    int status;
    bool supergroup;
    QString alt;
    qint32 duration;
    qint32 flags;
    qint32 h;
    QString performer;
    InputStickerSet stickerset;
    QString title;
    qint32 w;
    bool voice;
    QByteArray fakeKey;
    QUrl thumbnail;

    qint32 totalSize;
    qint32 transfaredSize;

    TelegramSharedPointer<MessageObject> result;
    TelegramSharedPointer<MessageObject> target;
    TelegramSharedPointer<MessageObject> replyTo;
    QPointer<ReplyMarkupObject> replyMarkup;

    TelegramThumbnailer *thumbnailer;

    static QSet<TelegramUploadHandler*> objects;
};

QSet<TelegramUploadHandler*> TelegramUploadHandlerPrivate::objects;

TelegramUploadHandler::TelegramUploadHandler(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramUploadHandlerPrivate;
    p->thumbnailer = 0;
    p->silent = false;
    p->noWebpage = false;
    p->sendFileType = TelegramEnums::SendFileTypeAutoDetect;
    p->status = StatusNone;
    p->totalSize = 0;
    p->transfaredSize = 0;
    p->supergroup = false;
    p->duration = 0;
    p->h = 200;
    p->w = 300;
    p->voice = false;
    TelegramUploadHandlerPrivate::objects.insert(this);
}

void TelegramUploadHandler::setEngine(TelegramEngine *engine)
{
    PROPERTY_SET_TRY(engine);
}

TelegramEngine *TelegramUploadHandler::engine() const
{
    PROPERTY_GET_TRY(engine);
}

void TelegramUploadHandler::setCurrentPeer(InputPeerObject *currentPeer)
{
    PROPERTY_SET_TRY(currentPeer);
}

InputPeerObject *TelegramUploadHandler::currentPeer() const
{
    PROPERTY_GET_TRY(currentPeer);
}

void TelegramUploadHandler::setText(const QString &text)
{
    PROPERTY_SET_TRY(text);
}

QString TelegramUploadHandler::text() const
{
    PROPERTY_GET_TRY(text);
}

void TelegramUploadHandler::setFile(const QString &file)
{
    PROPERTY_SET_TRY(file);
    setTotalSize(QFileInfo(QUrl(file).toLocalFile()).size());
}

QString TelegramUploadHandler::file() const
{
    PROPERTY_GET_TRY(file);
}

void TelegramUploadHandler::setAlt(const QString &alt)
{
    PROPERTY_SET_TRY(alt);
}

QString TelegramUploadHandler::alt() const
{
    PROPERTY_GET_TRY(alt);
}

void TelegramUploadHandler::setDuration(qint32 duration)
{
    PROPERTY_SET_TRY(duration);
}

qint32 TelegramUploadHandler::duration() const
{
    PROPERTY_GET_TRY(duration);
}

void TelegramUploadHandler::setH(qint32 h)
{
    PROPERTY_SET_TRY(h);
}

qint32 TelegramUploadHandler::h() const
{
    PROPERTY_GET_TRY(h);
}

void TelegramUploadHandler::setPerformer(const QString &performer)
{
    PROPERTY_SET_TRY(performer);
}

QString TelegramUploadHandler::performer() const
{
    PROPERTY_GET_TRY(performer);
}

void TelegramUploadHandler::setStickerset(const InputStickerSet &stickerset)
{
    PROPERTY_SET_TRY(stickerset);
}

InputStickerSet TelegramUploadHandler::stickerset() const
{
    PROPERTY_GET_TRY(stickerset);
}

void TelegramUploadHandler::setTitle(const QString &title)
{
    PROPERTY_SET_TRY(title);
}

QString TelegramUploadHandler::title() const
{
    PROPERTY_GET_TRY(title);
}

void TelegramUploadHandler::setVoice(bool voice)
{
    PROPERTY_SET_TRY(voice);
}

bool TelegramUploadHandler::voice() const
{
    PROPERTY_GET_TRY(voice);
}

void TelegramUploadHandler::setW(qint32 w)
{
    PROPERTY_SET_TRY(w);
}

qint32 TelegramUploadHandler::w() const
{
    PROPERTY_GET_TRY(w);
}

void TelegramUploadHandler::setSilent(bool silent)
{
    PROPERTY_SET_TRY(silent);
}

bool TelegramUploadHandler::silent() const
{
    PROPERTY_GET_TRY(silent);
}

void TelegramUploadHandler::setSupergroup(bool supergroup)
{
    PROPERTY_SET_TRY(supergroup);
}

bool TelegramUploadHandler::supergroup() const
{
    PROPERTY_GET_TRY(supergroup);
}

void TelegramUploadHandler::setNoWebpage(bool noWebpage)
{
    PROPERTY_SET_TRY(noWebpage);
}

bool TelegramUploadHandler::noWebpage() const
{
    PROPERTY_GET_TRY(noWebpage);
}

void TelegramUploadHandler::setSendFileType(int sendFileType)
{
    PROPERTY_SET_TRY(sendFileType);
}

int TelegramUploadHandler::sendFileType() const
{
    PROPERTY_GET_TRY(sendFileType);
}

void TelegramUploadHandler::setReplyTo(MessageObject *replyTo)
{
    PROPERTY_SET_TRY(replyTo);
}

MessageObject *TelegramUploadHandler::replyTo() const
{
    PROPERTY_GET_TRY(replyTo);
}

void TelegramUploadHandler::setReplyMarkup(ReplyMarkupObject *replyMarkup)
{
    PROPERTY_SET_TRY(replyMarkup);
}

ReplyMarkupObject *TelegramUploadHandler::replyMarkup() const
{
    PROPERTY_GET_TRY(replyMarkup);
}

void TelegramUploadHandler::setStatus(int status)
{
    PROPERTY_SET_TRY(status);
}

int TelegramUploadHandler::status() const
{
    PROPERTY_GET_TRY(status);
}

void TelegramUploadHandler::setFakeKey(const QByteArray &fakeKey)
{
    PROPERTY_SET_TRY(fakeKey);
}

QByteArray TelegramUploadHandler::fakeKey() const
{
    PROPERTY_GET_TRY(fakeKey);
}

void TelegramUploadHandler::setTarget(MessageObject *target)
{
    PROPERTY_SET_TRY(target);
}

MessageObject *TelegramUploadHandler::target() const
{
    PROPERTY_GET_TRY(target);
}

MessageObject *TelegramUploadHandler::result() const
{
    PROPERTY_GET_TRY(result);
}

void TelegramUploadHandler::setResult(const Message &message)
{
    if(p->result && p->result->operator ==(message))
        return;
    TelegramSharedDataManager *tsdm = p->engine->sharedData();
    if(!tsdm)
        p->result = new MessageObject(message);
    else
        p->result = tsdm->insertMessage(message);

    Q_EMIT resultChanged();
}

qint32 TelegramUploadHandler::transfaredSize() const
{
    PROPERTY_GET_TRY(transfaredSize);
}

void TelegramUploadHandler::setTransfaredSize(qint32 transfaredSize)
{
    PROPERTY_SET_TRY(transfaredSize);
}

qint32 TelegramUploadHandler::totalSize() const
{
    PROPERTY_GET_TRY(totalSize);
}

QUrl TelegramUploadHandler::thumbnail() const
{
    PROPERTY_GET_TRY(thumbnail);
}

void TelegramUploadHandler::setThumbnail(const QUrl &thumbnail)
{
    PROPERTY_SET_TRY(thumbnail);
}

void TelegramUploadHandler::setTotalSize(qint32 totalSize)
{
    PROPERTY_SET_TRY(totalSize);
}

void TelegramUploadHandler::onUpdate(const UpdatesType &updates, const Message &sentMsg)
{
    TelegramTools::analizeUpdatesType(updates, p->engine, [this](const Update &update){
        insertUpdate(update);
    }, sentMsg);
}

void TelegramUploadHandler::insertUpdate(const Update &update)
{
    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
    {
        const Message &msg = update.message();
        setResult(msg);
    }
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteMessages:
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
        break;
    case Update::typeUpdateChatParticipants:
        break;
    case Update::typeUpdateUserStatus:
        break;
    case Update::typeUpdateUserName:
        break;
    case Update::typeUpdateUserPhoto:
        break;
    case Update::typeUpdateContactRegistered:
        break;
    case Update::typeUpdateContactLink:
        break;
    case Update::typeUpdateNewAuthorization:
        break;
    case Update::typeUpdateNewEncryptedMessage:
        break;
    case Update::typeUpdateEncryptedChatTyping:
        break;
    case Update::typeUpdateEncryption:
        break;
    case Update::typeUpdateEncryptedMessagesRead:
        break;
    case Update::typeUpdateChatParticipantAdd:
        break;
    case Update::typeUpdateChatParticipantDelete:
        break;
    case Update::typeUpdateDcOptions:
        break;
    case Update::typeUpdateUserBlocked:
        break;
    case Update::typeUpdateNotifySettings:
        break;
    case Update::typeUpdateServiceNotification:
        break;
    case Update::typeUpdatePrivacy:
        break;
    case Update::typeUpdateUserPhone:
        break;
    case Update::typeUpdateReadHistoryInbox:
        break;
    case Update::typeUpdateReadHistoryOutbox:
        break;
    case Update::typeUpdateWebPage:
        break;
    case Update::typeUpdateReadMessagesContents:
        break;
    case Update::typeUpdateChannelTooLong:
        break;
    case Update::typeUpdateChannel:
        break;
    case Update::typeUpdateReadChannelInbox:
        break;
    case Update::typeUpdateDeleteChannelMessages:
        break;
    case Update::typeUpdateChannelMessageViews:
        break;
    case Update::typeUpdateChatAdmins:
        break;
    case Update::typeUpdateChatParticipantAdmin:
        break;
    case Update::typeUpdateNewStickerSet:
        break;
    case Update::typeUpdateStickerSetsOrder:
        break;
    case Update::typeUpdateStickerSets:
        break;
    case Update::typeUpdateSavedGifs:
        break;
    case Update::typeUpdateBotInlineQuery:
        break;
    case Update::typeUpdateBotInlineSend:
        break;
    }
}

QList<TelegramUploadHandler *> TelegramUploadHandler::getItems(TelegramEngine *engine, InputPeerObject *peer)
{
    QList<TelegramUploadHandler*> result;
    Q_FOREACH(TelegramUploadHandler *item, TelegramUploadHandlerPrivate::objects)
        if(item->p->engine == engine && (item->p->currentPeer == peer || !peer))
            result << item;
    return result;
}

QList<TelegramUploadHandler *> TelegramUploadHandler::getItems()
{
    return getItems(p->engine, p->currentPeer);
}

QStringList TelegramUploadHandler::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine) << FUNCTION_NAME(currentPeer);
}

bool TelegramUploadHandler::send()
{
    if(p->status != StatusNone && p->status != StatusError)
        return false;

    setFakeKey(QByteArray());
    if(p->file.isEmpty())
        return sendMessage();
    else
        return sendFile();
}

void TelegramUploadHandler::cancel()
{
    if(p->status != StatusUploading)
        return;
    if(!p->target)
        return;
}

bool TelegramUploadHandler::sendMessage()
{
    Message newMsg = createNewMessage();
    if(newMsg.classType() == Message::typeMessageEmpty)
        return false;

    newMsg.setMessage(p->text);
    if(p->replyTo)
        newMsg.setReplyToMsgId(p->replyTo->id());
    if(p->replyMarkup)
        newMsg.setReplyMarkup(p->replyMarkup->core());

    setResult(newMsg);
    setFakeKey(TelegramTools::identifier(newMsg));
    setStatus(StatusSending);

    DEFINE_DIS;
    Telegram *tg = p->engine->telegram();
    if(!tg) return false;

    const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel && !p->supergroup);

    tg->messagesSendMessage(p->noWebpage, broadcast, p->silent, false, p->currentPeer->core(), p->replyTo?p->replyTo->id():0,
                            p->text, TelegramTools::generateRandomId(), p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                            QList<MessageEntity>(), [this, dis, newMsg](TG_MESSAGES_SEND_MESSAGE_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis || !p->engine) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        onUpdate(result, newMsg);
        setStatus(StatusDone);
    });

    return true;
}

bool TelegramUploadHandler::sendFile()
{
    QFileInfo fileInfo(p->file);

    QMimeDatabase mdb;
    QMimeType t = mdb.mimeTypeForFile(p->file);

    int type = p->sendFileType;
    if(type == TelegramEnums::SendFileTypeAutoDetect)
    {
        if(t.name().contains("webp") || p->file.right(5) == ".webp")
            type = TelegramEnums::SendFileTypeSticker;
        else
        if(!t.name().contains("gif") && t.name().contains("image/"))
            type = TelegramEnums::SendFileTypePhoto;
        else
        if(t.name().contains("video/"))
            type = TelegramEnums::SendFileTypeVideo;
        else
        if(t.name().contains("audio/"))
            type = TelegramEnums::SendFileTypeAudio;
        else
            type = TelegramEnums::SendFileTypeDocument;
    }

    const qint64 uid = TelegramTools::generateRandomId();

    MessageMedia media(MessageMedia::typeMessageMediaEmpty);

    Photo photo;
    photo.setId(uid);
    photo.setAccessHash(0);
    photo.setDate(QDateTime::currentDateTime().toTime_t());

    Document doc;
    doc.setId(uid);
    doc.setAccessHash(0);
    doc.setDate(QDateTime::currentDateTime().toTime_t());
    doc.setMimeType(t.name());
    doc.setSize(fileInfo.size());

    switch(type)
    {
    case TelegramEnums::SendFileTypeDocument:
    {
        DocumentAttribute attrName(DocumentAttribute::typeDocumentAttributeFilename);
        attrName.setFileName(fileInfo.fileName());

        doc.setAttributes(QList<DocumentAttribute>()<<attrName);
        doc.setClassType(Document::typeDocument);
        media.setClassType(MessageMedia::typeMessageMediaDocument);
    }
        break;
    case TelegramEnums::SendFileTypeAudio:
    {
        DocumentAttribute attrName(DocumentAttribute::typeDocumentAttributeFilename);
        attrName.setFileName(fileInfo.fileName());

        DocumentAttribute attrAudio(DocumentAttribute::typeDocumentAttributeAudio);
        attrAudio.setVoice(p->voice);
        attrAudio.setDuration(p->duration);
        attrAudio.setTitle(p->title);
        attrAudio.setPerformer(p->performer);

        doc.setAttributes(QList<DocumentAttribute>()<<attrName<<attrAudio);
        doc.setClassType(Document::typeDocument);
        media.setClassType(MessageMedia::typeMessageMediaDocument);
    }
        break;
    case TelegramEnums::SendFileTypeSticker:
    {
        DocumentAttribute attrSticker(DocumentAttribute::typeDocumentAttributeSticker);
        attrSticker.setAlt(p->alt);
        attrSticker.setStickerset(p->stickerset);

        doc.setAttributes(QList<DocumentAttribute>()<<attrSticker);
        doc.setClassType(Document::typeDocument);
        media.setClassType(MessageMedia::typeMessageMediaDocument);
    }
        break;
    case TelegramEnums::SendFileTypeVideo:
    {
        DocumentAttribute attrName(DocumentAttribute::typeDocumentAttributeFilename);
        attrName.setFileName(fileInfo.fileName());

        DocumentAttribute attrVideo(DocumentAttribute::typeDocumentAttributeVideo);
        attrVideo.setDuration(p->duration);
        attrVideo.setW(p->w);
        attrVideo.setH(p->h);

        doc.setAttributes(QList<DocumentAttribute>()<<attrName<<attrVideo);
        doc.setClassType(Document::typeDocument);
        media.setClassType(MessageMedia::typeMessageMediaDocument);
    }
        break;
    case TelegramEnums::SendFileTypePhoto:
    {
        QImageReader image(p->file);
        QSize imgSize = image.size();

        setW(imgSize.width());
        setH(imgSize.height());

        FileLocation location(FileLocation::typeFileLocation);
        location.setLocalId(uid);
        location.setVolumeId(uid);

        PhotoSize size(PhotoSize::typePhotoSize);
        size.setW(p->w);
        size.setH(p->h);
        size.setLocation(location);

        photo.setSizes(QList<PhotoSize>()<<size);
        photo.setClassType(Photo::typePhoto);
        media.setClassType(MessageMedia::typeMessageMediaPhoto);
    }
        break;
    case TelegramEnums::SendFileTypeAnimated:
    {
        DocumentAttribute attrAnimated(DocumentAttribute::typeDocumentAttributeAnimated);
        doc.setAttributes(QList<DocumentAttribute>()<<attrAnimated);
        doc.setClassType(Document::typeDocument);
        media.setClassType(MessageMedia::typeMessageMediaDocument);
    }
        break;
    }

    media.setDocument(doc);
    media.setPhoto(photo);

    return sendDocument(media);
}

bool TelegramUploadHandler::sendDocument(const MessageMedia &media)
{
    Message newMsg = createNewMessage();
    if(newMsg.classType() == Message::typeMessageEmpty)
        return false;

    newMsg.setMedia(media);
    if(p->replyTo)
        newMsg.setReplyToMsgId(p->replyTo->id());
    if(p->replyMarkup)
        newMsg.setReplyMarkup(p->replyMarkup->core());

    setResult(newMsg);
    setFakeKey(TelegramTools::identifier(newMsg));
    setStatus(StatusSending);

    if(!p->thumbnailer)
        p->thumbnailer = new TelegramThumbnailer(this);

    QString thumbnail = p->thumbnailer->getThumbPath(p->engine->tempPath(), p->file);

    DEFINE_DIS;
    p->thumbnailer->createThumbnail(p->file, thumbnail, [this, dis, thumbnail](){
        if(!dis || !p->engine) return;
        sendDocument_step2(p->sendFileType, thumbnail);
    });

    return true;
}

void TelegramUploadHandler::sendDocument_step2(int type, const QString &thumbnail)
{
    Telegram *tg = p->engine->telegram();
    if(!tg) return;

    const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel && !p->supergroup);

    Message newMsg = p->result->core();
    DEFINE_DIS;
    TelegramCore::Callback<UploadSendFile> callback = [this, dis, newMsg](TG_UPLOAD_SEND_FILE_CUSTOM_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        switch(static_cast<int>(result.classType()))
        {
        case UploadSendFile::typeUploadSendFileCanceled:
        case UploadSendFile::typeUploadSendFileFinished:
        case UploadSendFile::typeUploadSendFileEmpty:
        {
            onUpdate(result.updates(), newMsg);
            setStatus(StatusDone);
        }
            break;

        case UploadSendFile::typeUploadSendFileProgress:
            setTotalSize(result.totalSize());
            setTransfaredSize(result.uploaded());
            break;
        }
    };

    QString filePath = QUrl(p->file).toLocalFile();

    switch(type)
    {
    case TelegramEnums::SendFileTypePhoto:
        tg->messagesSendPhoto(p->currentPeer->core(), TelegramTools::generateRandomId(), filePath,
                              p->replyTo?p->replyTo->id():0, p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                              broadcast, p->silent, false, callback);
        break;
    case TelegramEnums::SendFileTypeAnimated:
    case TelegramEnums::SendFileTypeSticker:
    case TelegramEnums::SendFileTypeDocument:
        tg->messagesSendDocument(p->currentPeer->core(), TelegramTools::generateRandomId(), filePath,
                                 QFileInfo::exists(thumbnail)?thumbnail:"", (type == TelegramEnums::SendFileTypeSticker),
                                 p->replyTo?p->replyTo->id():0, p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                                 broadcast, p->silent, false, callback);
        break;
    case TelegramEnums::SendFileTypeVideo:
        tg->messagesSendVideo(p->currentPeer->core(), TelegramTools::generateRandomId(), filePath,
                              0, 0, 0, QFileInfo::exists(thumbnail)?thumbnail:"",
                              p->replyTo?p->replyTo->id():0, p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                              broadcast, p->silent, false, callback);
        break;
    case TelegramEnums::SendFileTypeAudio:
        tg->messagesSendAudio(p->currentPeer->core(), TelegramTools::generateRandomId(), filePath,
                              0, p->replyTo?p->replyTo->id():0, p->replyMarkup?p->replyMarkup->core():ReplyMarkup::null,
                              broadcast, p->silent, false, callback);
        break;
    }
}

QByteArray TelegramUploadHandler::identifier() const
{
    return p->currentPeer? TelegramTools::identifier(p->currentPeer->core()) : QByteArray();
}

Message TelegramUploadHandler::createNewMessage()
{
    Message msg;
    if(!p->engine || !p->engine->our() || !p->engine->telegram() || !p->currentPeer)
        return msg;

    User user = p->engine->our()->user()->core();

    msg.setFromId(user.id());
    msg.setOut(true);
    msg.setViews(0);
    msg.setToId(TelegramTools::inputPeerPeer(p->currentPeer->core()));
    msg.setClassType(Message::typeMessage);
    msg.setDate(QDateTime::currentDateTime().toTime_t());
    msg.setId(TelegramTools::generateRandomId());

    return msg;
}

TelegramUploadHandler::~TelegramUploadHandler()
{
    TelegramUploadHandlerPrivate::objects.remove(this);
    delete p;
}

