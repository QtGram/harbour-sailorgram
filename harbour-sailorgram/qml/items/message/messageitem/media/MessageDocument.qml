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
            source: downloadHandler.thumbnail

            Rectangle {
                border.width: 1
                border.color: ColorScheme.colorizeLink(messageModelItem, context)
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
                text: messageModelItem ? messageModelItem.fileName : ""
                color: ColorScheme.colorizeTextItem(messageModelItem, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            ResizableLabel
            {
                id: lblmimesize
                width: parent.width - Theme.paddingSmall
                color: ColorScheme.colorizeTextItem(messageModelItem, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                text: messageModelItem ? TelegramHelper.formatBytes(messageModelItem.fileSize, 2) + " " + messageModelItem.fileMimeType : ""
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            MessageMediaProgressBar
            {
                id: progressbar
                width: parent.width - Theme.paddingSmall
                barHeight: Theme.paddingSmall
                barColor: ColorScheme.colorizeTextItem(messageModelItem, context)
                border { color: ColorScheme.colorizeLink(messageModelItem, context); width: 1 }
                visible: downloadHandler.downloading
                value: messagedocument.progressPercent
            }
        }
    }
}
