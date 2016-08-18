#ifndef SAILORGRAMDIALOGITEM_H
#define SAILORGRAMDIALOGITEM_H

#include <QObject>
#include <telegram/objects/dialogobject.h>
#include <telegram/objects/chatobject.h>
#include <telegram/objects/userobject.h>
#include <telegram/objects/messageobject.h>
#include <telegram/objects/messageactionobject.h>
#include <telegram/objects/inputpeerobject.h>
#include <telegramengine.h>
#include <telegramenums.h>
#include "../object/sailorgrammessageaction.h"
#include "../sailorgramenums.h"

class SailorgramDialogItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(InputPeerObject* peer READ peer NOTIFY peerChanged)
    Q_PROPERTY(ChatObject* chat READ chat NOTIFY chatChanged)
    Q_PROPERTY(UserObject* user READ user NOTIFY userChanged)
    Q_PROPERTY(UserObject* messageUser READ messageUser NOTIFY messageUserChanged)
    Q_PROPERTY(SailorgramMessageAction* messageAction READ messageAction NOTIFY messageActionChanged)
    Q_PROPERTY(QString messageText READ messageText NOTIFY messageTextChanged)
    Q_PROPERTY(QString messageDate READ messageDate NOTIFY messageDateChanged)
    Q_PROPERTY(QString typingUsers READ typingUsers NOTIFY typingUsersChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QByteArray peerKey READ peerKey CONSTANT FINAL)
    Q_PROPERTY(int unreadCount READ unreadCount WRITE setUnreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int messageType READ messageType NOTIFY messageTypeChanged)
    Q_PROPERTY(int secretChatState READ secretChatState NOTIFY secretChatStateChanged)
    Q_PROPERTY(bool isMute READ isMute WRITE setIsMute NOTIFY isMuteChanged)
    Q_PROPERTY(bool isBroadcast READ isBroadcast NOTIFY isBroadcastChanged)
    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isUser READ isUser NOTIFY isUserChanged)
    Q_PROPERTY(bool isSecretChat READ isSecretChat NOTIFY isSecretChatChanged)
    Q_PROPERTY(bool isActionMessage READ isActionMessage NOTIFY isActionMessageChanged)
    Q_PROPERTY(bool isMediaMessage READ isMediaMessage NOTIFY isMediaMessageChanged)
    Q_PROPERTY(bool isTextMessage READ isTextMessage NOTIFY isTextMessageChanged)
    Q_PROPERTY(bool isMessageOut READ isMessageOut NOTIFY isMessageOutChanged)
    Q_PROPERTY(bool isMessageUnread READ isMessageUnread NOTIFY isMessageUnreadChanged)

    public:
        explicit SailorgramDialogItem(QObject *parent = 0);
        SailorgramDialogItem(TelegramEngine* engine, DialogObject* dialog, QObject *parent = 0);
        InputPeerObject* peer() const;
        ChatObject* chat() const;
        UserObject* user() const;
        UserObject* messageUser() const;
        SailorgramMessageAction* messageAction() const;
        QString title() const;
        QString statusText() const;
        QString messageText() const;
        QString messageDate() const;
        QString typingUsers() const;
        QByteArray peerKey() const;
        int unreadCount() const;
        int messageType() const;
        int secretChatState() const;
        bool isMute() const;
        bool isBroadcast() const;
        bool isChat() const;
        bool isUser() const;
        bool isSecretChat() const;
        bool isActionMessage() const;
        bool isMediaMessage() const;
        bool isTextMessage() const;
        bool isMessageOut() const;
        bool isMessageUnread() const;
        void setChat(ChatObject* chat);
        void setUser(UserObject* user);
        void setUnreadCount(int c);
        void setIsMute(bool b);

    public: // C++ Only Setters
        void setTitle(const QString& title);
        void setStatusText(const QString& statustext);
        void setMessageText(const QString &topmessage);
        void setMessageDate(const QString& messagedate);
        void setTypingUsers(const QVariantList &typingusers);
        void setPeer(InputPeerObject* peer);
        void setTopMessage(MessageObject* topmessage);
        void setMessageUser(UserObject* user);
        void setMessageType(int messagetype);
        void setSecretChatState(int secretchatstate);
        void setIsSecretChat(bool b);
        void setIsMessageOut(bool b);
        void setIsMessageUnread(bool b);
        void updateUnreadCount(int c);
        void updateIsMute(bool b);

    signals:
        void peerChanged();
        void chatChanged();
        void userChanged();
        void messageTypeChanged();
        void messageUserChanged();
        void secretChatStateChanged();
        void titleChanged();
        void statusTextChanged();
        void messageActionChanged();
        void messageTextChanged();
        void messageDateChanged();
        void typingUsersChanged();
        void unreadCountChanged();
        void isBroadcastChanged();
        void isChatChanged();
        void isUserChanged();
        void isSecretChatChanged();
        void isActionMessageChanged();
        void isMediaMessageChanged();
        void isTextMessageChanged();
        void isMuteChanged();
        void isMessageOutChanged();
        void isMessageUnreadChanged();
        void unreadCountRequest(int c);
        void muteRequest(bool b);

    private:
        TelegramEngine* _engine;
        ChatObject* _chat;
        UserObject* _user;
        DialogObject* _dialog;
        InputPeerObject* _peer;
        UserObject* _messageuser;
        SailorgramMessageAction* _messageaction;
        QString _title;
        QString _statustext;
        QString _messagetext;
        QString _messagedate;
        QString _typingusers;
        int _messagetype;
        int _unreadcount;
        int _secretchatstate;
        bool _issecretchat;
        bool _ismute;
        bool _ismessageout;
        bool _ismessageunread;
};

#endif // SAILORGRAMDIALOGITEM_H
