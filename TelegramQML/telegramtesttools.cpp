#include "telegramtesttools.h"

#include <QDebug>

class TelegramTestToolsPrivate
{
public:
};

TelegramTestTools::TelegramTestTools(QObject *parent) :
    QObject(parent)
{
    p = new TelegramTestToolsPrivate;
}

void TelegramTestTools::connectDestroy(QObject *obj, const QJSValue &callback)
{
    qDebug() << "added" << obj;
    connect(obj, &QObject::destroyed, this, [this, callback, obj](){
        QJSValue jsCallback = callback;
        qDebug() << "destroyed" << obj;
        jsCallback.call();
    });
}

TelegramTestTools::~TelegramTestTools()
{
    delete p;
}

