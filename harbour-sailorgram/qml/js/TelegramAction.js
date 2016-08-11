.pragma library

.import harbour.sailorgram.TelegramQml 2.0 as Telegram
.import "TelegramHelper.js" as TelegramHelper

function actionType(messageaction, issecretchat) { //FIXME: ServiceUser not available in QML API
    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatCreate) {
        if(issecretchat === true)
            return qsTr("Secret chat created by «%1»").arg(TelegramHelper.completeName(messageaction.fromUser));

        return qsTr("Group created by «%1»").arg(TelegramHelper.completeName(messageaction.fromUser));
    }

    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatAddUser) {
        if(messageaction.fromUser)
            return qsTr("%1 added %2").arg(TelegramHelper.completeName(messageaction.fromUser)).arg(TelegramHelper.completeName(messageaction.toUser));

        return qsTr("%1 has joined the group").arg(TelegramHelper.completeName(messageaction.fromUser));
    }

    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatJoinedByLink)
        return qsTr("%1 has joined the group via invite link").arg(TelegramHelper.completeName(messageaction.fromUser));

    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatDeleteUser)
        return qsTr("%1 has left the group").arg(TelegramHelper.completeName(messageaction.fromUser));

    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatEditPhoto)
        return qsTr("«%1» updated group photo").arg(TelegramHelper.completeName(messageaction.fromUser));

    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatEditTitle)
        return qsTr("Group name changed to «%1»").arg(messageaction.title);

    if(messageaction.actionType === Telegram.MessageAction.TypeMessageActionChatMigrateTo) {
        if(messageaction.isChannel)
            return qsTr("The group was upgraded to a supergroup");

        if(messageaction.title.length > 0)
            return messageaction.title;
    }

    return "Unhandled Action: " + messageaction.actionType;
}
