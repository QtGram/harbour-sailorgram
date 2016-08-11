#include "sailorgraminitializer.h"
#include "../telegram/model/sailorgramdialogsmodel.h"
#include "../telegram/model/sailorgramdialogscovermodel.h"
#include "../telegram/item/sailorgramdialogitem.h"
#include "../telegram/object/sailorgrammessageaction.h"
#include "../telegram/sailorgramenums.h"
#include <QtQuick>

SailorgramInitializer::SailorgramInitializer()
{

}

void SailorgramInitializer::init(const char *uri, int major, int minor)
{
    qmlRegisterUncreatableType<SailorgramMessageAction>(uri, major, minor, "SailorgramMessageAction", QString());
    qmlRegisterUncreatableType<SailorgramEnums>(uri, major, minor, "SailorgramEnums", QString());

    qmlRegisterType<SailorgramDialogsModel>(uri, major, minor, "SailorgramDialogsModel");
    qmlRegisterType<SailorgramDialogsCoverModel>(uri, major, minor, "SailorgramDialogsCoverModel");

    qmlRegisterType<SailorgramDialogItem>(uri, major, minor, "SailorgramDialogItem");
}

