import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
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
    property MessageTypesPool messageTypesPool: MessageTypesPool { }
    property Context context
    property SailorgramDialogItem sgDialogItem
    property SailorgramForwardMessage sgForwardMessage: null
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

        for(var i = 0; i < messagesmodel.count; i++)
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
        messagemodel.deleteMessages(ids);
    }

    model: SailorgramMessageModel {
        id: messagemodel
        engine: context.engine
        currentPeer: sgDialogItem.peer
        limit: 50

        onCountChanged: {
            if((count <= 0) || (Qt.application.state !== Qt.ApplicationActive))
                return;

            messagemodel.markAsRead(); // We are in this chat, always mark these messages as read
        }
    }

    onSelectionModeChanged: {
        if(!selectionMode)
            clearSelection();
    }

    delegate: MessageItem {
        messageTypesPool: messageview.messageTypesPool
        context: messageview.context
        sgDialogItem: messageview.sgDialogItem
        sgMessageItem: model.item
        selected: selectedItems.indexOf(model.index) > -1
        highlighted: messageview.selectionMode ? selected : pressed
        showMenuOnPressAndHold: !messageview.selectionMode

        onReplyRequested: {
            messageview.headerItem.dialogReplyPreview.isForward = false;
            messageview.headerItem.dialogReplyPreview.sgMessageItem = model.item;
        }

        onForwardRequested: {
            var fwdpage = pageStack.push(Qt.resolvedUrl("../../pages/dialogs/forward/ForwardDialogPage.qml"), { "context": messageview.context });

            fwdpage.forwardRequested.connect(function(sgdialogitem) {
                if(sgdialogitem.peer === sgDialogItem.peer) { // Check if it is forwared is this chat
                    messageview.sgForwardMessage = messagemodel.prepareForward(model.item);
                    pageStack.pop();
                    return;
                }

                // FIXME: Forward Destination property is missing
                pageStack.replaceAbove(context.mainPage, Qt.resolvedUrl("../../pages/dialogs/DialogPage.qml"), { "context": messageview.context,
                                                                                                                 "sgDialogItem": sgdialogitem,
                                                                                                                 "sgForwardMessage": messagemodel.prepareForward(model.item) });
            });
        }

        onDeleteRequested: {
            remorseAction(qsTr("Deleting message..."), function() {
                messagemodel.deleteMessages([model.item.id]);
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

        id: bottomarea
        width: messageview.width
        spacing: Theme.paddingMedium
        height: messageview.selectionMode ? 0 : undefined

        Behavior on height { NumberAnimation { duration: 250 } }

        DialogReplyPreview {
            id: dialogreplypreview
            width: parent.width
            context: messageview.context
            sgMessageItem: sgForwardMessage ? sgForwardMessage.message : null
            isForward: sgForwardMessage !== null

            onCloseRequested: {
                dialogreplypreview.sgMessageItem = null;
                dialogtextinput.isForward = false;
                messagemodel.dismissForward(sgForwardMessage);
            }

            onSgMessageItemChanged: {
                if(!sgMessageItem)
                    return;

                dialogtextinput.focusTextArea();
                messageview.scrollToBottom();
            }
        }

        DialogTextInput {
            id: dialogtextinput
            width: parent.width
            sgMessageModel: messagemodel
            context: messageview.context
            sgReplyMessage: dialogreplypreview.sgMessageItem
            isForward: sgForwardMessage !== null
            visible: !discadedDialog && !waitingDialog
            onForwardRequested: messagemodel.forwardMessage(sgForwardMessage); // Forward in this chat

            onMessageSent: {
                messageview.sgForwardMessage = null;
            }
        }

        SecretDialogDiscarded {
            id: secretdialogdiscarded
            width: parent.width
            visible: discadedDialog
            sgDialogItem: messageview.sgDialogItem
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

            messagemodel.markAsRead();
        }
    }

    VerticalScrollDecorator { flickable: messageview }
    TelegramBackground { visible: !context.backgrounddisabled; z: -1 }
}
