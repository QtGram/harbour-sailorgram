.pragma library

.import "TelegramConstants.js" as TelegramConstants
.import "TelegramHelper.js" as TelegramHelper

function actionType(telegram, dialog, message) {
   var action = message.action;

   if(!action)
       return "";

   if(action.classType === TelegramConstants.typeMessageActionChatCreate) {
       var userid = message.fromId;

       if(dialog.encrypted) {
           var chat = telegram.encryptedChat(dialog.peer.userId);
           userid = chat.adminId;
       }

       return qsTr("%1 created by %2").arg(dialog.encrypted ? qsTr("Secret chat") : qsTr("Group")).arg(TelegramHelper.completeName(telegram.user(userid)));
   }

   if(action.classType === TelegramConstants.typeMessageActionChatAddUser)
       return qsTr("%1 has joined the group").arg(TelegramHelper.completeName(telegram.user(action.userId)));

   if(action.classType === TelegramConstants.typeMessageActionChatDeleteUser)
       return qsTr("%1 has left the group").arg(TelegramHelper.completeName(telegram.user(action.userId)));

   return "";
}
