import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../message/input"

Column
{
    property Message selectedMessage: null
    property string selectedMessageFrom
    property string selectedMessageText
    property bool replyMode: false
    property bool editMode: false

    function prepareReply(model) {
        prepareActions(model);
        editMode = false;
        replyMode = true;
        activateInput();
    }

    function prepareEdit(model) {
        prepareActions(model);
        editMode = true;
        replyMode = false;
        activateInput(selectedMessageText);
    }

    function prepareActions(model) {
        selectedMessage = model.item;
        selectedMessageFrom = model.messageFrom;
        selectedMessageText = model.messageText;
    }

    function activateInput(text) {
        if(text)
            messagetextinput.text = text;

        messagetextinput.focusTextArea();
    }

    function cancelActions() {
        if(editMode)
            messagetextinput.text = "";

        replyMode = editMode = false;
        selectedMessageFrom = selectedMessageText = "";
        selectedMessage = null;
    }

    id: dialoginputpanel
    visible: !messageslist.selectionMode

    Item { width: parent.width; height: Theme.paddingSmall }

    DialogNotificationSwitch
    {
        id: dialognotificationswitch
        width: parent.width
        visible: messagesmodel.isBroadcast && !messagesmodel.isWritable
    }

    MessageInputPreview
    {
        id: messageinputpeview
        previewMessage: selectedMessage
        previewFrom: selectedMessageFrom
        previewText: selectedMessageText
        width: parent.width
        onInputCancelled: dialoginputpanel.cancelActions()
    }

    MessageTextInput {
        id: messagetextinput
        width: parent.width
        onShareMedia: dialogmediapanel.expanded ? dialogmediapanel.hide() : dialogmediapanel.show()

        onSendMessage: {
            if(replyMode)
                messagesmodel.replyMessage(message, selectedMessage)
            else if(editMode)
                messagesmodel.editMessage(message, selectedMessage);
            else
                messagesmodel.sendMessage(message);

            dialoginputpanel.cancelActions();
        }
    }
}
