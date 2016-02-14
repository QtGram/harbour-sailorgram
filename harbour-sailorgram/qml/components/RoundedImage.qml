import QtQuick 2.0
import QtGraphicalEffects 1.0

Item
{
    property alias cache: image.cache
    property alias radius: mask.radius
    property alias asynchronous: image.asynchronous
    property alias source: image.source
    property alias fillMode: image.fillMode

    id: roundedimage

    Image
    {
        id: image
        anchors.fill: parent
        visible: false
    }

    Rectangle
    {
        id: mask
        anchors.fill: parent
        radius: width * 0.5
        visible: false
        color: "black"
    }

    OpacityMask
    {
        anchors.fill: mask
        source: image
        maskSource: mask
    }
}

