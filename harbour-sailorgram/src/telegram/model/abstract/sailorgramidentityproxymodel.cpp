#include "sailorgramidentityproxymodel.h"

SailorgramIdentityProxyModel::SailorgramIdentityProxyModel(QObject *parent) : QIdentityProxyModel(parent), _engine(NULL)
{

}

TelegramEngine *SailorgramIdentityProxyModel::engine() const
{
    return this->_engine;
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
    }

    connect(sourcemodel, &QAbstractItemModel::rowsInserted, this, &SailorgramIdentityProxyModel::onRowsInserted);
    connect(sourcemodel, &QAbstractItemModel::rowsRemoved, this, &SailorgramIdentityProxyModel::onRowsRemoved);
    connect(sourcemodel, &QAbstractItemModel::dataChanged, this, &SailorgramIdentityProxyModel::onDataChanged);

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
        this->changed(sourcemodel->index(i, 0), roles);

    emit dataChanged(this->mapFromSource(topleft), this->mapFromSource(bottomright), this->roleNames().keys().toVector());
}
