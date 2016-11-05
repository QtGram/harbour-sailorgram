import QtQuick 2.1
import Sailfish.Silica 1.0

Column
{
    property real topMargin

    id: messagelistheader
    visible: height > 0 && model.isWritable
    height: ph.height + messagetextinput.height

    Item { id: ph; width: parent.width; height: topMargin }

    MessageTextInput
    {
        id: messagetextinput
        width: parent.width
        onSendMessage: model.sendMessage(message)
    }
}
