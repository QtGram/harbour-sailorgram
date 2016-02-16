import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/dialog"
import "../../items/message"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

Page
{
    property Context context
    property Dialog dialog
    property Chat chat
    property EncryptedChat encryptedChat
    property User user

    id: dialogpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        var peerid = TelegramHelper.peerId(dialog);

        if(dialog.encrypted) {
            encryptedChat = context.telegram.encryptedChat(peerid);
            user = context.telegram.user(TelegramHelper.encryptedUserId(context, encryptedChat));
            return;
        }

        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(peerid);
        else
            user = context.telegram.user(peerid);
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        if(!canNavigateForward)
            pageStack.pushAttached(Qt.resolvedUrl("DialogInfoPage.qml"), { "context": dialogpage.context, "dialog": dialogpage.dialog, "chat": dialogpage.chat, "user": dialogpage.user });

        messageview.dialog = dialogpage.dialog
        messageview.setReaded();

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
                onClicked: messageview.loadMore();
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
            id: messageview
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
            context: dialogpage.context

            discadedDialog: {
                if(!dialogpage.encryptedChat || dialogpage.dialog.encrypted)
                    return false;

                return dialogpage.encryptedChat.classType === TelegramConstants.typeEncryptedChatDiscarded;
            }

            waitingDialog: {
                if(!dialogpage.encryptedChat || dialogpage.dialog.encrypted)
                    return false;

                return dialogpage.encryptedChat.classType === TelegramConstants.typeEncryptedChatWaiting;
            }

            waitingUserName: {
                if(!dialogpage.encryptedChat || !dialogpage.user || dialogpage.dialog.encrypted)
                    return "";

                if(dialogpage.encryptedChat !== TelegramConstants.typeEncryptedChatWaiting)
                    return "";

                return TelegramHelper.completeName(dialogpage.user);
            }
        }
    }
}
