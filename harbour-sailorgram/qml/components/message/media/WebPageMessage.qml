import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../message"

Column
{
    readonly property real calculatedWidth: Math.max(wpurl.calculatedWidth,
                                                     wptitle.calculatedWidth,
                                                     wpdescription.calculatedWidth,
                                                     imgthumbnail.sourceSize.width)

    property alias url: wpurl.rawText
    property alias title: wptitle.rawText
    property alias description: wpdescription.rawText
    property alias source: imgthumbnail.source
    property alias quoteColor: messagequote.color
    property color color: Theme.primaryColor

    id: webpageelement
    spacing: Theme.paddingSmall

    MessageText
    {
        id: wpurl
        emojiPath: context.sailorgram.emojiPath
        font.pixelSize: Theme.fontSizeSmall
        color: webpageelement.color
        width: parent.width
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }

    Row
    {
        id: preview
        width: parent.width
        height: previewcontent.height
        spacing: Theme.paddingSmall

        MessageQuote { id: messagequote; height: parent.height }

        Column
        {
            id: previewcontent
            width: parent.width - (Theme.paddingSmall * 2)
            spacing: Theme.paddingMedium

            MessageText
            {
                id: wptitle
                emojiPath: context.sailorgram.emojiPath
                color: webpageelement.color
                width: parent.width
                wrapMode: Text.Wrap
                font { bold: true; pixelSize: Theme.fontSizeSmall }
            }

            MessageText
            {
                id: wpdescription
                emojiPath: context.sailorgram.emojiPath
                color: webpageelement.color
                width: parent.width
                wrapMode: Text.Wrap
                font { pixelSize: Theme.fontSizeExtraSmall; italic: true }
            }

            Image
            {
                id: imgthumbnail
                width: parent.width
                asynchronous: true
                fillMode: Image.PreserveAspectFit
            }
        }
    }
}
