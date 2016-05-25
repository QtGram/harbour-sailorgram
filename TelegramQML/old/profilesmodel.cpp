/*
    Copyright (C) 2014 Aseman
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

#include "profilesmodel.h"
#include "telegramqml_macros.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QDebug>
#include <QUuid>

class ProfilesModelPrivate
{
public:
    QSqlDatabase db;
    QString path;
    QString configPath;

    QHash<QString,ProfilesModelItem*> data;
    QList<QString> numbers;
    QString connectionName;
};

ProfilesModel::ProfilesModel(QObject *parent) :
    TgAbstractListModel(parent)
{
    p = new ProfilesModelPrivate;
    p->connectionName = PROFILES_DB_CONNECTION + QUuid::createUuid().toString();
}

void ProfilesModel::setConfigPath(const QString &path)
{
    if(p->configPath == path)
        return;

    p->configPath = path;
    Q_EMIT configPathChanged();

    refresh();
}

QString ProfilesModel::configPath() const
{
    return p->configPath;
}

QString ProfilesModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return p->numbers.at(row);
}

int ProfilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->numbers.count();
}

QVariant ProfilesModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    const QString & key = id(index);
    switch( role )
    {
    case NumberRole:
        res = p->data[key]->number();
        break;

    case Qt::DisplayRole:
    case NameRole:
        res = p->data[key]->name();
        break;

    case IconRole:
        res = p->data[key]->icon();
        break;

    case MuteRole:
        res = p->data[key]->mute();
        break;

    case ItemRole:
        res = QVariant::fromValue<ProfilesModelItem*>(p->data[key]);
        break;
    }

    return res;
}

bool ProfilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const QString & key = id(index);
    switch( role )
    {
    case NumberRole:
    case ItemRole:
        return false;
        break;

    case Qt::DisplayRole:
    case NameRole:
        p->data[key]->setName(value.toString());
        return true;
        break;

    case IconRole:
        p->data[key]->setIcon(value.toString());
        return true;
        break;

    case MuteRole:
        p->data[key]->setMute(value.toBool());
        return true;
        break;
    }

    return false;
}

QHash<qint32, QByteArray> ProfilesModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( NumberRole, "number");
    res->insert( NameRole, "name");
    res->insert( IconRole, "icon");
    res->insert( MuteRole, "mute");
    res->insert( ItemRole, "item");

    return *res;
}

Qt::ItemFlags ProfilesModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int ProfilesModel::count() const
{
    return p->numbers.count();
}

QStringList ProfilesModel::keys() const
{
    return p->numbers;
}

ProfilesModelItem *ProfilesModel::add(const QString &number)
{
    if( p->data.contains(number) )
        return 0;

    beginInsertRows(QModelIndex(),count(),count());

    ProfilesModelItem *item = new ProfilesModelItem(number, this);
    item->setNumber(number);

    p->data[number] = item;
    p->numbers.append(number);

    Q_EMIT countChanged();
    Q_EMIT keysChanged();

    save(number);
    endInsertRows();

    return item;
}

bool ProfilesModel::remove(const QString &number)
{
    if( !p->data.contains(number) )
        return false;

    const int row = p->numbers.indexOf(number);
    beginRemoveRows(QModelIndex(),row,row);

    p->numbers.removeOne(number);
    ProfilesModelItem *item = p->data.take(number);
    item->deleteLater();

    bool removed = purge(number);
    endRemoveRows();

    Q_EMIT countChanged();
    Q_EMIT keysChanged();

    return removed;
}

bool ProfilesModel::containt(const QString &number)
{
    return p->data.contains(number);
}

ProfilesModelItem *ProfilesModel::get(const QString &number)
{
    return p->data.value(number);
}

void ProfilesModel::init_buffer()
{
    QSqlQuery profiles_query(p->db);
    profiles_query.prepare("SELECT number, name, icon, mute FROM profiles");
    profiles_query.exec();

    while( profiles_query.next() )
    {
        const QSqlRecord & record = profiles_query.record();

        beginInsertRows(QModelIndex(),count(),count());

        const QString & key = record.value(0).toString();
        ProfilesModelItem *item = new ProfilesModelItem(key, this);
        item->setNumber(key);
        item->setName( record.value(1).toString() );
        item->setIcon( record.value(2).toString() );
        item->setMute( record.value(3).toBool() );

        p->data[key] = item;
        p->numbers.append(key);
        endInsertRows();
    }

    Q_EMIT countChanged();
    Q_EMIT keysChanged();
}

void ProfilesModel::save(const QString &key)
{
    ProfilesModelItem *item = p->data.value(key);
    if( !item )
        return;

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO profiles (number,name,icon,mute) VALUES (:nmbr,:name,:icon,:mute)");
    query.bindValue(":nmbr", item->number());
    query.bindValue(":name", item->name());
    query.bindValue(":icon", item->icon());
    query.bindValue(":mute", item->mute());
    query.exec();
}

bool ProfilesModel::purge(const QString &number)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM profiles WHERE number=:number");
    query.bindValue(":number", number);

    bool hasDeleted = query.exec();
    if(!hasDeleted) {
        qCritical() << "could not remove profile" << query.lastError().text();
    }

    QString profileDirPath = p->configPath  + "/" + number;
    QDir profileDir(profileDirPath);
    profileDir.removeRecursively();

    return hasDeleted;
}

void ProfilesModel::refresh()
{
    if(p->configPath.isEmpty())
    {
        qDebug() << "configPath is empty. Couldn't init ProfilesModel";
        return;
    }

    p->path = p->configPath  + "/profiles.sqlite";

    QDir().mkpath(p->configPath);

    if( !QFileInfo::exists(p->path) )
        QFile::copy(PROFILES_DB_PATH,p->path);

    QFile(p->path).setPermissions(QFile::WriteOwner|QFile::WriteGroup|QFile::ReadUser|QFile::ReadGroup);

    p->db = QSqlDatabase::addDatabase("QSQLITE",p->connectionName);
    p->db.setDatabaseName(p->path);
    p->db.open();

    init_buffer();
}

ProfilesModel::~ProfilesModel()
{
    QString connectionName = p->connectionName;
    delete p;
    if(QSqlDatabase::contains(connectionName))
        QSqlDatabase::removeDatabase(connectionName);
}


class ProfilesModelItemPrivate
{
public:
    ProfilesModel *model;
    QString key;

    QString number;
    QString name;
    QString icon;
    bool mute;
};

ProfilesModelItem::ProfilesModelItem(const QString &key, ProfilesModel *parent) :
    QObject(parent)
{
    p = new ProfilesModelItemPrivate;
    p->mute = false;
    p->model = parent;
    p->key = key;
}

QString ProfilesModelItem::number() const
{
    return p->number;
}

void ProfilesModelItem::setNumber(const QString &number)
{
    if( number == p->number )
        return;

    p->number = number;
    save();
    Q_EMIT numberChanged();
}

QString ProfilesModelItem::name() const
{
    return p->name;
}

void ProfilesModelItem::setName(const QString &name)
{
    if( name == p->name )
        return;

    p->name = name;
    save();
    Q_EMIT nameChanged();
}

QString ProfilesModelItem::icon() const
{
    return p->icon;
}

void ProfilesModelItem::setIcon(const QString &icon)
{
    if( icon == p->icon )
        return;

    p->icon = icon;
    save();
    Q_EMIT iconChanged();
}

bool ProfilesModelItem::mute() const
{
    return p->mute;
}

void ProfilesModelItem::setMute(bool mute)
{
    if( mute == p->mute )
        return;

    p->mute = mute;
    save();
    Q_EMIT muteChanged();
}

void ProfilesModelItem::save()
{
    QMetaObject::invokeMethod( p->model, "save", Qt::QueuedConnection, Q_ARG(QString,p->key) );
}

ProfilesModelItem::~ProfilesModelItem()
{
    delete p;
}
