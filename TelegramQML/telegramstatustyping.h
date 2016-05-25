#ifndef TELEGRAMSTATUSTYPING_H
#define TELEGRAMSTATUSTYPING_H

#include <QObject>

#include "telegramqml_global.h"

class SendMessageActionObject;
class InputPeerObject;
class TelegramStatusTypingPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramStatusTyping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(InputPeerObject* peer READ peer WRITE setPeer NOTIFY peerChanged)
    Q_PROPERTY(SendMessageActionObject* action READ action WRITE setAction NOTIFY actionChanged)

public:
    TelegramStatusTyping(QObject *parent = 0);
    ~TelegramStatusTyping();

    void setPeer(InputPeerObject *peer);
    InputPeerObject *peer() const;

    void setAction(SendMessageActionObject *action);
    SendMessageActionObject *action();

    static QStringList requiredProperties();

Q_SIGNALS:
    void peerChanged();
    void actionChanged();
    void changed();

public Q_SLOTS:

private:
    TelegramStatusTypingPrivate *p;
};

#endif // TELEGRAMSTATUSTYPING_H
