#ifndef CONNECTIVITYCHECKER_H
#define CONNECTIVITYCHECKER_H

#include <QObject>
#include <QtDBus>
#include <telegramengine.h>

class ConnectivityChecker : public QObject
{
    Q_OBJECT

    public:
        explicit ConnectivityChecker(TelegramEngine* engine, QObject *parent = 0);
        bool connected() const;

    private slots:
        void onEngineStateChanged();
        void onConnmanPropertyChanged(QString key, QDBusVariant value);

    signals:
        void connectedChanged();

    private:
        TelegramEngine* _engine;
        bool _connected;
};

#endif // CONNECTIVITYCHECKER_H
