/*
    Copyright (C) 2014 Aseman, 2015 Canonical
    http://aseman.co

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

#include "telegramdetailedcontactsmodel.h"
#include "telegramqml.h"
#include "objects/types.h"

#include <telegram.h>
#include <QPointer>

class TelegramDetailedContactsModelPrivate
{
public:
    QPointer<TelegramQml> telegram;
    QList<qint64> contacts;
    bool initializing;
};

TelegramDetailedContactsModel::TelegramDetailedContactsModel(QObject *parent) :
    TgAbstractListModel(parent)
{
    p = new TelegramDetailedContactsModelPrivate;
    p->telegram = 0;
    p->initializing = false;
}

TelegramQml *TelegramDetailedContactsModel::telegram() const
{
    return p->telegram;
}

void TelegramDetailedContactsModel::setTelegram(TelegramQml *tgo)
{
    if(p->telegram == tgo)
        return;
    if(p->telegram)
    {
        disconnect(p->telegram, SIGNAL(contactsChanged()), this, SLOT(contactsChanged()));
        disconnect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(recheck()));
        disconnect(p->telegram, SIGNAL(connectedChanged()), this, SLOT(recheck()));
    }

    p->telegram = tgo;
    if(p->telegram)
    {
        connect(p->telegram, SIGNAL(contactsChanged()), this, SLOT(contactsChanged()));
        connect(p->telegram, SIGNAL(authLoggedInChanged()), this, SLOT(recheck()), Qt::QueuedConnection);
        connect(p->telegram, SIGNAL(connectedChanged()), this, SLOT(recheck()), Qt::QueuedConnection);
    }

    refresh();
    Q_EMIT telegramChanged();
}

qint64 TelegramDetailedContactsModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return p->contacts.at(row);
}

int TelegramDetailedContactsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->contacts.count();
}

QVariant TelegramDetailedContactsModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    const qint64 key = id(index);
    UserObject* user = p->telegram->user(key);

    switch( role )
    {
    case ItemRole:
        res = QVariant::fromValue<UserObject*>(user);
        break;
    case IdRole:
        res = user->id();
        break;
    case AccessHashRole:
        res = user->accessHash();
        break;
    case PhoneRole:
        res = user->phone();
        break;
    case FirstNameRole:
        res = user->firstName();
        break;
    case LastNameRole:
        res = user->lastName();
        break;
    case FullNameRole: {
        QString full = user->firstName();
        if (user->lastName().length() > 0) {
            full.append(" ").append(user->lastName());
        }
        res = full;
        break;
    }
    case UsernameRole:
        res = user->username();
        break;
    case PhotoRole:
        res = QVariant::fromValue<UserProfilePhotoObject*>(user->photo());
        break;
    case StatusRole:
        res = QVariant::fromValue<UserStatusObject*>(user->status());
        break;
    }

    return res;
}

QHash<qint32, QByteArray> TelegramDetailedContactsModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert(ItemRole, "user");
    res->insert(IdRole, "id");
    res->insert(AccessHashRole, "accessHash");
    res->insert(PhoneRole, "phoneRole");
    res->insert(FirstNameRole, "firstName");
    res->insert(LastNameRole, "lastName");
    res->insert(FullNameRole, "fullName");
    res->insert(UsernameRole, "username");
    res->insert(PhotoRole, "photo");
    res->insert(StatusRole, "status");
    return *res;
}

int TelegramDetailedContactsModel::count() const
{
    return p->contacts.count();
}

bool TelegramDetailedContactsModel::initializing() const
{
    return p->initializing;
}

void TelegramDetailedContactsModel::refresh()
{
    if( !p->telegram )
        return;

    contactsChanged();
    recheck();

    p->initializing = true;
    Q_EMIT initializingChanged();
}

void TelegramDetailedContactsModel::recheck()
{
    if( !p->telegram || !p->telegram->authLoggedIn() )
        return;
    Telegram *tg = p->telegram->telegram();
    if(tg && tg->isConnected())
        tg->contactsGetContacts();
}

void TelegramDetailedContactsModel::contactsChanged()
{
    const QList<qint64> & contacts_unsort = p->telegram->contacts();
    QMap<QString,qint64> sort_map;
    Q_FOREACH( qint64 id, contacts_unsort )
    {
        UserObject *user = p->telegram->user(id);
        sort_map.insertMulti( user->firstName() + " " + user->lastName(), id );
    }

    const QList<qint64> & contacts = sort_map.values();

    beginResetModel();
    p->contacts.clear();
    endResetModel();

    for( int i=0 ; i<contacts.count() ; i++ )
    {
        const qint64 dId = contacts.at(i);
        if( p->contacts.contains(dId) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->contacts.insert( i, dId );
        endInsertRows();
    }

    p->initializing = false;
    Q_EMIT initializingChanged();
}

TelegramDetailedContactsModel::~TelegramDetailedContactsModel()
{
    delete p;
}
