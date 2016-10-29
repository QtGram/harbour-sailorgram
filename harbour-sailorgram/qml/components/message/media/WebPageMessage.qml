import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../message"

Column
{
    property alias url: wpurl.rawText
    property alias title: wptitle.text
    property alias description: wpdescription.text
    property alias source: imgthumbnail.source
    property alias quoteColor: quoterect.color
    property color color: Theme.primaryColor

    id: webpageelement
    spacing: Theme.paddingSmall

    MessageText
    {
        id: wpurl
        emojiPath: context.sailorgram.emojiPath
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

        Rectangle
        {
            id: quoterect
            width: Theme.paddingSmall
            height: parent.height
        }

        Column
        {
            id: previewcontent
            width: parent.width - (Theme.paddingSmall * 2)
            spacing: Theme.paddingMedium

            Label
            {
                id: wptitle
                color: webpageelement.color
                width: parent.width
                wrapMode: Text.Wrap
                font { bold: true; pixelSize: Theme.fontSizeSmall }
            }

            Label
            {
                id: wpdescription
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
