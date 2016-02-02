#ifndef INSOMNIAC_H
#define INSOMNIAC_H

#include <QObject>
#include <QSocketNotifier>
#include "libiphb.h"

// Original source: https://github.com/tanghus/kitchen-timer-qml/tree/master/src/insomniac
class Insomniac: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int timerWindow READ timerWindow WRITE setTimerWindow)
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval)

    public:
        enum InsomniacError { NoError = 0, InvalidArgument, TimerFailed, InternalError };

    public:
        Insomniac(QObject *parent = 0);
        ~Insomniac();
        InsomniacError lastError() const;
        void wokeUp();

    public:
        int interval() const;
        void setInterval(int seconds);
        int timerWindow() const;
        void setTimerWindow(int seconds);
        bool repeat() const;
        void setRepeat(bool repeat);
        bool running() const;
        void setRunning(bool running);

    signals:
        void runningChanged();
        void timeout();
        void error(Insomniac::InsomniacError error);

    private:
        int _interval;
        int _timerwindow;
        bool _running;
        bool _repeat;
        iphb_t _iphbdhandler;
        QSocketNotifier* _notifier;
        Insomniac::InsomniacError _lasterror;

    public slots:
        void start(int interval, int timerwindow);
        void start();
        void stop();

    private slots:
        void heartbeatReceived(int sock);
};

#endif // INSOMNIAC_H
