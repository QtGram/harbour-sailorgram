import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0

Column
{
    property Message editMessage: null
    property string editMessageText
    property real topMargin

    id: messagelistheader
    visible: height > 0 && model.isWritable
    height: ph.height + messagetextinput.height

    onEditMessageTextChanged: {
        messagetextinput.text = editMessageText;

        if(editMessageText.length > 0)
            messagetextinput.focusTextArea();
    }

    Item { id: ph; width: parent.width; height: topMargin }

    MessageTextInput
    {
        id: messagetextinput
        width: parent.width

        onSendMessage: {
            if(editMessage) {
                model.editMessage(editMessage, message);
                editMessage = null;
                editMessageText = "";
                return;
            }

            model.sendMessage(message);
        }
    }
}
