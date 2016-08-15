#include "sailorgramforwardmessage.h"

SailorgramForwardMessage::SailorgramForwardMessage(QObject *parent): QObject(parent), _message(NULL)
{

}

SailorgramForwardMessage::SailorgramForwardMessage(SailorgramMessageItem* message, QObject* parent) : QObject(parent)
{
    message->setParent(this); // Take ownership
    this->_message = message;
}

SailorgramMessageItem* SailorgramForwardMessage::message() const
{
    return this->_message;
}

