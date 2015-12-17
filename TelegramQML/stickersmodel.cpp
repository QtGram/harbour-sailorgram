#include "stickersmodel.h"
#include "telegramqml.h"
#include "objects/types.h"

#include <telegram.h>

#include <QPointer>
#include <QDebug>

class StickersModelPrivate
{
public:
    QPointer<TelegramQml> telegram;

    bool initializing;
    QString currentSet;

    QList<qint64> list;
    QStringList installedStickerSets;
    QStringList stickerSets;
};

StickersModel::StickersModel(QObject *parent) :
    TgAbstractListModel(parent)
{
    p = new StickersModelPrivate;
    p->initializing = false;
}

TelegramQml *StickersModel::telegram() const
{
    return p->telegram;
}

void StickersModel::setTelegram(TelegramQml *tgo)
{
    if(p->telegram == tgo)
        return;
    if(p->telegram)
    {
        disconnect(p->telegram, SIGNAL(stickerRecieved(qint64)), this, SLOT(listChanged()));
        disconnect(p->telegram, SIGNAL(installedStickersChanged()), this, SLOT(listChanged()));
        disconnect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(recheck()));
    }

    p->telegram = tgo;
    if(p->telegram)
    {
        connect(p->telegram, SIGNAL(stickerRecieved(qint64)), this, SLOT(listChanged()));
        connect(p->telegram, SIGNAL(installedStickersChanged()), this, SLOT(listChanged()));
        connect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(recheck()), Qt::QueuedConnection);
    }

    refresh();
    Q_EMIT telegramChanged();
}

void StickersModel::refresh()
{
    if(!p->telegram)
        return;

    recheck();

    p->initializing = true;
    Q_EMIT initializingChanged();

    listChanged(true);
}

void StickersModel::recheck()
{
    if( !p->telegram || !p->telegram->authLoggedIn() )
        return;

    Telegram *tg = p->telegram->telegram();
    if(!tg)
        return;

    tg->messagesGetAllStickers(QString());
    if(!p->currentSet.isEmpty() && !p->currentSet.toLongLong() && p->currentSet != "0")
        p->telegram->getStickerSet(p->currentSet);
}

QString StickersModel::currentStickerSet() const
{
    return p->currentSet;
}

void StickersModel::setCurrentStickerSet(const QString &cat)
{
    if(p->currentSet == cat)
        return;

    p->currentSet = cat;
    refresh();
    Q_EMIT currentStickerSetChanged();
}

QStringList StickersModel::installedStickerSets() const
{
    return p->installedStickerSets;
}

QStringList StickersModel::stickerSets() const
{
    return p->stickerSets;
}

DocumentObject *StickersModel::stickerSetThumbnailDocument(const QString &id) const
{
    if(!p->telegram)
        return 0;

    QList<qint64> list = p->telegram->stickerSetDocuments(id.toLongLong());
    qSort(list.begin(), list.end());
    if(list.isEmpty())
        return p->telegram->nullSticker();
    else
        return p->telegram->sticker(list.first());
}

StickerSetObject *StickersModel::stickerSetItem(const QString &id) const
{
    if(!p->telegram)
        return 0;

    return p->telegram->stickerSet(id.toLongLong());
}

qint64 StickersModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int StickersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant StickersModel::data(const QModelIndex &index, int role) const
{
    const qint64 sid = id(index);
    QVariant result;
    switch(role)
    {
    case stickerIdRole:
        result = sid;
        break;

    case documentRole:
        if(p->telegram)
            result = QVariant::fromValue<DocumentObject*>(p->telegram->sticker(sid));
        break;

    case emoticonRole:
        break;

    case stickerSetNameRole:
        if(p->telegram)
            result = stickerOfDocument(p->telegram->sticker(sid)).shortName();
        break;

    case stickerSetIdRole:
        if(p->telegram)
            result = stickerOfDocument(p->telegram->sticker(sid)).id();
        break;

    case stickerSetAccessHashRole:
        if(p->telegram)
            result = stickerOfDocument(p->telegram->sticker(sid)).accessHash();
        break;

    }

    return result;
}

QHash<qint32, QByteArray> StickersModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( stickerIdRole, "stickerId");
    res->insert( documentRole, "document");
    res->insert( emoticonRole, "emoticon");
    res->insert( stickerSetNameRole, "stickerSetName");
    res->insert( stickerSetIdRole, "stickerSetId");
    res->insert( stickerSetAccessHashRole, "stickerSetAccessHash");
    return *res;
}

int StickersModel::count() const
{
    return p->list.count();
}

bool StickersModel::initializing() const
{
    return p->initializing;
}

void StickersModel::listChanged(bool cached)
{
    if(!p->telegram)
        return;

    p->installedStickerSets.clear();
    p->stickerSets.clear();

    const QList<qint64> &installedSets = p->telegram->installedStickerSets();
    Q_FOREACH(const qint64 cat, installedSets)
        p->installedStickerSets << QString::number(cat);
    qSort(p->installedStickerSets.begin(), p->installedStickerSets.end());

    const QList<qint64> &sets = p->telegram->stickerSets();
    Q_FOREACH(const qint64 cat, sets)
        p->stickerSets << QString::number(cat);
    qSort(p->stickerSets.begin(), p->stickerSets.end());

    Q_EMIT stickerSetsChanged();
    Q_EMIT installedStickerSetsChanged();

    const QList<qint64> &list = getList(p->currentSet);

    const int firstCount = p->list.count();
    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const qint64 dId = p->list.at(i);
        if( list.contains(dId) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }


    QList<qint64> temp_msgs = list;
    for( int i=0 ; i<temp_msgs.count() ; i++ )
    {
        const qint64 dId = temp_msgs.at(i);
        if( p->list.contains(dId) )
            continue;

        temp_msgs.removeAt(i);
        i--;
    }
    while( p->list != temp_msgs )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const qint64 dId = p->list.at(i);
            int nw = temp_msgs.indexOf(dId);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }


    for( int i=0 ; i<list.count() ; i++ )
    {
        const qint64 dId = list.at(i);
        if( p->list.contains(dId) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, dId );
        endInsertRows();
    }

    if(firstCount != p->list.count())
        Q_EMIT countChanged();
    if(!cached)
    {
        p->initializing = false;
        Q_EMIT initializingChanged();
    }
}

InputStickerSet StickersModel::stickerOfDocument(DocumentObject *obj) const
{
    const QList<DocumentAttribute> &attrs = obj->attributes();
    Q_FOREACH(const DocumentAttribute &attr, attrs)
        if(attr.classType() == DocumentAttribute::typeDocumentAttributeSticker)
            return attr.stickerset();

    return InputStickerSet();
}

QList<qint64> StickersModel::getList(const QString &id)
{
    QList<qint64> list;
    if(id.toLongLong() || id == "0")
        list = p->telegram->stickerSetDocuments(id.toLongLong());
    else
    if(!id.isEmpty())
        list = p->telegram->stickerSetDocuments(id);

    qSort(list.begin(), list.end());
    return list;
}

StickersModel::~StickersModel()
{
    delete p;
}

