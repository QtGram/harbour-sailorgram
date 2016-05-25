#ifndef TELEGRAMENGINE_H
#define TELEGRAMENGINE_H

#include <QObject>

#include "telegramqml_macros.h"
#include "tqobject.h"

class TelegramCache;
class UserFullObject;
class Telegram;
class TelegramAuthStore;
class TelegramHost;
class TelegramApp;
class TelegramProfileManagerModel;
class TelegramSharedDataManager;
class TelegramEnginePrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramEngine : public TqObject
{
    Q_OBJECT
    Q_ENUMS(AuthState)
    Q_ENUMS(LogLevel)

    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString configDirectory READ configDirectory WRITE setConfigDirectory NOTIFY configDirectoryChanged)
    Q_PROPERTY(TelegramApp* app READ app WRITE setApp NOTIFY appChanged)
    Q_PROPERTY(TelegramHost* host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(TelegramCache* cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(TelegramAuthStore* authStore READ authStore WRITE setAuthStore NOTIFY authStoreChanged)
    Q_PROPERTY(TelegramProfileManagerModel* profileManager READ profileManager WRITE setProfileManager NOTIFY profileManagerChanged)
    Q_PROPERTY(UserFullObject* our READ our NOTIFY ourChanged)
    Q_PROPERTY(qint32 timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(qint32 state READ state NOTIFY stateChanged)
    Q_PROPERTY(qint32 logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(QString tempPath READ tempPath WRITE setTempPath NOTIFY tempPathChanged)

public:
    enum AuthState {
        AuthUnknown,
        AuthInitializing,
        AuthNeeded,
        AuthFetchingOurDetails,
        AuthLoggedIn
    };

    enum LogLevel {
        LogLevelClean,
        LogLevelUseful,
        LogLevelFull
    };

    TelegramEngine(QObject *parent = 0);
    ~TelegramEngine();

    void setPhoneNumber(const QString &phoneNumber);
    QString phoneNumber() const;

    void setConfigDirectory(const QString &configDirectory);
    QString configDirectory() const;

    void setApp(TelegramApp *app);
    TelegramApp *app() const;

    void setHost(TelegramHost *host);
    TelegramHost *host() const;

    void setCache(TelegramCache *cache);
    TelegramCache *cache() const;

    void setAuthStore(TelegramAuthStore *authStore);
    TelegramAuthStore *authStore() const;

    void setProfileManager(TelegramProfileManagerModel *manager);
    TelegramProfileManagerModel *profileManager() const;

    void setTimeout(qint32 ms);
    qint32 timeout() const;

    void setLogLevel(qint32 level);
    qint32 logLevel() const;

    void setTempPath(const QString &tempPath);
    QString tempPath() const;

    virtual bool isValid() const;
    Telegram *telegram() const;
    TelegramSharedDataManager *sharedData() const;
    UserFullObject *our() const;

    qint32 state() const;
    static QStringList requiredProperties();

public Q_SLOTS:
    void logout();

Q_SIGNALS:
    void authLoggedIn();
    void authNeeded();
    void authLoggedOut();

    void phoneNumberChanged();
    void configDirectoryChanged();
    void appChanged();
    void hostChanged();
    void cacheChanged();
    void authStoreChanged();
    void profileManagerChanged();
    void timeoutChanged();
    void stateChanged();
    void telegramChanged();
    void logLevelChanged();
    void tempPathChanged();
    void ourChanged();

protected:
    void tryInit();
    void clean();
    void itemsChanged_slt();
    void cleanTemp();

    void setState(qint32 state);

private:
    TelegramEnginePrivate *p;
};

#endif // TELEGRAMENGINE_H
