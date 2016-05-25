#define DEFINE_DIS \
    QPointer<TelegramMembersListModel> dis = this;

#include "telegrammemberslistmodel.h"
#include "telegramsharedpointer.h"
#include "telegramtools.h"
#include "telegramshareddatamanager.h"
#include "telegramengine.h"

#include <telegram/objects/typeobjects.h>
#include <telegram.h>

#include <QPointer>
#include <QDateTime>
#include <QJSValue>
#include <QQmlEngine>
#include <QtQml>

class TelegramChatsMemebrsListModelItem
{
public:
    TelegramChatsMemebrsListModelItem() :
        date(0),
        inviterId(0),
        userId(0),
        kickedBy(0),
        chatClassType(0),
        channelClassType(0)
    {}
    ~TelegramChatsMemebrsListModelItem(){}

    TelegramChatsMemebrsListModelItem(const ChatParticipant &b) :
        TelegramChatsMemebrsListModelItem() {
        date = b.date();
        inviterId = b.inviterId();
        userId = b.userId();
        kickedBy = 0;
        chatClassType = static_cast<qint64>(b.classType());
        channelClassType = 0;
    }

    TelegramChatsMemebrsListModelItem(const ChannelParticipant &b) :
        TelegramChatsMemebrsListModelItem() {
        date = b.date();
        inviterId = b.inviterId();
        userId = b.userId();
        kickedBy = b.kickedBy();
        chatClassType = 0;
        channelClassType = static_cast<qint64>(b.classType());
    }

    bool operator==(const TelegramChatsMemebrsListModelItem &b) {
        return date == b.date &&
               inviterId == b.inviterId &&
               userId == b.userId &&
               kickedBy == b.kickedBy &&
               chatClassType == b.chatClassType &&
               channelClassType == b.channelClassType;
    }

    qint32 date;
    qint32 inviterId;
    qint32 userId;
    qint32 kickedBy;
    qint64 chatClassType;
    qint64 channelClassType;
    TelegramSharedPointer<UserObject> user;
    TelegramSharedPointer<InputPeerObject> peer;
    TelegramSharedPointer<UserObject> inviter;
    TelegramSharedPointer<UserObject> kicker;
};

class TelegramMembersListModelPrivate
{
public:
    TelegramSharedPointer<InputPeerObject> currentPeer;
    bool refreshing;
    qint32 filter;

    qint64 lastRequest;
    QList<TelegramChatsMemebrsListModelItem> list;
    QJSValue dateConvertorMethod;
};

TelegramMembersListModel::TelegramMembersListModel(QObject *parent) :
    TelegramAbstractEngineListModel(parent)
{
    p = new TelegramMembersListModelPrivate;
    p->refreshing = false;
    p->filter = ChannelParticipantsFilterObject::TypeChannelParticipantsRecent;
    p->lastRequest = 0;
}

void TelegramMembersListModel::setCurrentPeer(InputPeerObject *peer)
{
    if(p->currentPeer == peer)
        return;

    p->currentPeer = peer;
    refresh();
    Q_EMIT currentPeerChanged();
}

InputPeerObject *TelegramMembersListModel::currentPeer() const
{
    return p->currentPeer;
}

void TelegramMembersListModel::setFilter(qint32 filter)
{
    if(p->filter == filter)
        return;

    p->filter = filter;
    refresh();
    Q_EMIT filterChanged();
}

qint32 TelegramMembersListModel::filter() const
{
    return p->filter;
}

void TelegramMembersListModel::setRefreshing(bool refreshing)
{
    if(p->refreshing == refreshing)
        return;

    p->refreshing = refreshing;
    Q_EMIT refreshingChanged();
}

bool TelegramMembersListModel::refreshing() const
{
    return p->refreshing;
}

QJSValue TelegramMembersListModel::dateConvertorMethod() const
{
    return p->dateConvertorMethod;
}

void TelegramMembersListModel::setDateConvertorMethod(const QJSValue &method)
{
    if(p->dateConvertorMethod.isNull() && method.isNull())
        return;

    p->dateConvertorMethod = method;
    Q_EMIT dateConvertorMethodChanged();
}

TelegramChatsMemebrsListModelItem TelegramMembersListModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramMembersListModel::count() const
{
    return p->list.count();
}

QVariant TelegramMembersListModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const TelegramChatsMemebrsListModelItem item = id(index);
    switch(role)
    {
    case RoleDate:
        result = QDateTime::fromTime_t(item.date);
        break;
    case RoleInviter:
        result = QVariant::fromValue<UserObject*>(item.inviter.data());
        break;
    case RoleUser:
        result = QVariant::fromValue<UserObject*>(item.user.data());
        break;
    case RolePeer:
        result = QVariant::fromValue<InputPeerObject*>(item.peer.data());
        break;
    case RoleKickedBy:
        result = QVariant::fromValue<UserObject*>(item.kicker.data());
        break;
    case RoleStatus:
        result = TelegramTools::userStatus(item.user, [this](const QDateTime &dt){
            return convertDate(dt);
        });
        break;
    case RoleType:
        result = static_cast<int>(TypeUnknown);
        if(item.channelClassType)
        {
            switch(item.channelClassType)
            {
            case ChannelParticipant::typeChannelParticipant:
                result = static_cast<int>(TypeNormal);
                break;
            case ChannelParticipant::typeChannelParticipantCreator:
                result = static_cast<int>(TypeCreator);
                break;
            case ChannelParticipant::typeChannelParticipantEditor:
                result = static_cast<int>(TypeEditor);
                break;
            case ChannelParticipant::typeChannelParticipantKicked:
                result = static_cast<int>(TypeKicked);
                break;
            case ChannelParticipant::typeChannelParticipantModerator:
                result = static_cast<int>(TypeModerator);
                break;
            case ChannelParticipant::typeChannelParticipantSelf:
                result = static_cast<int>(TypeSelf);
                break;
            }
        }
        else
        if(item.chatClassType)
        {
            switch(item.chatClassType)
            {
            case ChatParticipant::typeChatParticipant:
                result = static_cast<int>(TypeNormal);
                break;
            case ChatParticipant::typeChatParticipantAdmin:
                result = static_cast<int>(TypeAdmin);
                break;
            case ChatParticipant::typeChatParticipantCreator:
                result = static_cast<int>(TypeCreator);
                break;
            }
        }
        break;
    }

    return result;
}

QHash<int, QByteArray> TelegramMembersListModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(RoleDate, "date");
    result->insert(RoleInviter, "inviter");
    result->insert(RoleUser, "user");
    result->insert(RoleKickedBy, "kickedBy");
    result->insert(RoleType, "type");
    result->insert(RolePeer, "peer");
    result->insert(RoleStatus, "status");

    return *result;
}

QStringList TelegramMembersListModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine)
                         << FUNCTION_NAME(currentPeer);
}

void TelegramMembersListModel::refresh()
{
    clean();
    if(!p->currentPeer || !mEngine || !mEngine->telegram())
        return;

    DEFINE_DIS;
    Telegram *tg = mEngine->telegram();

    switch(p->currentPeer->classType())
    {
    case InputPeerObject::TypeInputPeerChat:
        setRefreshing(true);

        setRefreshing(true);
        p->lastRequest = tg->messagesGetFullChat(p->currentPeer->chatId(), [this, dis](TG_MESSAGES_GET_FULL_CHAT_CALLBACK){
            if(!dis || !mEngine) return;
            if(p->lastRequest != msgId) return;
            setRefreshing(false);
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            TelegramSharedDataManager *tsdm = mEngine->sharedData();

            QHash<qint32, User> users;
            Q_FOREACH(const User &user, result.users())
                users[user.id()] = user;

            QList<TelegramChatsMemebrsListModelItem> items;
            Q_FOREACH(const ChatParticipant &cp, result.fullChat().participants().participants())
            {
                TelegramChatsMemebrsListModelItem item = cp;
                if(item.userId && users.contains(item.userId))
                {
                    const User &user = users.value(item.userId);
                    InputPeer peer(InputPeer::typeInputPeerUser);
                    peer.setUserId(item.userId);
                    peer.setAccessHash(user.accessHash());

                    item.user = tsdm->insertUser(user);
                    item.peer = tsdm->insertInputPeer(peer);
                }
                if(item.inviterId && users.contains(item.inviterId))
                    item.inviter = tsdm->insertUser(users.value(item.inviterId));
                if(item.kickedBy && users.contains(item.kickedBy))
                    item.kicker = tsdm->insertUser(users.value(item.kickedBy));

                items << item;
            }

            changed(items);
        });
        break;

    case InputPeerObject::TypeInputPeerChannel:
    {
        InputChannel channel(InputChannel::typeInputChannel);
        channel.setChannelId(p->currentPeer->channelId());
        channel.setAccessHash(p->currentPeer->accessHash());

        ChannelParticipantsFilterObject filter;
        filter.setClassType(static_cast<ChannelParticipantsFilterObject::ChannelParticipantsFilterClassType>(p->filter));

        setRefreshing(true);
        p->lastRequest = tg->channelsGetParticipants(channel, filter.core(), 0, 200,
                                    [this, dis](TG_CHANNELS_GET_PARTICIPANTS_CALLBACK){
            if(!dis || !mEngine) return;
            if(p->lastRequest != msgId) return;

            setRefreshing(false);
            if(!error.null) {
                setError(error.errorText, error.errorCode);
                return;
            }

            TelegramSharedDataManager *tsdm = mEngine->sharedData();

            QHash<qint32, User> users;
            Q_FOREACH(const User &user, result.users())
                users[user.id()] = user;

            QList<TelegramChatsMemebrsListModelItem> items;
            Q_FOREACH(const ChannelParticipant &cp, result.participants())
            {
                TelegramChatsMemebrsListModelItem item = cp;
                if(item.userId && users.contains(item.userId))
                    item.user = tsdm->insertUser(users.value(item.userId));
                if(item.inviterId && users.contains(item.inviterId))
                    item.inviter = tsdm->insertUser(users.value(item.inviterId));
                if(item.kickedBy && users.contains(item.kickedBy))
                    item.kicker = tsdm->insertUser(users.value(item.kickedBy));

                items << item;
            }

            changed(items);
        });
    }
        break;
    }
}

void TelegramMembersListModel::clean()
{
    beginResetModel();
    p->list.clear();
    p->lastRequest = 0;
    endResetModel();
}

void TelegramMembersListModel::changed(const QList<TelegramChatsMemebrsListModelItem> &list)
{
    bool count_changed = (list.count()!=p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const TelegramChatsMemebrsListModelItem &item = p->list.at(i);
        if( list.contains(item) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<TelegramChatsMemebrsListModelItem> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const TelegramChatsMemebrsListModelItem &item = temp_list.at(i);
        if( p->list.contains(item) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const TelegramChatsMemebrsListModelItem &item = p->list.at(i);
            int nw = temp_list.indexOf(item);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const TelegramChatsMemebrsListModelItem &item = list.at(i);
        if( p->list.contains(item) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, item );
        endInsertRows();
    }

    if(count_changed)
        Q_EMIT countChanged();
}

QString TelegramMembersListModel::convertDate(const QDateTime &td) const
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

TelegramMembersListModel::~TelegramMembersListModel()
{
    delete p;
}

