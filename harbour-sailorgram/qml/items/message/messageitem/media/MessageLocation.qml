import QtQuick 2.1
import QtLocation 5.0
import QtPositioning 5.0

MessageMediaItem
{
    id: messagelocation
    contentWidth: maxWidth
    contentHeight: maxWidth

    Map
    {
        id: map
        anchors.fill: parent
        gesture.enabled: false
        plugin: Plugin { preferred: ["osm", "here", "nokia", "gmaps"] }
        center: QtPositioning.coordinate(message.media.geo.lat, message.media.geo.longitude)
    }
}
