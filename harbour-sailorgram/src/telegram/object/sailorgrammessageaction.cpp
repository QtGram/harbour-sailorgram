#include "sailorgrammessageaction.h"
#include "../sailorgramenums.h"
#include "../sailorgramtools.h"
#include <telegramshareddatamanager.h>
#include <telegramtools.h>

SailorgramMessageAction::SailorgramMessageAction(QObject *parent): QObject(parent), _message(NULL), _fromuser(NULL)
{

}

SailorgramMessageAction::SailorgramMessageAction(TelegramEngine *engine, MessageObject *message, QObject *parent) : QObject(parent), _message(message), _fromuser(NULL)
{
    if(message->action())
    {
        TelegramSharedDataManager* tsdm = engine->sharedData();
        this->_fromuser = tsdm->getUser(TelegramTools::identifier(Peer::typePeerUser, message->fromId()));

        if(message->action()->userId())
            this->_touser = tsdm->getUser(TelegramTools::identifier(Peer::typePeerUser, message->action()->userId()));
    }
}

UserObject *SailorgramMessageAction::fromUser() const
{
    if(this->_fromuser.isNull())
        return NULL;

    return this->_fromuser.data();
}

UserObject *SailorgramMessageAction::toUser() const
{
    if(this->_touser.isNull())
        return NULL;

    return this->_touser.data();
}

int SailorgramMessageAction::actionType() const
{
    if(!this->_message->action())
        return SailorgramEnums::ActionTypeEmpty;

    return SailorgramTools::actionType(this->_message->action()->classType());
}

bool SailorgramMessageAction::isChannel() const
{
    if(this->_message->action())
        return this->_message->action()->channelId() != 0;

    return false;
}

QString SailorgramMessageAction::title() const
{
    if(this->_message->action())
        return this->_message->action()->title();

    return QString();
}
