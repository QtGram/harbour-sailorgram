import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    property real aspectRatio: {
        var w = fileHandler.imageSize.width;
        var h = fileHandler.imageSize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagephoto
    contentWidth: fileHandler.imageSize.width
    contentHeight: thumb.height

    Image
    {
        id: imgdownload
        anchors.centerIn: parent
        source: "image://theme/icon-m-cloud-download"
        visible: !messagephoto.fileHandler.downloaded && !messagephoto.transferInProgress
        z: 2
    }

    ProgressCircle
    {
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.5
        height: width
        visible: messagephoto.transferInProgress
        value: messagephoto.fileHandler.progressPercent / 100.0
        z: 2
    }

    MessageThumbnail
    {
        id: thumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        cache: !messagephoto.fileHandler.downloaded

        source: {
            if(!messagephoto.fileHandler.downloaded || messagephoto.transferInProgress)
                return messagephoto.fileHandler.thumbPath;

            return messagephoto.fileHandler.filePath;
        }
    }
}
