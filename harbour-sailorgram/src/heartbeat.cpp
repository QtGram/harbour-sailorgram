#include "heartbeat.h"

HeartBeat::HeartBeat(QObject *parent): QObject(parent), _connected(false), _telegram(NULL)
{
    this->_hbthread = new QThread(this);

    this->_timer = new QTimer();
    this->_timer->moveToThread(this->_hbthread);

    this->_socket = new QTcpSocket();
    this->_socket->moveToThread(this->_hbthread);

    connect(this->_hbthread, SIGNAL(started()), this->_timer, SLOT(start()), Qt::QueuedConnection);
    connect(this->_timer, SIGNAL(timeout()), this, SLOT(checkConnection()), Qt::QueuedConnection);
    connect(this, SIGNAL(connectionUpdated(bool)), this, SLOT(onConnectionUpdated(bool)), Qt::QueuedConnection);
}

int HeartBeat::interval() const
{
    return this->_timer->interval();
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
    if(this->_timer->interval() == interval)
        return;

    this->_timer->setInterval(interval);
    emit intervalChanged();
}

void HeartBeat::beat()
{
    this->_hbthread->start();
    this->onConnectionUpdated(this->_telegram->connected());
}

void HeartBeat::onConnectionUpdated(bool connected)
{
    if(this->_connected == connected)
        return;

    this->_connected = connected;
    emit connectedChanged();
}

void HeartBeat::checkConnection()
{
    this->_socket->connectToHost(this->_telegram->defaultHostAddress(), this->_telegram->defaultHostPort());
    this->_socket->waitForConnected(this->_timer->interval() / 2);

    bool connected = (this->_socket->state() == QAbstractSocket::ConnectedState);
    this->_socket->abort();

    emit connectionUpdated(connected);
}
