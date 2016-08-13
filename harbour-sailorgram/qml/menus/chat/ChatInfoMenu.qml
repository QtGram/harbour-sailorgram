import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"

ContextMenu
{
    property bool admin: false
    property Context context
    property User user
    property var peer

    id: chatinfomenu

    MenuItem
    {
        text: qsTr("Remove from Group")
        visible: chatinfomenu.admin

        onClicked: {
            liparticipant.remorseAction(qsTr("Removing from group"), function() {
                //FIXME: context.telegram.messagesDeleteChatUser(TelegramHelper.peerId(dialog), user.id);
            });
        }
    }

    MenuItem
    {
        text: qsTr("Profile")
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/contacts/ContactPage.qml"), { "context": chatinfomenu.context, "user": chatinfomenu.user })
    }
}
