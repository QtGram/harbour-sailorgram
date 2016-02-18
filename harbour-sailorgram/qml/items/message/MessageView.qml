import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components/telegram"
import "../../components/message"
import "../../items/dialog"
import "../../items/secretdialog"
import "../../items/message/messageitem"

SilicaListView
{
    property bool waitingUserName: false
    property bool discadedDialog: false
    property bool waitingDialog: false
    property Context context
    property Dialog dialog
    property MessageTypesPool messageTypesPool: MessageTypesPool { }

    id: messageview
    currentIndex: -1
    verticalLayoutDirection: ListView.BottomToTop
    spacing: Theme.paddingMedium
    cacheBuffer: Screen.height
    clip: true

    function setReaded() {
        messagesmodel.setReaded();
    }

    function loadMore() {
        messagesmodel.loadMore();
    }

    model: MessagesModel {
        id: messagesmodel
        stepCount: context.stepcount
        telegram: context.telegram
        dialog: messageview.dialog

        onCountChanged: {
            if((count <= 0) || (Qt.application.state !== Qt.ApplicationActive))
                return;

            messagesmodel.setReaded(); /* We are in this chat, always mark these messages as read */
        }
    }

    delegate: MessageItem {
        context: messageview.context
        messageTypesPool: messageview.messageTypesPool
        dialog: messageview.dialog
        message: item

        onReplyRequested: {
            messageview.headerItem.dialogReplyPreview.isForward = false;
            messageview.headerItem.dialogReplyPreview.message = item;
        }

        onForwardRequested: {
            messageview.headerItem.dialogReplyPreview.isForward = true;
            messageview.headerItem.dialogReplyPreview.message = item;
            messageview.headerItem.messageFwdGridView.message = item;
        }
    }

    header: Column {
        property alias dialogReplyPreview: dialogreplypreview
        property alias messageFwdGridView: messagefwdgridview

        id: bottomarea
        width: messageview.width
        spacing: Theme.paddingMedium

        DialogReplyPreview {
            id: dialogreplypreview
            width: parent.width
            context: messageview.context
            dialog: messageview.dialog

            onCloseRequested: {
                messagefwdgridview.message = null;
            }

            onVisibleChanged: {
                if(!visible)
                    return;

                dialogtextinput.focusTextArea();
                messageview.scrollToBottom();
            }
        }

        MessageForwardGridView {
            id: messagefwdgridview
            width: parent.width
            context: messageview.context

            onMessageForwarded: {
                dialogreplypreview.message = null;
            }
        }

        DialogTextInput {
            id: dialogtextinput
            width: parent.width
            messagesModel: messagesmodel
            context: messageview.context
            dialog: messageview.dialog
            replyMessage: dialogreplypreview.message
            visible: !discadedDialog && !waitingDialog

            onMessageSent: {
                dialogreplypreview.message = null;
                messagefwdgridview.message = null;
            }
        }

        SecretDialogDiscarded {
            id: secretdialogdiscarded
            width: parent.width
            visible: discadedDialog
        }

        SecretDialogWaiting {
            id: secretdialogwaiting
            width: parent.width
            visible: waitingDialog
            waitingUser: waitingUserName
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

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: context.sailorgram.connected && messagesmodel.refreshing && (messagesmodel.count <= 0)
        z: running ? 2 : 0
    }

    Connections
    {
        target: Qt.application

        onStateChanged: {
            if(Qt.application.state !== Qt.ApplicationActive)
                return;

            messagesmodel.setReaded();
        }
    }

    VerticalScrollDecorator { flickable:  messageview }
    TelegramBackground { visible: !context.backgrounddisabled; z: -1 }
}
