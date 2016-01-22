.pragma library

.import "TelegramConstants.js" as TelegramConstants
.import "TelegramHelper.js" as TelegramHelper

function actionType(telegram, dialog, message) {
   var action = message.action;

   if(!action)
       return "???";

   if(action.classType === TelegramConstants.typeMessageActionChatCreate) {
       var userid = message.fromId;

       if(dialog.encrypted) {
           var chat = telegram.encryptedChat(dialog.peer.userId);
           userid = chat.adminId;

           return qsTr("Secret chat created by «%1»").arg(TelegramHelper.completeName(telegram.user(userid)));
       }

       return qsTr("Group created by «%1»").arg(TelegramHelper.completeName(telegram.user(userid)));
   }

   if(action.classType === TelegramConstants.typeMessageActionChatAddUser) {
       if(message.fromId)
           return qsTr("%1 added %2").arg(TelegramHelper.completeName(telegram.user(message.fromId))).arg(TelegramHelper.completeName(telegram.user(action.userId)));

       return qsTr("%1 has joined the group").arg(TelegramHelper.completeName(telegram.user(action.userId)));
   }

   if(action.classType === TelegramConstants.typeMessageActionChatJoinedByLink) {
       if(action.inviterId) {
           var from = TelegramHelper.completeName(telegram.user(message.fromId))
           var inviter = TelegramHelper.completeName(telegram.user(action.inviterId));
           return qsTr("%1 added %2").arg(inviter).arg(from);
       }

       return qsTr("%1 has joined the group via invite link").arg(TelegramHelper.completeName(telegram.user(message.fromId)));
   }

   if(action.classType === TelegramConstants.typeMessageActionChatDeleteUser)
       return qsTr("%1 has left the group").arg(TelegramHelper.completeName(telegram.user(action.userId)));

   if(action.classType === TelegramConstants.typeMessageActionChatEditTitle)
       return qsTr("Group name changed to «%1»").arg(action.title);

   return "???";
}
