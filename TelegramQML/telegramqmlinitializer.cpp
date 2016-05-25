#include "telegrammessagesearchmodel.h"
#include "telegramqmlinitializer.h"
#include "telegramstatus.h"
#include "telegramstatustyping.h"
#include "telegramengine.h"
#include "telegramapp.h"
#include "telegramauthenticate.h"
#include "telegramdialoglistmodel.h"
#include "telegrammessagelistmodel.h"
#include "telegramprofilemanagermodel.h"
#include "telegrampeerdetails.h"
#include "telegramhost.h"
#include "telegramenums.h"
#include "telegramimageelement.h"
#include "telegrammessagefetcher.h"
#include "telegrammemberslistmodel.h"
#include "telegramtesttools.h"
#include "telegramstickerscategoriesmodel.h"
#include "telegramstickersmodel.h"
#include "telegramnotificationhandler.h"
#include "tqmldocumentexporter.h"
#include "telegramtopmessagesmodel.h"
#include "telegramcache.h"
#include "telegrammedialistmodel.h"
#include "telegramauthstore.h"
#include "telegramqmlsharedpointer.h"
#include "private/telegramdownloadhandler.h"

#include <telegram/objects/qmltools.h>
#include <QDir>
#include <qqml.h>

QStringList telegram_qml_indexCache;
QString telegram_qml_destination;

class MessagesFilterObj : public MessagesFilterObject
{
public:
    static QStringList requiredProperties() {
        return QStringList();
    }
};

class SendMessageActionObj : public SendMessageActionObject
{
public:
    static QStringList requiredProperties() {
        return QStringList();
    }
};

void TelegramQmlInitializer::init(const char *uri, bool exportMode)
{
    qtelegramRegisterQmlTypes(uri, 2, 0);
    qRegisterMetaType< QList<qint32> >("QList<qint32>");

    registerType<TelegramEngine>(uri, 2, 0, "Engine", exportMode);
    registerType<TelegramApp>(uri, 2, 0, "App", exportMode);
    registerType<TelegramHost>(uri, 2, 0, "Host", exportMode);
    registerType<TelegramCache>(uri, 2, 0, "Cache", exportMode);
    registerType<TelegramAuthStore>(uri, 2, 0, "AuthStore", exportMode);
    registerType<TelegramAuthenticate>(uri, 2, 0, "Authenticate", exportMode);

    registerModel<TelegramDialogListModel>(uri, 2, 0, "DialogListModel", exportMode);
    registerModel<TelegramMessageListModel>(uri, 2, 0, "MessageListModel", exportMode);
    registerModel<TelegramMessageSearchModel>(uri, 2, 0, "MessageSearchModel", exportMode);
    registerModel<TelegramMediaListModel>(uri, 2, 0, "MediaListModel", exportMode);
    registerModel<TelegramTopMessagesModel>(uri, 2, 0, "TopMessagesModel", exportMode);
    registerModel<TelegramStickersCategoriesModel>(uri, 2, 0, "StickersCategoriesModel", exportMode);
    registerModel<TelegramStickersModel>(uri, 2, 0, "StickersModel", exportMode);
    registerModel<TelegramMembersListModel>(uri, 2, 0, "MembersListModel", exportMode);
    registerModel<TelegramProfileManagerModel>(uri, 2, 0, "ProfileManagerModel", exportMode);

    registerType<TelegramImageElement>(uri, 2, 0, "Image", exportMode);
    registerType<TelegramDownloadHandler>(uri, 2, 0, "DownloadHandler", exportMode);

    registerType<TelegramMessageFetcher>(uri, 2, 0, "MessageFetcher", exportMode);
    registerType<TelegramPeerDetails>(uri, 2, 0, "PeerDetails", exportMode);
    registerType<TelegramNotificationHandler>(uri, 2, 0, "NotificationHandler", exportMode);
    registerType<TelegramStatus>(uri, 2, 0, "Status", exportMode);
    registerType<TelegramStatusTyping>(uri, 2, 0, "StatusTyping", exportMode);
    if(exportMode)
    {
        registerType<MessagesFilterObj>(uri, 2, 0, "MessagesFilter", exportMode);
        registerType<SendMessageActionObj>(uri, 2, 0, "SendMessageAction", exportMode);

        registerType<TqObject>(uri, 2, 0, "TqObject", exportMode);
        exportItem<TelegramAbstractListModel>(uri, 2, 0, "AbstractListModel");
        exportItem<TelegramAbstractEngineListModel>(uri, 2, 0, "AbstractEngineListModel");
    }

    registerType<TelegramQmlSharedPointer>(uri, 2, 0, "SharedPointer", exportMode);
    registerUncreatableType<TelegramEnums>(uri, 2, 0, "Enums", "It's just enums", exportMode);

    qmlRegisterType<TqmlDocumentExporter>(uri, 2, 0, "DocumentExporter");
    qmlRegisterType<TelegramTestTools>(uri, 2, 0, "TestTools");

    initializeTypes(uri);
}

void TelegramQmlInitializer::initializeTypes(const char *uri) {
    Q_UNUSED(uri)
}

template<typename T>
int TelegramQmlInitializer::registerType(const char *uri, int versionMajor, int versionMinor, const char *typeName, bool exportMode)
{
    if(exportMode)
        exportItem<T>(uri, versionMajor, versionMinor, typeName);
    else
        return qmlRegisterType<T>(uri, versionMajor, versionMinor, typeName);
    return 0;
}

template<typename T>
int TelegramQmlInitializer::registerModel(const char *uri, int versionMajor, int versionMinor, const char *typeName, bool exportMode)
{
    if(exportMode)
        exportModel<T>(uri, versionMajor, versionMinor, typeName);
    else
        return qmlRegisterType<T>(uri, versionMajor, versionMinor, typeName);
    return 0;
}

template<typename T>
int TelegramQmlInitializer::registerSingletonType(const char *uri, int versionMajor, int versionMinor, const char *typeName, QObject *(*callback)(QQmlEngine *, QJSEngine *), bool exportMode)
{
    if(exportMode)
        exportItem<T>(uri, versionMajor, versionMinor, typeName);
    else
        return qmlRegisterSingletonType<T>(uri, versionMajor, versionMinor, typeName, callback);
    return 0;
}

template<typename T>
int TelegramQmlInitializer::registerUncreatableType(const char *uri, int versionMajor, int versionMinor, const char *qmlName, const QString &reason, bool exportMode)
{
    if(exportMode)
        exportItem<T>(uri, versionMajor, versionMinor, qmlName);
    else
        return qmlRegisterUncreatableType<T>(uri, versionMajor, versionMinor, qmlName, reason);
    return 0;
}


void TelegramQmlInitializer::exportDocuments(const QString &destination)
{
    telegram_qml_destination = destination;

    QDir().mkpath(telegram_qml_destination);
    telegram_qml_indexCache.clear();

    TelegramQmlInitializer::init("TelegramQml", true);

    QString index = QString("# TelegramQml Documents\n\n");
    index += "### How to import:\n\n";
    index += "```c++\nimport TelegramQml 2.0\n```\n\nor\n\n"
             "```c++\nimport TelegramQml 2.0 as Telegram\n```\n\n";
    index += "### Backward Compatible:\n\n";
    index += "Backward compatible components will added soon. But we suggest to "
             "user our new components and tools.\n\n";
    index += "### Types\n\nHere are all components of the TelegramQml:\n\n";
    Q_FOREACH(const QString cmpnt, telegram_qml_indexCache)
    {
        if(cmpnt == "Engine")
            index += "\n##### Login and Auth\n\n";
        if(cmpnt == "Image")
            index += "\n##### File handler\n\n";
        if(cmpnt == "MessageFetcher")
            index += "\n##### Other\n\n";
        if(cmpnt == "TqObject")
            index += "\n##### Base\n\n";
        if(cmpnt == "DialogListModel")
            index += "\n##### Models\n\n";
        index += QString(" * [%1](%2.md)\n").arg(cmpnt).arg(cmpnt.toLower());
    }

    QString path = telegram_qml_destination + "/start.md";
    telegram_qml_destination.clear();
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(index.toUtf8());
    file.close();
}

QString TelegramQmlInitializer::fixType(const QString &type)
{
    if(type == "QSizeF" || type == "QSize")
        return "size";
    if(type == "QPoint" || type == "QPointF")
        return "point";
    if(type == "QUrl")
        return "url";
    if(type == "QString")
        return "string";
    if(type == "QByteArray")
        return "byte";
    if(type == "bool")
        return "boolean";
    if(type == "double" || type == "qreal")
        return "real";
    if(type == "QVariant")
        return "variant";
    if(type == "QJSValue")
        return "function(){[code]}";
    if(type == "QVariantMap")
        return "map";
    if(type == "QVariantList")
        return "list&lt;variant&gt;";
    if(type == "QStringList")
        return "list&lt;string&gt;";
    if(type == "QList<qint32>")
        return "list&lt;int&gt;";
    if(type.contains("*"))
    {
        if(type == "QObject*")
            return "object";
        if(type.contains("Telegram"))
        {
            QString name = QString(type).remove("*").remove("Telegram");
            return QString("[%1](%2.md)").arg(name).arg(name.toLower());
        }
        if(type.contains("Object"))
        {
            QString name = QString(type).remove("*").remove("Object");
            return QString("[%1](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/%2.md)")
                    .arg(name).arg(name.toLower());
        }
        if(type == "QQmlComponent*")
            return "Component";
        if(type == "QQuickItem")
            return "Item";
    }
    if(type.contains("Telegram"))
    {
        QString name = QString(type).remove("Telegram");
        return QString("[%1](%2.md)").arg(name).arg(name.toLower());
    }
    if(type == "QQuickItem")
        return "Item";
    if(type == "QObject")
        return "object";
    if(type.contains("Object"))
    {
        QString name = QString(type).remove("Object");
        return QString("[%1](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/%2.md)")
                .arg(name).arg(name.toLower());
    }
    return type;
}

template<typename T>
QString TelegramQmlInitializer::exportItem(const QString &module, int major, int minor, const QString &component, bool store)
{
    QString result;
    telegram_qml_indexCache << component;

    QMetaObject meta = T::staticMetaObject;
    QString inherits = fixType(meta.superClass()? meta.superClass()->className() : "");
    bool isModel = component.toLower().contains("model");

    result += QString("# %1\n\n").arg(component);

    QString headers;
    headers += QString(" * [Component details](#component-details)\n");

    QString details = QString("\n### Component details:\n\n");
    details += QString("|Detail|Value|\n"
                       "|------|-----|\n");
    details += QString("|%1|%2 %3.%4|\n").arg("Import").arg(module).arg(major).arg(minor);
    details += QString("|%1|<font color='#074885'>%2</font>|\n").arg("Component").arg(component);
    details += QString("|%1|<font color='#074885'>%2</font>|\n").arg("C++ class").arg(meta.className());
    details += QString("|%1|<font color='#074885'>%2</font>|\n").arg("Inherits").arg(inherits);
    details += QString("|%1|<font color='#074885'>%2</font>|\n").arg("Model").arg(isModel?"Yes":"No");

    QString requiredProperties;
    QString resultProperties;
    QStringList requireds = T::requiredProperties();
    QStringList propertiesSignals;
    for(int i=0; i<meta.propertyCount(); i++)
    {
        QMetaProperty property = meta.property(i);
        const QString &propertyName = property.name();
        const QString &propertyType = fixType(property.typeName());
        propertiesSignals << property.notifySignal().name();

        QString text = QString("* <font color='#074885'><b>%1</b></font>: %2").arg(propertyName).arg(propertyType);
        if(!property.isWritable())
            text += " (readOnly)";

        text += "\n";
        if(requireds.contains(propertyName))
            requiredProperties += text;
        else
        if(meta.propertyOffset()<=i)
            resultProperties += text;
    }

    QString enumResults;
    for(int i=meta.enumeratorOffset(); i<meta.enumeratorCount(); i++)
    {
        QMetaEnum enumerator = meta.enumerator(i);
        const QString &enumName = enumerator.name();

        enumResults += QString("\n##### %1\n\n").arg(enumName);
        enumResults += QString("|Key|Value|\n"
                               "|---|-----|\n");

        for(int j=0; j<enumerator.keyCount(); j++)
            enumResults += QString("|%1|%2|\n").arg(enumerator.key(j)).arg(enumerator.value(j));
    }

    QString resultSlots;
    QString resultSignals;
    for(int i=meta.methodOffset(); i<meta.methodCount(); i++)
    {
        QMetaMethod method = meta.method(i);
        if(method.access() != QMetaMethod::Public)
            continue;

        const QString &methodName = method.name();
        if(propertiesSignals.contains(methodName))
            continue;

        const QString &methodType = fixType(method.typeName());

        QString args;
        const QList<QByteArray> &paramNames = method.parameterNames();
        const QList<QByteArray> &paramTypes = method.parameterTypes();
        for(int j=0; j<paramNames.count(); j++)
        {
            if(j != 0)
                args += ", ";

            args += fixType(paramTypes[j]) + " " + paramNames[j];
        }

        QString text = QString(" * %1 <font color='#074885'><b>%2</b></font>(%3)\n").arg(methodType).arg(methodName).arg(args);
        switch(static_cast<int>(method.methodType()))
        {
        case QMetaMethod::Slot:
            resultSlots += text;
            break;
        case QMetaMethod::Signal:
            resultSignals += text;
            break;
        }
    }

    if(!requiredProperties.isEmpty())
    {
        headers += QString(" * [Required Properties](#required-properties)\n");
        requiredProperties = QString("\n### Required Properties\n\n") + requiredProperties;
    }
    if(!resultProperties.isEmpty())
    {
        headers += QString(" * [Normal Properties](#normal-properties)\n");
        resultProperties = QString("\n### Normal Properties\n\n") + resultProperties;
    }
    if(!enumResults.isEmpty())
    {
        headers += QString(" * [Enumerator](#enumerator)\n");
        enumResults = QString("\n### Enumerator\n\n") + enumResults;
    }
    if(!resultSlots.isEmpty())
    {
        headers += QString(" * [Methods](#methods)\n");
        resultSlots = QString("\n### Methods\n\n") + resultSlots;
    }
    if(!resultSignals.isEmpty())
    {
        headers += QString(" * [Signals](#signals)\n");
        resultSignals = QString("\n### Signals\n\n") + resultSignals;
    }
    if(isModel)
        headers += QString(" * [Roles](#roles)\n");

    result += headers + "\n";
    result += details + "\n";
    result += requiredProperties + "\n";
    result += resultProperties + "\n";
    result += resultSlots + "\n";
    result += resultSignals + "\n";
    result += enumResults + "\n";

    if(!store)
        return result;

    QString path = telegram_qml_destination + "/" + component.toLower() + ".md";
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
        return result;

    file.write(result.toUtf8());
    file.close();
    return result;
}

template<typename T>
QString TelegramQmlInitializer::exportModel(const QString &module, int major, int minor, const QString &component)
{
    QString result = exportItem<T>(module, major, minor, component, false);
    T *model = new T();

    QHash<qint32,QByteArray> roleNames = model->roleNames();
    QMap<qint32,QByteArray> rolesMap;
    QHashIterator<qint32,QByteArray> i(roleNames);
    while(i.hasNext())
    {
        i.next();
        rolesMap[i.key()] = i.value();
    }

    result += "\n### Roles\n\n";
    Q_FOREACH(const QByteArray &name, rolesMap)
        result += QString(" * model.<font color='#074885'>%1</font>\n").arg(QString(name));

    delete model;

    QString path = telegram_qml_destination + "/" + component.toLower() + ".md";
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
        return result;

    file.write(result.toUtf8());
    file.close();
    return result;
}

