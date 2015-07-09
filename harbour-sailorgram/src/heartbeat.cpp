#include "heartbeat.h"

HeartBeat::HeartBeat(QObject *parent): QObject(parent), _lastconnected(false), _telegram(NULL)
{
    this->_hbthread = new QThread(this);

    this->_timer = new QTimer();
    this->_timer->moveToThread(this->_hbthread);

    this->_socket = new QTcpSocket();
    this->_socket->moveToThread(this->_hbthread);

    connect(this->_hbthread, SIGNAL(started()), this->_timer, SLOT(start()), Qt::QueuedConnection);
    connect(this->_timer, SIGNAL(timeout()), this, SLOT(checkConnection()), Qt::QueuedConnection);
}

int HeartBeat::interval() const
{
    return this->_timer->interval();
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
    if(this->_timer->interval() == interval)
        return;

    this->_timer->setInterval(interval);
    emit intervalChanged();
}

void HeartBeat::beat()
{
    this->_hbthread->start();

    if(this->_lastconnected != this->_telegram->connected())
    {
        this->_lastconnected = this->_telegram->connected();
        emit connectedChanged(this->_telegram->connected());
    }
}

void HeartBeat::checkConnection()
{
    this->_socket->connectToHost(this->_telegram->defaultHostAddress(), this->_telegram->defaultHostPort());
    this->_socket->waitForConnected(this->_timer->interval() / 2);

    bool connected = (this->_socket->state() == QAbstractSocket::ConnectedState);

    if(connected != this->_lastconnected)
    {
        this->_lastconnected = connected;
        emit connectedChanged(connected);
    }

    this->_socket->abort();
}
