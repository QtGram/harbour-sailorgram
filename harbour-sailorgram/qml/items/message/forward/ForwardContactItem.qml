import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../items/peer"
import "../../../js/TelegramHelper.js" as TelegramHelper

BackgroundItem
{
    property Context context
    property User user

    id: forwardcontactitem

    PeerImage
    {
        id: peerimage
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.height - lblusername.contentHeight
        height: width
        context: forwardcontactitem.context
        user: forwardcontactitem.user
    }

    Label
    {
        id: lblusername
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        font.pixelSize: Theme.fontSizeTiny
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        text: TelegramHelper.completeName(user);
    }
}
