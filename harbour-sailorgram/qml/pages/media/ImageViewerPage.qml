import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0

Page
{
    property alias source: image.source

    id: mediaphotopage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent

        ImageViewer
        {
            id: image
            anchors.fill: parent
            enableZoom: !flickable.moving
        }
    }
}
