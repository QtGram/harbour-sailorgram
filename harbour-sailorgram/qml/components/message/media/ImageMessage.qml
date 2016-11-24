import QtQuick 2.1
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import "../../custom"

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

    FastBlur { anchors.fill: image; source: image; radius: (!mediamessageitem.downloaded || mediamessageitem.isVideo) ? 32.0 : 0.0 }

    Image
    {
        z: 2
        asynchronous: true
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        width: parent.width * 0.2
        height: parent.height * 0.2

        visible: {
            if(mediamessageitem.downloading)
                return false;

            return !mediamessageitem.downloaded || mediamessageitem.isVideo;
        }

        source: {
            if(mediamessageitem.isVideo && mediamessageitem.downloaded)
                return "image://theme/icon-m-play";

            return "image://theme/icon-m-cloud-download";
        }
    }

    BusyIndicator { z: 2; size: BusyIndicatorSize.Small; anchors.centerIn: parent; running: mediamessageitem.downloading }

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

    MouseArea
    {
        anchors.fill: parent
        enabled: !mediamessageitem.isSticker && !messageslist.selectionMode

        onClicked: {
            if(mediamessageitem.downloaded) {
                if(mediamessageitem.isVideo)
                    pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPlayerPage.qml"), { source: mediamessageitem.source, thumbnail: mediamessageitem.videoThumbnail });
                else
                    pageStack.push(Qt.resolvedUrl("../../../pages/media/ImageViewerPage.qml"), { source: imagemessage.source });

                return;
            }

            mediamessageitem.download();
        }
    }
}
