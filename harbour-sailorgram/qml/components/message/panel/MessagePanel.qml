import QtQuick 2.1
import Sailfish.Silica 1.0

DockedPanel
{
    signal replyRequested()
    signal editRequested()

    function checkActions(iswritable, ismessageout) {
        pireply.visible = iswritable;
        piedit.visible = iswritable && ismessageout;

        if(!pireply.visible && !piedit.visible)
            hide(true);
    }

    id: messagepanel
    height: Theme.itemSizeSmall
    dock: Dock.Bottom

    Row
    {
        spacing: Theme.paddingLarge
        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }

        MessagePanelItem
        {
            id: pireply
            icon: "image://theme/icon-s-repost"
            text: qsTr("Reply")

            onClicked: {
                replyRequested();
                messagepanel.hide();
            }
        }

        MessagePanelItem
        {
            id: piedit
            icon: "image://theme/icon-s-edit"
            text: qsTr("Edit")

            onClicked: {
                editRequested()
                messagepanel.hide();
            }
        }

        MessagePanelItem
        {
            id: piclose
            icon: "image://theme/icon-s-clear-opaque-cross"
            text: qsTr("Close")
            onClicked: messagepanel.hide()
        }
    }
}
