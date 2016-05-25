import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
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
    property bool selectionMode: false
    property Context context
    property var dialogModelItem
    property Message forwardedMessage
    property MessageTypesPool messageTypesPool: MessageTypesPool { }
    property var selectedItems: []

    id: messageview
    currentIndex: -1
    verticalLayoutDirection: ListView.BottomToTop
    cacheBuffer: Screen.height
    clip: true

    function markAsRead() {
        messagesmodel.markAsRead();
    }

    function loadBack() {
        messagesmodel.loadBack();
    }

    function selectAll() {
        if(!selectionMode)
            return;
        for(var i = 0; i < messagesmodel.count; ++i)
            selectedItems.push(i);
        selectedItemsChanged();
    }

    function clearSelection() {
        selectedItems = [];
    }

    function deleteSelected() {
        if(!selectionMode)
            return;

        var ids = [];

        selectedItems.forEach(function(i) { ids.push(messagesmodel.get(i)["item"].id) }); //FIXME: !!!
        clearSelection();
        messagesmodel.deleteMessages(ids);
    }

    onSelectionModeChanged: {
        if(!selectionMode)
            clearSelection();
    }

    model: MessageListModel {
        id: messagesmodel
        engine: context.engine
        currentPeer: dialogModelItem.peer

        onCountChanged: {
            if((count <= 0) || (Qt.application.state !== Qt.ApplicationActive))
                return;

            messagesmodel.markAsRead(); // We are in this chat, always mark these messages as read
        }
    }

    delegate: MessageItem {
        context: messageview.context
        messageTypesPool: messageview.messageTypesPool
        dialogModelItem: messageview.dialogModelItem
        messageModelItem: model
        selected: selectedItems.indexOf(model.index) > -1
        highlighted: messageview.selectionMode ? selected : pressed
        showMenuOnPressAndHold: !messageview.selectionMode

        onReplyRequested: {
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

        onClicked: {
            if(messageview.selectionMode)
            {
                if(selected)
                    messageview.selectedItems.splice(messageview.selectedItems.indexOf(model.index), 1);
                else
                    messageview.selectedItems.push(model.index);
                selectedItemsChanged();
            }
        }
    }

    header: Column {
        property alias dialogReplyPreview: dialogreplypreview
        property alias dialogTextInput: dialogtextinput

        id: bottomarea
        width: messageview.width
        spacing: Theme.paddingMedium
        height: messageview.selectionMode ? 0 : undefined

        Behavior on height { NumberAnimation { duration: 250 } }


        DialogReplyPreview {
            id: dialogreplypreview
            width: parent.width
            context: messageview.context
            //FIXME: dialog: messageview.dialog
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
            messageListModel: messagesmodel
            context: messageview.context
            peer: dialogModelItem.peer
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
