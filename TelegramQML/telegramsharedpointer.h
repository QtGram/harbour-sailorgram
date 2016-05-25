#ifndef TELEGRAMSHAREDPOINTER_H
#define TELEGRAMSHAREDPOINTER_H

#include "telegramqml_global.h"

#include <QSharedPointer>
#include <QHash>
#include <QSet>
#include <QDebug>

class TelegramTypeQObject;

void TELEGRAMQMLSHARED_EXPORT tg_share_pointer_append(void *dis, void *ptr);
bool TELEGRAMQMLSHARED_EXPORT tg_share_pointer_remove(void *dis, void *ptr);
bool TELEGRAMQMLSHARED_EXPORT tg_share_pointer_contains(void *ptr);

#ifdef DISABLE_SHARED_POINTER
template<typename T>
using TelegramSharedPointer = T*;
#else
template<class T>
class TELEGRAMQMLSHARED_EXPORT TelegramSharedPointer
{
public:
    TelegramSharedPointer(T *ptr = 0): value(0) { operator =(ptr); }
    TelegramSharedPointer(const TelegramSharedPointer<T> &b): value(0) { operator =(b.value); }
    virtual ~TelegramSharedPointer() { operator =(0); }

    static bool contains(void *ptr) { return tg_share_pointer_contains(ptr); }
    inline T *data() const { return value; }
    inline bool isNull() const { return !data(); }
    inline bool operator !() const { return isNull(); }
    inline T &operator*() const { return *data(); }
    inline T *operator->() const { return data(); }
    inline operator T*() const { return data(); }
    inline bool operator!=(T *b) { return !operator==(b); }
    inline bool operator==(T *b) { return value == b; }
    inline bool operator!=(const TelegramSharedPointer<T> &b) { return !operator==(b); }
    inline bool operator==(const TelegramSharedPointer<T> &b) { return value == b.value; }
    inline void operator=(const TelegramSharedPointer<T> &b) { operator=(b.value); }
    inline void operator=(T *b) {
        if(b == value) return;
        if(b) tg_share_pointer_append(this, b);
        if(value && tg_share_pointer_remove(this, value))
            delete value;
        value = b;
    }

private:
    T *value;
};
#endif

inline bool operator==(const TelegramSharedPointer<TelegramTypeQObject> &e1, const TelegramSharedPointer<TelegramTypeQObject> &e2)
{
    return e1.data() == e2.data();
}

inline uint qHash(const TelegramSharedPointer<TelegramTypeQObject> &key, uint seed)
{
    return qHash(static_cast<void*>(key.data()), seed);
}

#endif // TELEGRAMSHAREDPOINTER_H
