#include "dialogscovermodel.h"

DialogsCoverModel::DialogsCoverModel(QObject *parent) : QAbstractListModel(parent), _dialogsmodel(NULL), _maxdialogs(0)
{

}

TelegramDialogsModel *DialogsCoverModel::dialogsModel() const
{
    return this->_dialogsmodel;
}

void DialogsCoverModel::setDialogsModel(TelegramDialogsModel *dialogsmodel)
{
    if(this->_dialogsmodel == dialogsmodel)
        return;

    this->beginResetModel();

    if(this->_dialogsmodel)
        disconnect(this->_dialogsmodel, SIGNAL(countChanged()), this, NULL);

    this->_dialogsmodel = dialogsmodel;
    this->endResetModel();

    connect(this->_dialogsmodel, SIGNAL(countChanged()), this, SLOT(onCountChanged()));
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
    roles[DialogsCoverModel::ItemRole] = "item";

    return roles;
}

QVariant DialogsCoverModel::data(const QModelIndex &index, int role) const
{
    if(this->_dialogsmodel && (role == DialogsCoverModel::ItemRole))
        return QVariant::fromValue(this->_dialogsmodel->at(index.row()));

    return QVariant();
}

int DialogsCoverModel::rowCount(const QModelIndex& parent) const
{
    if(!this->_dialogsmodel)
        return 0;

    return qMin(this->_dialogsmodel->rowCount(parent), this->_maxdialogs);
}

void DialogsCoverModel::onCountChanged()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}
