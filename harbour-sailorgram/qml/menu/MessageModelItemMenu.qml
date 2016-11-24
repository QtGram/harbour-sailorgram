import QtQuick 2.1
import Sailfish.Silica 1.0

ContextMenu
{
    id: messagemodelitemmenu

    MenuItem
    {
        text: qsTr("Edit")
        visible: messagesmodel.isWritable && model.isMessageOut
        onClicked: editRequested()
    }

    MenuItem
    {
        text: qsTr("Reply")
        visible: messagesmodel.isWritable
        onClicked: replyRequested()
    }
}
