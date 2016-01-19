import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../components/mediaplayer/mediacomponents"
import "../../../../js/TelegramHelper.js" as TelegramHelper
import "../../../../js/ColorScheme.js" as ColorScheme

MessageMediaItem
{
    property FileLocation fileLocation: context.telegram.locationOfAudio(message.media.audio)

    id: messageaudio
    contentWidth: imgpreview.width + Math.max(lbldummyinfo.contentWidth, lblsize.contentWidth + lblduration.contentWidth) + Theme.paddingMedium
    contentHeight: row.height

    MediaPlayerTimings { id: mediaplayertimings }

    Row
    {
        id: row
        width: parent.width
        height: imgpreview.height
        spacing: Theme.paddingMedium

        MessageThumbnail
        {
            id: imgpreview
            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            source: "image://theme/icon-m-sounds"
            transferProgress: progressPercent

            Rectangle {
                border.width: 1
                border.color: ColorScheme.colorizeLink(message, context)
                anchors { fill: parent; margins: -Theme.paddingSmall }
                color: "transparent"
            }
        }

        Column
        {
            id: info
            width: parent.width
            height: imgpreview.height
            spacing: Theme.paddingSmall

            Label { id: lbldummyinfo; text: lblinfo.text; visible: false }

            Label
            {
                id: lblinfo
                height: parent.height / 3
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                color: ColorScheme.colorizeText(message, context)
                text: qsTr("Audio recording")
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Label
            {
                id: lblsize
                color: ColorScheme.colorizeText(message, context)
                height: parent.height / 3
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Size: %1").arg(TelegramHelper.formatBytes(mediaSize, 2))
            }

            Label
            {
                id: lblduration
                color: ColorScheme.colorizeText(message, context)
                height: parent.height / 3
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Duration: %1").arg(mediaplayertimings.displayDuration(message.media.audio.duration))
            }
        }
    }
}
