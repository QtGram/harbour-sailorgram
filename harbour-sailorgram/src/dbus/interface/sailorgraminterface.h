#ifndef SAILORGRAMINTERFACE_H
#define SAILORGRAMINTERFACE_H

#include <QtDBus>
#include "sailorgramadaptor.h"

class SailorgramInterface : public QObject
{
    Q_OBJECT

    public:
        explicit SailorgramInterface(QObject *parent = 0);

    public:
        static void sendWakeUp();

    public slots:
        void wakeUp();
        void openDialog(qint32 dialogid);

    signals:
        void wakeUpRequested();
        void openDialogRequested(qint32 dialogid);

    public:
        static const QString INTERFACE_NAME;
};

#endif // SAILORGRAMINTERFACE_H
