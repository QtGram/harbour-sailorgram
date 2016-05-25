#define DEFINE_DIS \
    QPointer<TelegramStickersModel> dis = this;

#define GET_INPUT \
    TelegramSharedPointer<InputStickerSetObject>(p->inputSet?p->inputSet:p->defaultInputSet)

#define RECENTS_LIMIT 20

#include "telegramstickersmodel.h"
#include "telegramengine.h"
#include "telegramsharedpointer.h"
#include "telegramshareddatamanager.h"
#include "telegramcache.h"

#include <telegram.h>
#include <telegram/objects/typeobjects.h>

#include <QPointer>
#include <QDebug>

class TelegramStickersItem
{
public:
    QByteArray key;
    TelegramSharedPointer<DocumentObject> document;
};

class TelegramStickersModelPrivate
{
public:
    bool refreshing;

    TelegramSharedPointer<InputStickerSetObject> inputSet;
    TelegramSharedPointer<InputStickerSetObject> defaultInputSet;

    QList<QByteArray> list;
    QHash<QByteArray, TelegramStickersItem> items;
};

TelegramStickersModel::TelegramStickersModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramStickersModelPrivate;
    p->refreshing = false;
    p->inputSet = 0;

    p->defaultInputSet = new InputStickerSetObject();
    p->defaultInputSet->setClassType(InputStickerSetObject::TypeInputStickerSetShortName);
}

QString TelegramStickersModel::shortName() const
{
    return p->defaultInputSet->shortName();
}

void TelegramStickersModel::setShortName(const QString &shortName)
{
    if(p->defaultInputSet->shortName() == shortName)
        return;

    p->defaultInputSet->setShortName(shortName);
    Q_EMIT shortNameChanged();
}

void TelegramStickersModel::setStickerSet(InputStickerSetObject *stickerSet)
{
    if(p->inputSet == stickerSet)
        return;

    p->inputSet = stickerSet;
    Q_EMIT stickerSetChanged();
}

InputStickerSetObject *TelegramStickersModel::stickerSet() const
{
    return p->inputSet;
}

void TelegramStickersModel::setDocuments(const QVariantList &list)
{
    if(documents() == list)
        return;

    QList<QByteArray> newList;
    QHash<QByteArray, TelegramStickersItem> items;
    Q_FOREACH(const QVariant &var, list)
    {
        TelegramStickersItem item;
        item.document = var.value<DocumentObject*>();
        item.key = TelegramTools::identifier(item.document->core());

        items[item.key] = item;
        newList << item.key;
    }

    changed(newList, items);
    Q_EMIT documentsChanged();
}

QVariantList TelegramStickersModel::documents() const
{
    QVariantList result;
    Q_FOREACH(const QByteArray &key, p->list )
    {
        TelegramStickersItem item = p->items.value(key);
        result << QVariant::fromValue<DocumentObject*>(item.document.data());
    }
    return result;
}

void TelegramStickersModel::refresh()
{
    clean();
    if(!mEngine || !mEngine->telegram())
        return;

    getFromServer();
}

void TelegramStickersModel::clean()
{
    beginResetModel();
    p->list.clear();
    p->items.clear();
    endResetModel();
}

void TelegramStickersModel::getFromServer()
{
    if( !mEngine || mEngine->state() != TelegramEngine::AuthLoggedIn )
        return;

    Telegram *tg = mEngine->telegram();
    if(!tg)
        return;

    DEFINE_DIS;
    tg->messagesGetStickerSet(GET_INPUT->core(), [this, dis](TG_MESSAGES_GET_STICKER_SET_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        QVariantList docs;
        Q_FOREACH(const Document &doc, result.documents())
            /*! This pointer will handled in the TelegramSharedPointer in the documents function !*/
            docs << QVariant::fromValue<DocumentObject*>(new DocumentObject(doc));

        setDocuments(docs);
    });
}

void TelegramStickersModel::changed(const QList<QByteArray> &list, const QHash<QByteArray, TelegramStickersItem> &items)
{
    p->items.unite(items);

    const bool resetState = (p->list.isEmpty() && !list.isEmpty());
    bool count_changed = (list.count()!=p->list.count());
    if(resetState)
        beginResetModel();

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const QByteArray &item = p->list.at(i);
        if( list.contains(item) )
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

QByteArray TelegramStickersModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramStickersModel::count() const
{
    return p->list.count();
}

QVariant TelegramStickersModel::data(const QModelIndex &index, int role) const
{
    QByteArray key = id(index);
    TelegramStickersItem item = p->items.value(key);
    QVariant result;
    switch(role)
    {
    case RoleDocument:
        result = QVariant::fromValue<DocumentObject*>(item.document.data());
        break;

    case RoleAlt:
    {
        Q_FOREACH(const DocumentAttribute &attr, item.document->attributes())
            if(attr.classType() == DocumentAttribute::typeDocumentAttributeSticker)
            {
                result = attr.alt();
                break;
            }
    }
        break;
    }

    return result;
}

QHash<qint32, QByteArray> TelegramStickersModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( RoleDocument, "document");
    res->insert( RoleAlt, "alt");
    return *res;
}

bool TelegramStickersModel::refreshing() const
{
    return p->refreshing;
}

QStringList TelegramStickersModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine) << FUNCTION_NAME(documents);
}

void TelegramStickersModel::showRecents()
{
    if(!mEngine)
        return;

    TelegramCache *cache = mEngine->cache();
    TelegramSharedDataManager *tsdm = mEngine->sharedData();
    if(!cache)
        return;

    const QList<Document> &docs = cache->readRecentStickers();

    QList< TelegramSharedPointer<DocumentObject> > *docObjs = new QList< TelegramSharedPointer<DocumentObject> >();
    QVariantList list;
    Q_FOREACH(const Document &doc, docs)
    {
        TelegramSharedPointer<DocumentObject> obj = tsdm->insertDocument(doc);
        (*docObjs) << obj;
        list << QVariant::fromValue<DocumentObject*>(obj.data());
    }

    setDocuments(list);
    delete docObjs;
}

void TelegramStickersModel::addToRecents(DocumentObject *doc)
{
    if(!mEngine || !doc)
        return;

    TelegramCache *cache = mEngine->cache();
    if(!cache)
        return;

    QList<Document> docs = cache->readRecentStickers();
    docs = docs.mid(0, RECENTS_LIMIT);
    docs.prepend(doc->core());

    cache->insert(docs);
}

TelegramStickersModel::~TelegramStickersModel()
{
    delete p;
}

