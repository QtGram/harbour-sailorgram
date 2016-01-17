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
    contentWidth: imgpreview.width + info.width + Theme.paddingMedium
    contentHeight: row.height

    Row
    {
        id: row
        anchors { left: parent.left; right: parent.right; top: parent.top }
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
            width: Math.max(lblinfo.paintedWidth, sizemimerow.width)
            height: imgpreview.height

            Label
            {
                id: lblinfo
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                text: isUpload ? context.sailorgram.fileName(message.upload.location) : fileLocation.fileName
                color: ColorScheme.colorizeText(message, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Row
            {
                id: sizemimerow
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
