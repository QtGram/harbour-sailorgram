#include "dialogscovermodel.h"

DialogsCoverModel::DialogsCoverModel(QObject *parent) : QAbstractListModel(parent), _telegramdialoglistmodel(NULL), _maxdialogs(0)
{

}

TelegramDialogListModel *DialogsCoverModel::dialogsModel() const
{
    return this->_telegramdialoglistmodel;
}

void DialogsCoverModel::setDialogsModel(TelegramDialogListModel *dialoglistmodel)
{
    if(this->_telegramdialoglistmodel == dialoglistmodel)
        return;

    this->beginResetModel();

    if(this->_telegramdialoglistmodel)
        disconnect(this->_telegramdialoglistmodel, &TelegramDialogListModel::countChanged, this, NULL);

    this->_telegramdialoglistmodel = dialoglistmodel;
    this->endResetModel();

    connect(this->_telegramdialoglistmodel, SIGNAL(countChanged()), this, SIGNAL(layoutChanged())); // Old Syntax, for now
}

int DialogsCoverModel::maxDialogs() const
{
    return this->_maxdialogs;
}

void DialogsCoverModel::setMaxDialogs(int i)
{
    if(this->_maxdialogs == i)
        return;

    this->_maxdialogs = i;
    emit maxDialogsChanged();
}

QHash<int, QByteArray> DialogsCoverModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    if(this->_telegramdialoglistmodel)
        roles = this->_telegramdialoglistmodel->roleNames();

    return roles;
}

QVariant DialogsCoverModel::data(const QModelIndex &index, int role) const
{
    if(!this->_telegramdialoglistmodel)
        return QVariant();

    return this->_telegramdialoglistmodel->data(index, role);
}

int DialogsCoverModel::rowCount(const QModelIndex& parent) const
{
    if(!this->_telegramdialoglistmodel)
        return 0;

    return qMin(this->_telegramdialoglistmodel->rowCount(parent), this->_maxdialogs);
}
