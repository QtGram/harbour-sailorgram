import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import ".."
import "../../../js/TelegramHelper.js" as TelegramHelper

MessageMediaItem
{
    id: messagedocument
    hasMedia: (message.media ? (message.media.classType === messageitem.typeMessageMediaDocument) : false)
    height: row.height
    width: Math.min(messageitem.width, row.width)
    fileLocation: telegram.locationOfDocument(message.media.document)

    Row
    {
        id: row
        anchors { left: parent.left; top: parent.top }
        height: imgpreview.height
        width: imgpreview.width + info.width
        spacing: Theme.paddingSmall

        Image
        {
            id: imgpreview
            height: 48 * Theme.pixelSize
            width: height * sourceSize.width / sourceSize.height
            source: message.media.document.thumb.location.download.location || "image://theme/icon-m-document"
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
                text: fileLocation.fileName
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
                    text: TelegramHelper.formatBytes(message.media.document.size, 2)
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
