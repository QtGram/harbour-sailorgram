.pragma library

.import "TelegramConstants.js" as TelegramConstants
.import "TelegramHelper.js" as TelegramHelper

function actionType(telegram, action) {
   if(!action)
       return "";

   switch(action.classType) {
        case TelegramConstants.typeMessageActionChatCreate:
            return qsTr("Group created");

        case TelegramConstants.typeMessageActionChatDeleteUser:
            var user = telegram.user(action.userId);
            return qsTr("%1 has left the group").arg(TelegramHelper.userName(user));

        default:
            break;
   }

   return "";
}
