import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../items/peer"
import "../../../js/TelegramHelper.js" as TelegramHelper

BackgroundItem
{
    property alias context: peerimage.context
    property alias peer: peerimage.peer
    property alias title: lbltitle.text

    id: forwarddialogitem

    PeerImage
    {
        id: peerimage
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.height - lbltitle.contentHeight
        height: width
        fallbackTitle: forwarddialogitem.title
        showType: false
    }

    Label
    {
        id: lbltitle
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        font.pixelSize: Theme.fontSizeTiny
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }
}
