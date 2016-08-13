#include "sailorgrampeer.h"

SailorgramPeer::SailorgramPeer(QObject *parent): QObject(parent), _user(NULL), _chat(NULL)
{

}

SailorgramPeer::SailorgramPeer(QVariant peer, QObject *parent) : QObject(parent)
{
    if(peer.canConvert<UserObject*>())
    {
        this->_user = peer.value<UserObject*>();
        this->_chat = NULL;
    }
    else if(peer.canConvert<ChatObject*>())
    {
        this->_chat = peer.value<ChatObject*>();
        this->_user = NULL;
    }
    else
    {
        this->_user = NULL;
        this->_chat = NULL;
    }
}

UserObject *SailorgramPeer::user() const
{
    return this->_user;
}

ChatObject *SailorgramPeer::chat() const
{
    return this->_chat;
}

bool SailorgramPeer::isUser() const
{
    return this->_user != NULL;
}

bool SailorgramPeer::isChat() const
{
    return this->_chat != NULL;
}

bool SailorgramPeer::isValid() const
{
    return this->_user || this->_chat;
}

