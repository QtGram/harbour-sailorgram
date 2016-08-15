#include "sailorgrampeer.h"
#include <telegram/types/peer.h>
#include <telegramshareddatamanager.h>
#include <telegramtools.h>

SailorgramPeer::SailorgramPeer(QObject *parent): QObject(parent), _inputpeer(NULL), _user(NULL), _chat(NULL)
{

}

SailorgramPeer::SailorgramPeer(QVariant peer, QObject *parent) : QObject(parent)
{
    if(peer.canConvert<UserObject*>())
    {
        this->_user = peer.value<UserObject*>();
        this->_chat = NULL;

        this->_inputpeer = new InputPeerObject(this);
        this->_inputpeer->setClassType(InputPeerObject::TypeInputPeerUser);
        this->_inputpeer->setUserId(this->_user->id());
    }
    else if(peer.canConvert<ChatObject*>())
    {
        this->_chat = peer.value<ChatObject*>();
        this->_user = NULL;

        this->_inputpeer = new InputPeerObject(this);
        this->_inputpeer->setClassType(InputPeerObject::TypeInputPeerUser);
        this->_inputpeer->setChatId(this->_chat->id());
    }
    else
    {
        this->_inputpeer = NULL;
        this->_user = NULL;
        this->_chat = NULL;
    }
}

SailorgramPeer::SailorgramPeer(TelegramEngine* engine, InputPeerObject* inputpeer, QObject *parent): QObject(parent), _inputpeer(inputpeer)
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

InputPeerObject *SailorgramPeer::inputPeer() const
{
    return this->_inputpeer;
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
