#include "sailorgramcontactsmodel.h"

SailorgramContactsModel::SailorgramContactsModel(QObject *parent) : QIdentityProxyModel(parent)
{
    this->_telegramcontactsmodel = new TelegramContactsModel(this);
    this->setSourceModel(this->_telegramcontactsmodel);

    connect(this->_telegramcontactsmodel, SIGNAL(telegramChanged()), this, SIGNAL(telegramChanged()));
    connect(this->_telegramcontactsmodel, SIGNAL(countChanged()), this, SIGNAL(countChanged()));
    connect(this->_telegramcontactsmodel, SIGNAL(initializingChanged()), this, SIGNAL(initializingChanged()));
}

TelegramQml *SailorgramContactsModel::telegram() const
{
    return this->_telegramcontactsmodel->telegram();
}

void SailorgramContactsModel::setTelegram(TelegramQml *telegram)
{
    this->_telegramcontactsmodel->setTelegram(telegram);
}

int SailorgramContactsModel::count() const
{
    return this->_telegramcontactsmodel->count();
}

bool SailorgramContactsModel::initializing() const
{
    return this->_telegramcontactsmodel->initializing();
}

QVariant SailorgramContactsModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!this->_telegramcontactsmodel)
        return QVariant();

    QModelIndex sourceindex = this->mapToSource(proxyIndex);

    if((role == SailorgramContactsModel::UserRole) || (role == SailorgramContactsModel::LetterRole))
    {
        ContactObject* contact = this->_telegramcontactsmodel->data(sourceindex, TelegramContactsModel::ItemRole).value<ContactObject*>();
        TelegramQml* telegram = this->_telegramcontactsmodel->telegram();
        UserObject* user = telegram->user(contact->userId());

        if(role == SailorgramContactsModel::UserRole)
            return QVariant::fromValue(user);

        // Letter Role
        return !user->firstName().isEmpty() ? user->firstName().mid(0, 1).toUpper() : user->lastName().mid(0, 1).toUpper();
    }
    else if(role == SailorgramContactsModel::ContactRole)
        return this->_telegramcontactsmodel->data(sourceindex, TelegramContactsModel::ItemRole);

    return this->_telegramcontactsmodel->data(sourceindex, role);
}

QHash<int, QByteArray> SailorgramContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[SailorgramContactsModel::ContactRole] = "contact";
    roles[SailorgramContactsModel::UserRole] = "user";
    roles[SailorgramContactsModel::LetterRole] = "firstLetter";
    return roles;
}

