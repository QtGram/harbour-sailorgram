/*
    Copyright (C) 2015 Canonical

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QRegExp>

#include "telegramdetailedcontactsmodel.h"
#include "telegramcontactsfiltermodel.h"

TelegramContactsFilterModel::TelegramContactsFilterModel(QObject *parent) :
        QSortFilterProxyModel(parent), mOwnId(0), mSearchTerm("")
{
}

TelegramContactsFilterModel::~TelegramContactsFilterModel()
{
}

bool TelegramContactsFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString fullName = sourceModel()->data(index, TelegramDetailedContactsModel::FullNameRole).toString();
    qint32 id = sourceModel()->data(index, TelegramDetailedContactsModel::IdRole).toInt();

    bool notSelf = (id != mOwnId);
    return notSelf && (fullName.contains(filterRegExp()));
}

QString TelegramContactsFilterModel::searchTerm() const
{
    return mSearchTerm;
}

void TelegramContactsFilterModel::setSearchTerm(const QString &searchTerm)
{
    if (mSearchTerm != searchTerm) {
        mSearchTerm = searchTerm;

        QString pattern = QString("\\b%1").arg(searchTerm);
        setFilterKeyColumn(0);
        setFilterRegExp(QRegExp(pattern, Qt::CaseInsensitive));

        Q_EMIT searchTermChanged();
    }
}

QVariant TelegramContactsFilterModel::get(int rowIndex) const {
    QVariantMap data;
    QModelIndex sourceIndex = mapToSource(index(rowIndex, 0));
    if (sourceIndex.isValid()) {
        QAbstractItemModel *source = sourceModel();
        QHash<int, QByteArray> roles = source->roleNames();
        Q_FOREACH(int role, roles.keys()) {
            data.insert(roles[role], source->data(sourceIndex, role));
        }
    }
    return data;
}

