import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../menus/dialog"
import "../../components"
import "../../items/peer"
import "../../items/dialog"
import "../../items/message"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    id: dialogpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(dialog.peer.chatId);
        else
            user = context.telegram.user(dialog.peer.userId);
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        if(!canNavigateForward)
            pageStack.pushAttached(Qt.resolvedUrl("DialogInfoPage.qml"), { "context": dialogpage.context, "dialog": dialogpage.dialog, "chat": dialogpage.chat, "user": dialogpage.user });

        messageview.messagesModel.telegram = dialogpage.context.telegram;
        messageview.messagesModel.dialog = dialogpage.dialog;
        messageview.messagesModel.setReaded();

        context.sailorgram.foregroundDialog = dialogpage.dialog;
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
            height: context.chatheaderhidden ? 0 : (dialogpage.isPortrait ? Theme.itemSizeSmall : Theme.itemSizeExtraSmall)
            context: dialogpage.context
            dialog: dialogpage.dialog
            chat: dialogpage.chat
            user: dialogpage.user
        }

        MessageView
        {
            property bool displayKeyboard: false

            id: messageview
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
            context: dialogpage.context

            headerHeight: {
                var h = dialogtextinput.height;

                if(dialogreplypreview.visible)
                    h += dialogreplypreview.height;

                return h;
            }

            onAtYEndChanged: {
                if(!atYEnd || !displayKeyboard)
                    return;

                dialogtextinput.focusTextArea();
                displayKeyboard = false;
            }

            delegate: MessageItem {
                context: dialogpage.context
                messageTypesPool: messageview.messageTypesPool
                message: item

                onReplyRequested: {
                    messageview.displayKeyboard = true;
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
                    context: dialogpage.context
                }

                DialogTextInput {
                    id: dialogtextinput
                    width: parent.width
                    messagesModel: messageview.messagesModel
                    context: dialogpage.context
                    dialog: dialogpage.dialog
                    replyMessage: dialogreplypreview.message

                    onMessageSent: {
                        dialogreplypreview.message = null;
                    }
                }
            }
        }
    }
}
