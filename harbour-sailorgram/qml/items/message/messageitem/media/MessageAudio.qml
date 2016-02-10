import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../components/message"
import "../../../../components/mediaplayer/mediacomponents"
import "../../../../js/TelegramHelper.js" as TelegramHelper
import "../../../../js/ColorScheme.js" as ColorScheme

MessageMediaItem
{
    id: messageaudio
    contentWidth: imgpreview.width + Math.max(lbldummyinfo.contentWidth, lbldummysize.contentWidth + lbldummyduration.contentWidth) + Theme.paddingMedium
    contentHeight: row.height

    MediaPlayerTimings { id: mediaplayertimings }

    Row
    {
        id: row
        width: parent.width
        height: info.height
        spacing: Theme.paddingMedium

        MessageThumbnail
        {
            id: imgpreview
            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            anchors.verticalCenter: info.verticalCenter
            source: "image://theme/icon-m-sounds"

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
            width: parent.width - imgpreview.width

            Label { id: lbldummyinfo; text: lblinfo.text; visible: false }
            Label { id: lbldummysize; text: lblsize.text; visible: false }
            Label { id: lbldummyduration; text: lblduration.text; visible: false }

            Label
            {
                id: lblinfo
                width: parent.width
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
                width: parent.width
                color: ColorScheme.colorizeText(message, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Size: %1").arg(TelegramHelper.formatBytes(mediaSize, 2))
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Label
            {
                id: lblduration
                width: parent.width
                color: ColorScheme.colorizeText(message, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Duration: %1").arg(mediaplayertimings.displayDuration(message.media.audio.duration))
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            MessageMediaProgressBar
            {
                id: progressbar
                width: parent.width - Theme.paddingSmall
                barHeight: Theme.paddingSmall
                barColor: ColorScheme.colorizeText(message, context)
                border { color: ColorScheme.colorizeLink(message, context); width: 1 }
                visible: messageaudio.transferInProgress
                value: messageaudio.fileHandler.progressPercent
            }
        }
    }
}
