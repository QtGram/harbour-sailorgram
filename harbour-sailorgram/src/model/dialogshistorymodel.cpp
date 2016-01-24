#include "dialogshistorymodel.h"

DialogsHistoryModel::DialogsHistoryModel(QObject *parent) : QSortFilterProxyModel(parent), _dialogsmodel(NULL), _historylength(0)
{

}

bool DialogsHistoryModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    if(!this->sourceModel())
        return false;

    return source_row < this->_historylength;
}

TelegramDialogsModel *DialogsHistoryModel::dialogsModel() const
{
    return this->_dialogsmodel;
}

void DialogsHistoryModel::setDialogsModel(TelegramDialogsModel *dialogsmodel)
{
    if(this->_dialogsmodel == dialogsmodel)
        return;

    this->_dialogsmodel = dialogsmodel;
    this->setSourceModel(dialogsmodel);
    this->invalidate();
    emit dialogsModelChanged();
}

int DialogsHistoryModel::historyLength() const
{
    return this->_historylength;
}

void DialogsHistoryModel::setHistoryLength(int hl)
{
    if(this->_historylength == hl)
        return;

    this->_historylength = hl;
    emit historyLengthChanged();
}
