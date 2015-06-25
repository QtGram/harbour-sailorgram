import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

MessageMediaItem
{
    id: messagedocument
    hasMedia: (message.media ? (message.media.classType === messageitem.typeMessageMediaDocument) : false)
    height: row.height
    width: Math.min(messageitem.width, row.width)

    onHasMediaChanged: {
         console.log(message.media.document.thumb.location.download.location);
    }

    Row
    {
        id: row
        anchors { left: parent.left; top: parent.top; }
        height: imgpreview.height
        width: imgpreview.width + lblinfo.paintedWidth

        Image
        {
            id: imgpreview
            height: 48 * Theme.pixelSize
            width: height * sourceSize.width / sourceSize.height
            source: hasMedia ? message.media.document.thumb.location.download.location : ""
        }

        Label
        {
            id: lblinfo
            height: imgpreview.height
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Theme.fontSizeExtraSmall
            text: message.media.document.thumb.location.download.location
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
        }
    }
}
