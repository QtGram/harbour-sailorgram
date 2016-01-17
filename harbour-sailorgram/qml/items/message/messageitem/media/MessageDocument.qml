import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../models"
import "../../../../js/TelegramHelper.js" as TelegramHelper

MessageMediaItem
{
    property FileLocation fileLocation: context.telegram.locationOfDocument(message.media.document)

    id: messagedocument
    contentWidth: Math.min(messageitem.width, row.width)
    contentHeight: row.height

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
            source: messagedocument.mediaThumbnail || "image://theme/icon-m-document"
            transferProgress: progressPercent
        }

        Column
        {
            id: info
            width: Math.max(lblinfo.paintedWidth, sizemimerow.width)
            height: imgpreview.height

            Label
            {
                id: lblinfo
                height: parent.height / 2
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                text: isUpload ? context.sailorgram.fileName(message.upload.location) : fileLocation.fileName
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            Row
            {
                id: sizemimerow
                height: parent.height / 2
                spacing: Theme.paddingMedium

                Label
                {
                    id: lblsize
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: TelegramHelper.formatBytes(mediaSize, 2)
                }

                Label
                {
                    id: lblmime
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: message.media.document.mimeType
                }
            }
        }
    }
}
