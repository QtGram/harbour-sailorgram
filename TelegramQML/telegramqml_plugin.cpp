#include "telegramqml_plugin.h"
#include "telegramqmlinitializer.h"

#include <qqml.h>

void TelegramQmlPlugin::registerTypes(const char *uri)
{
    // @uri com.mycompany.qmlcomponents
    TelegramQmlInitializer::init(uri);
}
