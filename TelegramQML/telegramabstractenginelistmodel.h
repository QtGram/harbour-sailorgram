#ifndef TELEGRAMABSTRACTENGINELISTMODEL_H
#define TELEGRAMABSTRACTENGINELISTMODEL_H

#include "telegramengine.h"
#include "telegramqml_global.h"
#include "telegramabstractlistmodel.h"
#include "telegramtools.h"

#include <QStringList>
#include <QPointer>

#include <functional>
#include <telegram.h>

class UpdatesType;
class Peer;
class User;
class Update;
class Chat;
class UpdatesState;
class UpdatesDifference;
class UpdatesChannelDifference;
class MessageFwdHeader;
class Message;
class SecretChatMessage;
class TELEGRAMQMLSHARED_EXPORT TelegramAbstractEngineListModel : public TelegramAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)

public:
    TelegramAbstractEngineListModel(QObject *parent = 0);
    virtual ~TelegramAbstractEngineListModel();

    typedef std::function<void ()> Callback;

    TelegramEngine *engine() const { return mEngine; }
    void setEngine(TelegramEngine *engine);

    static QStringList requiredProperties() {
        return QStringList();
    }

    Telegram *telegram() const {
        return mTelegram;
    }

public Q_SLOTS:
    void startTimer(int ms, Callback callback);

Q_SIGNALS:
    void engineChanged();

protected:
    virtual void refresh() = 0;
    virtual void clean() = 0;
    QPointer<TelegramEngine> mEngine;

    virtual void onUpdatesGetDifferenceAnswer(qint64 id, const QList<Message> &messages, const QList<SecretChatMessage> &secretChatMessages, const QList<Update> &otherUpdates, const QList<Chat> &chats, const QList<User> &users, const UpdatesState &state, bool isIntermediateState);
    virtual void onUpdatesGetStateAnswer(qint64 msgId, const UpdatesState &result);
    virtual void onUpdatesGetChannelDifferenceAnswer(qint64 msgId, const UpdatesChannelDifference &result);
    virtual void onUpdateSecretChatMessage(const SecretChatMessage &secretChatMessage, qint32 qts);
    virtual void onUpdates(const UpdatesType &update);

    virtual void timerEvent(QTimerEvent *e);

    virtual void connectTelegram();

private:
    QString mErrorText;
    qint32 mErrorCode;
    QPointer<Telegram> mTelegram;
    QHash<qint32, Callback> mTimers;
};

#endif // TELEGRAMABSTRACTENGINELISTMODEL_H
