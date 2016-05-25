#include "telegramprofilemanagermodel.h"
#include "telegramengine.h"

#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QUuid>
#include <QDir>
#include <QPointer>
#include <QSharedPointer>

class TelegramProfileManagerModelItem
{
public:
    TelegramProfileManagerModelItem() : engine(0) {}

    QString phoneNumber;
    bool mute;
    TelegramEngine *engine;

    bool operator ==(const TelegramProfileManagerModelItem &b) {
        return b.phoneNumber == phoneNumber &&
               b.mute == mute &&
               b.engine == engine;
    }
};

class TelegramProfileManagerModelPrivate
{
public:
    QList<TelegramProfileManagerModelItem> list;
    QString source;
    QSqlDatabase db;
    QString dbConnection;
    bool initializing;

    QPointer<QQmlComponent> engineDelegate;
};

TelegramProfileManagerModel::TelegramProfileManagerModel(QObject *parent) :
    TelegramAbstractListModel(parent)
{
    p = new TelegramProfileManagerModelPrivate;
    p->initializing = false;
    p->dbConnection = QUuid::createUuid().toString();
    p->db = QSqlDatabase::addDatabase("QSQLITE", p->dbConnection);

    connect(this, &TelegramProfileManagerModel::itemsChanged, this, &TelegramProfileManagerModel::itemsChanged_slt);
}

void TelegramProfileManagerModel::init()
{
    changed(QList<TelegramProfileManagerModelItem>());
    if(p->db.isOpen())
        p->db.close();

    if(p->source.isEmpty())
        return;

    p->db.setDatabaseName(p->source);
    if(!p->db.open())
        return;

    initTables();
    initBuffer();
}

void TelegramProfileManagerModel::initTables()
{
    QStringList queries = QStringList() <<
        "CREATE TABLE IF NOT EXISTS Profiles ("
        "phoneNumber VARCHAR(32) NOT NULL,"
        "mute BOOLEAN NOT NULL,"
        "PRIMARY KEY (phoneNumber))";

    Q_FOREACH(const QString &query_str, queries)
    {
        QSqlQuery query(p->db);
        query.prepare(query_str);
        query.exec();
    }
}

void TelegramProfileManagerModel::initBuffer()
{
    QSqlQuery query(p->db);
    query.prepare("SELECT phoneNumber, mute FROM Profiles");
    query.exec();

    QList<TelegramProfileManagerModelItem> list;
    while(query.next())
    {
        QSqlRecord record = query.record();
        TelegramProfileManagerModelItem item;
        item.phoneNumber = record.value("phoneNumber").toString();
        item.mute = record.value("mute").toBool();

        list << item;
    }

    changed(list);
}

void TelegramProfileManagerModel::itemsChanged_slt()
{
    Q_FOREACH(QObject *obj, _items)
    {
        if(qobject_cast<TelegramEngine*>(obj))
            qobject_cast<TelegramEngine*>(obj)->setProfileManager(this);
    }
}

void TelegramProfileManagerModel::setInitializing(bool initializing)
{
    if(p->initializing == initializing)
        return;

    p->initializing = initializing;
    Q_EMIT initializingChanged();
}

TelegramProfileManagerModelItem TelegramProfileManagerModel::id(const QModelIndex &index) const
{
    return p->list.at(index.row());
}

int TelegramProfileManagerModel::count() const
{
    return p->list.count();
}

QVariant TelegramProfileManagerModel::data(const QModelIndex &index, int role) const
{
    const TelegramProfileManagerModelItem &item = id(index);
    QVariant result;
    switch(role)
    {
    case DataPhoneNumber:
        result = item.phoneNumber;
        break;

    case DataMute:
        result = item.mute;
        break;

    case DataEngine:
        result = QVariant::fromValue<QObject*>(item.engine);
        break;
    }

    return result;
}

QHash<int, QByteArray> TelegramProfileManagerModel::roleNames() const
{
    static QHash<int, QByteArray> *result = 0;
    if(result)
        return *result;

    result = new QHash<int, QByteArray>();
    result->insert(DataPhoneNumber, "phoneNumber");
    result->insert(DataMute, "mute");
    result->insert(DataEngine, "engine");

    return *result;
}

void TelegramProfileManagerModel::setSource(const QString &source)
{
    if(p->source == source)
        return;

    p->source = source;

    const QString dir = QFileInfo(p->source).dir().path();
    if(!dir.isEmpty())
        QDir().mkpath(dir);

    init();
    Q_EMIT sourceChanged();
}

QString TelegramProfileManagerModel::source() const
{
    return p->source;
}

void TelegramProfileManagerModel::setEngineDelegate(QQmlComponent *engineDelegate)
{
    if(p->engineDelegate == engineDelegate)
        return;

    p->engineDelegate = engineDelegate;
    init();
    Q_EMIT engineDelegateChanged();
}

QQmlComponent *TelegramProfileManagerModel::engineDelegate() const
{
    return p->engineDelegate;
}

bool TelegramProfileManagerModel::initializing() const
{
    return p->initializing;
}

QStringList TelegramProfileManagerModel::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(source)
                         << FUNCTION_NAME(engineDelegate);
}

void TelegramProfileManagerModel::addNew()
{
    QList<TelegramProfileManagerModelItem> list = p->list;
    Q_FOREACH(const TelegramProfileManagerModelItem &item, list)
        if(item.phoneNumber.isEmpty())
            return;

    TelegramProfileManagerModelItem item;
    list << item;
    changed(list);
    if(list.isEmpty())
        return;

    TelegramEngine *engine = p->list.last().engine;
    if(!engine)
        return;

    connect(engine, &TelegramEngine::stateChanged, this, [this, engine](){
        int state = engine->state();
        if(state <= TelegramEngine::AuthNeeded)
            return;
        for(int i=0; i<p->list.count(); i++)
        {
            TelegramProfileManagerModelItem item = p->list.at(i);
            if(!item.phoneNumber.isEmpty())
                continue;

            QSqlQuery query(p->db);
            query.prepare("INSERT OR REPLACE INTO Profiles (phoneNumber, mute) VALUES (:phone, :mute)");
            query.bindValue(":phone", engine->phoneNumber());
            query.bindValue(":mute", false);
            query.exec();

            p->list[i].phoneNumber = engine->phoneNumber();
            Q_EMIT dataChanged(index(i), index(i), QVector<int>()<<DataPhoneNumber);
        }

        setInitializing(false);
    });

    connect(engine, &TelegramEngine::authLoggedOut, this, [this, engine](){
        remove(engine->phoneNumber());
    });

    setInitializing(true);
}

int TelegramProfileManagerModel::add(const QString &phoneNumber, bool mute, TelegramEngine *engine)
{
    for(int i=0; i<p->list.count(); i++)
        if(p->list.at(i).phoneNumber == phoneNumber)
            return AddResultExists;
    if(phoneNumber.trimmed().isEmpty())
        return AddResultInvalidPhone;

    QList<TelegramProfileManagerModelItem> list = p->list;
    TelegramProfileManagerModelItem item;
    item.phoneNumber = phoneNumber;
    item.mute = mute;
    item.engine = engine;

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Profiles (phoneNumber, mute) VALUES (:phone, :mute)");
    query.bindValue(":phone", phoneNumber);
    query.bindValue(":mute", mute);
    if(!query.exec())
        return AddResultDatabaseError;

    list << item;
    changed(list);
    return AddResultSucessfully;
}

bool TelegramProfileManagerModel::remove(const QString &phoneNumber)
{
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM Profiles WHERE phoneNumber=:phone");
    query.bindValue(":phone", phoneNumber);
    if(query.exec())
        return false;

    QList<TelegramProfileManagerModelItem> list = p->list;
    for(int i=0; i<list.count(); i++)
    {
        TelegramProfileManagerModelItem item = list.at(i);
        if(item.phoneNumber != phoneNumber)
            continue;

        list.removeAt(i);
        i--;
    }
    changed(list);
    return true;
}

void TelegramProfileManagerModel::changed(const QList<TelegramProfileManagerModelItem> &list)
{
    bool count_changed = (list.count()!=p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const TelegramProfileManagerModelItem &item = p->list.at(i);
        if( list.contains(item) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<TelegramProfileManagerModelItem> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const TelegramProfileManagerModelItem &item = temp_list.at(i);
        if( p->list.contains(item) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const TelegramProfileManagerModelItem &item = p->list.at(i);
            int nw = temp_list.indexOf(item);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        TelegramProfileManagerModelItem item = list.at(i);
        if( p->list.contains(item) )
            continue;

        if(p->engineDelegate && !item.engine)
        {
            QObject *engineObj = p->engineDelegate->create(p->engineDelegate->creationContext());
            TelegramEngine *engine = qobject_cast<TelegramEngine*>(engineObj);
            engine->setPhoneNumber(item.phoneNumber);
            if(!engine)
            {
                qDebug() << "engineDelegate must inherit from the Engine component.";
                delete engineObj;
            }
            else
            {
                engine->setParent(this);
                item.engine = engine;
            }
        }

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, item );
        endInsertRows();
    }

    if(count_changed)
        Q_EMIT countChanged();
}

TelegramProfileManagerModel::~TelegramProfileManagerModel()
{
    QString dbConnection = p->dbConnection;
    delete p;
    QSqlDatabase::removeDatabase(dbConnection);
}

