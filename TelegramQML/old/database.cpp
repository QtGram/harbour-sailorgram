#define FIRST_CHECK if(!p->core) return;

#include "database.h"
#include "databasecore.h"
#include "telegramqml_macros.h"

#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QDir>
#include <QDebug>


class DatabasePrivate
{
public:
    QString path;

    QThread *thread;
    DatabaseCore *core;
    DatabaseAbstractEncryptor *encrypter;

    QString phoneNumber;
    QString configPath;
};

Database::Database(QObject *parent) :
    QObject(parent)
{
    p = new DatabasePrivate;
    p->thread = 0;
    p->core = 0;
    p->encrypter = 0;
}

void Database::setPhoneNumber(const QString &phoneNumber)
{
    if(p->phoneNumber == phoneNumber)
        return;

    p->phoneNumber = phoneNumber;
    refresh();

    Q_EMIT phoneNumberChanged();
}

QString Database::phoneNumber() const
{
    return p->phoneNumber;
}

void Database::setConfigPath(const QString &path)
{
    if(p->configPath == path)
        return;

    p->configPath = path;
    refresh();

    Q_EMIT configPathChanged();
}

QString Database::configPath() const
{
    return p->configPath;
}

void Database::setEncrypter(DatabaseAbstractEncryptor *encrypter)
{
    p->encrypter = encrypter;
    if(p->core)
        QMetaObject::invokeMethod(p->core, "setEncrypter", Qt::QueuedConnection, Q_ARG(DatabaseAbstractEncryptor*, encrypter));
}

DatabaseAbstractEncryptor *Database::encrypter() const
{
    return p->encrypter;
}

void Database::insertUser(const User &user)
{
    FIRST_CHECK;
    DbUser duser;
    duser.user = user;

    QMetaObject::invokeMethod(p->core, "insertUser", Qt::QueuedConnection, Q_ARG(DbUser,duser));
}

void Database::insertChat(const Chat &chat)
{
    FIRST_CHECK;
    DbChat dchat;
    dchat.chat = chat;

    QMetaObject::invokeMethod(p->core, "insertChat", Qt::QueuedConnection, Q_ARG(DbChat,dchat));
}

void Database::insertDialog(const Dialog &dialog, bool encrypted)
{
    FIRST_CHECK;
    DbDialog ddlg;
    ddlg.dialog = dialog;

    QMetaObject::invokeMethod(p->core, "insertDialog", Qt::QueuedConnection, Q_ARG(DbDialog,ddlg), Q_ARG(bool,encrypted));
}

void Database::insertContact(const Contact &contact)
{
    FIRST_CHECK;
    DbContact dcnt;
    dcnt.contact = contact;

    QMetaObject::invokeMethod(p->core, "insertContact", Qt::QueuedConnection, Q_ARG(DbContact,dcnt));
}

void Database::insertMessage(const Message &message, bool encrypted)
{
    FIRST_CHECK;
    DbMessage dmsg;
    dmsg.message = message;

    QMetaObject::invokeMethod(p->core, "insertMessage", Qt::QueuedConnection, Q_ARG(DbMessage,dmsg), Q_ARG(bool,encrypted));
}

void Database::insertMediaEncryptedKeys(qint64 mediaId, const QByteArray &key, const QByteArray &iv)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "insertMediaEncryptedKeys", Qt::QueuedConnection, Q_ARG(qint64,mediaId), Q_ARG(QByteArray,key), Q_ARG(QByteArray,iv));
}

void Database::updateUnreadCount(qint64 chatId, int unreadCount)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "updateUnreadCount", Qt::QueuedConnection, Q_ARG(qint64,chatId), Q_ARG(int,unreadCount));
}

void Database::readFullDialogs()
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "readFullDialogs", Qt::QueuedConnection);
}

void Database::markMessagesAsReadFromMaxDate(qint32 chatId, qint32 maxDate)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "markMessagesAsReadFromMaxDate", Qt::QueuedConnection, Q_ARG(qint32, chatId), Q_ARG(qint32, maxDate));
}

void Database::markMessagesAsRead(const QList<qint32> &messages)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "markMessagesAsRead", Qt::QueuedConnection, Q_ARG(QList<qint32>, messages));
}

void Database::readMessages(const Peer &peer, int offset, int limit)
{
    FIRST_CHECK;
    DbPeer dpeer;
    dpeer.peer = peer;

    QMetaObject::invokeMethod(p->core, "readMessages", Qt::QueuedConnection, Q_ARG(DbPeer,dpeer), Q_ARG(int,offset), Q_ARG(int,limit) );
}

void Database::deleteMessage(qint64 msgId)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "deleteMessage", Qt::QueuedConnection, Q_ARG(qint64,msgId));
}

void Database::deleteDialog(qint64 dlgId)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "deleteDialog", Qt::QueuedConnection, Q_ARG(qint64,dlgId));
}

void Database::deleteHistory(qint64 dlgId)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "deleteHistory", Qt::QueuedConnection, Q_ARG(qint64,dlgId));
}

void Database::blockUser(qint64 userId)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "blockUser", Qt::QueuedConnection, Q_ARG(qint64, userId));
}

void Database::unblockUser(qint64 userId)
{
    FIRST_CHECK;
    QMetaObject::invokeMethod(p->core, "unblockUser", Qt::QueuedConnection, Q_ARG(qint64, userId));
}

void Database::userFounded_slt(const DbUser &user)
{
    Q_EMIT userFounded(user.user);
}

void Database::chatFounded_slt(const DbChat &chat)
{
    Q_EMIT chatFounded(chat.chat);
}

void Database::dialogFounded_slt(const DbDialog &dialog, bool encrypted)
{
    Q_EMIT dialogFounded(dialog.dialog, encrypted);
}

void Database::messageFounded_slt(const DbMessage &message)
{
    Q_EMIT messageFounded(message.message);
}

void Database::contactFounded_slt(const DbContact &contact)
{
    Q_EMIT contactFounded(contact.contact);
}

void Database::refresh()
{
    if(p->core && p->thread)
    {
        p->thread->quit();
        p->thread->wait();
        p->thread->deleteLater();
        p->core->deleteLater();
        p->thread = 0;
        p->core = 0;
    }

    if(p->phoneNumber.isEmpty() || p->configPath.isEmpty())
        return;

    const QString &dirPath = p->configPath + "/" + p->phoneNumber;
    QDir().mkpath(dirPath);
    p->path = dirPath + "/database.db";

    if( !QFileInfo::exists(p->path) )
        QFile::copy(DATABASE_DB_PATH,p->path);

    QFile(p->path).setPermissions(QFileDevice::WriteOwner|QFileDevice::WriteGroup|QFileDevice::ReadUser|QFileDevice::ReadGroup);

    p->core = new DatabaseCore(p->path, p->configPath, p->phoneNumber);
    p->core->setEncrypter(p->encrypter);

    p->thread = new QThread(this);
    p->thread->start();

    p->core->moveToThread(p->thread);

    connect(p->core, SIGNAL(chatFounded(DbChat))         , SLOT(chatFounded_slt(DbChat))         , Qt::QueuedConnection );
    connect(p->core, SIGNAL(userFounded(DbUser))         , SLOT(userFounded_slt(DbUser))         , Qt::QueuedConnection );
    connect(p->core, SIGNAL(dialogFounded(DbDialog,bool)), SLOT(dialogFounded_slt(DbDialog,bool)), Qt::QueuedConnection );
    connect(p->core, SIGNAL(messageFounded(DbMessage))   , SLOT(messageFounded_slt(DbMessage))   , Qt::QueuedConnection );
    connect(p->core, SIGNAL(contactFounded(DbContact))   , SLOT(contactFounded_slt(DbContact))   , Qt::QueuedConnection );
    connect(p->core, SIGNAL(mediaKeyFounded(qint64,QByteArray,QByteArray)),
            SIGNAL(mediaKeyFounded(qint64,QByteArray,QByteArray)), Qt::QueuedConnection );
}

Database::~Database()
{
    if(p->core && p->thread)
    {
        p->thread->quit();
        p->thread->wait();
        p->thread->deleteLater();
        p->core->deleteLater();
        p->thread = 0;
        p->core = 0;
    }

    delete p;
}
