#ifndef TELEGRAMQMLINITIALIZER_H
#define TELEGRAMQMLINITIALIZER_H

#include "telegramqml_global.h"

#include <QString>

class QQmlEngine;
class QJSEngine;
class QObject;
class TELEGRAMQMLSHARED_EXPORT TelegramQmlInitializer
{
public:
    static void init(const char *uri, bool exportMode = false);

    template<typename T>
    static int registerType(const char *uri, int versionMajor, int versionMinor, const char *typeName, bool exportMode);

    template<typename T>
    static int registerModel(const char *uri, int versionMajor, int versionMinor, const char *typeName, bool exportMode);

    template <typename T>
    static int registerSingletonType(const char *uri, int versionMajor, int versionMinor, const char *typeName,
                                 QObject *(*callback)(QQmlEngine *, QJSEngine *), bool exportMode);

    template<typename T>
    static int registerUncreatableType(const char *uri, int versionMajor, int versionMinor, const char *qmlName, const QString& reason, bool exportMode);

    static void exportDocuments(const QString &destination);

private:
    static void initializeTypes(const char *uri);

protected:
    template<typename T>
    static QString exportItem(const QString &module, int major, int minor, const QString &component, bool store = true);
    template<typename T>
    static QString exportModel(const QString &module, int major, int minor, const QString &component);

    static QString fixType(const QString &type);
};

#endif // TELEGRAMQMLINITIALIZER_H
