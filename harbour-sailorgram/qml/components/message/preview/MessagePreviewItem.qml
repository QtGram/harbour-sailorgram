import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../"

Item
{
    readonly property real calculatedWidth: messagequote.width +
                                            mediamessageitem.size +
                                            Math.max(lblfrom.calculatedWidth, lblmessage.calculatedWidth) +
                                            Theme.paddingSmall

    property alias quoteColor: messagequote.color
    property color color: Theme.primaryColor
    property alias message: mediamessageitem.message
    property alias previewFrom: lblfrom.rawText
    property alias previewText: lblmessage.rawText

    id: messagereplyitem
    height: Math.max(content.height, mediamessageitem.height) + Theme.paddingSmall

    Row
    {
        id: row
        width: parent.width
        height: parent.height
        spacing: Theme.paddingSmall

        MessageQuote { id: messagequote; height: parent.height - Theme.paddingSmall }

        MediaMessageItem
        {
            id: mediamessageitem
            anchors.verticalCenter: parent.verticalCenter

            size: {
                if(mediamessageitem.isImage || mediamessageitem.isVideo)
                    return Theme.itemSizeSmall;

                return 0;
            }

            imageDelegate: Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: mediamessageitem.thumbnail
                asynchronous: true
            }
        }

        Column
        {
            id: content
            width: parent.width

            MessageText
            {
                id: lblfrom
                font { bold: true; pixelSize: Theme.fontSizeExtraSmall }
                emojiPath: context.sailorgram.emojiPath
                width: parent.width - (Theme.paddingSmall * 2)
                horizontalAlignment: Text.AlignLeft
                color: messagereplyitem.color
                linkColor: messagereplyitem.quoteColor
                elide: Text.ElideRight
            }

            MessageText
            {
                id: lblmessage
                width: parent.width - (Theme.paddingSmall * 2)
                font { italic: true; pixelSize: Theme.fontSizeExtraSmall }
                emojiPath: context.sailorgram.emojiPath
                maximumLineCount: 3
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                color: messagereplyitem.color
                linkColor: messagereplyitem.quoteColor
                horizontalAlignment: Text.AlignLeft
            }
        }
    }
}
