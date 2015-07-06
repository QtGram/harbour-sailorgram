import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TelegramConstants.js" as TelegramConstants

PullDownMenu
{
    property Settings settings
    property Dialog dialog

    id: conversationmenu

    MenuItem
    {
        text: conversationpage.muted ? qsTr("Enable Notifications") : qsTr("Disable Notifications")

        onClicked: {
            var peerid = TelegramHelper.peerId(dialog.peer);

            if(conversationpage.muted)
                settings.telegram.userData.removeMute(peerid);
            else
                settings.telegram.userData.addMute(peerid);
        }
    }

    MenuItem
    {
        text: qsTr("Delete")

        onClicked: {
            remorsepopup.execute(qsTr("Deleting History"), function() {
                settings.telegram.messagesDeleteHistory(TelegramHelper.peerId(dialog.peer));
                pageStack.pop();
            });
        }
    }

    MenuItem
    {
        text: qsTr("Add Member")
        visible: TelegramHelper.isChat(dialog)
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/chat/AddContactsPage.qml"), { "settings": conversationmenu.settings, "dialog": conversationmenu.dialog })
    }

    MenuItem
    {
        text: qsTr("Add to Contacts")
        visible: !TelegramHelper.isChat(dialog) && !TelegramHelper.isTelegramUser(user) && (user.classType === TelegramConstants.typeUserRequest)
        onClicked: settings.telegram.addContact(user.firstName, user.lastName, user.phone)
    }
}

