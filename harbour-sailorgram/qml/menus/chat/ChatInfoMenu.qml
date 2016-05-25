import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

ContextMenu
{
    property Context context
    property Dialog dialog
    property User user

    id: chatinfomenu

    MenuItem
    {
        text: qsTr("Remove from Group")

        onClicked: {
            liparticipant.remorseAction(qsTr("Removing from group"), function() {
                context.telegram.messagesDeleteChatUser(TelegramHelper.peerId(dialog), user.id);
            });
        }
    }
}
