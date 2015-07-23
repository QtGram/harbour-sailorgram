#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

#include "telegramqml_global.h"

class Peer;
class Message;
class User;
class Dialog;
class Chat;
class DbUser;
class DbDialog;
class DbMessage;
class DbChat;
class DatabasePrivate;
class TELEGRAMQMLSHARED_EXPORT Database : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY configPathChanged)

public:
    Database(QObject *parent = 0);
    ~Database();

    void setPhoneNumber(const QString &phoneNumber);
    QString phoneNumber() const;

    void setConfigPath(const QString &path);
    QString configPath() const;

public Q_SLOTS:
    void insertUser(const User &user);
    void insertChat(const Chat &chat);
    void insertDialog(const Dialog &dialog, bool encrypted);
    void insertMessage(const Message &message);
    void insertMediaEncryptedKeys(qint64 mediaId, const QByteArray &key, const QByteArray &iv);

    void readFullDialogs();
    void readMessages(const Peer &peer, int offset, int limit);
    void markMessagesAsRead(const QList<qint32>& messages);
    void markMessagesAsReadFromMaxDate(qint32 chatId, qint32 maxDate);

    void deleteMessage(qint64 msgId);
    void deleteDialog(qint64 dlgId);
    void deleteHistory(qint64 dlgId);

Q_SIGNALS:
    void userFounded(const User &user);
    void chatFounded(const Chat &chat);
    void dialogFounded(const Dialog &dialog, bool encrypted);
    void messageFounded(const Message &message);
    void mediaKeyFounded(qint64 mediaId, const QByteArray &key, const QByteArray &iv);
    void phoneNumberChanged();
    void configPathChanged();

private Q_SLOTS:
    void userFounded_slt(const DbUser &user);
    void chatFounded_slt(const DbChat &chat);
    void dialogFounded_slt(const DbDialog &dialog, bool encrypted);
    void messageFounded_slt(const DbMessage &message);

private:
    void refresh();

private:
    DatabasePrivate *p;
};

#endif // DATABASE_H
