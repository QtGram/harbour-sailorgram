import QtQuick 2.1
import Sailfish.Silica 1.0

BackgroundItem
{
    property alias icon: img.source
    property alias text: lbl.text

    id: messagepanelitem
    width: Theme.itemSizeSmall
    height: content.height

    Column
    {
        id: content
        width: parent.width

        Image
        {
            id: img
            anchors.horizontalCenter: parent.horizontalCenter
            width: Theme.iconSizeSmall
            height: Theme.iconSizeSmall
        }

        Label
        {
            id: lbl
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
