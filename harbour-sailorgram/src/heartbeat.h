#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QThread>
#include <telegramqml.h>

class HeartBeat : public QThread
{
    Q_OBJECT

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        explicit HeartBeat(QObject *parent = 0);
        int interval() const;
        bool connected() const;
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);
        void setInterval(int interval);

    protected:
        virtual void run();

    signals:
        void connectedChanged();
        void intervalChanged();
        void telegramChanged();

    private:
        bool _connected;
        int _interval;
        TelegramQml* _telegram;
};

#endif // HEARTBEAT_H
