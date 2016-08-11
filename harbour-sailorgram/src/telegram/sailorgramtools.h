#ifndef SAILORGRAMTOOLS_H
#define SAILORGRAMTOOLS_H

#include <telegram/objects/dialogobject.h>
#include <telegram/objects/userobject.h>

class SailorgramTools
{
    private:
        SailorgramTools();

    public:
        static qint32 peerId(DialogObject* dialog);
        static QString completeName(UserObject* user);
        static int messageType(int mt);
        static int actionType(int at);
};

#endif // SAILORGRAMTOOLS_H
