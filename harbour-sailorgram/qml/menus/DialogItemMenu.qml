import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0

ContextMenu
{
    property SailorgramDialogItem sgDialogItem

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

            if(sgDialogItem.isSecretChat)
                msg = qsTr("Deleting Secret Chat");
            else if(sgDialogItem.isBroadcast)
                msg = qsTr("Deleting Channel");
            else if(sgDialogItem.isChat)
                msg = qsTr("Deleting Group");

            deleteRequested(msg);
        }
    }
}
