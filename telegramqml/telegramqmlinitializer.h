#ifndef TELEGRAMQMLINITIALIZER_H
#define TELEGRAMQMLINITIALIZER_H

#include "telegramqml_global.h"

class TELEGRAMQMLSHARED_EXPORT TelegramQmlInitializer
{
public:
    static void init(const char *uri);

private:
    static void initializeTypes(const char *uri);
};

#endif // TELEGRAMQMLINITIALIZER_H
