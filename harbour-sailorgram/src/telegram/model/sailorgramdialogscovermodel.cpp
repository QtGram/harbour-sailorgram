#include "sailorgramdialogscovermodel.h"

SailorgramDialogsCoverModel::SailorgramDialogsCoverModel(QObject *parent) : QAbstractListModel(parent), _dialoglistmodel(NULL), _maxdialogs(0)
{

}

SailorgramDialogsModel *SailorgramDialogsCoverModel::dialogsModel() const
{
    return this->_dialoglistmodel;
}

void SailorgramDialogsCoverModel::setDialogsModel(SailorgramDialogsModel *dialoglistmodel)
{
    if(this->_dialoglistmodel == dialoglistmodel)
        return;

    this->beginResetModel();

    if(this->_dialoglistmodel)
        disconnect(this->_dialoglistmodel, &SailorgramDialogsModel::countChanged, this, NULL);

    this->_dialoglistmodel = dialoglistmodel;
    this->endResetModel();

    connect(this->_dialoglistmodel, SIGNAL(countChanged()), this, SIGNAL(layoutChanged())); // Old Syntax, for now
}

int SailorgramDialogsCoverModel::maxDialogs() const
{
    return this->_maxdialogs;
}

void SailorgramDialogsCoverModel::setMaxDialogs(int i)
{
    if(this->_maxdialogs == i)
        return;

    this->_maxdialogs = i;
    emit maxDialogsChanged();
}

QHash<int, QByteArray> SailorgramDialogsCoverModel::roleNames() const
{
    if(this->_dialoglistmodel)
        return this->_dialoglistmodel->roleNames();

    return QHash<int, QByteArray>();
}

QVariant SailorgramDialogsCoverModel::data(const QModelIndex &index, int role) const
{
    if(!this->_dialoglistmodel)
        return QVariant();

    return this->_dialoglistmodel->data(index, role);
}

int SailorgramDialogsCoverModel::rowCount(const QModelIndex& parent) const
{
    if(!this->_dialoglistmodel)
        return 0;

    return qMin(this->_dialoglistmodel->rowCount(parent), this->_maxdialogs);
}
