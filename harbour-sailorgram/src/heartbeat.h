#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QThread>
#include <telegramqml.h>

class HeartBeat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        explicit HeartBeat(QObject *parent = 0);
        int interval() const;
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);
        void setInterval(int interval);

    public slots:
        void beat();

    signals:
        void connectedChanged(bool connected);
        void intervalChanged();
        void telegramChanged();

    private slots:
        void checkConnection();

    private:
        bool _lastconnected;
        QThread* _hbthread;
        QTimer* _timer;
        TelegramQml* _telegram;
        QTcpSocket* _socket;
};

#endif // HEARTBEAT_H
