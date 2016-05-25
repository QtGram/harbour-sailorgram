#include "telegramstatustyping.h"
#include "telegramsharedpointer.h"
#include "tqbaseobject.h"

#include <telegram/objects/sendmessageactionobject.h>
#include <telegram/objects/inputpeerobject.h>

#include <QPointer>

class TelegramStatusTypingPrivate
{
public:
    TelegramSharedPointer<InputPeerObject> peer;
    QPointer<SendMessageActionObject> action;
};

TelegramStatusTyping::TelegramStatusTyping(QObject *parent) :
    QObject(parent)
{
    p = new TelegramStatusTypingPrivate;
    setAction(new SendMessageActionObject(this));
    p->action->setClassType(SendMessageActionObject::TypeSendMessageTypingAction);

    connect(this, &TelegramStatusTyping::actionChanged, this, &TelegramStatusTyping::changed);
}

void TelegramStatusTyping::setPeer(InputPeerObject *peer)
{
    if(p->peer == peer)
        return;

    p->peer = peer;
    Q_EMIT peerChanged();
    Q_EMIT changed();
}

InputPeerObject *TelegramStatusTyping::peer() const
{
    return p->peer;
}

void TelegramStatusTyping::setAction(SendMessageActionObject *action)
{
    if(p->action == action)
        return;

    if(p->action)
        disconnect(p->action.data(), &SendMessageActionObject::coreChanged, this, &TelegramStatusTyping::actionChanged);
    p->action = action;
    if(p->action)
        connect(p->action.data(), &SendMessageActionObject::coreChanged, this, &TelegramStatusTyping::actionChanged);

    Q_EMIT actionChanged();
}

SendMessageActionObject *TelegramStatusTyping::action()
{
    return p->action;
}

QStringList TelegramStatusTyping::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(peer);
}

TelegramStatusTyping::~TelegramStatusTyping()
{
    delete p;
}

