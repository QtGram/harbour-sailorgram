#ifndef CONNECTIVITYCHECKER_H
#define CONNECTIVITYCHECKER_H

#include <QObject>
#include <QtDBus>
#include <telegramqml.h>

class ConnectivityChecker : public QObject
{
    Q_OBJECT

    public:
        explicit ConnectivityChecker(TelegramQml* telegram, QObject *parent = 0);
        bool connected() const;

    private slots:
        void onTelegramConnectedChanged();
        void onConnmanPropertyChanged(QString key, QDBusVariant value);

    signals:
        void connectedChanged();

    private:
        TelegramQml* _telegram;
        bool _connected;
};

#endif // CONNECTIVITYCHECKER_H
