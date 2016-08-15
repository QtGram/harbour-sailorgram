#ifndef SAILORGRAMFORWARDMESSAGE_H
#define SAILORGRAMFORWARDMESSAGE_H

#include <QObject>
#include "../item/sailorgrammessageitem.h"

class SailorgramForwardMessage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SailorgramMessageItem* message READ message CONSTANT FINAL)

    public:
        explicit SailorgramForwardMessage(QObject *parent = 0);
        explicit SailorgramForwardMessage(SailorgramMessageItem* message, QObject *parent = 0);
        SailorgramMessageItem *message() const;

    private:
        SailorgramMessageItem* _message;
};

#endif // SAILORGRAMFORWARDMESSAGE_H
