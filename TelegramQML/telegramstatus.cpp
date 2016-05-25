#define DEFINE_DIS \
    QPointer<TelegramStatus> dis = this;

#include "telegramstatus.h"
#include "telegramsharedpointer.h"
#include "telegramstatustyping.h"

#include <QPointer>
#include <telegram.h>

#include <telegram/objects/inputpeerobject.h>
#include <telegram/objects/sendmessageactionobject.h>

class TelegramStatusPrivate
{
public:
    QPointer<TelegramEngine> engine;
    QPointer<TelegramStatusTyping> typing;

    bool online;

    int onlineTimer;
    int typingTimer;
};

TelegramStatus::TelegramStatus(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramStatusPrivate;
    p->online = false;
    p->onlineTimer = 0;
    p->typingTimer = 0;

    setTyping(new TelegramStatusTyping(this));
}

void TelegramStatus::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramStatus::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramStatus::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramStatus::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramStatus::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramStatus::engine() const
{
    return p->engine;
}

void TelegramStatus::setOnline(bool online)
{
    if(p->online == online)
        return;

    p->online = online;
    if(p->online && !p->onlineTimer)
        p->onlineTimer = startTimer(60000);
    if(!p->online && p->onlineTimer)
    {
        killTimer(p->onlineTimer);
        p->onlineTimer = 0;
    }

    requestStatus(p->online);
    Q_EMIT onlineChanged();
}

bool TelegramStatus::online() const
{
    return p->online;
}

void TelegramStatus::setTyping(TelegramStatusTyping *typing)
{
    if(p->typing == typing)
        return;

    p->typing = typing;

    std::function<void ()> callback = [this](){
        if(p->typing && !p->typingTimer)
            p->typingTimer = startTimer(4000);
        if(!p->typing && p->typingTimer)
        {
            killTimer(p->typingTimer);
            p->typingTimer = 0;
        }
        if(p->typing && p->typing->peer())
            requestTyping(p->typing->peer(), p->typing->action());
    };

    connect(p->typing.data(), &TelegramStatusTyping::changed, this, callback);
    callback();

    Q_EMIT typingChanged();
}

TelegramStatusTyping *TelegramStatus::typing() const
{
    return p->typing;
}

QStringList TelegramStatus::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine);
}

void TelegramStatus::requestStatus(bool online)
{
    if(!p->engine || p->engine->state() != TelegramEngine::AuthLoggedIn)
        return;

    DEFINE_DIS;
    Telegram *tg = p->engine->telegram();
    tg->accountUpdateStatus(!online, [this, dis](TG_ACCOUNT_UPDATE_STATUS_CALLBACK){
        Q_UNUSED(msgId)
        Q_UNUSED(result)
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
    });
}

void TelegramStatus::requestTyping(InputPeerObject *peer, SendMessageActionObject *action)
{
    if(!p->engine || p->engine->state() != TelegramEngine::AuthLoggedIn)
        return;

    DEFINE_DIS;
    Telegram *tg = p->engine->telegram();
    tg->messagesSetTyping(peer->core(), action->core(), [this, dis](TG_MESSAGES_SET_TYPING_CALLBACK){
        Q_UNUSED(msgId)
        Q_UNUSED(result)
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            return;
        }
    });
}

void TelegramStatus::refresh()
{
    if(p->online)
        requestStatus(p->online);
}

void TelegramStatus::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == p->onlineTimer)
    {
        requestStatus(p->online);
    }
    else
    if(e->timerId() == p->typingTimer)
    {
        if(!p->typing || !p->typing->peer())
        {
            killTimer(p->typingTimer);
            p->typingTimer = 0;
        }
        else
            requestTyping(p->typing->peer(), p->typing->action());
    }
    else
        TqObject::timerEvent(e);
}

TelegramStatus::~TelegramStatus()
{
    delete p;
}

