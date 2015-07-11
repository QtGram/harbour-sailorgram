#include "heartbeat.h"

HeartBeat::HeartBeat(QObject *parent): QThread(parent), _connected(false), _interval(0), _telegram(NULL)
{

}

int HeartBeat::interval() const
{
    return this->_interval;
}

bool HeartBeat::connected() const
{
    return this->_connected;
}

TelegramQml *HeartBeat::telegram() const
{
    return this->_telegram;
}

void HeartBeat::setTelegram(TelegramQml *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    emit telegramChanged();
}

void HeartBeat::setInterval(int interval)
{
    if(this->_interval == interval)
        return;

    this->_interval = interval;
    emit intervalChanged();
}

void HeartBeat::run()
{
    for(; ;)
    {
        QTcpSocket socket;
        socket.connectToHost(this->_telegram->defaultHostAddress(), this->_telegram->defaultHostPort());
        socket.waitForConnected(this->_interval / 2);

        bool connected = (socket.state() == QAbstractSocket::ConnectedState);
        qDebug() << Q_FUNC_INFO << connected;

        if(this->_connected != connected)
        {
            this->_connected = connected;
            emit connectedChanged();
        }

        QThread::msleep(this->_interval);
    }
}
