import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../components"
import "../../../items/message/messageitem/media"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction
import "../../../js/TelegramMedia.js" as TelegramMedia
import "../../../js/TelegramConstants.js" as TelegramConstants

Row
{
    property alias showQuoteRect: rect.visible

    readonly property real contentWidth: {
        var w = imgmedia.visible ? imgmedia.width : 0;
        return w + Math.max(mtctextcontent.calculatedWidth, lbluser.calculatedWidth) + (showQuoteRect ? rect.width : 0) + spacing;
    }

    property Context context
    property Dialog dialog
    property Message message
    property real maxWidth
    property string titlePrefix: ""
    property bool showUser: true
    property color textColor: Theme.primaryColor
    property color linkColor: Theme.highlightColor

    id: messagepreview
    spacing: Theme.paddingSmall
    width: Math.min(contentWidth, maxWidth)
    height: column.height

    FileHandler
    {
        id: filehandler
        telegram: context.telegram
        target: messagepreview.message
        defaultThumbnail: TelegramMedia.defaultThumbnail(message)
    }

    Rectangle
    {
        id: rect
        color: Theme.secondaryHighlightColor
        width: Theme.paddingSmall
        height: parent.height
    }

    MessageThumbnail
    {
        id: imgmedia
        width: parent.height
        height: parent.height
        source: filehandler.downloaded ? filehandler.filePath : filehandler.thumbPath

        visible: {
            if(TelegramHelper.isWebPage(message) && !TelegramHelper.webPageHasThumb(message))
                return false;

            return TelegramHelper.isMediaMessage(message);
        }

        fillMode: {
            if(TelegramHelper.isWebPage(message) && TelegramHelper.webPageHasThumb(message))
                return Image.PreserveAspectCrop;

            return Image.PreserveAspectFit;
        }
    }

    Column
    {
        id: column
        width: parent.width - (imgmedia.visible ? imgmedia.width : 0)

        ResizableLabel
        {
            id: lbluser
            width: parent.width - Theme.paddingSmall
            visible: !TelegramHelper.isServiceMessage(message)
            color: messagepreview.textColor
            font.bold: true
            font.pixelSize: Theme.fontSizeTiny
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter

            text: {
                if(!message || TelegramHelper.isServiceMessage(message))
                    return "";

                if(!showUser)
                    return titlePrefix;

                if(titlePrefix.length > 0)
                    titlePrefix += " ";

                if(message.out)
                    return titlePrefix + qsTr("You");

                return titlePrefix + TelegramHelper.completeName(context.telegram.user(message.fromId))
            }
        }

        MessageTextContent
        {
            id: mtctextcontent
            font.pixelSize: Theme.fontSizeExtraSmall
            font.italic: true
            width: parent.width - Theme.paddingSmall
            horizontalAlignment: Text.AlignLeft
            emojiPath: context.sailorgram.emojiPath
            verticalAlignment: Text.AlignTop
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 3
            visible: text.length > 0
            color: messagepreview.textColor
            linkColor: messagepreview.linkColor
            openUrls: false

            rawText: {
                if(!message)
                    return "";

                if(TelegramHelper.isServiceMessage(message))
                    return TelegramAction.actionType(context.telegram, dialog, message);

                if(TelegramHelper.isSticker(context, message))
                    return "Sticker";

                if(TelegramHelper.isMediaMessage(message) && (message.media.classType === TelegramConstants.typeMessageMediaDocument))
                    return filehandler.fileName;

                return TelegramHelper.messageContent(context, message);
            }
        }
    }
}
