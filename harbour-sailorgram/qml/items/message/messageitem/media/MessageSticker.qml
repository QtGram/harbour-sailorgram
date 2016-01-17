import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    id: messagesticker
    contentWidth: thumb.width
    contentHeight: thumb.height

    MessageThumbnail
    {
        id: thumb
        anchors.fill: parent
        cache: !messagesticker.fileHandler.downloaded
        source: messagesticker.mediaThumbnail
        transferProgress: progressPercent
    }
}
