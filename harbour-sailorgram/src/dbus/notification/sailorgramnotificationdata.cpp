#include "sailorgramnotificationdata.h"

SailorgramNotificationData::SailorgramNotificationData(const QString& peerhex, TelegramEngine *engine, QObject *parent) : QObject(parent), _peerhex(peerhex), _engine(engine)
{

}

TelegramEngine *SailorgramNotificationData::engine() const
{
    return this->_engine;
}

