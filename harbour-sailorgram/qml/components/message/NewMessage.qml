import QtQuick 2.1
import Sailfish.Silica 1.0
import "../custom"

BackgroundRectangle
{
    id: newmessage
    width: parent.width
    height: visible ? label.contentHeight : 0

    Label
    {
        id: label
        anchors.fill: parent
        text: qsTr("%1   New messages   %1").arg("↓")
        font.pixelSize: Theme.fontSizeSmall
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
