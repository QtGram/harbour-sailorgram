import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

PullDownMenu
{
    property Context context
    property Dialog dialog

    id: conversationmenu

    MenuItem
    {
        text: conversationpage.muted ? qsTr("Enable Notifications") : qsTr("Disable Notifications")

        onClicked: {
            var peerid = TelegramHelper.peerId(dialog.peer);

            if(conversationpage.muted)
                context.telegram.userData.removeMute(peerid);
            else
                context.telegram.userData.addMute(peerid);
        }
    }

    MenuItem
    {
        text: qsTr("Delete")
        visible: !TelegramHelper.isChat(dialog)

        onClicked: {
            remorsepopup.execute(qsTr("Deleting History"), function() {
                context.telegram.messagesDeleteHistory(TelegramHelper.peerId(dialog.peer));
                pageStack.pop();
            });
        }
    }

    MenuItem
    {
        text: qsTr("Leave Group")
        visible: TelegramHelper.isChat(dialog)

        onClicked: {
            remorsepopup.execute(qsTr("Leaving Group"), function() {
                var peerid = TelegramHelper.peerId(dialog.peer);
                context.telegram.messagesDeleteChatUser(peerid, context.telegram.me);
                context.telegram.messagesDeleteHistory(peerid);
                pageStack.pop();
            });
        }

    }

    MenuItem
    {
        text: qsTr("Add Member")
        visible: TelegramHelper.isChat(dialog)
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/chat/AddContactsPage.qml"), { "context": conversationmenu.context, "dialog": conversationmenu.dialog })
    }

    MenuItem
    {
        text: qsTr("Add to Contacts")
        visible: !TelegramHelper.isChat(dialog) && !TelegramHelper.isTelegramUser(user) && (user.classType === TelegramConstants.typeUserRequest)
        onClicked: context.telegram.addContact(user.firstName, user.lastName, user.phone)
    }
}

