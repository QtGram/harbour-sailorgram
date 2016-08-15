#ifndef SAILORGRAMMESSAGEITEM_H
#define SAILORGRAMMESSAGEITEM_H

#include <telegram/objects/userobject.h>
#include <telegram/objects/messageobject.h>
#include <telegram/objects/inputpeerobject.h>
#include <telegramengine.h>
#include "../object/sailorgrammessageaction.h"
#include "../object/sailorgrammessagemedia.h"
#include "../object/sailorgrampeer.h"

class SailorgramMessageItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UserObject* fromUser READ fromUser NOTIFY fromUserChanged)
    Q_PROPERTY(UserObject* toUser READ toUser NOTIFY toUserChanged)
    Q_PROPERTY(SailorgramPeer* fromPeer READ fromPeer NOTIFY fromPeerChanged)
    Q_PROPERTY(SailorgramPeer* replyPeer READ replyPeer NOTIFY replyPeerChanged)
    Q_PROPERTY(SailorgramPeer* forwardPeer READ forwardPeer NOTIFY forwardPeerChanged)
    Q_PROPERTY(SailorgramMessageItem* messageReply READ messageReply CONSTANT FINAL)
    Q_PROPERTY(SailorgramMessageAction* messageAction READ messageAction NOTIFY messageActionChanged)
    Q_PROPERTY(SailorgramMessageMedia* messageMedia READ messageMedia NOTIFY messageMediaChanged)
    Q_PROPERTY(QString messageText READ messageText NOTIFY messageTextChanged)
    Q_PROPERTY(QString messageDate READ messageDate NOTIFY messageDateChanged)
    Q_PROPERTY(int messageType READ messageType NOTIFY messageTypeChanged)
    Q_PROPERTY(bool isActionMessage READ isActionMessage NOTIFY isActionMessageChanged)
    Q_PROPERTY(bool isMediaMessage READ isMediaMessage NOTIFY isMediaMessageChanged)
    Q_PROPERTY(bool isTextMessage READ isTextMessage NOTIFY isTextMessageChanged)
    Q_PROPERTY(bool isMessageUnread READ isMessageUnread NOTIFY isUnreadChanged)
    Q_PROPERTY(bool isMessageOut READ isMessageOut NOTIFY isOutChanged)
    Q_PROPERTY(bool isForward READ isForward NOTIFY isForwardChanged)
    Q_PROPERTY(bool hasReply READ hasReply NOTIFY hasReplyChanged)
    Q_PROPERTY(MessageObject* rawMessage READ rawMessage NOTIFY rawMessageChanged)

    private:
        enum MessageCategories { CategoryNormal, CategoryReply };

    private:
        SailorgramMessageItem(TelegramEngine* engine, MessageCategories messagetype, QObject *parent = 0);

    public:
        explicit SailorgramMessageItem(QObject *parent = 0);
        SailorgramMessageItem(TelegramEngine* engine, QObject *parent = 0);
        UserObject* fromUser() const;
        UserObject* toUser() const;
        SailorgramPeer* fromPeer() const;
        SailorgramPeer* replyPeer() const;
        SailorgramPeer* forwardPeer() const;
        SailorgramMessageItem* messageReply();
        SailorgramMessageAction* messageAction() const;
        SailorgramMessageMedia* messageMedia() const;
        QString messageText() const;
        QString messageDate() const;
        int messageType() const;
        bool isActionMessage() const;
        bool isMediaMessage() const;
        bool isTextMessage() const;
        bool isMessageUnread() const;
        bool isMessageOut() const;
        bool isForward() const;
        bool hasReply() const;

    public: // C++ Only getters
        MessageObject* rawMessage() const;

    public: // C++ Only setters
        void setMessage(MessageObject* message);
        void setReplyPeer(SailorgramPeer* replypeer);
        void setForwardPeer(SailorgramPeer* forwardpeer);
        void setFromPeer(InputPeerObject* inputpeer);
        void setFromUser(UserObject* user);
        void setToUser(UserObject* user);
        void setMessageType(int messagetype);
        void setMessageText(const QString& messagetext);
        void setMessageDate(const QString& messagedate);
        void setIsMessageUnread(bool b);
        void setIsMessageOut(bool b);

    signals:
        void fromUserChanged();
        void toUserChanged();
        void fromPeerChanged();
        void replyPeerChanged();
        void forwardPeerChanged();
        void messageActionChanged();
        void messageMediaChanged();
        void messageTextChanged();
        void messageDateChanged();
        void messageTypeChanged();
        void isActionMessageChanged();
        void isMediaMessageChanged();
        void isTextMessageChanged();
        void isUnreadChanged();
        void isOutChanged();
        void isForwardChanged();
        void hasReplyChanged();
        void rawMessageChanged();

    private:
        TelegramEngine* _engine;
        MessageObject* _message;
        MessageCategories _messagecategory;
        SailorgramMessageAction* _messageaction;
        SailorgramMessageMedia* _messagemedia;
        SailorgramMessageItem* _messagereply;
        UserObject* _fromuser;
        UserObject* _touser;
        SailorgramPeer* _frompeer;
        SailorgramPeer* _replypeer;
        SailorgramPeer* _forwardpeer;
        QString _messagetext;
        QString _messagedate;
        int _messagetype;
        bool _ismessageunread;
        bool _ismessageout;

};

#endif // SAILORGRAMMESSAGEITEM_H
