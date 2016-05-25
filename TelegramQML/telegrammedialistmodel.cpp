#include "telegrammedialistmodel.h"

class TelegramMediaListModelPrivate
{
public:
};

TelegramMediaListModel::TelegramMediaListModel(QObject *parent) :
    TelegramMessageSearchModel(parent)
{
    p = new TelegramMediaListModelPrivate;
}

QStringList TelegramMediaListModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine)
                         << FUNCTION_NAME(messageFilter);
}

TelegramMediaListModel::~TelegramMediaListModel()
{
    delete p;
}
