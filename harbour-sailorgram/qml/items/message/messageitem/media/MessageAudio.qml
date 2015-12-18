import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../components/mediaplayer/mediacomponents"
import "../../../../js/TelegramHelper.js" as TelegramHelper

MessageMediaItem
{
    property FileLocation fileLocation: context.telegram.locationOfAudio(message.media.audio)

    id: messageaudio
    height: row.height
    width: Math.min(messageitem.width, row.width)

    MediaPlayerTimings { id: mediaplayertimings }

    Row
    {
        id: row
        anchors { left: parent.left; top: parent.top }
        height: imgpreview.height
        width: imgpreview.width + info.width
        spacing: Theme.paddingSmall

        MessageThumbnail
        {
            id: imgpreview
            source: messageaudio.mediaThumbnail || "image://theme/icon-m-sounds"
            transferProgress: progressPercent
        }

        Column
        {
            id: info
            width: Math.max(lblinfo.paintedWidth, lblduration.paintedWidth, lblsize.paintedWidth)
            height: imgpreview.height

            Label
            {
                id: lblinfo
                height: parent.height / 3
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Audio recording")
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Label
            {
                id: lblsize
                height: parent.height / 3
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Size: %1").arg(TelegramHelper.formatBytes(mediaSize, 2))
            }

            Label
            {
                id: lblduration
                height: parent.height / 3
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Duration: %1").arg(mediaplayertimings.displayDuration(message.media.audio.duration))
            }
        }
    }
}
