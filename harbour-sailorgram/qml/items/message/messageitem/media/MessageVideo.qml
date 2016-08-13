import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../../../components/mediaplayer/mediacomponents"

MessageMediaItem
{
    property real aspectRatio: {
        var w = videothumb.imageSize.width;
        var h = videothumb.imageSize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    MediaPlayerTimings { id: mediaplayertimings }

    id: messagevideo
    contentWidth: videothumb.imageSize.width
    contentHeight: videothumb.height

    Image
    {
        anchors.centerIn: parent
        source: "image://theme/icon-m-play"
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
        id: videothumb
        width: parent.width
        height: aspectRatio ? (width / aspectRatio) : 0
        useTelegramImage: true
        context: messagevideo.context
        source: sgMessageItem.messageMedia.rawMedia
    }
}
