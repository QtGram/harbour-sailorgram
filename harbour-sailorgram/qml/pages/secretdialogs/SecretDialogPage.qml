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
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property Dialog dialog
    property EncryptedChat chat: context.telegram.encryptedChat(dialog.peer.userId)
    property User user: context.telegram.user((chat.adminId === context.telegram.me) ? chat.participantId : chat.adminId)

    id: secretdialogpage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        if(!canNavigateForward)
            pageStack.pushAttached(Qt.resolvedUrl("../dialogs/DialogInfoPage.qml"), { "context": secretdialogpage.context, "dialog": secretdialogpage.dialog, "user": secretdialogpage.user });

        if(context.sailorgram.foregroundDialog === secretdialogpage.dialog)
            messageview.messagesModel.setReaded();

        messageview.messagesModel.telegram = secretdialogpage.context.telegram;
        messageview.messagesModel.dialog = secretdialogpage.dialog;

        context.sailorgram.foregroundDialog = secretdialogpage.dialog;
        context.sailorgram.closeNotification(dialog);
    }

    PopupMessage
    {
        id: popupmessage
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            id: pulldownmenu

            MenuItem
            {
                text: qsTr("Load more messages")
                onClicked: messageview.messagesModel.loadMore();
            }
        }

        PeerItem
        {
            id: header
            visible: !context.chatheaderhidden
            anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: context.chatheaderhidden ? 0 : Theme.paddingMedium }
            height: context.chatheaderhidden ? 0 : (secretdialogpage.isPortrait ? Theme.itemSizeSmall : Theme.itemSizeExtraSmall)
            context: secretdialogpage.context
            dialog: secretdialogpage.dialog
            user: secretdialogpage.user
        }

        MessageView
        {
            id: messageview
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
            context: secretdialogpage.context

            headerHeight: {
                var h = 0;

                if(dialogtextinput.visible)
                    h += dialogtextinput.height;

                if(dialogreplypreview.visible)
                    h += dialogreplypreview.height;

                if(!dialogtextinput.visible && !dialogreplypreview.visible)
                    h = headerarea.height;

                return h;
            }

            delegate: MessageItem {
                context: secretdialogpage.context
                messageTypesPool: messageview.messageTypesPool
                message: item

                onReplyRequested: {
                    dialogreplypreview.message = item;
                    messageview.scrollToBottom();
                }
            }

            Column {
                id: headerarea
                y: messageview.headerItem.y
                parent: messageview.contentItem
                width: parent.width

                DialogReplyPreview {
                    id: dialogreplypreview
                    width: parent.width
                    context: secretdialogpage.context
                }

                DialogTextInput {
                    id: dialogtextinput
                    width: parent.width
                    messagesModel: messageview.messagesModel
                    context: secretdialogpage.context
                    dialog: secretdialogpage.dialog
                    visible: chat && (chat.classType !== TelegramConstants.typeEncryptedChatDiscarded) && (chat.classType !== TelegramConstants.typeEncryptedChatWaiting)

                    onMessageSent: {
                        dialogreplypreview.message = null;
                    }
                }

                SecretDialogDiscarded {
                    width: parent.width
                    chat: secretdialogpage.chat
                }

                SecretDialogWaiting {
                    width: parent.width
                    chat: secretdialogpage.chat
                    user: secretdialogpage.user
                }
            }
        }
    }
}
