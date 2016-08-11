#include "sailorgramdialogitem.h"
#include "../sailorgramtools.h"

SailorgramDialogItem::SailorgramDialogItem(QObject *parent) : QObject(parent), _engine(NULL), _chat(NULL), _user(NULL), _dialog(NULL), _peer(NULL), _messageuser(NULL), _messageaction(NULL)
{
    this->_messagetype = TelegramEnums::TypeTextMessage;
    this->_issecretchat = false;
    this->_ismute = false;
    this->_ismessageout = false;
    this->_ismessageunread = false;
    this->_unreadcount = 0;
}

SailorgramDialogItem::SailorgramDialogItem(TelegramEngine *engine, DialogObject* dialog, QObject *parent) : QObject(parent), _engine(engine), _chat(NULL), _user(NULL), _dialog(dialog), _peer(NULL), _messageuser(NULL), _messageaction(NULL)
{
    this->_messagetype = TelegramEnums::TypeTextMessage;
    this->_issecretchat = false;
    this->_ismute = false;
    this->_ismessageout = false;
    this->_ismessageunread = false;
    this->_unreadcount = 0;
}

int SailorgramDialogItem::messageType() const
{
    return SailorgramTools::messageType(this->_messagetype);
}

InputPeerObject *SailorgramDialogItem::peer() const
{
    return this->_peer;
}

UserObject *SailorgramDialogItem::messageUser() const
{
    return this->_messageuser;
}

SailorgramMessageAction *SailorgramDialogItem::messageAction() const
{
    return this->_messageaction;
}

QString SailorgramDialogItem::title() const
{
    return this->_title;
}

QString SailorgramDialogItem::messageText() const
{
    return this->_messagetext;
}

QString SailorgramDialogItem::messageDate() const
{
    return this->_messagedate;
}

QString SailorgramDialogItem::typingUsers() const
{
    return this->_typingusers;
}

int SailorgramDialogItem::unreadCount() const
{
    return this->_unreadcount;
}

bool SailorgramDialogItem::isMute() const
{
    return this->_ismute;
}

bool SailorgramDialogItem::isBroadcast() const
{
    if(!this->_chat)
        return false;

    return this->_chat->broadcast();
}

bool SailorgramDialogItem::isChat() const
{
    return this->_chat != NULL;
}

bool SailorgramDialogItem::isSecretChat() const
{
    return this->_issecretchat;
}

bool SailorgramDialogItem::isActionMessage() const
{
    return this->_messagetype == TelegramEnums::TypeActionMessage;
}

bool SailorgramDialogItem::isMediaMessage() const
{
    return this->_messagetype != TelegramEnums::TypeTextMessage;
}

bool SailorgramDialogItem::isTextMessage() const
{
    return this->_messagetype == TelegramEnums::TypeTextMessage;
}

bool SailorgramDialogItem::isMessageOut() const
{
    return this->_ismessageout;
}

bool SailorgramDialogItem::isMessageUnread() const
{
    return this->_ismessageunread;
}

void SailorgramDialogItem::setChat(ChatObject *chat)
{
    this->_chat = chat;
}

void SailorgramDialogItem::setUser(UserObject *user)
{
    this->_user = user;
}

void SailorgramDialogItem::setMessageText(const QString& messagetext)
{
    if(this->_messagetext == messagetext)
        return;

    this->_messagetext = messagetext;
    emit messageTextChanged();
}

void SailorgramDialogItem::setMessageDate(const QString &messagedate)
{
    if(this->_messagedate == messagedate)
        return;

    this->_messagedate = messagedate;
    emit messageDateChanged();
}

void SailorgramDialogItem::setTypingUsers(const QVariantList &typingusers)
{
    if(typingusers.length() == 1)
    {
        UserObject* user = typingusers.first().value<UserObject*>();
        this->_typingusers = tr("%1 is typing...").arg(SailorgramTools::completeName(user));
    }
    else if(typingusers.length() == 2)
    {
        UserObject* user1 = typingusers[0].value<UserObject*>();
        UserObject* user2 = typingusers[1].value<UserObject*>();
        this->_typingusers = tr("%1 and %2 are typing...").arg(SailorgramTools::completeName(user1), SailorgramTools::completeName(user2));
    }
    else if(typingusers.length() >= 2)
        this->_typingusers = tr("%1 member(s) are typing...").arg(typingusers.length());
    else
        this->_typingusers.clear();

    emit typingUsersChanged();
}

void SailorgramDialogItem::setPeer(InputPeerObject *peer)
{
    if(this->_peer == peer)
        return;

    this->_peer = peer;
    emit peerChanged();
}

void SailorgramDialogItem::setTopMessage(MessageObject *topmessage)
{
    if(this->_messageaction)
    {
        this->_messageaction->deleteLater();
        this->_messageaction = NULL;
    }

    if(topmessage->action())
    {
        this->_messageaction = new SailorgramMessageAction(this->_engine, topmessage, this);
        emit messageActionChanged();
    }
}

void SailorgramDialogItem::setMessageUser(UserObject *user)
{
    if(this->_messageuser == user)
        return;

    this->_messageuser = user;
    emit messageUserChanged();
}

void SailorgramDialogItem::setMessageType(int messagetype)
{
    if(this->_messagetype == messagetype)
        return;

    this->_messagetype = messagetype;
    emit messageTypeChanged();
    emit isActionMessageChanged();
    emit isMediaMessageChanged();
    emit isTextMessageChanged();
}

void SailorgramDialogItem::setUnreadCount(int c)
{
    if(this->_unreadcount == c)
        return;

    this->_unreadcount = c;
    emit unreadCountChanged();
}

void SailorgramDialogItem::setIsSecretChat(bool b)
{
    if(this->_issecretchat == b)
        return;

    this->_issecretchat = b;
    emit isSecretChatChanged();
}

void SailorgramDialogItem::setIsMute(bool b)
{
    if(this->_ismute == b)
        return;

    this->_ismute = b;
    emit isMuteChanged();
}

void SailorgramDialogItem::setIsMessageOut(bool b)
{
    if(this->_ismessageout == b)
        return;

    this->_ismessageout = b;
    emit isMessageOutChanged();
}

void SailorgramDialogItem::setIsMessageUnread(bool b)
{
    if(this->_ismessageunread == b)
        return;

    this->_ismessageunread = b;
    emit isMessageUnreadChanged();
}

void SailorgramDialogItem::setTitle(const QString &title)
{
    if(this->_title == title)
        return;

    this->_title = title;
    emit titleChanged();
}
