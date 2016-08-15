#ifndef SAILORGRAMPEER_H
#define SAILORGRAMPEER_H

#include <QObject>
#include <telegram/objects/inputpeerobject.h>
#include <telegram/objects/userobject.h>
#include <telegram/objects/chatobject.h>
#include <telegramengine.h>

class SailorgramPeer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(InputPeerObject* inputPeer READ inputPeer CONSTANT FINAL)
    Q_PROPERTY(UserObject* user READ user CONSTANT FINAL)
    Q_PROPERTY(ChatObject* chat READ chat CONSTANT FINAL)
    Q_PROPERTY(bool isUser READ isUser CONSTANT FINAL)
    Q_PROPERTY(bool isChat READ isChat CONSTANT FINAL)
    Q_PROPERTY(bool isValid READ isValid CONSTANT FINAL)

    public:
        explicit SailorgramPeer(QObject *parent = 0);
        SailorgramPeer(QVariant peer, QObject *parent = 0);
        SailorgramPeer(TelegramEngine* engine, InputPeerObject* inputpeer, QObject *parent = 0);
        InputPeerObject* inputPeer() const;
        UserObject* user() const;
        ChatObject* chat() const;
        bool isUser() const;
        bool isChat() const;
        bool isValid() const;

    public: //C++ only setters
        void setUser(UserObject* user);
        void setChat(ChatObject* chat);

    private:
        InputPeerObject* _inputpeer;
        UserObject* _user;
        ChatObject* _chat;
};

#endif // SAILORGRAMPEER_H
