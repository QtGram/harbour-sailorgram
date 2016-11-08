import QtQuick 2.1
import QtGraphicalEffects 1.0

Item
{
    property alias source: image.source
    property bool needsBlur: false

    id: imagemessage

    layer.enabled: true

    layer.effect: OpacityMask {
        maskSource: Rectangle {
            anchors.fill: imagemessage
            radius: context.bubbleradius
        }
    }

    Image
    {
        id: image
        anchors.fill: parent
        asynchronous: true
    }

    FastBlur
    {
        anchors.fill: image
        source: image
        radius: needsBlur ? 32.0 : 0.0
    }
}
