import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    property real aspectRatio: {
        var w = message.media.photo.sizes.last.w;
        var h = message.media.photo.sizes.last.h;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagephoto
    contentWidth: message.media.photo.sizes.last.w
    contentHeight: thumb.height

    Image
    {
        id: imgdownload
        anchors.centerIn: parent
        source: "image://theme/icon-m-cloud-download"
        visible: !messagephoto.fileHandler.downloaded
        z: 2
    }

    MessageThumbnail
    {
        id: thumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        cache: !messagephoto.fileHandler.downloaded
        source: messagephoto.mediaThumbnail
        transferProgress: progressPercent
    }
}
