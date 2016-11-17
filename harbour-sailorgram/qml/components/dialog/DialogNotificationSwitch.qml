import QtQuick 2.1
import Sailfish.Silica 1.0
import "../custom"

BackgroundItem
{
    id: dialognotificationswitch
    height: Theme.itemSizeSmall

    BackgroundRectangle { anchors.fill: parent }

    Label
    {
        text: messagesmodel.isMuted ? qsTr("Enable notifications") : qsTr("Disable notifications")
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    onClicked: {
        messagesmodel.isMuted = !messagesmodel.isMuted;
    }
}
