#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QThread>
#include <telegram.h>
#include <telegramqml.h>

class HeartBeat : public QThread
{
    Q_OBJECT

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
        int _connected;
        int _interval;
        TelegramQml* _telegram;
};

#endif // HEARTBEAT_H
