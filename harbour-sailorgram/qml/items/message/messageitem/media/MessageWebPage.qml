import QtQuick 2.1
import Sailfish.Silica 1.0
import "../"
import "../../../../js/ColorScheme.js" as ColorScheme
import "../../../../js/TelegramConstants.js" as TelegramConstants

MessageMediaItem
{
    readonly property bool hasPureImage: (message.media.webpage.title.length <= 0) && (message.media.webpage.description.length <= 0)

    readonly property real aspectRatio: {
        var w = fileHandler.imageSize.width;
        var h = fileHandler.imageSize.height;

        if(!w || !h)
            return 0;

        return w / h;
    }

    id: messagewebpage

    contentWidth: {
        if(hasPureImage)
            return Math.max(lbldummytextcontent.contentWidth, fileHandler.imageSize.width);

        return Math.max(lbldummytextcontent.contentWidth, lbldummytitle.contentWidth, lbldummydescription.contentWidth);
    }

    contentHeight: {
        if(hasPureImage)
            return lbldummytextcontent.contentHeight + (aspectRatio ? maxWidth / aspectRatio : 0)

        return lbldummytextcontent.contentHeight + lbldummytitle.contentHeight + lbldummydescription.contentHeight;
    }

    Label { id: lbldummytextcontent; text: mtctextcontent.text; textFormat: Text.StyledText; font.pixelSize: mtctextcontent.font.pixelSize; wrapMode: Text.NoWrap; visible: false }
    Label { id: lbldummytitle; text: mtctitle.text; textFormat: Text.StyledText; font.pixelSize: mtctitle.font.pixelSize; wrapMode: Text.NoWrap; visible: false }
    Label { id: lbldummydescription; text: mtcdescription.text; textFormat: Text.StyledText; font.pixelSize: mtctitle.font.pixelSize; wrapMode: Text.NoWrap; visible: false }

    MessageTextContent
    {
        id: mtctextcontent
        width: parent.width - Theme.paddingSmall
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        font.pixelSize: Theme.fontSizeSmall
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
        color: ColorScheme.colorizeText(message, context)
        linkColor: ColorScheme.colorizeLink(message, context)
        emojiPath: context.sailorgram.emojiPath
        rawText: message.media.webpage.url
    }

    Item
    {
        anchors { top: mtctextcontent.bottom; topMargin: Theme.paddingSmall }
        width: parent.width - Theme.paddingSmall
        height: parent.height - mtctextcontent.contentHeight

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
            height: parent.height

            MessageTextContent
            {
                id: mtctitle
                anchors { left: parent.left; top: parent.top; right: parent.right }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                color: ColorScheme.colorizeText(message, context)
                linkColor: ColorScheme.colorizeLink(message, context)
                emojiPath: context.sailorgram.emojiPath
                rawText: message.media.webpage.title
            }

            MessageTextContent
            {
                id: mtcdescription
                anchors { left: parent.left; top: mtctitle.bottom; right: parent.right }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                color: ColorScheme.colorizeText(message, context)
                linkColor: ColorScheme.colorizeLink(message, context)
                emojiPath: context.sailorgram.emojiPath
                rawText: message.media.webpage.description
            }
        }

        MessageThumbnail
        {
            id: webpagethumb
            height: parent.height
            anchors { right: parent.right; top: parent.top; bottom: parent.bottom; rightMargin: Theme.paddingSmall }

            width: {
                if(hasPureImage)
                    return  parent.width - (rect.width + 2 * Theme.paddingSmall);

                if(aspectRatio)
                    return height / aspectRatio;

                return parent.height;
            }

            source: {
                if(!messagewebpage.fileHandler.downloaded || messagewebpage.transferInProgress)
                    return messagewebpage.fileHandler.thumbPath;

                return messagewebpage.fileHandler.filePath;
            }
        }
    }
}
