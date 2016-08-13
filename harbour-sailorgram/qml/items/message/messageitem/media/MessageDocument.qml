import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../../models"
import "../../../../components"
import "../../../../components/message"
import "../../../../js/TelegramHelper.js" as TelegramHelper
import "../../../../js/ColorScheme.js" as ColorScheme

MessageMediaItem
{
    id: messagedocument
    contentWidth: imgpreview.width + Math.max(lblinfo.calculatedWidth, lblmimesize.calculatedWidth) + Theme.paddingMedium
    contentHeight: row.height

    Row
    {
        id: row
        width: parent.width
        height: Math.max(info.height, imgpreview.height)
        spacing: Theme.paddingMedium

        MessageThumbnail
        {
            id: imgpreview
            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            anchors.verticalCenter: info.verticalCenter
            useTelegramImage: true
            context: messagedocument.context
            source: sgMessageItem.messageMedia.rawMedia

            Rectangle {
                border.width: 1
                border.color: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                anchors { fill: parent; margins: -Theme.paddingSmall }
                color: "transparent"
            }
        }

        Column
        {
            id: info
            width: parent.width - imgpreview.width
            spacing: Theme.paddingSmall

            ResizableLabel
            {
                id: lblinfo
                width: parent.width - Theme.paddingSmall
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeExtraSmall
                text: sgMessageItem.messageMedia.fileName
                color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            ResizableLabel
            {
                id: lblmimesize
                width: parent.width - Theme.paddingSmall
                color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                text: TelegramHelper.formatBytes(sgMessageItem.messageMedia.fileSize, 2) + " " + sgMessageItem.messageMedia.fileMimeType
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            MessageMediaProgressBar
            {
                id: progressbar
                width: parent.width - Theme.paddingSmall
                barHeight: Theme.paddingSmall
                barColor: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                border { color: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context); width: 1 }
                visible: sgMessageItem.messageMedia.isTransfering
                value: sgMessageItem.messageMedia.transferProgress * 100
            }
        }
    }
}
