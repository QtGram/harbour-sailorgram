#ifndef TELEGRAMSTATUS_H
#define TELEGRAMSTATUS_H

#include "telegramengine.h"
#include "tqobject.h"

class SendMessageActionObject;
class InputPeerObject;
class TelegramStatusTyping;
class TelegramStatusPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramStatus : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(TelegramStatusTyping* typing READ typing WRITE setTyping NOTIFY typingChanged)

public:
    TelegramStatus(QObject *parent = 0);
    ~TelegramStatus();

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    void setOnline(bool online);
    bool online() const;

    void setTyping(TelegramStatusTyping *typing);
    TelegramStatusTyping *typing() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void engineChanged();
    void onlineChanged();
    void typingChanged();

public Q_SLOTS:
    void requestStatus(bool online);
    void requestTyping(InputPeerObject *peer, SendMessageActionObject *action);

protected:
    void refresh();
    void timerEvent(QTimerEvent *e);

private:
    TelegramStatusPrivate *p;
};

#endif // TELEGRAMSTATUS_H
