import QtQuick 2.1
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import "../../custom"

Item
{
    property alias source: image.source

    id: imagemessage

    BlurredImage
    {
        id: image
        anchors.fill: parent
        cache: false
    }

    ProgressIndicator
    {
        size: Math.min(parent.width, parent.height) * 0.8
        backgroundColor: messagemodelitem.bubbleColor
        progressColor: messagemodelitem.textColor
        visible: mediamessageitem.uploading
        value: mediamessageitem.progress
        anchors.centerIn: parent
        z: 2
    }
}
