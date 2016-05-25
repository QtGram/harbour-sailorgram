#include "telegramsharedpointer.h"

#include <QHash>
#include <QSet>

#ifndef DISABLE_SHARED_POINTER
QHash<void*, QSet<void*> > tg_share_pointer_data;

void tg_share_pointer_append(void *dis, void *ptr)
{
    tg_share_pointer_data[ptr].insert(dis);
}

bool tg_share_pointer_remove(void *dis, void *ptr)
{
    tg_share_pointer_data[ptr].remove(dis);
    if(tg_share_pointer_data[ptr].isEmpty())
    {
        tg_share_pointer_data.remove(ptr);
        return true;
    }
    else
        return false;
}

bool tg_share_pointer_contains(void *ptr)
{
    return tg_share_pointer_data.contains(ptr);
}
#endif
