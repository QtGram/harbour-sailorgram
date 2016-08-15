#include "sailorgrampeer.h"
#include <telegram/types/peer.h>
#include <telegramshareddatamanager.h>
#include <telegramtools.h>

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

SailorgramPeer::SailorgramPeer(TelegramEngine* engine, InputPeerObject* inputpeer, QObject *parent): QObject(parent)
{
    TelegramSharedDataManager* tsdm = engine->sharedData();

    if(inputpeer->userId())
    {
        this->_user = tsdm->getUser(TelegramTools::identifier(Peer::typePeerUser, inputpeer->userId())).data();
        this->_chat = NULL;
    }
    else if(inputpeer->chatId())
    {
        this->_chat = tsdm->getChat(TelegramTools::identifier(Peer::typePeerChat, inputpeer->chatId())).data();
        this->_user = NULL;
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

void SailorgramPeer::setUser(UserObject *user)
{
    if(this->_user == user)
        return;

    this->_user = user;
    emit userChanged();
}

void SailorgramPeer::setChat(ChatObject *chat)
{
    if(this->_chat == chat)
        return;

    this->_chat = chat;
    emit chatChanged();
}

