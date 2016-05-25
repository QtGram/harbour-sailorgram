#ifndef TELEGRAMPEERDETAILS_H
#define TELEGRAMPEERDETAILS_H

#include "tqobject.h"

#include <QDateTime>
#include <QJSValue>

class UpdatesType;
class Peer;
class User;
class Update;
class Chat;
class UserFullObject;
class ChatFullObject;
class TelegramEngine;
class InputPeerObject;
class TelegramPeerDetailsPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramPeerDetails : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(InputPeerObject* peer READ peer WRITE setPeer NOTIFY peerChanged)
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)

    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isUser READ isUser NOTIFY isUserChanged)
    Q_PROPERTY(bool isChannel READ isChannel NOTIFY isChannelChanged)

    Q_PROPERTY(QString key READ key NOTIFY keyChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(int participantsCount READ participantsCount NOTIFY participantsCountChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)

    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(bool blocked READ blocked WRITE setBlocked NOTIFY blockedChanged)
    Q_PROPERTY(bool joined READ joined WRITE setJoined NOTIFY joinedChanged)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)

    Q_PROPERTY(UserFullObject* userFull READ userFull NOTIFY userFullChanged)
    Q_PROPERTY(ChatFullObject* chatFull READ chatFull NOTIFY chatFullChanged)
    Q_PROPERTY(QVariantList chatUsers READ chatUsers NOTIFY chatUsersChanged)

public:
    TelegramPeerDetails(QObject *parent = 0);
    ~TelegramPeerDetails();

    void setPeer(InputPeerObject *peer);
    InputPeerObject *peer() const;

    QString username() const;
    void setUsername(const QString &username);

    bool isChat() const;
    bool isUser() const;
    bool isChannel() const;

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

    QString key() const;
    QString displayName() const;
    int participantsCount() const;
    QString statusText() const;
    QString phoneNumber() const;

    void setMute(bool mute);
    bool mute() const;

    void setBlocked(bool blocked);
    bool blocked() const;

    void setJoined(bool joined);
    bool joined() const;

    bool refreshing() const;

    UserFullObject *userFull() const;
    ChatFullObject *chatFull() const;
    QVariantList chatUsers() const;

    static QStringList requiredProperties();

public Q_SLOTS:

Q_SIGNALS:
    void peerChanged();
    void engineChanged();
    void dateConvertorMethodChanged();
    void usernameChanged();

    void isChatChanged();
    void isUserChanged();
    void isChannelChanged();

    void keyChanged();
    void displayNameChanged();
    void participantsCountChanged();
    void statusTextChanged();
    void phoneNumberChanged();

    void muteChanged();
    void blockedChanged();
    void joinedChanged();
    void refreshingChanged();

    void userFullChanged();
    void chatFullChanged();
    void chatUsersChanged();

protected:
    void refresh();
    void fetchUsername();
    void initTelegram();

    void connectChatSignals(class ChatObject *chat, bool dc = false);
    void connectUserSignals(class UserObject *user, bool dc = false);
    void connectDialogSignals(class DialogObject *dialog, bool dc = false);

    void insertChatFull(const class MessagesChatFull &result);
    void setRefreshing(bool refreshing);

    QString convertDate(const QDateTime &td) const;

    virtual void onUpdates(const UpdatesType &udts);
    void insertUpdate(const Update &update);

private:
    TelegramPeerDetailsPrivate *p;
};

#endif // TELEGRAMPEERDETAILS_H
