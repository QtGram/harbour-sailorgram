import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    id: messagesticker
    contentWidth: maxWidth
    contentHeight: maxWidth

    downloadHandler {
        onSourceChanged: downloadHandler.download(); // Autodownload stickers
    }

    MessageThumbnail
    {
        id: thumb
        anchors.fill: parent
        cache: !downloadHandler.downloaded
        source: downloadHandler.downloaded ? downloadHandler.destination : downloadHandler.thumbnail
    }
}
