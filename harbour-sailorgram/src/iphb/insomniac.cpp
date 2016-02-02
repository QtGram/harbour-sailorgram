#include "insomniac.h"
#include <QDebug>

#include <errno.h>
#include <stdio.h>

Insomniac::Insomniac(QObject *parent): QObject(parent) , _interval(0) , _timerwindow(120) , _running(false) , _repeat(true) , _iphbdhandler(0) , _notifier(0)
{
    this->_iphbdhandler = iphb_open(0);

    if(!this->_iphbdhandler)
    {
        this->_lasterror = Insomniac::InternalError;
        qDebug() << "iphb_open error" << _iphbdhandler<< errno <<strerror(errno);
        return;
    }

    int sockfd = iphb_get_fd(this->_iphbdhandler);

    if(!(sockfd > -1))
    {
        this->_lasterror = Insomniac::InternalError;
        qDebug() << "socket failure"<<strerror(errno);
        return;
    }

    this->_notifier = new QSocketNotifier(sockfd, QSocketNotifier::Read);

    if(!QObject::connect(this->_notifier, SIGNAL(activated(int)), this, SLOT(heartbeatReceived(int))))
    {
        delete this->_notifier;
        this->_notifier = 0;
        this->_lasterror = Insomniac::TimerFailed;
        qDebug() << "timer failure";
        return;
    }

    this->_notifier->setEnabled(false);
}

Insomniac::~Insomniac()
{
    if(this->_iphbdhandler)
        iphb_close(this->_iphbdhandler);

    if(this->_notifier)
        delete this->_notifier;
}

void Insomniac::wokeUp()
{
    if(!this->_running)
        return;

    if(!(this->_iphbdhandler && this->_notifier))
    {
        this->_lasterror = Insomniac::InternalError;
        emit error(this->_lasterror);
        return;
    }

    this->_notifier->setEnabled(false);
    iphb_I_woke_up(this->_iphbdhandler);

    this->_running = false;
    emit runningChanged();

    this->_lasterror = Insomniac::NoError;
    this->start();
}

int Insomniac::interval() const
{
    return this->_interval;
}

void Insomniac::setInterval(int seconds)
{
    this->_interval = seconds;
}

void Insomniac::setRepeat(bool repeat)
{
    this->_repeat = repeat;
}

bool Insomniac::repeat() const
{
    return this->_repeat;
}

int Insomniac::timerWindow() const
{
    return this->_timerwindow;
}

/**
Sets the timer's timeout window in seconds.

The timeout window is a window of time set around the interval in which the timer will timeout.

It is wise to have timeout window quite big so all users of this service get synced.

For example if your preferred wait is 120 seconds and you can wait anywhere within 10 seconds,
use interval of 120 and timerWindow of 10. This means the timer will timeout anywhere from
115 seconds to 125 seconds.

*/
void Insomniac::setTimerWindow(int seconds)
{
    this->_timerwindow = seconds;
}

void Insomniac::start(int interval, int timerwindow)
{
    this->_interval = interval;
    this->_timerwindow = timerwindow;

    this->start();
}

void Insomniac::start()
{
    if(this->_running)
        return;

    if(!(this->_iphbdhandler && this->_notifier))
    {
        this->_lasterror = Insomniac::InternalError;
        emit error(this->_lasterror);
        return;
    }

    int mustwait = 0;
    time_t unixtime = iphb_wait(this->_iphbdhandler, this->_interval - (this->_timerwindow * .5), this->_interval + (this->_timerwindow * .5) , mustwait);

    if(unixtime == static_cast<time_t>(-1))
    {
        this->_lasterror = Insomniac::TimerFailed;
        emit error(this->_lasterror);
        return;
    }

    this->_notifier->setEnabled(true);
    this->_running = true;

    emit runningChanged();
    this->_lasterror = Insomniac::NoError;
}

void Insomniac::stop()
{
    if(!this->_running)
        return;

    if(!(this->_iphbdhandler && this->_notifier))
    {
        this->_lasterror = Insomniac::InternalError;
        emit error(this->_lasterror);
        return;
    }

    this->_notifier->setEnabled(false);
    iphb_discard_wakeups(this->_iphbdhandler);

    this->_running = false;
    emit runningChanged();

    this->_lasterror = Insomniac::NoError;
}

void Insomniac::heartbeatReceived(int sock)
{
    Q_UNUSED(sock);

    this->stop();
    emit timeout();

    if(this->_repeat)
        this->start();
}

bool Insomniac::running() const
{
    return this->_running;
}

void Insomniac::setRunning(bool running)
{
   if(this->_running == running)
       return;

   if(running)
       this->start();
   else
       this->stop();
}

