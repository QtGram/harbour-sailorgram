import QtQuick 2.1
import Sailfish.Silica 1.0

Row
{
    readonly property real contentWidth: image.width + Math.max(lblhiddenfilename.contentWidth, lblhiddenfilesize.contentWidth) + Theme.paddingSmall
    property alias fileName: lblhiddenfilename.text
    property alias fileSize: lblhiddenfilesize.text
    property color color: Theme.primaryColor

    id: filemessage
    spacing: Theme.paddingSmall
    height: image.height

    Image
    {
        id: image
        asynchronous: true
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
    }

    Column
    {
        id: coltext
        width: parent.width - image.width
        height: parent.height

        Label { id: lblhiddenfilename; visible: false }
        Label { id: lblhiddenfilesize; visible: false }

        Label
        {
            id: lblfilename
            width: parent.width - Theme.paddingSmall
            color: filemessage.color
            text: lblhiddenfilename.text
            elide: Text.ElideRight
        }

        Label
        {
            id: lblfilesize
            width: parent.width
            color: filemessage.color
            text: lblhiddenfilesize.text
            elide: Text.ElideRight
        }
    }
}
