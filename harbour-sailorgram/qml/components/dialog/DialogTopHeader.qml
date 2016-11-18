import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../custom"

Item
{
    property alias title: lbltitle.text
    property alias statusText: lblstatus.text
    property alias peer: peerimage.peer

    id: dialogtopheader
    width: parent.width
    height: Theme.itemSizeSmall + (Theme.paddingSmall * 2)

    Column
    {
        anchors { left: parent.left; right: peerimage.left; verticalCenter: peerimage.verticalCenter; leftMargin: Theme.horizontalPageMargin; rightMargin: Theme.paddingMedium }

        Label
        {
            id: lbltitle
            width: parent.width
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            font { family: Theme.fontFamilyHeading; pixelSize: Theme.fontSizeSmall }
            color: Theme.highlightColor
            elide: Text.ElideRight
        }

        Label
        {
            id: lblstatus
            width: parent.width
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            elide: Text.ElideRight
        }
    }

    PeerImage
    {
        id: peerimage
        anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: Theme.horizontalPageMargin }
        size: Theme.itemSizeSmall
        backgroundColor: Theme.secondaryHighlightColor
        foregroundColor: Theme.primaryColor
        fontPixelSize: Theme.fontSizeLarge
        peer: model.item
    }
}
