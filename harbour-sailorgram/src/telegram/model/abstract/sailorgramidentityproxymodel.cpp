#include "sailorgramidentityproxymodel.h"

SailorgramIdentityProxyModel::SailorgramIdentityProxyModel(QObject *parent) : QIdentityProxyModel(parent), _engine(NULL)
{

}

TelegramEngine *SailorgramIdentityProxyModel::engine() const
{
    return this->_engine;
}

int SailorgramIdentityProxyModel::count() const
{
    return this->rowCount();
}

void SailorgramIdentityProxyModel::setEngine(TelegramEngine *engine)
{
    if(this->_engine == engine)
        return;

    this->_engine = engine;
    this->init(engine);
    emit engineChanged();
}

void SailorgramIdentityProxyModel::inserted(QModelIndex sourceindex)
{
    Q_UNUSED(sourceindex);
    emit countChanged();
}

void SailorgramIdentityProxyModel::removed(QModelIndex sourceindex)
{
    Q_UNUSED(sourceindex);
    emit countChanged();
}

void SailorgramIdentityProxyModel::bindTo(QAbstractListModel *sourcemodel)
{
    QAbstractItemModel* oldsourcemodel = this->sourceModel();

    if(oldsourcemodel)
    {
        disconnect(oldsourcemodel, &QAbstractItemModel::rowsInserted, this, &SailorgramIdentityProxyModel::onRowsInserted);
        disconnect(oldsourcemodel, &QAbstractItemModel::rowsRemoved, this, &SailorgramIdentityProxyModel::onRowsRemoved);
        disconnect(oldsourcemodel, &QAbstractItemModel::dataChanged, this, &SailorgramIdentityProxyModel::onDataChanged);
        disconnect(oldsourcemodel, &QAbstractItemModel::modelReset, this, &SailorgramIdentityProxyModel::onModelReset);
    }

    connect(sourcemodel, &QAbstractItemModel::rowsInserted, this, &SailorgramIdentityProxyModel::onRowsInserted);
    connect(sourcemodel, &QAbstractItemModel::rowsRemoved, this, &SailorgramIdentityProxyModel::onRowsRemoved);
    connect(sourcemodel, &QAbstractItemModel::dataChanged, this, &SailorgramIdentityProxyModel::onDataChanged);
    connect(sourcemodel, &QAbstractItemModel::modelReset, this, &SailorgramIdentityProxyModel::onModelReset);

    this->setSourceModel(sourcemodel);
}

void SailorgramIdentityProxyModel::onRowsInserted(const QModelIndex &, int start, int end)
{
    QAbstractItemModel* sourcemodel = this->sourceModel();

    if(!sourcemodel)
        return;

    for(int i = start; i <= end; i++)
        this->inserted(sourcemodel->index(i, 0));
}

void SailorgramIdentityProxyModel::onRowsRemoved(const QModelIndex &, int start, int end)
{
    QAbstractItemModel* sourcemodel = this->sourceModel();

    if(!sourcemodel)
        return;

    for(int i = start; i <= end; i++)
        this->removed(sourcemodel->index(i, 0));
}

void SailorgramIdentityProxyModel::onDataChanged(const QModelIndex &topleft, const QModelIndex &bottomright, const QVector<int>& roles)
{
    QAbstractItemModel* sourcemodel = this->sourceModel();

    if(!sourcemodel)
        return;

    for(int i = topleft.row(); i <= bottomright.row(); i++)
    {
        QModelIndex sourceindex = sourcemodel->index(i, 0);

        if(this->contains(sourceindex))
            this->changed(sourceindex, roles);
        else
            this->inserted(sourceindex);
    }
}

void SailorgramIdentityProxyModel::onModelReset()
{
    this->clear();

    QAbstractItemModel* sourcemodel = this->sourceModel();

    if(!sourcemodel)
        return;

    for(int i = 0; i < sourcemodel->rowCount(); i++)
        this->inserted(sourcemodel->index(i, 0));
}
