import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../menus/dialog"
import "../../components"
import "../../items/peer"
import "../../items/dialog"
import "../../items/secretdialog"
import "../../items/message"
import "../../items/message/messageitem"
import "../../js/TelegramConstants.js" as TelegramConstants

Page
{
    property Context context
    property Dialog dialog
    property EncryptedChat chat: context.telegram.encryptedChat(dialog.peer.userId)
    property User user: context.telegram.user((chat.adminId === context.telegram.me) ? chat.participantId : chat.adminId)

    id: secretconversationpage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("../dialogs/DialogInfoPage.qml"), { "context": secretconversationpage.context, "dialog": secretconversationpage.dialog, "user": secretconversationpage.user });
        context.foregroundDialog = secretconversationpage.dialog;

        messagesmodel.telegram = secretconversationpage.context.telegram;
        messagesmodel.dialog = secretconversationpage.dialog;
        messagesmodel.setReaded();
    }

    RemorsePopup { id: remorsepopup }

    Timer
    {
        id: refreshtimer
        repeat: true
        interval: 10000
        onTriggered: messagesmodel.refresh()
        Component.onCompleted: start()
    }

    PopupMessage
    {
        id: popupmessage
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: messagesmodel.refreshing
    }

    PeerItem
    {
        id: header
        visible: !context.chatheaderhidden
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: Theme.paddingMedium }
        height: context.chatheaderhidden ? 0 : Theme.itemSizeSmall
        context: secretconversationpage.context
        dialog: secretconversationpage.dialog
        user: secretconversationpage.user
    }

    MessageView
    {
        id: messageview
        anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: bottomcontainer.top; topMargin: Theme.paddingSmall }
        context: secretconversationpage.context

        onAtYBeginningChanged: {
            if(!atYBeginning || messagesmodel.refreshing || (messageview.count <= 0))
                return;

            messagesmodel.loadMore();
        }

        model: MessagesModel {
            id: messagesmodel

            onCountChanged: {
                if(!count)
                    return;

                messagesmodel.setReaded(); /* We are in this chat, always mark these messages as read */
            }
        }

        delegate: MessageItem {
            context: secretconversationpage.context
            message: item
        }

        header: Item {
            width: messageview.width
            height: dialogtextinput.visible ? dialogtextinput.height : Theme.itemSizeSmall
        }

        Column {
            id: headerarea
            y: messageview.headerItem.y
            parent: messageview.contentItem
            width: parent.width

            DialogTextInput {
                id: dialogtextinput
                width: parent.width
                messagesModel: messagesmodel
                context: dialogpage.context
                dialog: dialogpage.dialog
                visible: chat && (chat.classType !== TelegramConstants.typeEncryptedChatDiscarded) && (chat.classType !== TelegramConstants.typeEncryptedChatWaiting)
            }

            SecretDialogDiscarded {
                width: parent.width
                chat: secretconversationpage.chat
            }

            SecretDialogWaiting {
                width: parent.width
                chat: secretconversationpage.chat
                user: secretconversationpage.user
            }
        }
    }
}
