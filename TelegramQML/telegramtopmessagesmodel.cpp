#define DEFINE_DIS \
    QPointer<TelegramTopMessagesModel> dis = this;

#include "telegramtopmessagesmodel.h"
#include "telegramsharedpointer.h"

#include <telegram/objects/typeobjects.h>
#include <telegram.h>

#include <QDebug>

class TelegramTopMessagesModelPrivate
{
public:
    qint64 lastRequest;
    int dialogsLimit;
    int visibleType;
};

TelegramTopMessagesModel::TelegramTopMessagesModel(QObject *parent) :
    TelegramMessageListModel(parent)
{
    p = new TelegramTopMessagesModelPrivate;
    p->lastRequest = 0;
    p->dialogsLimit = 10;
    p->visibleType = VisibleTypeAll;
    setLimit(5);
}

void TelegramTopMessagesModel::setDialogsLimit(int dialogsLimit)
{
    if(p->dialogsLimit == dialogsLimit)
        return;

    p->dialogsLimit = dialogsLimit;
    refresh();
    Q_EMIT dialogsLimitChanged();
}

int TelegramTopMessagesModel::dialogsLimit() const
{
    return p->dialogsLimit;
}

void TelegramTopMessagesModel::setVisibleType(int visibleType)
{
    if(p->visibleType == visibleType)
        return;

    p->visibleType = visibleType;
    refresh();
    Q_EMIT visibleTypeChanged();
}

int TelegramTopMessagesModel::visibleType() const
{
    return p->visibleType;
}

void TelegramTopMessagesModel::recheck()
{
    refresh();
}

bool tg_top_mlist_model_dialog_sort(const Dialog &d1, const Dialog &d2)
{
    return d1.unreadCount() >= d2.unreadCount();
}

void TelegramTopMessagesModel::refresh()
{
    clean();
    if(!mEngine || !mEngine->telegram())
        return;

    setRefreshing(true);

    DEFINE_DIS;
    Telegram *tg = mEngine->telegram();
    p->lastRequest = tg->messagesGetDialogs(0, 0, InputPeer::null, 100, [this, dis](TG_MESSAGES_GET_DIALOGS_CALLBACK){
        if(!dis || p->lastRequest != msgId) return;
        p->lastRequest = 0;
        if(!error.null) {
            setRefreshing(false);
            setError(error.errorText, error.errorCode);
            return;
        }

        QHash<QByteArray, Chat> chats;
        QHash<QByteArray, qint64> accessHashes;
        Q_FOREACH(const User &user, result.users())
            accessHashes[TelegramTools::identifier(user)] = user.accessHash();
        Q_FOREACH(const Chat &chat, result.chats())
        {
            const QByteArray &key = TelegramTools::identifier(chat);
            accessHashes[key] = chat.accessHash();
            chats[key] = chat;
        }

        QList<Dialog> dialogs = result.dialogs();
        for(int i=0; i<dialogs.length(); i++)
        {
            const Dialog dialog = dialogs.at(i);
            const Peer peer = dialog.peer();
            const QByteArray &key = TelegramTools::identifier(peer);

            if(dialog.unreadCount() == 0)
            {
                dialogs.removeAt(i);
                i--;
                continue;
            }

            switch(static_cast<int>(peer.classType()))
            {
            case Peer::typePeerUser:
                if( !(p->visibleType & VisibleTypeUser) )
                {
                    dialogs.removeAt(i);
                    i--;
                }
                break;
            case Peer::typePeerChat:
                if( !(p->visibleType & VisibleTypeChat) )
                {
                    dialogs.removeAt(i);
                    i--;
                }
                break;
            case Peer::typePeerChannel:
            {
                Chat chat = chats.value(key);
                if( (!chat.megagroup() && !(p->visibleType & VisibleTypeChannel)) ||
                    (chat.megagroup() && !(p->visibleType & VisibleTypeMegaGroup)))
                {
                    dialogs.removeAt(i);
                    i--;
                }
            }
                break;
            }
        }

        qStableSort(dialogs.begin(), dialogs.end(), tg_top_mlist_model_dialog_sort);
        dialogs = dialogs.mid(0, p->dialogsLimit);

        QList<InputPeer> peers;
        Q_FOREACH(const Dialog &dlg, dialogs)
            peers << TelegramTools::peerInputPeer(dlg.peer(), accessHashes[TelegramTools::identifier(dlg)]);

        loadDialogs(peers, dialogs);
    });
}

void TelegramTopMessagesModel::loadDialogs(const QList<InputPeer> &peers, const QList<Dialog> &dialogs)
{
    loadDialogs_prv(peers, dialogs);
}

void TelegramTopMessagesModel::loadDialogs_prv(QList<InputPeer> peers, QList<Dialog> dialogs)
{
    if(peers.isEmpty())
    {
        setRefreshing(false);
        return;
    }
    if(!mEngine || !mEngine->telegram())
        return;

    InputPeer peer = peers.takeFirst();
    Dialog dialog = dialogs.takeFirst();

    const int recieveLimit = (dialog.unreadCount()>limit()? limit() : dialog.unreadCount());

    DEFINE_DIS;
    Telegram *tg = mEngine->telegram();
    p->lastRequest = tg->messagesGetHistory(peer, 0, 0, 0, recieveLimit, 0, 0, [this, dis, peers, dialogs](TG_MESSAGES_GET_MESSAGES_CALLBACK) {
        if(!dis || p->lastRequest != msgId) return;
        p->lastRequest = 0;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }

        setHasBackMore(false);
        processOnResult(result, true);

        if(!dis || p->lastRequest)
            return;

        loadDialogs_prv(peers, dialogs);

        //NOTE: QTimer::singleShot(500, this, [this, dis, peers, dialogs](){
        //    if(!dis || p->lastRequest) return;
        //    loadDialogs_prv(peers, dialogs);
        //});
    });
}

TelegramTopMessagesModel::~TelegramTopMessagesModel()
{
    delete p;
}
