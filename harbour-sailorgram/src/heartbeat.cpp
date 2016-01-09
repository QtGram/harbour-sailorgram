#include "heartbeat.h"

HeartBeat::HeartBeat(QObject *parent): QThread(parent), _connected(-1), _interval(0), _cancontinue(true), _telegram(NULL)
{

}

int HeartBeat::interval() const
{
    return this->_interval;
}

bool HeartBeat::connected() const
{
    if(this->_connected == -1)
    {
        if(this->_telegram && this->_telegram->telegram())
            return this->_telegram->telegram()->isConnected();

        return false;
    }

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

void HeartBeat::setStopped(bool stopped)
{
    bool cancontinue = !stopped;

    if(this->_cancontinue == cancontinue)
        return;

    this->_cancontinue = cancontinue;

    if(!cancontinue)
    {
        this->_connected = false;
        emit connectedChanged();
    }
    else
        this->start();
}

void HeartBeat::run()
{
    while(this->_cancontinue)
    {
        QTcpSocket socket;
        socket.connectToHost(this->_telegram->defaultHostAddress(), this->_telegram->defaultHostPort());
        socket.waitForConnected(this->_interval / 2);

        int connected = (socket.state() == QAbstractSocket::ConnectedState);

        if(this->_connected != connected)
        {
            this->_connected = connected;
            emit connectedChanged();
        }

        QThread::msleep(this->_interval);
    }
}
