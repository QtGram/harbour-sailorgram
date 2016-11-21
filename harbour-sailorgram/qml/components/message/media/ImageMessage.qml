import QtQuick 2.1
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

Item
{
    property alias source: image.source

    id: imagemessage

    Image
    {
        id: image
        anchors.fill: parent
        asynchronous: true
        cache: false
    }

    FastBlur { anchors.fill: image; source: image; visible:!mediamessageitem.downloaded; radius: !mediamessageitem.downloaded ? 32.0 : 0.0 }

    Image
    {
        z: 2
        source: "image://theme/icon-m-cloud-download"
        asynchronous: true
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        width: parent.width * 0.2
        height: parent.height * 0.2
        visible: !mediamessageitem.downloaded && !mediamessageitem.downloading
    }

    BusyIndicator { z: 2; size: BusyIndicatorSize.Small; anchors.centerIn: parent; running: mediamessageitem.downloading }

    ProgressCircle
    {
        z: 2
        width: Math.min(parent.width, parent.height) * 0.8
        height: width
        anchors.centerIn: parent
        visible: mediamessageitem.uploading
        value: mediamessageitem.progress
    }

    MouseArea
    {
        anchors.fill: parent
        enabled: !mediamessageitem.isSticker

        onClicked: {
            if(mediamessageitem.downloaded) {
                pageStack.push(Qt.resolvedUrl("../../../pages/media/ImageViewerPage.qml"), { source: imagemessage.source });
                return;
            }

            mediamessageitem.download();
        }
    }
}
