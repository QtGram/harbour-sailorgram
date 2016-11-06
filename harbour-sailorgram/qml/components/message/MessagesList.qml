import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/message/input"
import "../../items"
import "../../model"

SilicaListView
{
    property Message selectedMessage: null
    property string selectedMessageFrom
    property string selectedMessageText
    property bool replyMode: false
    property bool editMode: false

    function prepareActions(model) {
        messageslist.selectedMessage = model.item;
        messageslist.selectedMessageFrom = model.messageFrom;
        messageslist.selectedMessageText = model.messageText;
    }

    Connections
    {
        target: messagepanel

        onReplyRequested: {
            editMode = false;
            replyMode = true;

            headerItem.activateInput();
        }

        onEditRequested: {
            editMode = true;
            replyMode = false;

            headerItem.activateInput(selectedMessageText);
        }
    }

    id: messageslist
    spacing: Theme.paddingLarge
    cacheBuffer: Screen.height * 2
    verticalLayoutDirection: ListView.BottomToTop
    currentIndex: -1

    Component.onCompleted: messageslist.positionViewAtIndex(model.newMessageIndex, ListView.Center);

    header: Column {
        function activateInput(text) {
            if(text)
                messagetextinput.text = text;

            messagetextinput.focusTextArea();
        }

        function cancelActions() {
            replyMode = editMode = false;
            selectedMessageFrom = selectedMessageText = "";
            selectedMessage = null;
        }

        id: messagelistheader
        width: messageslist.width
        height: messagereplyinput.height + messagetextinput.height
        visible: messagesmodel.isWritable

        Item { width: parent.width; height: Theme.paddingSmall }

        MessageReplyInput {
            id: messagereplyinput
            replyMessage: replyMode ? selectedMessage : null
            replyFrom: replyMode ? selectedMessageFrom : ""
            replyText: replyMode ? selectedMessageText : ""
            width: parent.width
            onReplyCancelled: messagelistheader.cancelActions()
        }

        MessageTextInput {
            id: messagetextinput
            width: parent.width

            onSendMessage: {
                if(replyMode)
                    messagesmodel.replyMessage(message, selectedMessage)
                else if(editMode)
                    messagesmodel.editMessage(message, selectedMessage);
                else
                    messagesmodel.sendMessage(message);

                messagelistheader.cancelActions();
            }
        }
    }

    delegate: Column {
        width: parent.width
        spacing: Theme.paddingSmall

        NewMessage { id: newmessage; visible: model.isMessageNew }

        Row {
            width: parent.width

            Item {
                id: picontainer
                anchors.top: parent.top
                x: Theme.paddingSmall
                height: peerimage.height

                width: {
                    if(messagesmodel.isChat && !model.isMessageOut && !model.isMessageService)
                        return peerimage.size;

                    return 0;
                }

                PeerImage {
                    id: peerimage
                    size: Theme.iconSizeSmallPlus
                    peer: model.needsPeerImage ? model.item : null
                    visible: model.needsPeerImage
                    backgroundColor: Theme.secondaryHighlightColor
                    foregroundColor: Theme.primaryColor
                    fontPixelSize: Theme.fontSizeExtraSmall
                }
            }

            MessageModelItem {
                width: parent.width - picontainer.width - Theme.paddingSmall
                maxWidth: width * 0.8
                maxMediaWidth: Screen.width * 0.5

                onClicked: {
                    if(messagepanel.expanded) {
                        messagepanel.hide();
                        return;
                    }

                    prepareActions(model);

                    messagepanel.checkActions(messagesmodel.isWritable, model.isMessageOut)
                    messagepanel.show();
                }
            }
        }
    }

    VerticalScrollDecorator { flickable: messageslist }
}
