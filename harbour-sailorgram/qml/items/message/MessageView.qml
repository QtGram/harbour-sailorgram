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
    property alias waitingUserName: secretdialogwaiting.waitingUser
    property bool discadedDialog: false
    property bool waitingDialog: false
    property bool displayKeyboard: false
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

    onAtYEndChanged: {
        if(!atYEnd || !displayKeyboard)
            return;

        dialogtextinput.focusTextArea();
        displayKeyboard = false;
    }

    header: Item {
        id: headeritem
        width: messageview.width

        height: {
            var h = 0;

            if(dialogtextinput.visible)
                h += dialogtextinput.height;

            if(dialogreplypreview.visible)
                h += dialogreplypreview.height;

            if(!dialogtextinput.visible && !dialogreplypreview.visible)
                h = headerarea.height;

            return h;
        }
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
            context: messageview.context
            dialog: messageview.dialog
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
