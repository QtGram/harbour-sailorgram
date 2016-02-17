import QtQuick 2.1
import Sailfish.Silica 1.0
import "../js/TelegramHelper.js" as TelegramHelper

ContextMenu
{
    property var dialog

    signal deleteRequested(string remorsemsg)
    signal markAsReadRequested()

    id: dialogitemmenu

    MenuItem
    {
        text: qsTr("Mark as read")
        onClicked: markAsReadRequested();
    }

    MenuItem
    {
        text: qsTr("Delete")

        onClicked: {
            var msg = qsTr("Deleting Conversation");

            if(dialog.encrypted)
                msg = qsTr("Deleting Secret Chat");
            else if(TelegramHelper.isChat(dialog))
                msg = qsTr("Deleting Group");

            deleteRequested(msg);
        }
    }
}
