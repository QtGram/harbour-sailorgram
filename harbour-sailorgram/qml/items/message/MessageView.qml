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
    property alias typing: messagemodel.typingUsers
    property alias statusText: messagemodel.statusText
    property bool waitingUserName: false
    property bool discadedDialog: false
    property bool waitingDialog: false
    property bool selectionMode: false
    property MessageTypesPool messageTypesPool: MessageTypesPool { }
    property Context context
    property Message forwardedMessage
    property string title
    property var peer
    property var chat
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

    MessageListModel {
        id: messagemodel
        engine: context.engine
        currentPeer: messageview.peer
        limit: 50

        onCountChanged: {
            if((count <= 0) || (Qt.application.state !== Qt.ApplicationActive))
                return;

            messagemodel.markAsRead(); // We are in this chat, always mark these messages as read
        }
    }

    model: messagemodel

    onSelectionModeChanged: {
        if(!selectionMode)
            clearSelection();
    }

    delegate: MessageItem {
        messageTypesPool: messageview.messageTypesPool
        context: messageview.context
        mediaItem: model.mediaItem
        chat: messageview.chat
        fromUser: model.fromUserItem
        serviceUser: null //FIXME: !!!
        dialogTitle: messageview.title
        messageText: model.message
        fileName: model.fileName
        fileMimeType: model.fileMimeType
        fileSize: model.fileSize
        fileDuration: model.fileDuration
        messageOut: model.out
        messageUnread: model.unread
        messageDateTime: model.dateTime
        messageType: model.messageType
        forwardFromPeer: model.forwardFromPeer
        forwardFromType: model.forwardFromType
        replyMessage: model.replyMessage
        replyPeer: model.replyPeer
        replyType: model.replyType
        serviceItem: model.serviceItem
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
            if(messageview.selectionMode) {
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
            messageListModel: messagemodel
            context: messageview.context
            peer: messageview.peer
            isForward: forwardedMessage !== null
            replyMessage: dialogreplypreview.message
            visible: !discadedDialog && !waitingDialog
            //FIXME: onForwardRequested: context.telegram.forwardMessages([dialogreplypreview.message.id], TelegramHelper.peerId(messageview.dialog)); // Forward in this chat

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

    Connections
    {
        target: Qt.application

        onStateChanged: {
            if(Qt.application.state !== Qt.ApplicationActive)
                return;

            messagesmodel.markAsRead();
        }
    }

    VerticalScrollDecorator { flickable: messageview }
    TelegramBackground { visible: !context.backgrounddisabled; z: -1 }
}
