#define DEFINE_DIS \
    QPointer<TelegramMessageSearchModel> dis = this;

#include "telegrammessagesearchmodel.h"
#include "telegramsharedpointer.h"

#include <telegram/objects/typeobjects.h>

#include <telegram.h>

class TelegramMessageSearchModelPrivate
{
public:
    TelegramSharedPointer<InputPeerObject> peer;
    QString keyword;
    QDateTime minDate;
    QDateTime maxDate;
    int filter;
    qint64 lastRequest;
};

TelegramMessageSearchModel::TelegramMessageSearchModel(QObject *parent) :
    TelegramMessageListModel(parent)
{
    p = new TelegramMessageSearchModelPrivate;
    p->lastRequest = 0;
    p->minDate = QDateTime::fromTime_t(0);
    p->maxDate = p->minDate;
    p->filter = MessagesFilterObject::TypeInputMessagesFilterEmpty;
}

void TelegramMessageSearchModel::setCurrentPeer(InputPeerObject *peer)
{
    if(p->peer == peer)
        return;

    p->peer = peer;
    refresh();
    Q_EMIT currentPeerChanged();
    Q_EMIT keyChanged();
    Q_EMIT megagroupChanged();
    Q_EMIT editableChanged();
}

InputPeerObject *TelegramMessageSearchModel::currentPeer() const
{
    return p->peer;
}

void TelegramMessageSearchModel::setKeyword(const QString &keyword)
{
    if(p->keyword == keyword)
        return;

    p->keyword = keyword;
    refresh();
    Q_EMIT keywordChanged();
}

QString TelegramMessageSearchModel::keyword() const
{
    return p->keyword;
}

void TelegramMessageSearchModel::setMessageFilter(int filter)
{
    if(p->filter == filter)
        return;

    p->filter = filter;
    refresh();
    Q_EMIT messageFilterChanged();
}

int TelegramMessageSearchModel::messageFilter() const
{
    return p->filter;
}

void TelegramMessageSearchModel::setMinimumDate(const QDateTime &minDate)
{
    if(p->minDate == minDate)
        return;

    p->minDate = minDate;
    refresh();
    Q_EMIT minimumDateChanged();
}

QDateTime TelegramMessageSearchModel::minimumDate() const
{
    return p->minDate;
}

void TelegramMessageSearchModel::setMaximumDate(const QDateTime &maxDate)
{
    if(p->maxDate == maxDate)
        return;

    p->maxDate = maxDate;
    refresh();
    Q_EMIT maximumDateChanged();
}

QDateTime TelegramMessageSearchModel::maximumDate() const
{
    return p->maxDate;
}

QStringList TelegramMessageSearchModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine)
                         << FUNCTION_NAME(currentPeer)
                         << FUNCTION_NAME(keyword);
}

void TelegramMessageSearchModel::loadBack()
{
    getFromServer(true);
}

void TelegramMessageSearchModel::refresh()
{
    clean();
    getFromServer(false);
}

void TelegramMessageSearchModel::getFromServer(bool more)
{
    if((p->keyword.isEmpty() && (p->filter == MessagesFilterObject::TypeInputMessagesFilterEmpty || !p->peer))
       || !mEngine || !mEngine->telegram())
        return;

    setRefreshing(true);
    DEFINE_DIS;
    Telegram::Callback<MessagesMessages> callback = [this, dis, more](TG_MESSAGES_GET_MESSAGES_CALLBACK){
        if(!dis || p->lastRequest != msgId) return;
        setRefreshing(false);
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
        setHasBackMore(false);
        processOnResult(result, more);
    };

    MessagesFilterObject filter;
    filter.setClassType(p->filter);

    Telegram *tg = mEngine->telegram();
    if(p->peer)
    {
        int offset = more? count() : 0;
        p->lastRequest = tg->messagesSearch(p->peer->core(), p->keyword, filter.core(), p->minDate.toTime_t(),
                                            p->maxDate.toTime_t(), offset, 0, limit(), callback);
    }
    else
    {
        int offset = 0;
        if(more)
        {
            MessageObject *msg = get(count(), RoleMessageItem).value<MessageObject*>();
            if(msg) offset = msg->id();
        }
        p->lastRequest = tg->messagesSearchGlobal(p->keyword, 0, InputPeer::null, offset, limit(), callback);
    }
}

TelegramMessageSearchModel::~TelegramMessageSearchModel()
{
    delete p;
}

