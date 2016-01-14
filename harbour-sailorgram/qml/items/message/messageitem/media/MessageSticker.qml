import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    id: messagesticker
    width: thumb.width
    height: thumb.height

    MessageThumbnail
    {
        id: thumb
        anchors { left: parent.left; top: parent.top }
        cache: !messagesticker.fileHandler.downloaded
        source: messagesticker.mediaThumbnail
        transferProgress: progressPercent
    }
}
