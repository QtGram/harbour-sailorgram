#include "telegramqmlsharedpointer.h"
#include "tqbaseobject.h"

TelegramQmlSharedPointer::TelegramQmlSharedPointer(QObject *parent) :
    QObject(parent)
{

}

void TelegramQmlSharedPointer::setObject(QObject *object)
{
    if(object == mObject)
        return;

    if(mObject) disconnect(mObject.data(), &QObject::destroyed, this, &TelegramQmlSharedPointer::clean);
    mObject = object;
    if(mObject) connect(mObject.data(), &QObject::destroyed, this, &TelegramQmlSharedPointer::clean);

    Q_EMIT objectChanged();
}

QObject *TelegramQmlSharedPointer::object() const
{
    return mObject;
}

QStringList TelegramQmlSharedPointer::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(object);
}

void TelegramQmlSharedPointer::clean()
{
    mObject = 0;
}

TelegramQmlSharedPointer::~TelegramQmlSharedPointer()
{

}
