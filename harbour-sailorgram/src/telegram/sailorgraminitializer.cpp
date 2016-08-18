#include "sailorgraminitializer.h"
#include "../telegram/model/sailorgramdialogscovermodel.h"
#include "../telegram/model/sailorgramcontactsmodel.h"
#include "../telegram/model/sailorgramdialogsmodel.h"
#include "../telegram/model/sailorgrammessagemodel.h"
#include "../telegram/item/sailorgramdialogitem.h"
#include "../telegram/item/sailorgrammessageitem.h"
#include "../telegram/object/sailorgrampeer.h"
#include "../telegram/object/sailorgrammessageaction.h"
#include "../telegram/object/sailorgrammessagemedia.h"
#include "../telegram/object/sailorgramforwardmessage.h"
#include "../telegram/object/media/sailorgramwebpagemedia.h"
#include "../telegram/object/media/sailorgramgeopointmedia.h"
#include "../telegram/sailorgramnotifications.h"
#include "../telegram/sailorgramenums.h"
#include <QtQuick>

SailorgramInitializer::SailorgramInitializer()
{

}

void SailorgramInitializer::init(const char *uri, int major, int minor)
{
    qmlRegisterUncreatableType<SailorgramEnums>(uri, major, minor, "SailorgramEnums", QString());
    qmlRegisterUncreatableType<SailorgramWebPageMedia>(uri, major, minor, "SailorgramWebPageMedia", QString());
    qmlRegisterUncreatableType<SailorgramGeoPointMedia>(uri, major, minor, "SailorgramGeoPointMedia", QString());
    qmlRegisterUncreatableType<SailorgramMessageMedia>(uri, major, minor, "SailorgramMessageMedia", QString());
    qmlRegisterUncreatableType<SailorgramMessageAction>(uri, major, minor, "SailorgramMessageAction", QString());

    qmlRegisterType<SailorgramPeer>(uri, major, minor, "SailorgramPeer");
    qmlRegisterType<SailorgramNotifications>(uri, major, minor, "SailorgramNotifications");
    qmlRegisterType<SailorgramForwardMessage>(uri, major, minor, "SailorgramForwardMessage");
    qmlRegisterType<SailorgramDialogItem>(uri, major, minor, "SailorgramDialogItem");
    qmlRegisterType<SailorgramMessageItem>(uri, major, minor, "SailorgramMessageItem");

    qmlRegisterType<SailorgramDialogsCoverModel>(uri, major, minor, "SailorgramDialogsCoverModel");
    qmlRegisterType<SailorgramDialogsModel>(uri, major, minor, "SailorgramDialogsModel");
    qmlRegisterType<SailorgramContactsModel>(uri, major, minor, "SailorgramContactsModel");
    qmlRegisterType<SailorgramMessageModel>(uri, major, minor, "SailorgramMessageModel");
}
