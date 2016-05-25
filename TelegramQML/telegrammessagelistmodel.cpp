#define DEFINE_DIS \
    QPointer<TelegramMessageListModel> dis = this;

#define PROCESS_ROW_CHANGE(KEY, ROLES) \
    int row = p->list.indexOf(KEY); \
    if(row >= 0) \
        Q_EMIT dataChanged(index(row), index(row), QVector<int>()ROLES);

#include "telegrammessagelistmodel.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"
#include "telegramcache.h"
#include "private/telegramuploadhandler.h"
#include "private/telegramdownloadhandler.h"

#include <QDateTime>
#include <QQmlEngine>
#include <QPointer>
#include <QtQml>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>
#include <telegram.h>

class TelegramMessageListItem
{
public:
    TelegramMessageListItem() {}
    virtual ~TelegramMessageListItem() {}
    QByteArray id;
    TelegramSharedPointer<ChatObject> fromChat;
    TelegramSharedPointer<UserObject> fromUser;

    TelegramSharedPointer<ChatObject> toChat;
    TelegramSharedPointer<UserObject> toUser;

    TelegramSharedPointer<InputPeerObject> toPeerItem;

    TelegramSharedPointer<MessageObject> message;

    TelegramSharedPointer<UserObject> fwdUser;
    TelegramSharedPointer<ChatObject> fwdChat;
    TelegramSharedPointer<UserObject> replyUser;
    TelegramSharedPointer<ChatObject> replyChat;
    TelegramSharedPointer<MessageObject> replyMsg;

    QPointer<TelegramUploadHandler> upload;
    QPointer<TelegramDownloadHandler> download;
};

class TelegramMessageListModelPrivate
{
public:
    qint64 lastRequest;
    bool refreshing;
    bool hasBackMore;

    QList<QByteArray> list;
    QHash<QByteArray, TelegramMessageListItem> items;
    QSet<QObject*> connecteds;

    TelegramSharedPointer<InputPeerObject> currentPeer;
    TelegramSharedPointer<DialogObject> currentDialog;
    QList<qint32> messageList;
    QSet<QByteArray> sendings;
    QJSValue dateConvertorMethod;
    bool useCache;
    int limit;

    QHash<ChatObject*, QHash<UserObject*, int> > typingChats;

    int repliesTimer;
    int loadSuspenderTimer;
    bool lastIsCache;

    QHash<QByteArray, Message> repliesToFetch;
};

TelegramMessageListModel::TelegramMessageListModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramMessageListModelPrivate;
    p->refreshing = false;
    p->hasBackMore = false;
    p->repliesTimer = 0;
    p->loadSuspenderTimer = 0;
    p->lastRequest = 0;
    p->limit = 100;
    p->lastIsCache = false;
    p->useCache = true;
}

bool TelegramMessageListModel::refreshing() const
{
    return p->refreshing;
}

QByteArray TelegramMessageListModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramMessageListModel::count() const
{
    return p->list.count();
}

QVariant TelegramMessageListModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const QByteArray &key = TelegramMessageListModel::id(index);
    const TelegramMessageListItem &item = p->items.value(key);

    switch(role)
    {
    case RoleMessageItem:
        result = QVariant::fromValue<MessageObject*>(item.message);
        break;
    case RoleMediaItem:
        result = QVariant::fromValue<MessageMediaObject*>(item.message->media());
        break;
    case RoleServiceItem:
        result = QVariant::fromValue<MessageActionObject*>(item.message->action());
        break;
    case RoleMarkupItem:
        result = QVariant::fromValue<ReplyMarkupObject*>(item.message->replyMarkup());
        break;
    case RoleEntityList:
        break;
    case RoleFromUserItem:
        result = QVariant::fromValue<UserObject*>(item.fromUser);
        break;
    case RoleToUserItem:
        result = QVariant::fromValue<UserObject*>(item.toUser);
        break;
    case RoleToChatItem:
        result = QVariant::fromValue<ChatObject*>(item.toChat);
        break;
    case RoleToPeerItem:
        result = QVariant::fromValue<InputPeerObject*>(item.toPeerItem);
        break;

    case RoleMessage:
        if(item.message) {
            if(!item.message->media()->caption().isEmpty())
                result = item.message->media()->caption();
            else
                result = item.message->message();
        } else {
            result = "";
        }
        break;
    case RoleDateTime:
        result = convertDate( QDateTime::fromTime_t(item.message->date()) );
        break;
    case RoleDate:
        result = QDateTime::fromTime_t(item.message->date()).date().toString("yyyy-MM-dd");
        break;
    case RoleUnread:
        if(p->currentDialog) result = (p->currentDialog->readOutboxMaxId()<item.message->id());
        else result = true;
        break;
    case RoleSent:
        result = !p->sendings.contains(item.id);
        break;
    case RoleOut:
        result = item.message->out();
        break;
    case RoleReplyMessage:
        result = QVariant::fromValue<MessageObject*>(item.replyMsg);
        break;
    case RoleReplyMsgId:
        if(item.message)
            result = item.message->replyToMsgId();
        else
            result = 0;
        break;
    case RoleReplyPeer:
        if(item.replyUser) result = QVariant::fromValue<UserObject*>(item.replyUser);
        else
        if(item.replyChat) result = QVariant::fromValue<ChatObject*>(item.replyChat);
        break;
    case RoleForwardFromPeer:
        if(item.fwdUser) result = QVariant::fromValue<UserObject*>(item.fwdUser);
        else
        if(item.fwdChat) result = QVariant::fromValue<ChatObject*>(item.fwdChat);
        break;
    case RoleForwardDate:
        result = QDateTime::fromTime_t(item.message->fwdFrom()->date());
        break;

    case RoleMessageType:
        result = static_cast<int>(TelegramTools::messageType(item.message));
        break;

    case RoleReplyType:
        result = static_cast<int>(TelegramTools::messageType(item.replyMsg));
        break;

    case RoleFileName:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeFilename)
                {
                    result = attr.fileName();
                    break;
                }
        }
        break;
    case RoleFileMimeType:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document())
            result = item.message->media()->document()->mimeType();
        break;
    case RoleFileTitle:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                {
                    result = attr.title();
                    break;
                }
        }
        break;
    case RoleFilePerformer:
        result = "";
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                {
                    result = attr.performer();
                    break;
                }
        }
        break;
    case RoleFileDuration:
        result = 0;
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio ||
                   attr.classType() == DocumentAttribute::typeDocumentAttributeVideo)
                {
                    result = attr.duration();
                    break;
                }
        }
        break;
    case RoleFileIsVoice:
        result = false;
        if(item.message && item.message->media() && item.message->media()->document()) {
            Q_FOREACH(const DocumentAttribute &attr, item.message->media()->document()->attributes())
                if(attr.classType() == DocumentAttribute::typeDocumentAttributeAudio)
                {
                    result = attr.voice();
                    break;
                }
        }
        break;
    case RoleFileSize:
        if(item.message && item.message->media() && item.message->media()->document())
            result = item.message->media()->document()->size();
        else
            result = 0;
        break;

    case RoleDownloadable:
    {
        MessageMediaObject *media = item.message->media();
        if(media)
            result = (media->classType() == MessageMediaObject::TypeMessageMediaDocument ||
                      media->classType() == MessageMediaObject::TypeMessageMediaPhoto);
        else
            result = false;
    }
        break;

    case RoleUploading:
        result = (item.upload != 0);
        break;
    case RoleDownloading:
        result = (item.download != 0);
        break;
    case RoleTransfaring:
        if(item.upload)
            result = true;
        else
        if(item.download)
            result = item.download->downloading();
        else
            result = false;
        break;
    case RoleTransfared:
        if(item.upload)
            result = (item.upload->status() == TelegramUploadHandler::StatusDone);
        else
        if(item.download)
            result = item.download->check();
        else
            result = false;
        break;
    case RoleTransfaredSize:
        if(item.upload)
            result = item.upload->transfaredSize();
        else
        if(item.download)
            result = item.download->downloadedSize();
        else
            result = 0;
        break;

    case RoleTotalSize:
        if(item.upload)
            result = item.upload->totalSize();
        else
        if(item.download)
            result = (item.download->downloadTotal()? item.download->downloadTotal() : item.download->size());
        else
            result = 0;
        break;

    case RoleFilePath:
        if(item.upload)
            result = item.upload->file();
        else
        if(item.download)
            result = item.download->destination();
        else
            result = "";
        break;
    case RoleThumbPath:
        if(item.upload)
            result = item.upload->file();
        else
        if(item.download)
            result = item.download->thumbnail();
        else
            result = "";
        break;
    }
    return result;
}

bool TelegramMessageListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    const QByteArray &key = TelegramMessageListModel::id(index);
    const TelegramMessageListItem &item = p->items.value(key);

    switch(role)
    {
    case RoleDownloading:
        if(item.download)
        {
            if(value.toBool())
                item.download->download();
            else
                item.download->stop();
            result = true;
        }
        break;
    }

    return result;
}

QHash<int, QByteArray> TelegramMessageListModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(RoleMessage, "message");
    result->insert(RoleDateTime, "dateTime");
    result->insert(RoleDate, "date");
    result->insert(RoleUnread, "unread");
    result->insert(RoleSent, "sent");
    result->insert(RoleOut, "out");
    result->insert(RoleReplyMsgId, "replyMsgId");
    result->insert(RoleReplyMessage, "replyMessage");
    result->insert(RoleReplyPeer, "replyPeer");
    result->insert(RoleForwardFromPeer, "forwardFromPeer");
    result->insert(RoleForwardDate, "forwardDate");
    result->insert(RoleMessageType, "messageType");
    result->insert(RoleReplyType, "replyType");

    result->insert(RoleMessageItem, "item");
    result->insert(RoleMediaItem, "mediaItem");
    result->insert(RoleServiceItem, "serviceItem");
    result->insert(RoleMarkupItem, "markupItem");
    result->insert(RoleEntityList, "entityList");
    result->insert(RoleFromUserItem, "fromUserItem");
    result->insert(RoleToUserItem, "toUserItem");
    result->insert(RoleToChatItem, "toChatItem");
    result->insert(RoleToPeerItem, "toPeerItem");

    result->insert(RoleFileName, "fileName");
    result->insert(RoleFileMimeType, "fileMimeType");
    result->insert(RoleFileTitle, "fileTitle");
    result->insert(RoleFilePerformer, "filePerformer");
    result->insert(RoleFileDuration, "fileDuration");
    result->insert(RoleFileIsVoice, "fileIsVoice");
    result->insert(RoleFileSize, "fileSize");

    result->insert(RoleDownloadable, "downloadable");
    result->insert(RoleUploading, "uploading");
    result->insert(RoleDownloading, "downloading");
    result->insert(RoleTransfaring, "transfaring");
    result->insert(RoleTransfared, "transfared");
    result->insert(RoleTransfaredSize, "transfaredSize");
    result->insert(RoleTotalSize, "totalSize");
    result->insert(RoleFilePath, "filePath");
    result->insert(RoleThumbPath, "thumbPath");

    return *result;
}

void TelegramMessageListModel::setCurrentPeer(InputPeerObject *peer)
{
    if(p->currentPeer == peer)
        return;

    p->currentPeer = peer;
    refresh();
    Q_EMIT currentPeerChanged();
    Q_EMIT keyChanged();
    Q_EMIT megagroupChanged();
    Q_EMIT editableChanged();
}

InputPeerObject *TelegramMessageListModel::currentPeer() const
{
    return p->currentPeer;
}

void TelegramMessageListModel::setMessageList(const QList<qint32> &list)
{
    if(p->messageList == list)
        return;

    p->messageList = list;
    refresh();
    Q_EMIT messageListChanged();
}

QList<qint32> TelegramMessageListModel::messageList() const
{
    return p->messageList;
}

QJSValue TelegramMessageListModel::dateConvertorMethod() const
{
    return p->dateConvertorMethod;
}

void TelegramMessageListModel::setDateConvertorMethod(const QJSValue &method)
{
    if(p->dateConvertorMethod.isNull() && method.isNull())
        return;

    p->dateConvertorMethod = method;
    Q_EMIT dateConvertorMethodChanged();
}

void TelegramMessageListModel::setUseCache(bool useCache)
{
    if(p->useCache == useCache)
        return;

    p->useCache = useCache;
    Q_EMIT useCacheChanged();
}

bool TelegramMessageListModel::useCache() const
{
    return p->useCache;
}

bool TelegramMessageListModel::megagroup() const
{
    if(!p->currentPeer || !mEngine)
        return false;

    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    QByteArray key = TelegramTools::identifier(p->currentPeer->core());
    TelegramSharedPointer<ChatObject> chat = tsdm->getChat(key);
    if(!chat)
        return false;

    return chat->megagroup();
}

bool TelegramMessageListModel::editable() const
{
    if(!p->currentPeer || !mEngine)
        return false;

    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    QByteArray key = TelegramTools::identifier(p->currentPeer->core());
    TelegramSharedPointer<ChatObject> chat = tsdm->getChat(key);
    if(!chat || chat->classType() == ChatObject::TypeChat)
        return true;

    return chat->moderator() || chat->editor() || chat->creator() || chat->democracy() || chat->megagroup();
}

int TelegramMessageListModel::limit() const
{
    return p->limit;
}

void TelegramMessageListModel::setLimit(int limit)
{
    if(p->limit == limit)
        return;

    p->limit = limit;
    Q_EMIT limitChanged();
}

QByteArray TelegramMessageListModel::key() const
{
    if(p->currentPeer)
        return TelegramTools::identifier( TelegramTools::inputPeerPeer(p->currentPeer->core()) );
    else
        return QByteArray();
}

QVariantList TelegramMessageListModel::typingUsers() const
{
    QVariantList result;
    if(!mEngine || !mEngine->sharedData())
        return result;

    QPointer<TelegramSharedDataManager> tsdm = mEngine->sharedData();
    TelegramSharedPointer<ChatObject> chat = tsdm->getChat(key());

    QList<UserObject*> users = p->typingChats.value(chat).keys();
    Q_FOREACH(UserObject *user, users)
        result << QVariant::fromValue<QObject*>(user);
    return result;
}

QStringList TelegramMessageListModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine)
                         << FUNCTION_NAME(currentPeer);
}

bool TelegramMessageListModel::sendMessage(const QString &message, MessageObject *replyTo, ReplyMarkupObject *replyMarkup, const QJSValue &callback)
{
    TelegramUploadHandler *handler = new TelegramUploadHandler(this);
    handler->setEngine(mEngine);
    handler->setCurrentPeer(p->currentPeer);
    handler->setText(message);
    handler->setReplyTo(replyTo);
    handler->setReplyMarkup(replyMarkup);
    handler->setSupergroup(megagroup());

    p->lastIsCache = false;

    connect(handler, &TelegramUploadHandler::errorChanged, this, [this, handler]{
        setError(handler->errorText(), handler->errorCode());
        delete handler;
        resort();
    }, Qt::QueuedConnection);

    connect(handler, &TelegramUploadHandler::statusChanged, this, [this, handler, callback](){
        if(mEngine != handler->engine() || p->currentPeer != handler->currentPeer())
            return;
        if(!handler->result() || handler->status() != TelegramUploadHandler::StatusDone)
            return;

        TelegramSharedDataManager *tsdm = mEngine->sharedData();
        QByteArray key;
        const QByteArray oldKey = handler->fakeKey();
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(handler->result()->core(), &key);
        item.fromUser = tsdm->insertUser(mEngine->our()->user()->core());
        item.id = key;
        if(handler->replyTo())
            item.replyMsg = tsdm->insertMessage(handler->replyTo()->core());

        p->items[item.id] = item;
        const int oldIndex = p->list.indexOf(oldKey);
        if(oldIndex >= 0)
            p->list.replace(oldIndex, item.id);

        delete handler;
        connectMessageSignals(item.id, item.message);
        resort();

        PROCESS_ROW_CHANGE(item.id, << roleNames().keys().toVector())

        QByteArray toId = TelegramTools::identifier(item.message->toId()->core());
        TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(toId);
        if(dialog && dialog->topMessage() < item.message->id())
            dialog->setTopMessage(item.message->id());

        TelegramCache *cache = mEngine->cache();
        if(cache && p->useCache)
        {
            if(item.message) cache->insert(item.message->core());
            if(item.fwdUser) cache->insert(item.fwdUser->core());
            if(item.fwdChat) cache->insert(item.fwdChat->core());
            if(item.message) cache->insert(item.message->core());
            if(item.fromUser) cache->insert(item.fromUser->core());
            if(item.fromChat) cache->insert(item.fromChat->core());
        }

        if(callback.isCallable())
            QJSValue(callback).call();
    }, Qt::QueuedConnection);

    if(!handler->send())
    {
        delete handler;
        return false;
    }

    resort();
    return true;
}

bool TelegramMessageListModel::sendFile(int type, const QString &file, MessageObject *replyTo, ReplyMarkupObject *replyMarkup, const QJSValue &callback)
{
    TelegramUploadHandler *handler = new TelegramUploadHandler(this);
    handler->setEngine(mEngine);
    handler->setCurrentPeer(p->currentPeer);
    handler->setFile(file);
    handler->setSendFileType(type);
    handler->setReplyTo(replyTo);
    handler->setReplyMarkup(replyMarkup);
    handler->setSupergroup(megagroup());

    p->lastIsCache = false;

    connect(handler, &TelegramUploadHandler::errorChanged, this, [this, handler]{
        setError(handler->errorText(), handler->errorCode());
        delete handler;
        resort();
    }, Qt::QueuedConnection);

    connect(handler, &TelegramUploadHandler::statusChanged, this, [this, handler, callback](){
        if(mEngine != handler->engine() || p->currentPeer != handler->currentPeer())
            return;
        if(!handler->result() || handler->status() != TelegramUploadHandler::StatusDone)
            return;

        TelegramSharedDataManager *tsdm = mEngine->sharedData();
        QByteArray key;
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(handler->result()->core(), &key);
        item.fromUser = tsdm->insertUser(mEngine->our()->user()->core());
        item.id = key;
        if(handler->replyTo())
            item.replyMsg = tsdm->insertMessage(handler->replyTo()->core());

        p->items[item.id] = item;
        const int oldIndex = p->list.indexOf(handler->fakeKey());
        if(oldIndex >= 0)
            p->list.replace(oldIndex, item.id);

        delete handler;
        connectMessageSignals(item.id, item.message);
        resort();

        PROCESS_ROW_CHANGE(item.id, << roleNames().keys().toVector())

        QByteArray toId = TelegramTools::identifier(item.message->toId()->core());
        TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(toId);
        if(dialog && dialog->topMessage() < item.message->id())
            dialog->setTopMessage(item.message->id());

        if(callback.isCallable())
            QJSValue(callback).call();
    }, Qt::QueuedConnection);

    if(!handler->send())
    {
        delete handler;
        return false;
    }

    resort();

    MessageObject *msg = handler->result();
    if(msg)
    {
        const QByteArray &key = TelegramTools::identifier(msg->core());
        connectUploaderSignals(key, handler);
    }

    return true;
}

void TelegramMessageListModel::deleteMessages(const QList<qint32> &msgs, const QJSValue &jsCallback)
{
    QPointer<InputPeerObject> cPeer = currentPeer();
    if(!mEngine || !mEngine->telegram() || !cPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    Telegram::Callback<MessagesAffectedMessages> callback = [this, dis, msgs, jsCallback, cPeer](TG_MESSAGES_DELETE_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        TelegramCache *cache = mEngine && p->useCache? mEngine->cache() : 0;

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        Q_FOREACH(TelegramMessageListItem item, items) {
            if(!item.message)
                continue;
            if(!msgs.contains(item.message->id()))
                continue;

            items.remove(item.id);
            if(cache && cPeer)
                cache->deleteMessage(cPeer->core(), item.message->id());
        }

        changed(items);
        if(jsCallback.isCallable())
            QJSValue(jsCallback).call();
    };

    if(cPeer->classType() == InputPeerObject::TypeInputPeerChannel) {
        InputChannel input(InputChannel::typeInputChannel);
        input.setChannelId(cPeer->channelId());
        input.setAccessHash(cPeer->accessHash());
        tg->channelsDeleteMessages(input, msgs, callback);
    } else {
        tg->messagesDeleteMessages(msgs, callback);
    }
}

void TelegramMessageListModel::forwardMessages(InputPeerObject *fromInputPeer, const QList<qint32> &msgs, const QJSValue &callback)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer || !fromInputPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    /*! Fix access hash because of the damn javanscript problem
     * on the 64bit int type
     */
    if(fromInputPeer->channelId())
    {
        TelegramSharedPointer<ChatObject> chat = tsdm->getChat(TelegramTools::identifier(fromInputPeer->core()));
        if(chat)
            fromInputPeer->setAccessHash(chat->accessHash());
    }

    QList<qint64> randomIds;
    for(int i=0; i<msgs.count(); i++)
        randomIds << TelegramTools::generateRandomId();

//    const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel && !megagroup());

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesForwardMessages(false, false, fromInputPeer->core(), msgs,
                                randomIds, p->currentPeer->core(), [this, dis, callback](TG_MESSAGES_FORWARD_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        p->lastIsCache = false;
        onUpdates(result);
        if(callback.isCallable())
            QJSValue(callback).call();
    });
}

void TelegramMessageListModel::resendMessage(qint32 msgId, const QString &newCaption, const QJSValue &callback)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesGetMessages(QList<qint32>()<<msgId, [this, dis, newCaption, tg, callback](TG_MESSAGES_GET_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        if(result.messages().isEmpty()) {
            setError("TelegramQml internal error: resendMessage: Can't get the message", -1);
            return;
        }

        InputMedia media = TelegramTools::mediaInputMedia(result.messages().first().media());
        media.setCaption(newCaption);

        const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel);
        tg->messagesSendMedia(broadcast, false, false, p->currentPeer->core(), 0, media,
                              TelegramTools::generateRandomId(), ReplyMarkup::null,
                              [this, dis, callback](TG_MESSAGES_SEND_MEDIA_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            p->lastIsCache = false;
            onUpdates(result);
            if(callback.isCallable())
                QJSValue(callback).call();
        });
    });
}

void TelegramMessageListModel::sendSticker(DocumentObject *doc, MessageObject *replyTo, ReplyMarkupObject *replyMarkup, const QJSValue &callback)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    const bool broadcast = (p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel && !megagroup());

    InputDocument document(InputDocument::typeInputDocument);
    document.setId(doc->id());
    document.setAccessHash(doc->accessHash());

    InputMedia media(InputMedia::typeInputMediaDocument);
    media.setIdInputDocument(document);

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesSendMedia(broadcast, false, false, p->currentPeer->core(), replyTo? replyTo->id() : 0,
                          media, TelegramTools::generateRandomId(), replyMarkup? replyMarkup->core() : ReplyMarkup::null,
                          [this, dis, callback](TG_MESSAGES_SEND_MEDIA_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        p->lastIsCache = false;
        onUpdates(result);
        if(callback.isCallable())
            QJSValue(callback).call();
    });
}

void TelegramMessageListModel::markAsRead(const QJSValue &callback)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    QPointer<TelegramSharedDataManager> tsdm = mEngine->sharedData();

    const InputPeer &input = p->currentPeer->core();
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    if(input.classType() == InputPeer::typeInputPeerChannel)
    {
        InputChannel inputChannel(InputChannel::typeInputChannel);
        inputChannel.setChannelId(input.channelId());
        inputChannel.setAccessHash(input.accessHash());
        tg->channelsReadHistory(inputChannel, 0, [this, dis, input, tsdm, callback](TG_CHANNELS_READ_HISTORY_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            if(!result || !tsdm)
                return;

            const QByteArray &key = TelegramTools::identifier( TelegramTools::inputPeerPeer(input) );
            TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(key);
            if(dialog)
                dialog->setUnreadCount(0);
            if(callback.isCallable())
                QJSValue(callback).call();
        });
    }
    else
    {
        tg->messagesReadHistory(input, 0, [this, dis, input, tsdm, callback](TG_MESSAGES_READ_HISTORY_CALLBACK){
            Q_UNUSED(msgId)
            if(!dis) return;
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }
            if(!tsdm)
                return;

            const QByteArray &key = TelegramTools::identifier( TelegramTools::inputPeerPeer(input) );
            TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(key);
            if(dialog)
                dialog->setUnreadCount(0);
            if(callback.isCallable())
                QJSValue(callback).call();
        });
    }
}

void TelegramMessageListModel::clearHistory(bool justClear, const QJSValue &callback)
{
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    const InputPeer &input = p->currentPeer->core();
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    tg->messagesDeleteHistory(justClear, input, 0, [this, dis, input, callback](TG_MESSAGES_DELETE_HISTORY_CALLBACK){
        Q_UNUSED(msgId)
        Q_UNUSED(result)
        if(!dis || !mEngine) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        if(callback.isCallable())
            QJSValue(callback).call();
    });
}

void TelegramMessageListModel::clearHistoryAnswer(qint64 msgId, const MessagesAffectedHistory &result)
{
    Q_UNUSED(msgId)
    Q_UNUSED(result)
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    Telegram *tg = mEngine->telegram();
    const InputPeer &peer = tg->lastArguments().value("peer").value<InputPeer>();
    if(!p->currentPeer || !(p->currentPeer->core() == peer))
        return;

    mEngine->cache()->deleteMessages(peer);
    clean();
}

void TelegramMessageListModel::loadFrom(qint32 msgId)
{
    if(!p->currentPeer || !mEngine)
        return;

    clean();
    getMessagesFromServer(msgId, -p->limit/2, p->limit);
}

void TelegramMessageListModel::loadBack()
{
    if(!p->hasBackMore || !p->currentPeer || !mEngine)
        return;

    int offsetId = 0;
    if(!p->list.isEmpty())
    {
        const QByteArray &lastKey = p->list.last();
        const TelegramMessageListItem &item = p->items.value(lastKey);
        if(item.message)
            offsetId = item.message->id();
    }

    getMessagesFromServer(offsetId, 0, p->limit);
}

void TelegramMessageListModel::loadFront()
{
    if(!p->hasBackMore || !p->currentPeer || !mEngine)
        return;

    int offsetId = 0;
    if(!p->list.isEmpty())
    {
        const QByteArray &lastKey = p->list.first();
        const TelegramMessageListItem &item = p->items.value(lastKey);
        if(item.message)
            offsetId = item.message->id();
    }

    getMessagesFromServer(offsetId, -p->limit, p->limit);
}

void TelegramMessageListModel::refresh()
{
    clean();
    if(!mEngine || !mEngine->telegram() || !p->currentPeer)
        return;

    const QByteArray &peerId = TelegramTools::identifier(p->currentPeer->core());
    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    p->currentDialog = tsdm->getDialog(peerId);

    connectDialogSignals(peerId, p->currentDialog);

    p->repliesToFetch.clear();
    if(p->repliesTimer)
        QObject::killTimer(p->repliesTimer);

    if(!p->messageList.isEmpty())
        getMessageListFromServer();
    else
    {
        TelegramCache *cache = mEngine->cache();
        if(cache && p->useCache)
        {
            processOnResult(cache->readMessages(p->currentPeer->core(), 0, p->limit), true);
            p->lastIsCache = true;
        }

        Telegram *tg = mEngine->telegram();
        Settings *settings = tg->settings();
        bool isSecretChat = false;
        if(p->currentPeer && settings)
        {
            QList<SecretChat*> list = settings->secretChats();
            Q_FOREACH(SecretChat *sc, list)
            {
                InputPeer peer = TelegramTools::secretChatInputPeer(sc);
                if(peer == p->currentPeer->core())
                {
                    isSecretChat = true;
                    break;
                }
            }
        }

        if(!isSecretChat)
            getMessagesFromServer(0, 0, p->limit);
    }
}

void TelegramMessageListModel::clean()
{
    beginResetModel();
    if(p->loadSuspenderTimer)
        killTimer(p->loadSuspenderTimer);
    p->loadSuspenderTimer = 0;
    p->hasBackMore = true;
    p->lastIsCache = false;
    p->items.clear();
    p->list.clear();
    Q_EMIT countChanged();
    endResetModel();
}

void TelegramMessageListModel::resort()
{
    Q_FOREACH(const QByteArray &key, p->sendings)
        p->items.remove(key);

    p->sendings.clear();
    changed(p->items);
}

void TelegramMessageListModel::setRefreshing(bool stt)
{
    if(p->refreshing == stt)
        return;

    p->refreshing = stt;
    Q_EMIT refreshingChanged();
}

QByteArray TelegramMessageListModel::identifier() const
{
    return p->currentPeer? TelegramTools::identifier(p->currentPeer->core()) : QByteArray();
}

void TelegramMessageListModel::setHasBackMore(bool stt)
{
    p->hasBackMore = stt;
}

void TelegramMessageListModel::processOnResult(const MessagesMessages &result, bool append)
{
    QHash<QByteArray, TelegramMessageListItem> items;
    if(append && !p->lastIsCache)
        items = p->items;

    TelegramCache *cache = (mEngine? mEngine->cache() : 0);
    if(append && result.count()==0 && cache && p->currentPeer) /*! Clear cache if the dialog history cleared !*/
        cache->deleteMessages(p->currentPeer->core());

    processOnResult(result, &items);
    changed(items);
    fetchReplies(result.messages());
}

QString TelegramMessageListModel::convertDate(const QDateTime &td) const
{
    QQmlEngine *engine = qmlEngine(this);
    if(p->dateConvertorMethod.isCallable() && engine)
        return p->dateConvertorMethod.call(QList<QJSValue>()<<engine->toScriptValue<QDateTime>(td)).toString();
    else
    if(!p->dateConvertorMethod.isNull() && !p->dateConvertorMethod.isUndefined())
        return p->dateConvertorMethod.toString();
    else
    {
        const QDateTime &current = QDateTime::currentDateTime();
        const qint64 secs = td.secsTo(current);
        const int days = td.daysTo(current);
        if(secs < 24*60*60)
            return days? "Yesterday " + td.toString("HH:mm") : td.toString("HH:mm");
        else
            return td.toString("MMM dd, HH:mm");
    }
}

void TelegramMessageListModel::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == p->repliesTimer)
    {
        QObject::killTimer(p->repliesTimer);
        p->repliesTimer = 0;
        fetchReplies(p->repliesToFetch.values(), 0);
        p->repliesToFetch.clear();
    }
    else
    if(e->timerId() == p->loadSuspenderTimer)
    {
        killTimer(p->loadSuspenderTimer);
        p->loadSuspenderTimer = 0;
    }
    else
        TelegramAbstractEngineListModel::timerEvent(e);
}

void TelegramMessageListModel::connectTelegram()
{
    if(telegram() == mEngine->telegram())
        return;

    if(telegram())
        disconnect(telegram(), &Telegram::messagesDeleteHistoryAnswer, this, &TelegramMessageListModel::clearHistoryAnswer);
    if(mEngine->telegram())
        connect(mEngine->telegram(), &Telegram::messagesDeleteHistoryAnswer, this, &TelegramMessageListModel::clearHistoryAnswer);

    TelegramAbstractEngineListModel::connectTelegram();
}

void TelegramMessageListModel::getMessagesFromServer(int offsetId, int addOffset, int limit)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;
    if(p->loadSuspenderTimer)
        return;

    setRefreshing(true);

    const InputPeer &input = p->currentPeer->core();
    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    p->lastRequest = tg->messagesGetHistory(input, offsetId, 0, addOffset, limit, 0, 0, [this, dis, limit](TG_MESSAGES_GET_HISTORY_CALLBACK){
        if(!dis || p->lastRequest != msgId) return;
        setRefreshing(false);
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        if(result.messages().count() < limit)
            setHasBackMore(false);

        processOnResult(result, true);

        if(p->loadSuspenderTimer)
            killTimer(p->loadSuspenderTimer);
        p->loadSuspenderTimer = QObject::startTimer(200);
    });
}

void TelegramMessageListModel::getMessageListFromServer()
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    setRefreshing(true);

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    p->lastRequest = tg->messagesGetMessages(p->messageList, [this, dis](TG_MESSAGES_GET_MESSAGES_CALLBACK){
        if(!dis || p->lastRequest != msgId) return;
        setRefreshing(false);
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        setHasBackMore(false);
        processOnResult(result);
    });
}

void TelegramMessageListModel::fetchReplies(QList<Message> messages, int delay)
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn || !p->currentPeer)
        return;

    for(int i=0; i<messages.count(); i++)
    {
        const Message msg = messages.at(i);
        const QByteArray &key = TelegramTools::identifier(msg);
        if(!p->items.contains(key))
            continue;

        TelegramMessageListItem &item = p->items[key];
        Q_FOREACH(const TelegramMessageListItem &rpl, p->items)
        {
            if(!rpl.message || rpl.message->id() != msg.replyToMsgId())
                continue;
            if(rpl.fromUser)
            {
                item.replyMsg = rpl.message;
                item.replyUser = rpl.fromUser;
                messages.removeAt(i);
                i--;
                PROCESS_ROW_CHANGE(item.id, <<RoleReplyPeer<<RoleReplyMessage<<RoleReplyType )
                break;
            }
            else
            if(rpl.fromChat)
            {
                item.replyMsg = rpl.message;
                item.replyChat = rpl.fromChat;
                messages.removeAt(i);
                i--;
                PROCESS_ROW_CHANGE(item.id, <<RoleReplyPeer<<RoleReplyMessage<<RoleReplyType )
                break;
            }
        }
    }
    if(messages.isEmpty())
        return;

    if(delay)
    {
        Q_FOREACH(const Message &msg, messages)
            p->repliesToFetch[TelegramTools::identifier(msg)] = msg;

        if(p->repliesTimer)
            killTimer(p->repliesTimer);
        p->repliesTimer = QObject::startTimer(delay);
        return;
    }

    QHash<qint32, Message> repliesMap;
    Q_FOREACH(const Message &msg, messages)
        if(msg.replyToMsgId())
            repliesMap[msg.replyToMsgId()] = msg;

    DEFINE_DIS;
    Telegram::Callback<MessagesMessages > callBack = [this, dis, repliesMap](TG_MESSAGES_GET_MESSAGES_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        TelegramSharedDataManager *tsdm = mEngine->sharedData();
        if(!tsdm) return;

        QHash<qint32, User> users;
        Q_FOREACH(const User &user, result.users()) users[user.id()] = user;
        QHash<qint32, Chat> chats;
        Q_FOREACH(const Chat &chat, result.chats()) chats[chat.id()] = chat;

        Q_FOREACH(const Message &msg, result.messages())
        {
            if(!repliesMap.contains(msg.id()))
                continue;

            Message orgMsg = repliesMap.value(msg.id());
            QByteArray key = TelegramTools::identifier(orgMsg);
            if(!p->items.contains(key)) continue;

            TelegramMessageListItem &item = p->items[key];
            item.replyMsg = tsdm->insertMessage(msg);
            if(users.contains(msg.fromId()))
                item.replyUser = tsdm->insertUser(users.value(msg.fromId()));
            else
            if(chats.contains(msg.fromId()))
                item.replyChat = tsdm->insertChat(chats.value(msg.fromId()));

            PROCESS_ROW_CHANGE(key, <<RoleReplyPeer<<RoleReplyMessage<<RoleReplyType )
        }
    };

    Telegram *tg = mEngine->telegram();
    if(p->currentPeer->classType() == InputPeerObject::TypeInputPeerChannel)
    {
        InputChannel channel(InputChannel::typeInputChannel);
        channel.setChannelId(p->currentPeer->channelId());
        channel.setAccessHash(p->currentPeer->accessHash());

        tg->channelsGetMessages(channel, repliesMap.keys(), callBack);
    }
    else
    {
        tg->messagesGetMessages(repliesMap.keys(), callBack);
    }
}

void TelegramMessageListModel::processOnResult(const MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items)
{
    TelegramSharedDataManager *tsdm = mEngine->sharedData();

    QHash<qint32, QByteArray> messageFromPeers;
    QHash<QByteArray, QByteArray> messageToPeers;
    QHash<qint32, QByteArray> messageForwardsUsers;
    QHash<qint32, QByteArray> messageForwardsChats;

    Q_FOREACH(const Message &msg, result.messages())
    {
        QByteArray key;
        TelegramMessageListItem item;
        item.message = tsdm->insertMessage(msg, &key);
        item.id = key;

        if(item.message && item.message->media()->classType() != MessageMediaObject::TypeMessageMediaEmpty)
        {
            item.download = new TelegramDownloadHandler(item.message);

            connectDownloaderSignals(item.id, item.download);

            item.download->setEngine(mEngine);
            item.download->setSource(new MessageMediaObject(item.message->media()->core()));
        }

        (*items)[key] = item;

        messageFromPeers.insertMulti(msg.fromId(), key);
        messageToPeers.insertMulti( TelegramTools::identifier(msg.toId()), key );

        if(msg.fwdFrom().fromId())
            messageForwardsUsers.insertMulti(msg.fwdFrom().fromId(), key);
        if(msg.fwdFrom().channelId())
            messageForwardsChats.insertMulti(msg.fwdFrom().channelId(), key);

        connectMessageSignals(key, item.message);
    }

    Q_FOREACH(const Chat &chat, result.chats())
    {
        QByteArray chatKey = TelegramTools::identifier(chat);
        InputPeer peer = TelegramTools::chatInputPeer(chat);

        if(messageFromPeers.contains(chat.id()))
        {
            QList<QByteArray> keys = messageFromPeers.values(chat.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.fromChat = tsdm->insertChat(chat);
                connectChatSignals(key, item.fromChat);
            }
        }
        if(messageToPeers.contains(chatKey))
        {
            QList<QByteArray> keys = messageToPeers.values(chatKey);
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.toChat = tsdm->insertChat(chat);
                item.toPeerItem = tsdm->insertInputPeer(peer);
                connectChatSignals(key, item.toChat);
            }
        }
        if(messageForwardsChats.contains(chat.id()))
        {
            QList<QByteArray> keys = messageForwardsChats.values(chat.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.fwdChat = tsdm->insertChat(chat);
                connectChatSignals(key, item.fromChat);
            }
        }
    }

    Q_FOREACH(const User &user, result.users())
    {
        QByteArray userKey = TelegramTools::identifier(user);
        InputPeer peer = TelegramTools::userInputPeer(user);

        if(messageFromPeers.contains(user.id()))
        {
            QList<QByteArray> keys = messageFromPeers.values(user.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.fromUser = tsdm->insertUser(user);
                if(!item.message->out() && !item.toPeerItem)
                    item.toPeerItem = tsdm->insertInputPeer(peer);

                connectUserSignals(key, item.fromUser);
            }
        }
        if(messageToPeers.contains(userKey))
        {
            QList<QByteArray> keys = messageToPeers.values(userKey);
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.toUser = tsdm->insertUser(user);
                if(item.message->out() && !item.toPeerItem)
                    item.toPeerItem = tsdm->insertInputPeer(peer);

                connectUserSignals(key, item.toUser);
            }
        }
        if(messageForwardsUsers.contains(user.id()))
        {
            QList<QByteArray> keys = messageForwardsUsers.values(user.id());
            Q_FOREACH(const QByteArray &key, keys)
            {
                TelegramMessageListItem &item = (*items)[key];
                item.fwdUser = tsdm->insertUser(user);
                connectUserSignals(key, item.fromUser);
            }
        }
    }
}

void TelegramMessageListModel::changed(QHash<QByteArray, TelegramMessageListItem> items)
{
    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    QList<QByteArray> list = getSortedList(items);
    if(mEngine && mEngine->our() && p->currentPeer)
    {
        QList<TelegramUploadHandler*> handlerItems = TelegramUploadHandler::getItems(mEngine, p->currentPeer);
        Q_FOREACH(TelegramUploadHandler *handler, handlerItems)
        {
            MessageObject *handlerMsg = handler->result();
            if(!handlerMsg)
                continue;

            for(int i=0; i<list.count(); i++)
            {
                const QByteArray &l = list.at(i);
                MessageObject *msg = items.value(l).message;
                if(!msg)
                    continue;
                if(msg->date() > handlerMsg->date())
                    continue;

                QByteArray key;
                TelegramMessageListItem item;
                item.message = tsdm->insertMessage(handlerMsg->core(), &key);
                item.fromUser = tsdm->insertUser(mEngine->our()->user()->core());
                if(handler->replyTo())
                    item.replyMsg = tsdm->insertMessage(handler->replyTo()->core());
                item.id = key;
                item.upload = handler;
                items[key] = item;
                list.removeAll(key);
                list.insert(i, key);
                p->sendings.insert(item.id);
                connectUploaderSignals(item.id, item.upload);
                break;
            }
        }
    }

    p->items.unite(items);

    const bool resetState = (p->list.isEmpty() && !list.isEmpty());
    bool count_changed = (list.count()!=p->list.count());
    if(resetState)
        beginResetModel();

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const QByteArray &key = p->list.at(i);
        if( list.contains(key) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<QByteArray> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const QByteArray &item = temp_list.at(i);
        if( p->list.contains(item) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const QByteArray &item = p->list.at(i);
            int nw = temp_list.indexOf(item);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const QByteArray &item = list.at(i);
        if( p->list.contains(item) )
            continue;

        if(!resetState) beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, item );
        if(!resetState) endInsertRows();
    }

    if(resetState)
        endResetModel();

    p->items = items;
    if(count_changed)
        Q_EMIT countChanged();
}

const QHash<QByteArray, TelegramMessageListItem> *tg_mlist_model_lessthan_items = 0;
bool tg_mlist_model_sort(const QByteArray &s1, const QByteArray &s2);

QList<QByteArray> TelegramMessageListModel::getSortedList(const QHash<QByteArray, TelegramMessageListItem> &items)
{
    tg_mlist_model_lessthan_items = &items;
    QList<QByteArray> list = items.keys();
    qStableSort(list.begin(), list.end(), tg_mlist_model_sort);
    return list;
}

void TelegramMessageListModel::connectMessageSignals(const QByteArray &id, MessageObject *message)
{
    if(!message || p->connecteds.contains(message)) return;
    connect(message, &MessageObject::viewsChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleUnread);
    });
    p->connecteds.insert(message);
    connect(message, &MessageObject::destroyed, this, [this, message](){ if(p) p->connecteds.remove(message); });
}

void TelegramMessageListModel::connectChatSignals(const QByteArray &id, ChatObject *chat)
{
    Q_UNUSED(id)
    if(!chat || p->connecteds.contains(chat)) return;

    p->connecteds.insert(chat);
    connect(chat, &ChatObject::destroyed, this, [this, chat](){ if(p) p->connecteds.remove(chat); });
}

void TelegramMessageListModel::connectUserSignals(const QByteArray &id, UserObject *user)
{
    Q_UNUSED(id)
    if(!user || p->connecteds.contains(user)) return;

    p->connecteds.insert(user);
    connect(user, &UserObject::destroyed, this, [this, user](){ if(p) p->connecteds.remove(user); });
}

void TelegramMessageListModel::connectDialogSignals(const QByteArray &id, DialogObject *dialog)
{
    Q_UNUSED(id)
    if(!dialog || p->connecteds.contains(dialog)) return;

    connect(dialog, &DialogObject::readOutboxMaxIdChanged, this, [this, dialog](){
        if(dialog != p->currentDialog) return;
        Q_FOREACH(const QByteArray &id, p->list)
        {
            PROCESS_ROW_CHANGE(id, << RoleUnread);
        }
    });

    p->connecteds.insert(dialog);
    connect(dialog, &DialogObject::destroyed, this, [this, dialog](){ if(p) p->connecteds.remove(dialog); });
}

void TelegramMessageListModel::connectUploaderSignals(const QByteArray &id, TelegramUploadHandler *handler)
{
    if(!handler || p->connecteds.contains(handler)) return;

    connect(handler, &TelegramUploadHandler::transfaredSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfaredSize);
    });
    connect(handler, &TelegramUploadHandler::totalSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTotalSize);
    });
    connect(handler, &TelegramUploadHandler::statusChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfared << RoleTransfaring << RoleFilePath << RoleThumbPath << RoleUploading);
    });

    p->connecteds.insert(handler);
    connect(handler, &TelegramUploadHandler::destroyed, this, [this, handler](){ if(p) p->connecteds.remove(handler); });
}

void TelegramMessageListModel::connectDownloaderSignals(const QByteArray &id, TelegramDownloadHandler *downloader)
{
    if(!downloader || p->connecteds.contains(downloader)) return;

    connect(downloader, &TelegramDownloadHandler::downloadedSizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfaredSize);
    });
    connect(downloader, &TelegramDownloadHandler::sizeChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTotalSize);
    });
    connect(downloader, &TelegramDownloadHandler::downloadingChanged, this, [this, id](){
        PROCESS_ROW_CHANGE(id, << RoleTransfared << RoleTransfaring << RoleFilePath << RoleThumbPath << RoleDownloading);
    });

    p->connecteds.insert(downloader);
    connect(downloader, &TelegramDownloadHandler::destroyed, this, [this, downloader](){ if(p) p->connecteds.remove(downloader); });
}

void TelegramMessageListModel::onUpdates(const UpdatesType &updates)
{
    TelegramTools::analizeUpdatesType(updates, mEngine, [this](const Update &update){
        insertUpdate(update);
    });
}

void TelegramMessageListModel::insertUpdate(const Update &update)
{
    if(!mEngine || !p->currentPeer)
        return;

    Telegram *tg = mEngine->telegram();
    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    if(!tg || !tsdm)
        return;

    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
    {
        const Message &msg = update.message();
        const Peer &peer = TelegramTools::messagePeer(msg);
        const QByteArray &id = TelegramTools::identifier(peer);
        if(id != key())
            return;

        Peer fromPeer;
        if(msg.fromId())
        {
            fromPeer.setUserId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerUser);
        }

        QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);

        TelegramMessageListItem item;
        item.fromUser = tsdm->getUser(fromPeerKey);
        item.fromChat = tsdm->getChat(fromPeerKey);
        item.message = tsdm->insertMessage(msg, &item.id);
        item.fwdUser = tsdm->getUser(TelegramTools::identifier(Peer::typePeerUser, msg.fwdFrom().fromId()));
        item.fwdChat = tsdm->getChat(TelegramTools::identifier(Peer::typePeerChannel, msg.fwdFrom().channelId()));

        TelegramCache *cache = mEngine->cache();
        if(cache && p->useCache)
        {
            if(item.message) cache->insert(item.message->core());
            if(item.fwdUser) cache->insert(item.fwdUser->core());
            if(item.fwdChat) cache->insert(item.fwdChat->core());
            if(item.fromUser) cache->insert(item.fromUser->core());
            if(item.fromChat) cache->insert(item.fromChat->core());
        }

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        items[item.id] = item;

        connectMessageSignals(item.id, item.message);
        changed(items);
        fetchReplies(QList<Message>()<<msg);
    }
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteChannelMessages:
    {
        if(p->currentPeer->channelId() != update.channelId())
            break;
    }
    case Update::typeUpdateDeleteMessages:
    {
        const QList<qint32> &messages = update.messages();

        QHash<QByteArray, TelegramMessageListItem> items = p->items;
        Q_FOREACH(TelegramMessageListItem item, items)
            if(item.message && messages.contains(item.message->id()))
                items.remove(item.id);

        changed(items);
    }
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
    {
        const qint32 userId = update.userId();
        const qint32 chatId = update.chatId();

        UserObject *user = 0;
        ChatObject *chat = 0;
        QByteArray id;
        Q_FOREACH(TelegramMessageListItem item, p->items)
        {
            if(item.fromUser && item.fromUser->id() == userId)
            {
                user = item.fromUser;
                if(type == Update::typeUpdateUserTyping)
                    id = item.id;
            }
            else
            if(item.fromChat && item.fromChat->id() == chatId)
            {
                chat = item.fromChat;
                if(type == Update::typeUpdateChatUserTyping)
                    id = item.id;
            }
        }

        if(user)
        {
            p->typingChats[chat][user]++;
            Q_EMIT typingUsersChanged();
            startTimer(5000, [this, chat, user, id](){
                int &count = p->typingChats[chat][user];
                count--;
                if(count == 0) {
                    p->typingChats[chat].remove(user);
                    if(p->typingChats.value(chat).isEmpty())
                        p->typingChats.remove(chat);
                }
                Q_EMIT typingUsersChanged();
            });
        }
    }
        break;
    case Update::typeUpdateChatParticipants:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.fromChat && item.fromChat->id() == update.participants().chatId())
                item.fromChat->setParticipantsCount(update.participants().participants().count());
    }
        break;
    case Update::typeUpdateUserStatus:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.fromUser && item.fromUser->id() == update.userId())
                item.fromUser->status()->operator =(update.status());
    }
        break;
    case Update::typeUpdateUserName:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.fromUser && item.fromUser->id() == update.userId())
            {
                UserObject *user = item.fromUser;
                user->setFirstName(update.firstName());
                user->setLastName(update.lastName());
                user->setUsername(update.username());
            }
    }
        break;
    case Update::typeUpdateUserPhoto:
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.fromUser && item.fromUser->id() == update.userId())
            {
                UserObject *user = item.fromUser;
                user->photo()->operator =(update.photo());
            }
    }
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
    {
        Q_FOREACH(TelegramMessageListItem item, p->items)
            if(item.fromUser && item.fromUser->id() == update.userId())
            {
                UserObject *user = item.fromUser;
                user->setPhone(update.phone());
            }
    }
        break;
    case Update::typeUpdateReadHistoryInbox:
    {
        const QByteArray &id = TelegramTools::identifier(update.peer());
        if(key() == id)
        {
            Q_FOREACH(const QByteArray &key, p->list)
            {
                TelegramMessageListItem item = p->items.value(key);
                if(!item.message)
                    continue;

                item.message->setViews(1);
            }
        }
    }
        break;
    case Update::typeUpdateReadHistoryOutbox:
    {
        const QByteArray &id = TelegramTools::identifier(update.peer());
        if(key() == id)
        {
            Q_FOREACH(const QByteArray &key, p->list)
            {
                TelegramMessageListItem item = p->items.value(key);
                if(!item.message)
                    continue;

                PROCESS_ROW_CHANGE(item.id, << RoleUnread);
            }
        }
    }
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
    {
        Peer peer(Peer::typePeerChannel);
        peer.setChannelId(update.channelId());
        const QByteArray &id = TelegramTools::identifier(peer);
        if(key() == id)
        {
            Q_FOREACH(const QByteArray &key, p->list)
            {
                TelegramMessageListItem item = p->items.value(key);
                if(!item.message)
                    continue;

                PROCESS_ROW_CHANGE(item.id, << RoleUnread);
            }
        }
    }
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
    case Update::typeUpdateEditMessage:
    case Update::typeUpdateEditChannelMessage:
    {
        const Message &msg = update.message();
        switch(static_cast<int>(msg.toId().classType()))
        {
        case Peer::typePeerChannel:
            if(msg.toId().channelId() == p->currentPeer->channelId())
                tsdm->insertMessage(msg);
            break;
        case Peer::typePeerChat:
            if(msg.toId().chatId() == p->currentPeer->chatId())
                tsdm->insertMessage(msg);
            break;
        case Peer::typePeerUser:
            if(msg.toId().userId() == p->currentPeer->userId())
                tsdm->insertMessage(msg);
            break;
        }
    }
        break;
    case Update::typeUpdateChannelPinnedMessage:
        break;
    case Update::typeUpdateBotCallbackQuery:
        break;
    case Update::typeUpdateInlineBotCallbackQuery:
        break;
    }
}

TelegramMessageListModel::~TelegramMessageListModel()
{
    TelegramMessageListModelPrivate *tmp = p;
    p = 0;
    delete tmp;
}


bool tg_mlist_model_sort(const QByteArray &s1, const QByteArray &s2)
{
    const TelegramMessageListItem &i1 = tg_mlist_model_lessthan_items->value(s1);
    const TelegramMessageListItem &i2 = tg_mlist_model_lessthan_items->value(s2);
    if(i1.message->date() != i1.message->date())
        return i1.message->date() > i2.message->date();
    else
        return i1.message->id() > i2.message->id();
}
