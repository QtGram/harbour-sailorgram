#define DEFINE_DIS \
    QPointer<TelegramStickersCategoriesModel> dis = this;

#define PROCESS_ROW_CHANGE(KEY, ROLES) \
    int row = p->list.indexOf(KEY); \
    if(row >= 0) \
        Q_EMIT dataChanged(index(row), index(row), QVector<int>()ROLES);

#include "telegramstickerscategoriesmodel.h"
#include "telegramsharedpointer.h"
#include "telegramshareddatamanager.h"

#include <telegram/objects/typeobjects.h>
#include <telegram.h>

#include <QDebug>

class TelegramStickersCategoriesItem
{
public:
    QByteArray id;
    TelegramSharedPointer<StickerSetObject> stickerSet;
    TelegramSharedPointer<InputStickerSetObject> inputItem;
    QList< TelegramSharedPointer<DocumentObject> > documents;
};

class TelegramStickersCategoriesModelPrivate
{
public:
    bool refreshing;
    qint64 lastRequest;

    QHash<QByteArray, TelegramStickersCategoriesItem> items;
    QList<QByteArray> list;
};

TelegramStickersCategoriesModel::TelegramStickersCategoriesModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramStickersCategoriesModelPrivate;
    p->refreshing = false;
    p->lastRequest = 0;
}

bool TelegramStickersCategoriesModel::refreshing() const
{
    return p->refreshing;
}

void TelegramStickersCategoriesModel::setRefreshing(bool refreshing)
{
    if(p->refreshing == refreshing)
        return;

    p->refreshing = refreshing;
    Q_EMIT refreshingChanged();
}

QByteArray TelegramStickersCategoriesModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramStickersCategoriesModel::count() const
{
    return p->list.count();
}

QVariant TelegramStickersCategoriesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    QByteArray key = id(index);
    if(!p->items.contains(key))
        return result;

    TelegramStickersCategoriesItem item = p->items.value(key);
    switch(role)
    {
    case RoleStickerSetItem:
        result = QVariant::fromValue<StickerSetObject*>(item.stickerSet);
        break;
    case RoleInstalled:
        result = item.stickerSet->installed();
        break;
    case RoleDisabled:
        result = item.stickerSet->disabled();
        break;
    case RoleOfficial:
        result = item.stickerSet->official();
        break;
    case RoleTitle:
        result = item.stickerSet->title();
        break;
    case RoleShortName:
        result = item.stickerSet->shortName();
        break;
    case RoleCount:
        result = item.stickerSet->count();
        break;
    case RoleHash:
        result = item.stickerSet->hash();
        break;
    case RoleInputItem:
        result = QVariant::fromValue<InputStickerSetObject*>(item.inputItem);
        break;
    case RoleDocumentItems:
    {
        QVariantList list;
        Q_FOREACH(TelegramSharedPointer<DocumentObject> doc, item.documents)
            list << QVariant::fromValue<DocumentObject*>(doc.data());
        result = list;
    }
        break;
    }

    return result;
}

QHash<int, QByteArray> TelegramStickersCategoriesModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(RoleStickerSetItem, "message");
    result->insert(RoleInstalled, "installed");
    result->insert(RoleDisabled, "disabled");
    result->insert(RoleOfficial, "official");
    result->insert(RoleTitle, "title");
    result->insert(RoleShortName, "shortName");
    result->insert(RoleCount, "count");
    result->insert(RoleHash, "hash");
    result->insert(RoleInputItem, "inputItem");
    result->insert(RoleDocumentItems, "documentItems");

    return *result;
}

QStringList TelegramStickersCategoriesModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine);
}

void TelegramStickersCategoriesModel::refresh()
{
    clean();
    if(!mEngine || !mEngine->telegram())
        return;

    getStickersFromServer();
}

void TelegramStickersCategoriesModel::clean()
{
    beginResetModel();
    p->items.clear();
    p->list.clear();
    Q_EMIT countChanged();
    endResetModel();
}

void TelegramStickersCategoriesModel::getStickersFromServer()
{
    if(mEngine->state() != TelegramEngine::AuthLoggedIn)
        return;

    setRefreshing(true);

    Telegram *tg = mEngine->telegram();
    DEFINE_DIS;
    p->lastRequest = tg->messagesGetAllStickers(0, [this, dis, tg](TG_MESSAGES_GET_ALL_STICKERS_CALLBACK){
        if(!dis || !mEngine || p->lastRequest != msgId) return;
        p->lastRequest = 0;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        TelegramSharedDataManager *tsdm = mEngine->sharedData();

        QList<QByteArray> order;
        QHash<QByteArray, TelegramStickersCategoriesItem> items;
        Q_FOREACH(const StickerSet &set, result.sets())
        {
            InputStickerSetObject *input = new InputStickerSetObject();
            input->setClassType(InputStickerSetObject::TypeInputStickerSetID);
            input->setId(set.id());
            input->setAccessHash(set.accessHash());

            QByteArray key;
            TelegramStickersCategoriesItem item;
            item.stickerSet = tsdm->insertStickerSet(set, &key);
            item.id = key;
            item.inputItem = input;

            items[item.id] = item;
            order << item.id;

            tg->messagesGetStickerSet(input->core(), [this, dis, key](TG_MESSAGES_GET_STICKER_SET_CALLBACK){
                Q_UNUSED(msgId)
                if(!dis || !mEngine) return;
                if(!error.null) {
                    setError(error.errorText, error.errorCode);
                    return;
                }
                if(!p->items.contains(key))
                    return;

                TelegramSharedDataManager *tsdm = mEngine->sharedData();
                TelegramStickersCategoriesItem &item = p->items[key];

                item.documents.clear();
                Q_FOREACH(const Document &doc, result.documents())
                    item.documents << tsdm->insertDocument(doc);

                PROCESS_ROW_CHANGE(key, <<RoleDocumentItems)
            });
        }

        changed(order, items);
        setRefreshing(false);
    });
}

void TelegramStickersCategoriesModel::changed(const QList<QByteArray> &list, const QHash<QByteArray, TelegramStickersCategoriesItem> &items)
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

TelegramStickersCategoriesModel::~TelegramStickersCategoriesModel()
{
    delete p;
}

