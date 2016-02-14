import QtQuick 2.1

MessageMediaItem
{
    id: messagelocation
    contentWidth: maxWidth
    contentHeight: maxWidth

    Image
    {
        id: mapthumbnail
        asynchronous: true
        cache: true
        anchors.fill: parent
        source: context.locationThumbnail(message.media.geo.lat, message.media.geo.longitude, messagelocation.width, messagelocation.height, 14);
    }
}
