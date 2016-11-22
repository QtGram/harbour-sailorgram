#include "dialogscovermodel.h"

DialogsCoverModel::DialogsCoverModel(QObject *parent) : QIdentityProxyModel(parent), _dialogsmodel(NULL), _maxdialogs(0)
{

}

DialogsModel *DialogsCoverModel::dialogsModel() const
{
    return this->_dialogsmodel;
}

int DialogsCoverModel::maxDialogs() const
{
    return this->_maxdialogs;
}

void DialogsCoverModel::setDialogsModel(DialogsModel *dialogsmodel)
{
    if(this->_dialogsmodel == dialogsmodel)
        return;

    this->_dialogsmodel = dialogsmodel;
    this->setSourceModel(dialogsmodel);
    emit dialogsModelChanged();
}

void DialogsCoverModel::setMaxDialogs(int maxdialogs)
{
    if(this->_maxdialogs == maxdialogs)
        return;

    this->_maxdialogs = maxdialogs;
    emit maxDialogsChanged();
}

int DialogsCoverModel::rowCount(const QModelIndex &) const
{
    if(!this->_dialogsmodel)
        return 0;

    return qMin(this->_maxdialogs, this->_dialogsmodel->rowCount());
}
