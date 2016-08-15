#include "sailorgrammessageitem.h"
#include "../sailorgramtools.h"

SailorgramMessageItem::SailorgramMessageItem(QObject *parent): QObject(parent), _engine(NULL), _message(NULL), _messageaction(NULL), _messagemedia(NULL), _messagereply(NULL), _fromuser(NULL), _touser(NULL), _frompeer(NULL), _replypeer(NULL), _forwardpeer(NULL)
{
    this->_messagetype = TelegramEnums::TypeTextMessage;
}

SailorgramMessageItem::SailorgramMessageItem(TelegramEngine *engine, QObject *parent): SailorgramMessageItem(engine, SailorgramMessageItem::CategoryNormal, parent)
{

}

SailorgramMessageItem::SailorgramMessageItem(TelegramEngine *engine, MessageCategories messagecategory, QObject *parent) : QObject(parent), _engine(engine), _message(NULL), _messagecategory(messagecategory), _messageaction(NULL), _messagereply(NULL), _fromuser(NULL), _touser(NULL), _frompeer(NULL), _replypeer(NULL), _forwardpeer(NULL)
{
    this->_messagetype = TelegramEnums::TypeTextMessage;
    this->_messagemedia = new SailorgramMessageMedia(engine, this);
}

UserObject *SailorgramMessageItem::fromUser() const
{
    return this->_fromuser;
}

UserObject *SailorgramMessageItem::toUser() const
{
    return this->_touser;
}

SailorgramPeer *SailorgramMessageItem::fromPeer() const
{
    return this->_frompeer;
}

SailorgramPeer *SailorgramMessageItem::replyPeer() const
{
    return this->_replypeer;
}

SailorgramPeer *SailorgramMessageItem::forwardPeer() const
{
    return this->_forwardpeer;
}

SailorgramMessageItem *SailorgramMessageItem::messageReply()
{
    if(!this->_messagereply)
    {
        this->_messagereply = new SailorgramMessageItem(this->_engine, SailorgramMessageItem::CategoryReply, this);
        emit hasReplyChanged();
    }

    return this->_messagereply;
}

SailorgramMessageAction *SailorgramMessageItem::messageAction() const
{
    return this->_messageaction;
}

SailorgramMessageMedia *SailorgramMessageItem::messageMedia() const
{
    return this->_messagemedia;
}

QString SailorgramMessageItem::messageText() const
{
    return this->_messagetext;
}

QString SailorgramMessageItem::messageDate() const
{
    return this->_messagedate;
}

int SailorgramMessageItem::messageType() const
{
    return SailorgramTools::messageType(this->_messagetype);
}

bool SailorgramMessageItem::isActionMessage() const
{
    return this->_messagetype == TelegramEnums::TypeActionMessage;
}

bool SailorgramMessageItem::isMediaMessage() const
{
    return this->_messagetype != TelegramEnums::TypeTextMessage;
}

bool SailorgramMessageItem::isTextMessage() const
{
    return this->_messagetype == TelegramEnums::TypeTextMessage;
}

bool SailorgramMessageItem::isMessageUnread() const
{
    return this->_ismessageunread;
}

bool SailorgramMessageItem::isMessageOut() const
{
    return this->_ismessageout;
}

bool SailorgramMessageItem::hasReply() const
{
    return this->_replypeer != NULL;
}

MessageObject *SailorgramMessageItem::rawMessage() const
{
    return this->_message;
}

bool SailorgramMessageItem::isForward() const
{
    return this->_forwardpeer != NULL;
}

void SailorgramMessageItem::setMessage(MessageObject *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    emit rawMessageChanged();

    if(this->_messageaction)
    {
        this->_messageaction->deleteLater();
        this->_messageaction = NULL;
    }

    if(!message)
        return;

    if(message->action())
    {
        this->_messageaction = new SailorgramMessageAction(this->_engine, message, this);
        emit messageActionChanged();
    }

    if(this->_messagecategory != SailorgramMessageItem::CategoryNormal)
    {
        this->_messagetext = SailorgramTools::messageText(message);
        this->_messagedate = SailorgramTools::messageDate(message);

        emit messageTextChanged();
        emit messageDateChanged();
    }
}

void SailorgramMessageItem::setReplyPeer(SailorgramPeer *replypeer)
{
    if(this->_replypeer == replypeer)
        return;

    if(this->_replypeer)
    {
        this->_replypeer->deleteLater();
        this->_replypeer = NULL;
    }

    if(replypeer->isValid())
    {
        this->_replypeer = replypeer;
        this->_replypeer->setParent(this); // Take ownership
    }
    else
        replypeer->deleteLater();

    emit replyPeerChanged();
}

void SailorgramMessageItem::setForwardPeer(SailorgramPeer *forwardpeer)
{
    if(this->_forwardpeer == forwardpeer)
        return;

    if(this->_forwardpeer)
        this->_forwardpeer->deleteLater();

    if(forwardpeer->isValid())
    {
        this->_forwardpeer = forwardpeer;
        this->_forwardpeer->setParent(this); // Take ownership
    }
    else
        forwardpeer->deleteLater();

    emit forwardPeerChanged();
}

void SailorgramMessageItem::setFromPeer(InputPeerObject *inputpeer)
{
    if(this->_frompeer)
        this->_frompeer->deleteLater();

    this->_frompeer = new SailorgramPeer(this->_engine, inputpeer, this);
    emit fromPeerChanged();
}

void SailorgramMessageItem::setFromUser(UserObject *user)
{
    if(this->_fromuser == user)
        return;

    this->_fromuser = user;
    emit fromUserChanged();
}

void SailorgramMessageItem::setToUser(UserObject *user)
{
    if(this->_touser == user)
        return;

    this->_touser = user;
    emit fromUserChanged();
}

void SailorgramMessageItem::setMessageType(int messagetype)
{
    if(this->_messagetype == messagetype)
        return;

    this->_messagetype = messagetype;
    emit messageTypeChanged();
    emit isActionMessageChanged();
    emit isMediaMessageChanged();
    emit isTextMessageChanged();
}

void SailorgramMessageItem::setMessageText(const QString &messagetext)
{
    if(this->_messagetext == messagetext)
        return;

    this->_messagetext = messagetext;
    emit messageTextChanged();
}

void SailorgramMessageItem::setMessageDate(const QString &messagedate)
{
    if(this->_messagedate == messagedate)
        return;

    this->_messagedate = messagedate;
    emit messageDateChanged();
}

void SailorgramMessageItem::setIsMessageUnread(bool b)
{
    if(this->_ismessageunread == b)
        return;

    this->_ismessageunread = b;
    emit isUnreadChanged();
}

void SailorgramMessageItem::setIsMessageOut(bool b)
{
    if(this->_ismessageout == b)
        return;

    this->_ismessageout = b;
    emit isOutChanged();
}
