import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../models"
import "../../../../components/message"
import "../../../../js/TelegramHelper.js" as TelegramHelper
import "../../../../js/ColorScheme.js" as ColorScheme

MessageMediaItem
{
    property FileLocation fileLocation: context.telegram.locationOfDocument(message.media.document)

    id: messagedocument
    contentWidth: imgpreview.width + Math.max(lbldummyinfo.contentWidth, lbldummymimesize.contentWidth) + Theme.paddingMedium
    contentHeight: row.height

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
            source: messagedocument.mediaThumbnail || "image://theme/icon-m-document"

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
            spacing: Theme.paddingSmall

            Label { id: lbldummyinfo; text: lblinfo.text; visible: false }
            Label { id: lbldummymimesize; text: lblmimesize.text; visible: false }

            Label
            {
                id: lblinfo
                width: parent.width - Theme.paddingSmall
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                text: fileHandler.fileName
                color: ColorScheme.colorizeText(message, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Label
            {
                id: lblmimesize
                width: parent.width - Theme.paddingSmall
                color: ColorScheme.colorizeText(message, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                text: TelegramHelper.formatBytes(mediaSize, 2) + " " + message.media.document.mimeType
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
                visible: messagedocument.transferInProgress
                value: messagedocument.fileHandler.progressPercent
            }
        }
    }
}
