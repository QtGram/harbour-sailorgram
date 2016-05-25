import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../../../components/mediaplayer/mediacomponents"

MessageMediaItem
{
    property real aspectRatio: {
        var w = downloadHandler.imageSize.width;
        var h = downloadHandler.imageSize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    MediaPlayerTimings { id: mediaplayertimings }

    id: messagevideo
    contentWidth: downloadHandler.imageSize.width
    contentHeight: thumb.height

    Image
    {
        id: videothumb
        anchors.centerIn: parent
        source: "image://theme/icon-m-play"
        z: 2
    }

    ProgressCircle
    {
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.5
        height: width
        visible: downloadHandler.downloading
        //FIXME: value: messagevideo.downloadHandler.progressPercent / 100.0
        z: 2
    }

    MessageThumbnail
    {
        id: thumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        cache: false
        source: downloadHandler.thumbnail
    }
}
