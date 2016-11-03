#ifndef SAILORGRAMADAPTOR_H
#define SAILORGRAMADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QDebug>

class SailorgramAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.harbour.sailorgram")

    public:
        explicit SailorgramAdaptor(QObject *parent = 0);

    public slots:
        void wakeUp();
        void openDialog(qint32 dialogid);
};

#endif // SAILORGRAMADAPTOR_H
