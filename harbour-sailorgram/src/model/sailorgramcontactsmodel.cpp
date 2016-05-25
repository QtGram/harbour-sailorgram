#include "sailorgramcontactsmodel.h"
#include <telegram/objects/userobject.h>

SailorgramContactsModel::SailorgramContactsModel(QObject *parent) : QIdentityProxyModel(parent)
{
    this->_telegramdialoglistmodel = new TelegramDialogListModel(this);
    this->_telegramdialoglistmodel->setVisibility(TelegramDialogListModel::VisibilityContacts | TelegramDialogListModel::VisibilityEmptyDialogs);
    this->_telegramdialoglistmodel->setSortFlag((QList<qint32>() << TelegramDialogListModel::SortByName << TelegramDialogListModel::SortByOnline));
    this->setSourceModel(this->_telegramdialoglistmodel);

    connect(this->_telegramdialoglistmodel, &TelegramDialogListModel::engineChanged, this, &SailorgramContactsModel::engineChanged);
    connect(this->_telegramdialoglistmodel, &TelegramDialogListModel::countChanged, this, &SailorgramContactsModel::countChanged);
}

TelegramEngine *SailorgramContactsModel::engine() const
{
    return this->_telegramdialoglistmodel->engine();
}

void SailorgramContactsModel::setEngine(TelegramEngine *engine)
{
    this->_telegramdialoglistmodel->setEngine(engine);
}

int SailorgramContactsModel::count() const
{
    return this->_telegramdialoglistmodel->count();
}

QVariant SailorgramContactsModel::data(const QModelIndex &proxyIndex, int role) const
{
    QModelIndex sourceindex = this->mapToSource(proxyIndex);

    if(role == SailorgramContactsModel::FirstLetterRole)
    {
        UserObject* user = this->_telegramdialoglistmodel->data(sourceindex, TelegramDialogListModel::RoleUserItem).value<UserObject*>();
        return !user->firstName().isEmpty() ? user->firstName().mid(0, 1).toUpper() : user->lastName().mid(0, 1).toUpper();
    }

    return this->_telegramdialoglistmodel->data(sourceindex, role);
}

QHash<int, QByteArray> SailorgramContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->_telegramdialoglistmodel->roleNames();

    roles[SailorgramContactsModel::FirstLetterRole] = "firstLetter";
    return roles;
}

