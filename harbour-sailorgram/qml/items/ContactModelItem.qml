import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../components/peer"

ListItem
{
    id: contactmodelitem
    contentHeight: Theme.itemSizeSmall

    PeerImage
    {
        id: peerimage
        anchors { left: parent.left; top: parent.top; leftMargin: Theme.paddingSmall }
        size: contactmodelitem.height
        peer: model.item
    }

    Label
    {
        id: lblfullname
        anchors { left: peerimage.right; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        font { family: Theme.fontFamilyHeading; pixelSize: Theme.fontSizeSmall }
        text: model.fullName
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
        color: Theme.highlightColor
    }

    Label
    {
        id: lblstatus
        anchors { left: peerimage.right; top: lblfullname.bottom; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        font.pixelSize: Theme.fontSizeExtraSmall
        text: model.statusText
    }
}
