import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../components/mediaplayer/mediacomponents"
import "../../../../js/TelegramHelper.js" as TelegramHelper

MessageMediaItem
{
    property FileLocation fileLocation: context.telegram.locationOfVideo(message.media.video)

    property real aspectRatio: {
        var w = fileHandler.imageSize.width;
        var h = fileHandler.imageSize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    MediaPlayerTimings { id: mediaplayertimings }

    id: messagevideo
    contentWidth: fileHandler.imageSize.width
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
        visible: messagevideo.transferInProgress
        value: messagevideo.fileHandler.progressPercent / 100.0
        z: 2
    }

    MessageThumbnail
    {
        id: thumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        cache: false
        source: fileHandler.thumbPath
    }
}
