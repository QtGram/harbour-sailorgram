#ifndef TELEGRAMCACHE_H
#define TELEGRAMCACHE_H

#include "tqobject.h"

#include <QJSValue>
#include <QObject>

#include <telegram/types/types.h>

class TelegramEngine;
class TelegramCachePrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramCache : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QJSValue encryptMethod READ encryptMethod WRITE setEncryptMethod NOTIFY encryptMethodChanged)
    Q_PROPERTY(QJSValue decryptMethod READ decryptMethod WRITE setDecryptMethod NOTIFY decryptMethodChanged)
    Q_PROPERTY(TelegramEngine* engine READ engine NOTIFY engineChanged)
    Q_PROPERTY(qint32 pts READ pts NOTIFY ptsChanged)
    Q_PROPERTY(bool updating READ updating NOTIFY updatingChanged)

public:
    TelegramCache(QObject *parent = 0);
    ~TelegramCache();

    void setPath(const QString &path);
    QString path() const;

    void setEncryptMethod(const QJSValue &method);
    QJSValue encryptMethod() const;

    void setDecryptMethod(const QJSValue &method);
    QJSValue decryptMethod() const;

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    void setPts(qint32 pts);
    qint32 pts() const;

    bool updating() const;
    void setUpdating(bool updating);

    void insert(const Message &message);
    void insert(const User &user);
    void insert(const Chat &chat);
    void insert(const ChatFull &chat);
    void insert(const UserFull &user);
    void insert(const QList<Dialog> &dialogs);
    void insert(const Dialog &dialog);
    void insert(const QList<Document> &recentStickers);

    void insertMe(const UserFull &user);
    UserFull readMe() const;

    MessagesMessages readMessages(const InputPeer &peer, int offset, int limit) const;
    MessagesMessages readMessages(const Peer &peer, int offset, int limit) const;

    void deleteMessage(const InputPeer &peer, int msgId);
    void deleteMessage(const Peer &peer, int msgId);

    void deleteMessages(const InputPeer &peer);
    void deleteMessages(const Peer &peer);

    Chat readChat(const InputPeer &peer) const;
    Chat readChat(const Peer &peer) const;

    ChatFull readChatFull(const InputPeer &peer) const;
    ChatFull readChatFull(const Peer &peer) const;

    User readUser(const InputPeer &peer) const;
    User readUser(const Peer &peer) const;

    UserFull readUserFull(const InputPeer &peer) const;
    UserFull readUserFull(const Peer &peer) const;

    QList<Document> readRecentStickers() const;

    MessagesDialogs readDialogs() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void pathChanged();
    void encryptMethodChanged();
    void decryptMethodChanged();
    void engineChanged();
    void ptsChanged();
    void updatingChanged();

protected:
    void refresh();
    QString getMessageFolder(const Peer &peer) const;

    QMap<QString, QVariant> readMap(const QString &path) const;
    bool writeMap(const QString &path, const QMap<QString, QVariant> &map) const;

    QList<QVariant> readList(const QString &path) const;
    bool writeList(const QString &path, const QList<QVariant> &list) const;

    QByteArray read(const QString &path) const;
    bool write(const QString &path, QByteArray data) const;

    bool zeroFile(const QString &path) const;

private:
    void messagesReaded(qint64 msgId, const MessagesMessages &messages);
    void dialogsReaded(qint64 msgId, const MessagesDialogs &dialogs);

    void onUpdates(const UpdatesType &update);
    void insertUpdate(const Update &update);

    void updates(qint64 msgId, const UpdatesType &result);
    void updatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<class SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    void updateSecretChatMessage(const class SecretChatMessage &secretChatMessage, qint32 qts);

    void loadFromPts(qint32 pts);

private:
    TelegramCachePrivate *p;
};

#endif // TELEGRAMCACHE_H
