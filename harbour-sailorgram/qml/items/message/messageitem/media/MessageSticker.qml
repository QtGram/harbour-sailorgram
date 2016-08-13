import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    id: messagesticker
    contentWidth: maxWidth
    contentHeight: maxWidth

    MessageThumbnail
    {
        id: thumb
        anchors.fill: parent
        useTelegramImage: true
        autoDownload: true
        context: messagesticker.context
        source: sgMessageItem.messageMedia.rawMedia
    }
}
