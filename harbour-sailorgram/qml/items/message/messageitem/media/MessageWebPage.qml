import QtQuick 2.1
import Sailfish.Silica 1.0
import "../"
import "../../../../js/ColorScheme.js" as ColorScheme
import "../../../../js/TelegramConstants.js" as TelegramConstants

MessageMediaItem
{
    readonly property bool hasPureImage: {
        if(!messageModelItem)
            return false;

        var webpage = messageModelItem.mediaItem.webpage;
        return (webpage.title.length <= 0) && (webpage.description.length <= 0);
    }

    readonly property real aspectRatio: {
        var imagesize = downloadHandler.imageSize;
        var w = imagesize.width;
        var h = imagesize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagewebpage

    contentWidth: {
        if(hasPureImage)
            return Math.max(mtctextcontent.calculatedWidth, downloadHandler.imageSize.width);

        return Math.max(mtctextcontent.calculatedWidth, mtctitle.calculatedWidth, mtcdescription.calculatedWidth);
    }

    contentHeight: {
        if(hasPureImage)
            return Math.min(mtctextcontent.calculatedHeight + (aspectRatio ? maxWidth / aspectRatio : 0), maxWidth);

        return mtctextcontent.calculatedHeight + mtctitle.calculatedHeight + mtcdescription.calculatedHeight;
    }

    Component.onCompleted: {
        if(context.autoloadimages && !downloadHandler.downloaded)
            downloadHandler.download();
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
        color: ColorScheme.colorizeTextItem(messageModelItem, context)
        linkColor: ColorScheme.colorizeLink(messageModelItem, context)
        emojiPath: context.sailorgram.emojiPath
        rawText: messageModelItem ? messageModelItem.mediaItem.webpage.url : ""
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
            visible: !hasPureImage
            width: !hasPureImage ? (parent.width - rect.width - webpagethumb.width - Theme.paddingSmall) : 0

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
                color: ColorScheme.colorizeTextItem(messageModelItem, context)
                linkColor: ColorScheme.colorizeLink(messageModelItem, context)
                emojiPath: context.sailorgram.emojiPath
                rawText: messageModelItem ? messageModelItem.mediaItem.webpage.title : ""
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
                color: ColorScheme.colorizeTextItem(messageModelItem, context)
                linkColor: ColorScheme.colorizeLink(messageModelItem, context)
                emojiPath: context.sailorgram.emojiPath
                rawText: messageModelItem ? messageModelItem.mediaItem.webpage.description : ""
            }
        }

        MessageThumbnail
        {
            id: webpagethumb
            height: parent.height
            anchors { right: parent.right; top: parent.top; bottom: parent.bottom; rightMargin: Theme.paddingSmall }
            visible: webpagethumb.status === Image.Ready
            fillMode: Image.PreserveAspectCrop
            blurRadius: messagewebpage.downloadHandler.downloaded && !messagewebpage.downloadHandler.downloading ? 0.0 : 32.0

            width: {
                if(!visible)
                    return 0;

                if(hasPureImage)
                    return  parent.width - (rect.width + 2 * Theme.paddingSmall);

                return parent.height;
            }

            source: {
                if(!messagewebpage.downloadHandler.downloaded || messagewebpage.downloadHandler.downloading)
                    return messagewebpage.downloadHandler.thumbnail;

                return messagewebpage.downloadHandler.destination;
            }
        }
    }
}
