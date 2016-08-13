#include "sailorgramcontactsmodel.h"
#include <telegram/objects/userobject.h>
#include <telegramdialoglistmodel.h>

SailorgramContactsModel::SailorgramContactsModel(QObject *parent) : SailorgramDialogsModel(parent)
{

}

void SailorgramContactsModel::init(TelegramEngine *engine)
{
    if(this->_dialoglistmodel)
        this->_dialoglistmodel->deleteLater();

    this->_dialoglistmodel = new TelegramDialogListModel(this);

    this->_dialoglistmodel->setVisibility(TelegramDialogListModel::VisibilityContacts |
                                          TelegramDialogListModel::VisibilityEmptyDialogs);

    this->_dialoglistmodel->setSortFlag(QList<qint32>() << TelegramDialogListModel::SortByName <<
                                                           TelegramDialogListModel::RoleStatusText <<
                                                           TelegramDialogListModel::SortByOnline);

    this->_dialoglistmodel->setEngine(engine);
    this->bindTo(this->_dialoglistmodel);
}

QVariant SailorgramContactsModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!this->_dialoglistmodel)
        return QVariant();

    if(role == SailorgramContactsModel::RoleFirstLetter)
    {
        QModelIndex sourceindex = this->mapToSource(proxyIndex);
        UserObject* user = this->_dialoglistmodel->data(sourceindex, TelegramDialogListModel::RoleUserItem).value<UserObject*>();
        return !user->firstName().isEmpty() ? user->firstName().mid(0, 1).toUpper() : user->lastName().mid(0, 1).toUpper();
    }

    return SailorgramDialogsModel::data(proxyIndex, role);
}

QHash<int, QByteArray> SailorgramContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles = SailorgramDialogsModel::roleNames();

    roles[SailorgramContactsModel::RoleFirstLetter] = "firstLetter";
    return roles;
}
