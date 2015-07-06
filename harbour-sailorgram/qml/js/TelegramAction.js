.pragma library

.import "TelegramConstants.js" as TelegramConstants
.import "TelegramHelper.js" as TelegramHelper

function actionType(telegram, action) {
   if(!action)
       return "";

   switch(action.classType) {
        case TelegramConstants.typeMessageActionChatCreate:
            return qsTr("Group created");

        case TelegramConstants.typeMessageActionChatAddUser:
            return qsTr("%1 has joined the group").arg(TelegramHelper.userName(telegram.user(action.userId)));

        case TelegramConstants.typeMessageActionChatDeleteUser:
            return qsTr("%1 has left the group").arg(TelegramHelper.userName(telegram.user(action.userId)));

        default:
            break;
   }

   return "";
}
