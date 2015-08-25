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


#ifndef TELEGRAMCONTACTSFILTERMODEL_H
#define TELEGRAMCONTACTSFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "telegramqml_global.h"

class TELEGRAMQMLSHARED_EXPORT TelegramContactsFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)

public:
    TelegramContactsFilterModel(QObject *parent = 0);
    ~TelegramContactsFilterModel();

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    Q_INVOKABLE QVariant get(int rowIndex) const;

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);

Q_SIGNALS:
    void searchTermChanged();
    void countChanged();

private:
    qint64 mOwnId;
    QString mSearchTerm;
};

#endif // TELEGRAMCONTACTSFILTERMODEL_H
