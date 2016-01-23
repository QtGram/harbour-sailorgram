import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../models"
import "../../../../js/TelegramHelper.js" as TelegramHelper
import "../../../../js/ColorScheme.js" as ColorScheme

MessageMediaItem
{
    property FileLocation fileLocation: context.telegram.locationOfDocument(message.media.document)

    id: messagedocument
    contentWidth: imgpreview.width + Math.max(lbldummyinfo.contentWidth, lblsize.contentWidth + lblmime.contentWidth) + Theme.paddingMedium
    contentHeight: row.height

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
            source: messagedocument.mediaThumbnail || "image://theme/icon-m-document"
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

            Label { id: lbldummyinfo; text: lblinfo.text; visible: false }

            Label
            {
                id: lblinfo
                width: parent.width
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                text: isUpload ? context.sailorgram.fileName(message.upload.location) : fileHandler.fileName
                color: ColorScheme.colorizeText(message, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Row
            {
                id: sizemimerow
                width: parent.width
                height: parent.height - lblinfo.contentHeight
                spacing: Theme.paddingMedium

                Label
                {
                    id: lblsize
                    color: ColorScheme.colorizeText(message, context)
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: TelegramHelper.formatBytes(mediaSize, 2)
                }

                Label
                {
                    id: lblmime
                    color: ColorScheme.colorizeText(message, context)
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: message.media.document.mimeType
                }
            }
        }
    }
}
