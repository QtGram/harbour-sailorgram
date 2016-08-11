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
    Q_PROPERTY(UserObject* messageUser READ messageUser NOTIFY messageUserChanged)
    Q_PROPERTY(SailorgramMessageAction* messageAction READ messageAction NOTIFY messageActionChanged)
    Q_PROPERTY(QString messageText READ messageText NOTIFY messageTextChanged)
    Q_PROPERTY(QString messageDate READ messageDate NOTIFY messageDateChanged)
    Q_PROPERTY(QString typingUsers READ typingUsers NOTIFY typingUsersChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int messageType READ messageType NOTIFY messageTypeChanged)
    Q_PROPERTY(bool isMute READ isMute NOTIFY isMuteChanged)
    Q_PROPERTY(bool isBroadcast READ isBroadcast NOTIFY isBroadcastChanged)
    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isSecretChat READ isSecretChat NOTIFY isSecretChatChanged)
    Q_PROPERTY(bool isActionMessage READ isActionMessage NOTIFY isActionMessageChanged)
    Q_PROPERTY(bool isMediaMessage READ isMediaMessage NOTIFY isMediaMessageChanged)
    Q_PROPERTY(bool isTextMessage READ isTextMessage NOTIFY isTextMessageChanged)
    Q_PROPERTY(bool isMessageOut READ isMessageOut NOTIFY isMessageOutChanged)
    Q_PROPERTY(bool isMessageUnread READ isMessageUnread NOTIFY isMessageUnreadChanged)

    public:
        explicit SailorgramDialogItem(QObject *parent = 0);
        SailorgramDialogItem(TelegramEngine* engine, DialogObject* dialog, QObject *parent = 0);
        int messageType() const;
        InputPeerObject* peer() const;
        UserObject* messageUser() const;
        SailorgramMessageAction* messageAction() const;
        QString title() const;
        QString messageText() const;
        QString messageDate() const;
        QString typingUsers() const;
        int unreadCount() const;
        bool isMute() const;
        bool isBroadcast() const;
        bool isChat() const;
        bool isSecretChat() const;
        bool isActionMessage() const;
        bool isMediaMessage() const;
        bool isTextMessage() const;
        bool isMessageOut() const;
        bool isMessageUnread() const;
        void setChat(ChatObject* chat);
        void setUser(UserObject* user);

    public: // C++ Only Setters
        void setTitle(const QString& title);
        void setMessageText(const QString &topmessage);
        void setMessageDate(const QString& messagedate);
        void setTypingUsers(const QVariantList &typingusers);
        void setPeer(InputPeerObject* peer);
        void setTopMessage(MessageObject* topmessage);
        void setMessageUser(UserObject* user);
        void setMessageType(int messagetype);
        void setUnreadCount(int c);
        void setIsSecretChat(bool b);
        void setIsMute(bool b);
        void setIsMessageOut(bool b);
        void setIsMessageUnread(bool b);

    signals:
        void messageTypeChanged();
        void peerChanged();
        void messageUserChanged();
        void titleChanged();
        void messageActionChanged();
        void messageTextChanged();
        void messageDateChanged();
        void typingUsersChanged();
        void unreadCountChanged();
        void isBroadcastChanged();
        void isChatChanged();
        void isSecretChatChanged();
        void isActionMessageChanged();
        void isMediaMessageChanged();
        void isTextMessageChanged();
        void isMuteChanged();
        void isMessageOutChanged();
        void isMessageUnreadChanged();

    private:
        TelegramEngine* _engine;
        ChatObject* _chat;
        UserObject* _user;
        DialogObject* _dialog;
        InputPeerObject* _peer;
        UserObject* _messageuser;
        SailorgramMessageAction* _messageaction;
        QString _title;
        QString _messagetext;
        QString _messagedate;
        QString _typingusers;
        int _messagetype;
        int _unreadcount;
        bool _issecretchat;
        bool _ismute;
        bool _ismessageout;
        bool _ismessageunread;
};

#endif // SAILORGRAMDIALOGITEM_H
