import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../../items/peer"

BackgroundItem
{
    property alias context: peerimage.context
    property SailorgramDialogItem sgDialogItem

    id: forwarditem

    PeerImage
    {
        id: peerimage
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.height - lbltitle.contentHeight
        height: width
        showType: false
        peer: sgDialogItem.peer
        isChat: sgDialogItem.isChat
        isBroadcast: sgDialogItem.isBroadcast
        isSecretChat: sgDialogItem.isSecretChat
        fallbackTitle: sgDialogItem.title
    }

    Label
    {
        id: lbltitle
        text: sgDialogItem.title
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        font.pixelSize: Theme.fontSizeTiny
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }
}
