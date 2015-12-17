#include "tgabstractlistmodel.h"

#include <QHash>

TgAbstractListModel::TgAbstractListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

QStringList TgAbstractListModel::roles() const
{
    QStringList result;
    const QHash<int,QByteArray> &roles = roleNames();
    QHashIterator<int,QByteArray> i(roles);
    while(i.hasNext())
    {
        i.next();
        result << i.value();
    }

    qSort(result.begin(), result.end());
    return result;
}

QVariant TgAbstractListModel::get(int row, int role) const
{
    if(row >= rowCount())
        return QVariant();

    const QModelIndex &idx = index(row,0);
    return data(idx , role);
}

QVariantMap TgAbstractListModel::get(int index) const
{
    if(index >= rowCount())
        return QVariantMap();

    QVariantMap result;
    const QHash<int,QByteArray> &roles = roleNames();
    QHashIterator<int,QByteArray> i(roles);
    while(i.hasNext())
    {
        i.next();
        result[i.value()] = get(index, i.key());
    }

    return result;
}

TgAbstractListModel::~TgAbstractListModel()
{
}

