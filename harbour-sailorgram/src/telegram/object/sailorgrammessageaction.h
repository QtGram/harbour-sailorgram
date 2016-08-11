#ifndef SAILORGRAMMESSAGEACTION_H
#define SAILORGRAMMESSAGEACTION_H

#include <QObject>
#include <telegram/objects/messageobject.h>
#include <telegram/objects/userobject.h>
#include <telegramsharedpointer.h>
#include <telegramengine.h>

class SailorgramMessageAction : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UserObject* fromUser READ fromUser NOTIFY fromUserChanged)
    Q_PROPERTY(UserObject* toUser READ toUser CONSTANT FINAL)
    Q_PROPERTY(QString title READ title CONSTANT FINAL)
    Q_PROPERTY(int actionType READ actionType CONSTANT FINAL)
    Q_PROPERTY(bool isChannel READ isChannel CONSTANT FINAL)

    public:
        explicit SailorgramMessageAction(QObject *parent = 0);
        SailorgramMessageAction(TelegramEngine* engine, MessageObject* message, QObject *parent = 0);
        UserObject* fromUser() const;
        UserObject* toUser() const;
        QString title() const;
        int actionType() const;
        bool isChannel() const;

    signals:
        void fromUserChanged();

    private:
        MessageObject* _message;
        TelegramSharedPointer<UserObject> _fromuser;
        TelegramSharedPointer<UserObject> _touser;
};

#endif // SAILORGRAMMESSAGEACTION_H
