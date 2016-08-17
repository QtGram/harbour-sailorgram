import QtQuick 2.1
import Sailfish.Silica 1.0
import "../"
import "../../../../js/ColorScheme.js" as ColorScheme

MessageMediaItem
{
    readonly property real aspectRatio: {
        var imagesize = webpagethumb.imageSize;
        var w = imagesize.width;
        var h = imagesize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagewebpage

    contentWidth: {
        if(sgMessageItem.messageMedia.webPage.hasPureImage)
            return Math.max(mtctextcontent.calculatedWidth, webpagethumb.imageSize.width);

        return Math.max(mtctextcontent.calculatedWidth, mtctitle.calculatedWidth, mtcdescription.calculatedWidth);
    }

    contentHeight: {
        if(sgMessageItem.messageMedia.webPage.hasPureImage)
            return Math.min(mtctextcontent.calculatedHeight + (aspectRatio ? maxWidth / aspectRatio : 0), maxWidth);

        return mtctextcontent.calculatedHeight + mtctitle.calculatedHeight + mtcdescription.calculatedHeight;
    }

    Component.onCompleted: {
        if(context.autoloadimages && !sgMessageItem.messageMedia.isTransfered)
            sgMessageItem.messageMedia.download();
    }

    MessageTextContent
    {
        id: mtctextcontent
        width: parent.width - Theme.paddingSmall
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        font.pixelSize: Theme.fontSizeSmall
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
        color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
        linkColor: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
        emojiPath: context.sailorgram.emojiPath
        rawText: sgMessageItem.messageMedia.webPage.url
    }

    Item
    {
        anchors { top: mtctextcontent.bottom; bottom: parent.bottom; topMargin: Theme.paddingSmall }
        width: parent.width - Theme.paddingSmall

        Rectangle
        {
            id: rect
            anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
            color: Theme.secondaryHighlightColor
            width: Theme.paddingSmall
            height: parent.height
        }

        Item
        {
            anchors { left: rect.right; top: parent.top; bottom: parent.bottom; leftMargin: Theme.paddingSmall }
            visible: !sgMessageItem.messageMedia.webPage.hasPureImage
            width: !sgMessageItem.messageMedia.webPage.hasPureImage ? (parent.width - rect.width - webpagethumb.width - Theme.paddingSmall) : 0

            MessageTextContent
            {
                id: mtctitle
                anchors { left: parent.left; top: parent.top; right: parent.right }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                maximumLineCount: 2
                elide: Text.ElideRight
                color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                linkColor: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                emojiPath: context.sailorgram.emojiPath
                rawText: sgMessageItem.messageMedia.webPage.title
            }

            MessageTextContent
            {
                id: mtcdescription
                anchors { left: parent.left; top: mtctitle.bottom; right: parent.right }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                maximumLineCount: 2
                elide: Text.ElideRight
                color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                linkColor: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                emojiPath: context.sailorgram.emojiPath
                rawText: sgMessageItem.messageMedia.webPage.description
            }
        }

        MessageThumbnail
        {
            id: webpagethumb
            height: parent.height
            anchors { right: parent.right; top: parent.top; bottom: parent.bottom; rightMargin: Theme.paddingSmall }
            visible: webpagethumb.status === Image.Ready
            fillMode: Image.PreserveAspectCrop
            useTelegramImage: true
            autoDownload: true
            context: messagewebpage.context
            source: sgMessageItem.rawMessage

            width: {
                if(!visible)
                    return 0;

                if(sgMessageItem.messageMedia.webPage.hasPureImage)
                    return  parent.width - (rect.width + 2 * Theme.paddingSmall);

                return parent.height;
            }
        }
    }
}
