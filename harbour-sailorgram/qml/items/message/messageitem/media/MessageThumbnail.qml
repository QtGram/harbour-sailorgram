import QtQuick 2.1
import QtGraphicalEffects 1.0

Item
{
    property alias cache: thumbnailimage.cache
    property alias source: thumbnailimage.source
    property alias fillMode: thumbnailimage.fillMode
    property alias blurRadius: thumbnailblur.radius

    id: messagethumbnail

    Image
    {
        id: thumbnailimage
        anchors.fill: parent
        asynchronous: true
        fillMode: Image.PreserveAspectFit
    }

    FastBlur
    {
        id: thumbnailblur
        anchors.fill: thumbnailimage
        source: thumbnailimage

        Behavior on radius { NumberAnimation { duration: 250 } }
    }
}
