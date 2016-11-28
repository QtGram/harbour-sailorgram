import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../custom"

Row
{
    readonly property real contentWidth: image.width + Math.max(lblhiddenfilename.contentWidth, lblhiddenfilesize.contentWidth) + Theme.paddingSmall
    property alias fileName: lblhiddenfilename.text
    property alias fileSize: lblhiddenfilesize.text
    property color color: Theme.primaryColor

    id: filemessage
    spacing: Theme.paddingSmall
    height: image.height

    Image
    {
        id: image
        asynchronous: true
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall

        source: {
            if(mediamessageitem.downloading)
                return "";

            if(!mediamessageitem.downloaded)
                return "image://theme/icon-m-cloud-download";

            if(mediamessageitem.isAudio)
                return "image://theme/icon-m-sounds";

            if(mediamessageitem.isVideo)
                return "image://theme/icon-m-video";

            if(mediamessageitem.isImage)
                return "image://theme/icon-m-image";

            return "image://theme/icon-m-file-document";
        }

        BusyIndicator
        {
            anchors.centerIn: parent
            size: BusyIndicatorSize.Medium
            running: mediamessageitem.downloading
        }

        MouseArea
        {
            anchors.fill: parent

            onClicked: {
                if(!mediamessageitem.downloaded) {
                    mediamessageitem.download();
                    return;
                }

                messagepopup.popup(qsTr("Opening %1").arg(filemessage.fileName));
                Qt.openUrlExternally("file://" + mediamessageitem.source);
            }
        }
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

    Column
    {
        id: coltext
        width: parent.width - image.width
        height: parent.height

        Label { id: lblhiddenfilename; visible: false }
        Label { id: lblhiddenfilesize; visible: false }

        Label
        {
            id: lblfilename
            width: parent.width - Theme.paddingSmall
            color: filemessage.color
            font.pixelSize: Theme.fontSizeSmall
            text: lblhiddenfilename.text
            elide: Text.ElideRight
        }

        Label
        {
            id: lblfilesize
            width: parent.width
            color: filemessage.color
            text: lblhiddenfilesize.text
            elide: Text.ElideRight
            font.pixelSize: Theme.fontSizeExtraSmall
        }
    }
}
