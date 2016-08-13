import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    property real aspectRatio: {
        var imagesize = messagephotothumb.imageSize;
        var w = imagesize.width;
        var h = imagesize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagephoto
    contentWidth: messagephotothumb.imageSize.width
    contentHeight: messagephotothumb.height

    Component.onCompleted: {
        if(context.autoloadimages && !sgMessageItem.messageMedia.isTransfered)
            sgMessageItem.messageMedia.download();
    }

    Image
    {
        id: imgdownload
        anchors.centerIn: parent
        source: "image://theme/icon-m-cloud-download"
        visible: !sgMessageItem.messageMedia.isTransfered && !sgMessageItem.messageMedia.isTransfering
        z: 2
    }

    ProgressCircle
    {
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.5
        height: width
        visible: sgMessageItem.messageMedia.isTransfering
        value: sgMessageItem.messageMedia.transferProgress
        z: 2
    }

    MessageThumbnail
    {
        id: messagephotothumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        useTelegramImage: true
        context: messagephoto.context
        source: sgMessageItem.messageMedia.rawMedia
    }
}
