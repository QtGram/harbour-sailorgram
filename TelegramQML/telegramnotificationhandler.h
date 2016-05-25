#ifndef TELEGRAMNOTIFICATIONHANDLER_H
#define TELEGRAMNOTIFICATIONHANDLER_H

#include "tqobject.h"
#include "telegramqml_global.h"

class UpdatesType;
class Peer;
class User;
class Update;
class Chat;
class MessageFwdHeader;
class TelegramEngine;
class TelegramNotificationHandlerPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramNotificationHandler : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int globalUnreadCount READ globalUnreadCount NOTIFY globalUnreadCountChanged)

public:
    TelegramNotificationHandler(QObject *parent = 0);
    ~TelegramNotificationHandler();

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    int unreadCount() const;
    int globalUnreadCount() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void engineChanged();
    void unreadCountChanged();
    void globalUnreadCountChanged();
    void newMessage(const QString &title, const QString &message, const QString &peerKey);

public Q_SLOTS:

protected:
    virtual void onUpdates(const UpdatesType &update);
    virtual void refreshUnreads();

    void insertUpdate(const Update &update);
    void refresh();

private:
    TelegramNotificationHandlerPrivate *p;
};

#endif // TELEGRAMNOTIFICATIONHANDLER_H
