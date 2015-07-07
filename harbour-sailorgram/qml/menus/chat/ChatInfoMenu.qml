import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

ContextMenu
{
    property Settings settings
    property Dialog dialog
    property User user

    id: chatinfomenu

    MenuItem
    {
        text: qsTr("Remove from Group")

        onClicked: {
            liparticipant.remorseAction(qsTr("Removing from group"), function() {
                settings.telegram.messagesDeleteChatUser(TelegramHelper.peerId(dialog.peer), user.id);
            });
        }
    }
}
