#define DECLARE_INSERT(VALUE, FOLDER_PATH) \
    QDir().mkpath(FOLDER_PATH); \
    const QByteArray &key = TelegramTools::identifier(VALUE); \
    writeMap(QString(FOLDER_PATH + "/" + QCryptographicHash::hash(key,QCryptographicHash::Md5).toHex()), VALUE.toMap());

#include "telegramcache.h"
#include "telegramtools.h"
#include "telegramengine.h"
#include "telegramshareddatamanager.h"

#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QDir>
#include <QQmlEngine>
#include <QtQml>
#include <QPointer>
#include <QDebug>
#include <QSettings>

#include <telegram.h>
#include <telegram/objects/chatobject.h>
#include <telegram/objects/userobject.h>

class TelegramCachePrivate
{
public:
    QString path;
    QJSValue encryptMethod;
    QJSValue decryptMethod;
    QPointer<TelegramEngine> engine;
    QPointer<Telegram> telegram;
    QSettings *settings;
    qint32 pts;
    bool updating;
};

class SortUnitType
{
public:
    SortUnitType(): num(0){}
    QChar ch;
    quint64 num;
};

QList<SortUnitType> analize_file_name(const QString &fileName)
{
    QList<SortUnitType> res;
    for(int i=0; i<fileName.length(); i++)
    {
        const QChar &ch = fileName[i];
        if(ch.isNumber())
        {
            int num = QString(ch).toInt();
            if(res.isEmpty() || !res.last().ch.isNull() )
                res << SortUnitType();

            SortUnitType & resUnit = res[res.length()-1];
            resUnit.num = resUnit.num*10 + num;
        }
        else
        {
            SortUnitType unit;
            unit.ch = ch;
            res << unit;
        }
    }

    return res;
}

bool fileListSort(const QFileInfo &f1, const QFileInfo &f2)
{
    const QString & s1 = f1.fileName();
    const QString & s2 = f2.fileName();

    const QList<SortUnitType> &ul1 = analize_file_name(s1);
    const QList<SortUnitType> &ul2 = analize_file_name(s2);

    for(int i=0; i<ul1.length() && i<ul2.length(); i++)
    {
        const SortUnitType &u1 = ul1.at(i);
        const SortUnitType &u2 = ul2.at(i);

        if(u1.ch.isNull() && !u2.ch.isNull())
            return true;
        if(!u1.ch.isNull() && u2.ch.isNull())
            return false;
        if(!u1.ch.isNull() && !u2.ch.isNull())
        {
            if(u1.ch < u2.ch)
                return true;
            if(u1.ch > u2.ch)
                return false;
        }
        if(u1.ch.isNull() && u2.ch.isNull())
        {
            if(u1.num < u2.num)
                return true;
            if(u1.num > u2.num)
                return false;
        }
    }

    return ul1.length() < ul2.length();
}

bool fileListDeSort(const QFileInfo &f1, const QFileInfo &f2)
{
    return !fileListSort(f1, f2);
}


TelegramCache::TelegramCache(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramCachePrivate;
    p->settings = 0;
    p->pts = 0;
    p->updating = false;
}

void TelegramCache::setPath(const QString &path)
{
    if(p->path == path)
        return;

    if(p->settings)
        delete p->settings;

    p->path = path;
    p->pts = 0;

    if(!p->path.isEmpty())
    {
        QDir().mkpath(p->path);
        p->settings = new QSettings(p->path + "/conf", QSettings::IniFormat, this);
        p->pts = p->settings->value("pts", 0).toInt();
    }

    Q_EMIT ptsChanged();
    Q_EMIT pathChanged();
}

QString TelegramCache::path() const
{
    return p->path;
}

void TelegramCache::setEncryptMethod(const QJSValue &method)
{
    if(p->encryptMethod.isNull() && method.isNull())
        return;

    p->encryptMethod = method;
    Q_EMIT encryptMethodChanged();
}

QJSValue TelegramCache::encryptMethod() const
{
    return p->encryptMethod;
}

void TelegramCache::setDecryptMethod(const QJSValue &method)
{
    if(p->decryptMethod.isNull() && method.isNull())
        return;

    p->decryptMethod = method;
    Q_EMIT decryptMethodChanged();
}

QJSValue TelegramCache::decryptMethod() const
{
    return p->decryptMethod;
}

void TelegramCache::setEngine(TelegramEngine *engine)
{
    if(p->engine == engine)
        return;

    if(p->engine)
    {
        disconnect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramCache::refresh);
        disconnect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramCache::refresh);
    }

    p->engine = engine;
    if(p->engine)
    {
        connect(p->engine.data(), &TelegramEngine::telegramChanged, this, &TelegramCache::refresh);
        connect(p->engine.data(), &TelegramEngine::stateChanged, this, &TelegramCache::refresh);
    }

    refresh();
    Q_EMIT engineChanged();
}

TelegramEngine *TelegramCache::engine() const
{
    return p->engine;
}

void TelegramCache::setPts(qint32 pts)
{
    if(!p->telegram)
        return;
    if(p->pts == pts)
        return;

    p->pts = pts;
    if(p->settings)
        p->settings->setValue("pts", pts);

    Q_EMIT ptsChanged();
}

qint32 TelegramCache::pts() const
{
    return p->pts;
}

bool TelegramCache::updating() const
{
    return p->updating;
}

void TelegramCache::setUpdating(bool updating)
{
    if(p->updating == updating)
        return;

    p->updating = updating;
    Q_EMIT updatingChanged();
}

void TelegramCache::insert(const User &user)
{
    const QString folderPath = p->path + "/users";
    DECLARE_INSERT(user, folderPath);
}

void TelegramCache::insert(const Chat &chat)
{
    const QString folderPath = p->path + "/chats";
    DECLARE_INSERT(chat, folderPath);
}

void TelegramCache::insert(const ChatFull &chat)
{
    const QString folderPath = p->path + "/chatfulls";
    DECLARE_INSERT(chat, folderPath);
}

void TelegramCache::insert(const UserFull &user)
{
    const QString folderPath = p->path + "/userfulls";
    DECLARE_INSERT(user, folderPath);
}

void TelegramCache::insert(const QList<Dialog> &dialogs)
{
    const QString filePath = p->path + "/dialogs";

    QVariantList list;
    Q_FOREACH(const Dialog &dlg, dialogs)
        list << dlg.toMap();

    writeList(filePath, list);
}

void TelegramCache::insert(const Dialog &dialog)
{
    Q_UNUSED(dialog)
}

void TelegramCache::insert(const QList<Document> &recentStickers)
{
    const QString filePath = p->path + "/recent-stickers";

    QVariantList list;
    Q_FOREACH(const Document &doc, recentStickers)
        list << doc.toMap();

    writeList(filePath, list);
}

QList<Document> TelegramCache::readRecentStickers() const
{
    QList<Document> result;
    const QString filePath = p->path + "/recent-stickers";
    const QList<QVariant> &list = readList(filePath);
    Q_FOREACH(const QVariant &var, list)
    {
        const Document &doc = Document::fromMap( var.toMap() );
        result << doc;
    }
    return result;
}

void TelegramCache::insertMe(const UserFull &user)
{
    QDir().mkpath(p->path);
    writeMap(p->path + "/me", user.toMap());
}

UserFull TelegramCache::readMe() const
{
    const QMap<QString, QVariant> &map = readMap(p->path + "/me");
    return UserFull::fromMap(map);
}

void TelegramCache::insert(const Message &msg)
{
    const QString folderPath = getMessageFolder(TelegramTools::messagePeer(msg));
    const QString filePath = folderPath + "/" + QString::number(msg.id());
    writeMap(filePath, msg.toMap());
}

MessagesMessages TelegramCache::readMessages(const InputPeer &peer, int offset, int limit) const
{
    return readMessages(TelegramTools::inputPeerPeer(peer), offset, limit);
}

MessagesMessages TelegramCache::readMessages(const Peer &peer, int offset, int limit) const
{
    MessagesMessages result(MessagesMessages::typeMessagesMessages);

    const QString folderPath = getMessageFolder(peer);
    QStringList files = QDir(folderPath).entryList(QDir::Files);
    qStableSort(files.begin(), files.end(), fileListDeSort);

    files = files.mid(offset, limit);

    QHash<QByteArray, Chat> chats;
    QHash<QByteArray, User> users;

    QList<Message> messages;
    Q_FOREACH(const QString &f, files)
    {
        const QString path = folderPath + "/" + f;
        const QMap<QString, QVariant> &map = readMap(path);
        if(map.isEmpty())
            continue;

        const Message &msg = Message::fromMap(map);
        const Peer &toPeer = msg.toId();
        const QByteArray &toKey = TelegramTools::identifier(toPeer);
        switch(static_cast<int>(toPeer.classType()))
        {
        case Peer::typePeerChannel:
        case Peer::typePeerChat:
            if(!chats.contains(toKey))
                chats[toKey] = readChat(toPeer);
            break;
        case Peer::typePeerUser:
            if(!users.contains(toKey))
                users[toKey] = readUser(toPeer);
            break;
        }

        if(msg.fromId())
        {
            Peer fromPeer(Peer::typePeerUser);
            fromPeer.setUserId(msg.fromId());
            const QByteArray &fromKey = TelegramTools::identifier(fromPeer);
            switch(static_cast<int>(fromPeer.classType()))
            {
            case Peer::typePeerChannel:
            case Peer::typePeerChat:
                if(!chats.contains(fromKey))
                    chats[fromKey] = readChat(fromPeer);
                break;
            case Peer::typePeerUser:
                if(!users.contains(fromKey))
                    users[fromKey] = readUser(fromPeer);
                break;
            }
        }

        messages << msg;
    }

    result.setMessages(messages);
    result.setChats(chats.values());
    result.setUsers(users.values());
    result.setCount(messages.count());

    return result;
}

void TelegramCache::deleteMessage(const InputPeer &peer, int msgId)
{
    return deleteMessage(TelegramTools::inputPeerPeer(peer), msgId);
}

void TelegramCache::deleteMessage(const Peer &peer, int msgId)
{
    const QString &messageFolder = getMessageFolder(peer);
    const QString messageFile = messageFolder + "/" + QString::number(msgId);
    zeroFile(messageFile);
}

void TelegramCache::deleteMessages(const InputPeer &peer)
{
    return deleteMessages(TelegramTools::inputPeerPeer(peer));
}

void TelegramCache::deleteMessages(const Peer &peer)
{
    const QString &messageFolder = getMessageFolder(peer);
    const QStringList &files = QDir(messageFolder).entryList(QDir::Files);
    Q_FOREACH(const QString &f, files)
        zeroFile(messageFolder + "/" + f);
}

Chat TelegramCache::readChat(const InputPeer &peer) const
{
    return readChat(TelegramTools::inputPeerPeer(peer));
}

Chat TelegramCache::readChat(const Peer &peer) const
{
    const QString folderPath = p->path + "/chats";
    const QString filePath = folderPath + "/" + QCryptographicHash::hash(TelegramTools::identifier(peer),
                                                                         QCryptographicHash::Md5).toHex();
    const QMap<QString, QVariant> &map = readMap(filePath);
    return Chat::fromMap(map);
}

ChatFull TelegramCache::readChatFull(const InputPeer &peer) const
{
    return readChatFull(TelegramTools::inputPeerPeer(peer));
}

ChatFull TelegramCache::readChatFull(const Peer &peer) const
{
    const QString folderPath = p->path + "/chatfulls";
    const QString filePath = folderPath + "/" + QCryptographicHash::hash(TelegramTools::identifier(peer),
                                                                         QCryptographicHash::Md5).toHex();
    const QMap<QString, QVariant> &map = readMap(filePath);
    return ChatFull::fromMap(map);
}

User TelegramCache::readUser(const InputPeer &peer) const
{
    return readUser(TelegramTools::inputPeerPeer(peer));
}

User TelegramCache::readUser(const Peer &peer) const
{
    const QString folderPath = p->path + "/users";
    const QString filePath = folderPath + "/" + QCryptographicHash::hash(TelegramTools::identifier(peer),
                                                                         QCryptographicHash::Md5).toHex();;
    const QMap<QString, QVariant> &map = readMap(filePath);
    return User::fromMap(map);
}

UserFull TelegramCache::readUserFull(const InputPeer &peer) const
{
    return readUserFull(TelegramTools::inputPeerPeer(peer));
}

UserFull TelegramCache::readUserFull(const Peer &peer) const
{
    const QString folderPath = p->path + "/userfulls";
    const QString filePath = folderPath + "/" + QCryptographicHash::hash(TelegramTools::identifier(peer),
                                                                         QCryptographicHash::Md5).toHex();;
    const QMap<QString, QVariant> &map = readMap(filePath);
    return UserFull::fromMap(map);
}

MessagesDialogs TelegramCache::readDialogs() const
{
    MessagesDialogs result(MessagesDialogs::typeMessagesDialogs);

    const QString filePath = p->path + "/dialogs";
    const QList<QVariant> &list = readList(filePath);
    QList<Dialog> dialogs;

    QHash<QByteArray, Chat> chats;
    QHash<QByteArray, User> users;
    QHash<QByteArray, Message> messages;

    Q_FOREACH(const QVariant &var, list)
    {
        const Dialog &dialog = Dialog::fromMap( var.toMap() );

        const Peer &peer = dialog.peer();
        const QByteArray &key = TelegramTools::identifier(peer);
        switch(static_cast<int>(peer.classType()))
        {
        case Peer::typePeerChannel:
        case Peer::typePeerChat:
            if(!chats.contains(key))
                chats[key] = readChat(peer);
            break;
        case Peer::typePeerUser:
            if(!users.contains(key))
                users[key] = readUser(peer);
            break;
        }

        if(dialog.topMessage())
        {
            QByteArray topMsgKey = TelegramTools::identifier(dialog.peer(), dialog.topMessage());
            const QString &messageFolder = getMessageFolder(peer);
            const QString messageFile = messageFolder + "/" + QString::number(dialog.topMessage());
            messages[topMsgKey] = Message::fromMap( readMap(messageFile) );
        }

        dialogs << dialog;
    }

    result.setDialogs(dialogs);
    result.setChats(chats.values());
    result.setUsers(users.values());
    result.setMessages(messages.values());
    result.setCount(dialogs.count());

    return result;
}

QStringList TelegramCache::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(path);
}

void TelegramCache::refresh()
{
    if(!p->engine)
        return;

    Telegram *tg = p->engine->telegram();
    if(tg == p->telegram)
        return;

    p->telegram = tg;
    if(!p->telegram)
        return;

    connect(p->telegram.data(), &Telegram::messagesGetHistoryAnswer, this, &TelegramCache::messagesReaded);
    connect(p->telegram.data(), &Telegram::messagesGetDialogsAnswer, this, &TelegramCache::dialogsReaded);
    connect(p->telegram.data(), &Telegram::updates, this, &TelegramCache::onUpdates);

    connect(p->telegram.data(), &Telegram::channelsCreateChannelAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsEditAdminAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsEditTitleAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsEditPhotoAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsJoinChannelAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsLeaveChannelAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsInviteToChannelAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsKickFromChannelAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsDeleteChannelAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsToggleInvitesAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsToggleSignaturesAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::channelsUpdatePinnedMessageAnswer, this, &TelegramCache::updates);

    connect(p->telegram.data(), &Telegram::messagesSendMessageAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesSendMediaAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesForwardMessagesAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesEditChatTitleAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesEditChatPhotoAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesAddChatUserAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesDeleteChatUserAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesCreateChatAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesForwardMessageAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesSendBroadcastAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesImportChatInviteAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesStartBotAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesToggleChatAdminsAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesMigrateChatAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesSendInlineBotResultAnswer, this, &TelegramCache::updates);
    connect(p->telegram.data(), &Telegram::messagesEditMessageAnswer, this, &TelegramCache::updates);

    const qint32 currentPts = pts();
    connect(p->telegram.data(), &Telegram::authLoggedIn, this, [this, currentPts](){
        loadFromPts(currentPts);
    });
}

QString TelegramCache::getMessageFolder(const Peer &peer) const
{
    const QByteArray &peerKey = TelegramTools::identifier(peer);
    const QString folderPath = p->path + "/messages/" + QCryptographicHash::hash(peerKey,QCryptographicHash::Md5).toHex();
    QDir().mkpath(folderPath);
    return folderPath;
}

QMap<QString, QVariant> TelegramCache::readMap(const QString &path) const
{
    QMap<QString, QVariant> result;
    QByteArray data = read(path);
    if(data.isEmpty())
        return result;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> result;
    return result;
}

bool TelegramCache::writeMap(const QString &path, const QMap<QString, QVariant> &map) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << map;

    if(p->encryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(this);
        if(engine) {
            QJSValue res = p->encryptMethod.call(QList<QJSValue>()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    return write(path, data);
}

QList<QVariant> TelegramCache::readList(const QString &path) const
{
    QList<QVariant> result;
    QByteArray data = read(path);
    if(data.isEmpty())
        return result;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> result;
    return result;
}

bool TelegramCache::writeList(const QString &path, const QList<QVariant> &list) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << list;

    if(p->encryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(this);
        if(engine) {
            QJSValue res = p->encryptMethod.call(QList<QJSValue>()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    return write(path, data);
}

QByteArray TelegramCache::read(const QString &path) const
{
    QByteArray data;
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
        return data;

    data = file.readAll();
    file.close();

    if(p->decryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(parent());
        if(engine) {
            QJSValue res = p->decryptMethod.call(QList<QJSValue>()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    return data;
}

bool TelegramCache::write(const QString &path, QByteArray data) const
{
    zeroFile(path);

    QFile file(path);
    if(!file.open(QFile::WriteOnly))
        return false;

    if(p->encryptMethod.isCallable()) {
        QQmlEngine *engine = qmlEngine(parent());
        if(engine) {
            QJSValue res = p->encryptMethod.call(QList<QJSValue>()<<engine->toScriptValue<QByteArray>(data));
            data = engine->fromScriptValue<QByteArray>(res);
        }
    }

    file.write(data);
    file.close();
    return true;
}

bool TelegramCache::zeroFile(const QString &path) const
{
    QFile file(path);
    if(!file.exists())
        return false;
    if(!file.open(QFile::ReadWrite))
        return QFile::remove(path);

    QByteArray data;
    for(int i=0; i<file.size(); i++)
        data += (char)0;

    file.seek(0);
    file.write(data);
    file.flush();
    file.close();
    file.remove();
    return true;
}

void TelegramCache::messagesReaded(qint64 msgId, const MessagesMessages &messages)
{
    Q_UNUSED(msgId)

    Q_FOREACH(const Message &msg, messages.messages())
        insert(msg);
    Q_FOREACH(const Chat &chat, messages.chats())
        insert(chat);
    Q_FOREACH(const User &user, messages.users())
        insert(user);
}

void TelegramCache::dialogsReaded(qint64 msgId, const MessagesDialogs &dialogs)
{
    Q_UNUSED(msgId)

    insert(dialogs.dialogs());
    Q_FOREACH(const Message &msg, dialogs.messages())
        insert(msg);
    Q_FOREACH(const Chat &chat, dialogs.chats())
        insert(chat);
    Q_FOREACH(const User &user, dialogs.users())
        insert(user);
}

void TelegramCache::onUpdates(const UpdatesType &updates)
{
    TelegramTools::analizeUpdatesType(updates, p->engine, [this](const Update &update){
        insertUpdate(update);
    });
}

void TelegramCache::insertUpdate(const Update &update)
{
    if(!p->engine)
        return;

    if(update.pts() > pts())
        setPts(update.pts());


    Telegram *tg = p->engine->telegram();
    TelegramSharedDataManager *tsdm = p->engine->sharedData();
    if(!tg || !tsdm)
        return;

    const uint type = static_cast<int>(update.classType());
    switch(type)
    {
    case Update::typeUpdateNewChannelMessage:
    case Update::typeUpdateNewMessage:
    {
        const Message &msg = update.message();

        Peer fromPeer;
        if(msg.fromId())
        {
            fromPeer.setUserId(msg.fromId());
            fromPeer.setClassType(Peer::typePeerUser);
        }

        QByteArray fromPeerKey = TelegramTools::identifier(fromPeer);

        TelegramSharedPointer<ChatObject> chat = tsdm->getChat(fromPeerKey);
        TelegramSharedPointer<UserObject> user = tsdm->getUser(fromPeerKey);

        insert(msg);
        if(chat) insert(chat->core());
        if(user) insert(user->core());
    }
        break;
    case Update::typeUpdateMessageID:
        break;
    case Update::typeUpdateDeleteChannelMessages:
    case Update::typeUpdateDeleteMessages:
    {
        QList<qint32> messages = update.messages();
        QStringList dirs = QDir(p->path + "/messages").entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        Q_FOREACH(const QString &dir, dirs)
        {
            const QString &filePath = p->path + "/messages/" + dir;
            Q_FOREACH(qint32 msgId, messages)
                if(zeroFile(filePath + "/" + QString::number(msgId)))
                    messages.removeOne(msgId);
        }
    }
        break;
    case Update::typeUpdateUserTyping:
    case Update::typeUpdateChatUserTyping:
        break;
    case Update::typeUpdateChatParticipants:
        break;
    case Update::typeUpdateUserStatus:
        break;
    case Update::typeUpdateUserName:
    {
        Peer peer(Peer::typePeerUser);
        peer.setUserId(update.userId());

        User user = readUser(peer);
        if(user.classType() == User::typeUser)
        {
            user.setUsername(update.username());
            user.setFirstName(update.firstName());
            user.setLastName(update.lastName());
            insert(user);
        }
    }
        break;
    case Update::typeUpdateUserPhoto:
    {
        Peer peer(Peer::typePeerUser);
        peer.setUserId(update.userId());

        User user = readUser(peer);
        if(user.classType() == User::typeUser)
        {
            user.setPhoto(update.photo());
            insert(user);
        }
    }
        break;
    case Update::typeUpdateContactRegistered:
        break;
    case Update::typeUpdateContactLink:
        break;
    case Update::typeUpdateNewAuthorization:
        break;
    case Update::typeUpdateNewEncryptedMessage:
        break;
    case Update::typeUpdateEncryptedChatTyping:
        break;
    case Update::typeUpdateEncryption:
        break;
    case Update::typeUpdateEncryptedMessagesRead:
        break;
    case Update::typeUpdateChatParticipantAdd:
        break;
    case Update::typeUpdateChatParticipantDelete:
        break;
    case Update::typeUpdateDcOptions:
        break;
    case Update::typeUpdateUserBlocked:
        break;
    case Update::typeUpdateNotifySettings:
        break;
    case Update::typeUpdateServiceNotification:
        break;
    case Update::typeUpdatePrivacy:
        break;
    case Update::typeUpdateUserPhone:
    {
        Peer peer(Peer::typePeerUser);
        peer.setUserId(update.userId());

        User user = readUser(peer);
        if(user.classType() == User::typeUser)
        {
            user.setPhone(update.phone());
            insert(user);
        }
    }
        break;
    case Update::typeUpdateReadHistoryInbox:
        break;
    case Update::typeUpdateReadHistoryOutbox:
        break;
    case Update::typeUpdateWebPage:
        break;
    case Update::typeUpdateReadMessagesContents:
        break;
    case Update::typeUpdateChannelTooLong:
        break;
    case Update::typeUpdateChannel:
        break;
    case Update::typeUpdateReadChannelInbox:
        break;
    case Update::typeUpdateChannelMessageViews:
        break;
    case Update::typeUpdateChatAdmins:
        break;
    case Update::typeUpdateChatParticipantAdmin:
        break;
    case Update::typeUpdateNewStickerSet:
        break;
    case Update::typeUpdateStickerSetsOrder:
        break;
    case Update::typeUpdateStickerSets:
        break;
    case Update::typeUpdateSavedGifs:
        break;
    case Update::typeUpdateBotInlineQuery:
        break;
    case Update::typeUpdateBotInlineSend:
        break;
    case Update::typeUpdateEditMessage:
    case Update::typeUpdateEditChannelMessage:
        break;
    case Update::typeUpdateChannelPinnedMessage:
        break;
    case Update::typeUpdateBotCallbackQuery:
        break;
    case Update::typeUpdateInlineBotCallbackQuery:
        break;
    }
}

void TelegramCache::updates(qint64 msgId, const UpdatesType &result)
{
    Q_UNUSED(msgId)
    onUpdates(result);
}

void TelegramCache::updatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState)
{

}

void TelegramCache::updateSecretChatMessage(const SecretChatMessage &secretChatMessage, qint32 qts)
{

}

void TelegramCache::loadFromPts(qint32 pts)
{
    if(!p->telegram)
    {
        setUpdating(false);
        return;
    }

    setUpdating(true);
    QPointer<TelegramCache> dis = this;
    p->telegram->updatesGetDifference(pts, QDateTime::currentDateTime().toTime_t(), 0, [this, pts, dis](TG_UPDATES_GET_DIFFERENCE_CALLBACK){
        Q_UNUSED(msgId)
        if(!dis) return;
        if(!error.null) {
            setError(error.errorText, error.errorCode);
            setUpdating(false);
            return;
        }

        qint32 maxPts = result.state().pts();

        Q_FOREACH(const Chat &chat, result.chats())
            insert(chat);
        Q_FOREACH(const User &user, result.users())
            insert(user);
        Q_FOREACH(const Message &msg, result.newMessages())
            insert(msg);
        Q_FOREACH(const Update &upd, result.otherUpdates())
        {
            insertUpdate(upd);
            if(upd.pts() > maxPts)
                maxPts = upd.pts();
        }

        if(maxPts > pts)
            loadFromPts(maxPts);
        else
        {
            p->telegram->updatesGetState([this, dis](TG_UPDATES_GET_STATE_CALLBACK){
                Q_UNUSED(msgId)
                if(!dis) return;
                if(!error.null) {
                    setError(error.errorText, error.errorCode);
                    setUpdating(false);
                    return;
                }
                setPts(result.pts());
                setUpdating(false);
            });
        }
    });
}

TelegramCache::~TelegramCache()
{
    delete p;
}

