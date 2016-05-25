import QtQuick 2.1
import Sailfish.Silica 1.0

MessageMediaItem
{
    property real aspectRatio: {
        var imagesize = downloadHandler.imageSize;
        var w = imagesize.width;
        var h = imagesize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagephoto
    contentWidth: downloadHandler.imageSize.width
    contentHeight: thumb.height

    Component.onCompleted: {
        if(context.autoloadimages && !downloadHandler.downloaded)
            downloadHandler.download();
    }

    Image
    {
        id: imgdownload
        anchors.centerIn: parent
        source: "image://theme/icon-m-cloud-download"
        visible: !downloadHandler.downloaded && !downloadHandler.downloading
        z: 2
    }

    ProgressCircle
    {
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.5
        height: width
        visible: downloadHandler.downloading
        value: downloadHandler.progressPercent / 100.0
        z: 2
    }

    MessageThumbnail
    {
        id: thumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        cache: !downloadHandler.downloaded
        blurRadius: downloadHandler.downloaded && !downloadHandler.downloading ? 0.0 : 32.0

        source: {
            if(!downloadHandler.downloaded || downloadHandler.downloading)
                return downloadHandler.thumbnail;

            return downloadHandler.destination;
        }
    }
}
