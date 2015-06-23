#include "telegramqml_plugin.h"
#include "telegramqmlinitializer.h"

#include <qqml.h>

void TelegramQmlPlugin::registerTypes(const char *uri)
{
    // @uri harbour.sailorgram.TelegramQml
    TelegramQmlInitializer::init(uri);
}
