import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components/telegram"
import "../../components/message"
import "../../items/dialog"
import "../../items/secretdialog"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

SilicaListView
{
    property bool waitingUserName: false
    property bool discadedDialog: false
    property bool waitingDialog: false
    property Context context
    property Dialog dialog
    property Message forwardedMessage
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
            messageview.headerItem.dialogReplyPreview.isForward = false;
            messageview.headerItem.dialogReplyPreview.message = item;
        }

        onForwardRequested: {
            var fwdpage = pageStack.push(Qt.resolvedUrl("../../pages/dialogs/forward/ForwardDialogPage.qml"), { "context": messageview.context });

            fwdpage.forwardRequested.connect(function(peerid, iscontact) {
                if(TelegramHelper.peerId(messageview.dialog) === peerid) {
                    messageview.headerItem.dialogReplyPreview.isForward = true;
                    messageview.headerItem.dialogReplyPreview.message = item;
                    pageStack.pop();
                    return;
                }

                var dialog = iscontact ? context.telegram.fakeDialogObject(peerid, false) : context.telegram.dialog(peerid);
                pageStack.replaceAbove(context.mainPage, Qt.resolvedUrl("../../pages/dialogs/DialogPage.qml"), { "context": context, "dialog": dialog, "forwardedMessage": item });
            });
        }
    }

    header: Column {
        property alias dialogReplyPreview: dialogreplypreview
        property alias dialogTextInput: dialogtextinput

        id: bottomarea
        width: messageview.width
        spacing: Theme.paddingMedium

        DialogReplyPreview {
            id: dialogreplypreview
            width: parent.width
            context: messageview.context
            dialog: messageview.dialog
            isForward: forwardedMessage !== null
            message: forwardedMessage

            onCloseRequested: {
                dialogtextinput.isForward = false;
            }

            onMessageChanged: {
                if(!message)
                    return;

                dialogtextinput.focusTextArea();
                messageview.scrollToBottom();
            }
        }

        DialogTextInput {
            id: dialogtextinput
            width: parent.width
            messagesModel: messagesmodel
            context: messageview.context
            dialog: messageview.dialog
            isForward: forwardedMessage !== null
            replyMessage: dialogreplypreview.message
            visible: !discadedDialog && !waitingDialog
            onForwardRequested: context.telegram.forwardMessages([dialogreplypreview.message.id], TelegramHelper.peerId(messageview.dialog)); // Forward in this chat

            onMessageSent: {
                forwardedMessage = null;
                dialogreplypreview.message = null;
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
