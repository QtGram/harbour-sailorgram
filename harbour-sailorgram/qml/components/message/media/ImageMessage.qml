import QtQuick 2.1
import Sailfish.Silica 1.0
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

    Image { id: image; anchors.fill: parent; asynchronous: true; }
    FastBlur { anchors.fill: image; source: image; radius: needsBlur ? 32.0 : 0.0 }

    Image
    {
        z: 2
        source: "image://theme/icon-m-cloud-download"
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        width: parent.width * 0.2
        height: parent.height * 0.2
        visible: !mediamessageitem.downloaded && !mediamessageitem.downloading
    }

    BusyIndicator { z: 2; size: BusyIndicatorSize.Small; anchors.centerIn: parent; running: mediamessageitem.downloading }
    MouseArea { anchors.fill: parent; onClicked: mediamessageitem.download() }
}
