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
        text: parent.muted ? qsTr("Enable Notifications") : qsTr("Disable Notifications")

        onClicked: {
            var peerid = TelegramHelper.peerId(dialog);

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
            var msg = qsTr("Deleting Conversation");

            if(dialog.encrypted)
                msg = qsTr("Deleting Secret Chat");
            else if(TelegramHelper.isChat(dialog))
                msg = qsTr("Deleting Group");

            remorsepopup.execute(msg, function() {
                dialog.unreadCount = 0;

                if(dialog.encrypted)
                    context.telegram.messagesDiscardEncryptedChat(dialog.peer.userId);
                else
                    context.telegram.messagesDeleteHistory(TelegramHelper.peerId(dialog));

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
                var peerid = TelegramHelper.peerId(dialog);
                context.telegram.messagesDeleteChatUser(peerid, context.telegram.me);
                context.telegram.messagesDeleteHistory(peerid);
                pageStack.pop();
            });
        }
    }

    /*
    MenuItem
    {
        text: qsTr("Change Picture")
        visible: TelegramHelper.isChat(dialog)

        onClicked: {
            var picker = pageStack.push(Qt.resolvedUrl("../picker/FilePickerPage.qml"), { "rootPage": conversationpage, "mime": "image" })

            picker.filePicked.connect(function(file) {
            });
        }
    }
    */

    MenuItem
    {
        text: qsTr("Change Title")
        visible: TelegramHelper.isChat(dialog)
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/chat/ChangeChatTitle.qml"), { "context": conversationmenu.context, "dialog": conversationmenu.dialog })
    }

    MenuItem
    {
        text: qsTr("Add Member")
        visible: TelegramHelper.isChat(dialog)
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/chat/AddContactsPage.qml"), { "context": conversationmenu.context, "dialog": conversationmenu.dialog })
    }

    MenuItem
    {
        text: qsTr("Add to Contacts")
        visible: !TelegramHelper.isChat(dialog) && !TelegramHelper.isTelegramUser(user) && (user.classType === TelegramConstants.typeUserRequest)
        onClicked: context.telegram.addContact(user.firstName, user.lastName, user.phone)
    }
}

