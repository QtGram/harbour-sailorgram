import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    id: messagephoto
    width: thumb.width
    height: thumb.height

    MessageThumbnail
    {
        id: thumb
        anchors { left: parent.left; top: parent.top }
        source: messagephoto.mediaThumbnail
        imageSize: Qt.size(message.media.photo.sizes.last.w, message.media.photo.sizes.last.h)
        transferProgress: progressPercent
    }
}
