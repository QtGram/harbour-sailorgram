import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../components/telegram"
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

TelegramPullDownMenu
{
    property Dialog dialog

    id: conversationmenu

    MenuItem
    {
        text: secretconversationpage.muted ? qsTr("Enable Notifications") : qsTr("Disable Notifications")

        onClicked: {
            var peerid = TelegramHelper.peerId(dialog.peer);

            if(secretconversationpage.muted)
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
            remorsepopup.execute(dialog.encrypted ? qsTr("Deleting Secret Chat") : qsTr("Deleting History"), function() {
                if(dialog.encrypted)
                    context.telegram.messagesDiscardEncryptedChat(dialog.peer.userId);
                else
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

