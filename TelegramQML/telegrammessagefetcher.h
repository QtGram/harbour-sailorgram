#ifndef TELEGRAMMESSAGEFETCHER_H
#define TELEGRAMMESSAGEFETCHER_H

#include "tqobject.h"
#include "telegramengine.h"

class UserObject;
class MessageObject;
class InputPeerObject;
class TelegramMessageFetcherPrivate;
class TelegramMessageFetcher : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(qint32 messageId READ messageId WRITE setMessageId NOTIFY messageIdChanged)
    Q_PROPERTY(InputPeerObject* inputPeer READ inputPeer WRITE setInputPeer NOTIFY inputPeerChanged)
    Q_PROPERTY(MessageObject* result READ result NOTIFY resultChanged)
    Q_PROPERTY(UserObject* fromUser READ fromUser NOTIFY fromUserChanged)
    Q_PROPERTY(int mediaType READ mediaType NOTIFY mediaTypeChanged)


public:
    TelegramMessageFetcher(QObject *parent = 0);
    ~TelegramMessageFetcher();

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    void setInputPeer(InputPeerObject *inputPeer);
    InputPeerObject *inputPeer() const;

    void setMessageId(qint32 msgId);
    qint32 messageId() const;

    MessageObject *result() const;
    UserObject *fromUser() const;
    int mediaType() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void engineChanged();
    void inputPeerChanged();
    void messageIdChanged();
    void resultChanged();
    void fromUserChanged();
    void mediaTypeChanged();

public Q_SLOTS:

protected:
    void refresh();
    void clean();

private:
    TelegramMessageFetcherPrivate *p;
};

#endif // TELEGRAMMESSAGEFETCHER_H
