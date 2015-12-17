#ifndef TELEGRAMQML_MACROS
#define TELEGRAMQML_MACROS

#include <QDebug>

#define PROFILES_DB_CONNECTION "profiles_connection"
#define PROFILES_DB_PATH ":/database/profiles.sqlite"

#define USERDATA_DB_CONNECTION "userdata_connection"
#define USERDATA_DB_PATH ":/database/userdata.sqlite"

#define DATABASE_DB_CONNECTION "database_connection"
#define DATABASE_DB_PATH ":/database/database.sqlite"

#define CHECK_QUERY_ERROR(QUERY_OBJECT) \
    if(QUERY_OBJECT.lastError().isValid()) \
        qDebug() << __FUNCTION__ << QUERY_OBJECT.lastError().text();

#endif // TELEGRAMQML_MACROS

