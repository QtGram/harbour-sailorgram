.pragma library

.import harbour.sailorgram.TelegramQml 2.0 as Telegram
.import "TelegramHelper.js" as TelegramHelper

function actionType(serviceitem, fromuser, serviceuser, issecretchat) { //FIXME: ServiceUser not available in QML API
    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatCreate) {
        if(issecretchat === true)
            return qsTr("Secret chat created by «%1»").arg(TelegramHelper.completeName(fromuser));

        return qsTr("Group created by «%1»").arg(TelegramHelper.completeName(fromuser));
    }

    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatAddUser) {
        if(serviceuser)
            return qsTr("%1 added %2").arg(TelegramHelper.completeName(fromuser)).arg(TelegramHelper.completeName(serviceuser));

        return qsTr("%1 has joined the group").arg(TelegramHelper.completeName(fromuser));
    }

    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatJoinedByLink)
        return qsTr("%1 has joined the group via invite link").arg(TelegramHelper.completeName(fromuser));

    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatDeleteUser)
        return qsTr("%1 has left the group").arg(TelegramHelper.completeName(fromuser));

    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatEditPhoto)
        return qsTr("«%1» updated group photo").arg(TelegramHelper.completeName(fromuser));

    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatEditTitle)
        return qsTr("Group name changed to «%1»").arg(serviceitem.title);

    if(serviceitem.classType === Telegram.MessageAction.TypeMessageActionChatMigrateTo) {
        if(serviceitem.channelId)
            return qsTr("The group was upgraded to a supergroup");

        if(serviceitem.title.length > 0)
            return serviceitem.title;
    }

    return "Unhandled Action: " + serviceitem.classType;
}
