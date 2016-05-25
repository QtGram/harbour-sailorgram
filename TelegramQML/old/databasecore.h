#ifndef DATABASECORE_H
#define DATABASECORE_H

#include "telegramqml_global.h"
#include "databaseabstractencryptor.h"

#include <QObject>
#include <telegram/types/types.h>

class TELEGRAMQMLSHARED_EXPORT DbChat { public: DbChat(): chat(Chat::typeChatEmpty){} Chat chat; };
class TELEGRAMQMLSHARED_EXPORT DbUser { public: DbUser(): user(User::typeUserEmpty){} User user; };
class TELEGRAMQMLSHARED_EXPORT DbDialog { public: DbDialog(): dialog(){} Dialog dialog; };
class TELEGRAMQMLSHARED_EXPORT DbContact { public: DbContact(): contact(){} Contact contact; };
class TELEGRAMQMLSHARED_EXPORT DbMessage { public: DbMessage(): message(){} Message message; };
class TELEGRAMQMLSHARED_EXPORT DbPeer { public: DbPeer(): peer(Peer::typePeerUser){} Peer peer; };

class TELEGRAMQMLSHARED_EXPORT DatabaseNormalEncrypter: public DatabaseAbstractEncryptor
{
public:
    QVariant encrypt(const QString &text, bool) { return text; }
    QString decrypt(const QVariant &data) { return data.toString(); }
};

class DatabaseCorePrivate;
class TELEGRAMQMLSHARED_EXPORT DatabaseCore : public QObject
{
    Q_OBJECT
public:
    DatabaseCore(const QString &path, const QString &configPath, const QString &phoneNumber, QObject *parent = 0);
    ~DatabaseCore();

public Q_SLOTS:
    void setEncrypter(DatabaseAbstractEncryptor *encrypter);
    DatabaseAbstractEncryptor *encrypter() const;

    void reconnect();
    void disconnect();

    void insertUser(const DbUser &user);
    void insertChat(const DbChat &chat);
    void insertDialog(const DbDialog &dialog, bool encrypted);
    void insertContact(const DbContact &contact);
    void insertMessage(const DbMessage &message, bool encrypted);
    void insertMediaEncryptedKeys(qint64 mediaId, const QByteArray &key, const QByteArray &iv);

    void updateUnreadCount(qint64 chatId, int unreadCount);

    void readFullDialogs();
    void readMessages(const DbPeer &peer, int offset, int limit);
    void markMessagesAsRead(const QList<qint32>& messages);
    void markMessagesAsReadFromMaxDate(qint32 chatId, qint32 maxDate);

    void setValue(const QString &key, const QString &value);
    QString value(const QString &key) const;

    void deleteMessage(qint64 msgId);
    void deleteDialog(qint64 dlgId);
    void deleteHistory(qint64 dlgId);

    void blockUser(qint64 userId);
    void unblockUser(qint64 userId);

Q_SIGNALS:
    void userFounded(const DbUser &user);
    void chatFounded(const DbChat &chat);
    void dialogFounded(const DbDialog &dialog, bool encrypted);
    void contactFounded(const DbContact &contact);
    void messageFounded(const DbMessage &message);
    void mediaKeyFounded(qint64 mediaId, const QByteArray &key, const QByteArray &iv);
    void valueChanged(const QString &value);

private:
    void readDialogs();
    void readUsers();
    void readChats();
    void readContacts();

    void init_buffer();
    void update_db();
    void update_moveFiles();
    QHash<qint64, QStringList> userFiles();
    QHash<qint64, QStringList> userFilesOf(const QString &mediaColumn);
    QHash<qint64, QStringList> userPhotos();
    QHash<qint64, QStringList> userProfilePhotosOf(const QString &table);

    QList<qint32> stringToUsers(const QString &str);
    QString usersToString( const QList<qint32> &users );

    void insertAudio(const Audio &audio);
    void insertVideo(const Video &video);
    void insertDocument(const Document &document);
    void insertGeo(int id, const GeoPoint &geo);
    void insertPhoto(const Photo &photo);
    void insertPhotoSize(qint64 pid, const QList<PhotoSize> &sizes);

    Audio readAudio(qint64 id);
    Video readVideo(qint64 id);
    Document readDocument(qint64 id);
    GeoPoint readGeo(qint64 id);
    Photo readPhoto(qint64 id);
    QPair<QByteArray, QByteArray> readMediaKey(qint64 mediaId);
    QList<PhotoSize> readPhotoSize(qint64 pid);

    void begin();
    void commit();

protected:
    void timerEvent(QTimerEvent *e);

private:
    DatabaseCorePrivate *p;
};

Q_DECLARE_METATYPE(DbUser)
Q_DECLARE_METATYPE(DbChat)
Q_DECLARE_METATYPE(DbDialog)
Q_DECLARE_METATYPE(DbContact)
Q_DECLARE_METATYPE(DbMessage)
Q_DECLARE_METATYPE(DbPeer)

#endif // DATABASECORE_H
