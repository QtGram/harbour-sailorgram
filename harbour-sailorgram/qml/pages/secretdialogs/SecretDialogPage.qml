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

        pageStack.pushAttached(Qt.resolvedUrl("../dialogs/DialogInfoPage.qml"), { "context": secretdialogpage.context, "dialog": secretdialogpage.dialog, "user": secretdialogpage.user });

        messagesmodel.telegram = secretdialogpage.context.telegram;
        messagesmodel.dialog = secretdialogpage.dialog;
        messagesmodel.setReaded();

        context.sailorgram.foregroundDialog = secretdialogpage.dialog;
        context.sailorgram.closeNotification(dialog);
    }

    RemorsePopup { id: remorsepopup }

    Connections
    {
        target: Qt.application

        onStateChanged: {
            if(Qt.application.state !== Qt.ApplicationActive)
                return;

            messagesmodel.setReaded();
        }
    }

    Timer
    {
        id: refreshtimer
        repeat: true
        interval: 10000
        running: !context.sailorgram.daemonized
        onTriggered: messagesmodel.refresh()
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
                onClicked: messagesmodel.loadMore();
            }
        }

        BusyIndicator
        {
            anchors.centerIn: parent
            size: BusyIndicatorSize.Large
            running: context.sailorgram.connected && messagesmodel.refreshing && (messageview.count <= 0)
            z: running ? 2 : 0
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

            model: MessagesModel {
                id: messagesmodel
                stepCount: context.stepcount

                onCountChanged: {
                    if((count <= 0) || (Qt.application.state !== Qt.ApplicationActive))
                        return;

                    messagesmodel.setReaded(); /* We are in this chat, always mark these messages as read */
                }
            }

            delegate: MessageItem {
                context: secretdialogpage.context
                messageTypesPool: messageview.messageTypesPool
                message: item
            }

            header: Item {
                width: messageview.width
                height: dialogtextinput.visible ? dialogtextinput.height : headerarea.height
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
                    context: secretdialogpage.context
                    dialog: secretdialogpage.dialog
                    visible: chat && (chat.classType !== TelegramConstants.typeEncryptedChatDiscarded) && (chat.classType !== TelegramConstants.typeEncryptedChatWaiting)
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
