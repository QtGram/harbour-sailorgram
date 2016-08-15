#ifndef SAILORGRAMTOOLS_H
#define SAILORGRAMTOOLS_H

#include <telegram/objects/dialogobject.h>
#include <telegram/objects/messageobject.h>
#include <telegram/objects/userobject.h>

class SailorgramTools
{
    private:
        SailorgramTools();

    public:
        static qint32 peerId(DialogObject* dialog);
        static QString completeName(UserObject* user);
        static QString messageText(MessageObject* message);
        static QString messageDate(MessageObject* message);

    public:
        static int messageType(int mt);
        static int actionType(int at);
        static int sendFileType(int sft);
        static int sendMessageActionType(int smat);
        static int secretChatState(int scs);

    public:
        static quint32 reverseSendFileType(int sft);
        static quint32 reverseSendMessageActionType(int smat);
};

#endif // SAILORGRAMTOOLS_H
