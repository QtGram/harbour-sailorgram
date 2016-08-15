import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../components"
import "../../../items/message/messageitem/media"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction

Row
{
    property alias showQuoteRect: rect.visible

    readonly property real contentWidth: {
        var w = imgmedia.visible ? imgmedia.width : 0;
        return w + Math.max(mtctextcontent.calculatedWidth, lbluser.calculatedWidth) + (showQuoteRect ? rect.width : 0) + spacing;
    }

    property Context context
    property SailorgramMessageItem sgMessageItem
    property SailorgramPeer sgPeer
    property real maxWidth
    property string titlePrefix: ""
    property bool showUser: true
    property color textColor: Theme.primaryColor
    property color linkColor: Theme.highlightColor

    id: messagepreview
    spacing: Theme.paddingSmall
    width: Math.min(contentWidth, maxWidth)
    height: column.height

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
        useTelegramImage: true
        context: messagepreview.context
        source: sgMessageItem ? sgMessageItem.messageMedia.rawMedia : null

        visible: {
            if(!sgMessageItem)
                return false;

            if((sgMessageItem.messageType === SailorgramEnums.MessageTypeWebPage) && sgMessageItem.messageMedia.webPage.hasThumbnail)
                return false;

            return !sgMessageItem.isTextMessage;
        }

        fillMode: {
            if(sgMessageItem && (sgMessageItem.messageType === SailorgramEnums.MessageTypeWebPage) && sgMessageItem.messageMedia.webPage.hasThumbnail)
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
            visible: sgMessageItem && !sgMessageItem.isActionMessage
            color: messagepreview.textColor
            font.bold: true
            font.pixelSize: Theme.fontSizeTiny
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter

            text: {
                if(!sgMessageItem || sgMessageItem.isActionMessage)
                    return "";

                if(!showUser)
                    return titlePrefix;

                if(titlePrefix.length > 0)
                    titlePrefix += " ";

                if(sgMessageItem.isMessageOut)
                    return titlePrefix + qsTr("You");

                if(sgPeer.isChat)
                    return titlePrefix + sgPeer.chat.title;
                else
                    return titlePrefix + TelegramHelper.completeName(sgPeer.user);
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
            visible: sgMessageItem && text.length > 0
            color: messagepreview.textColor
            linkColor: messagepreview.linkColor
            openUrls: false

            rawText: {
                if(!sgMessageItem)
                    return "";

                if(sgMessageItem.isActionMessage)
                    return TelegramAction.actionType(sgMessageItem.messageAction,sgMessageItem, false); //NOTE: isSecretChat

                if(sgMessageItem.messageType === SailorgramEnums.MessageTypeSticker)
                    return "Sticker";

                if(sgMessageItem.messageType === SailorgramEnums.MessageTypeDocument)
                    return sgMessageItem.messageMedia.fileName;

                if(sgMessageItem.messageType === SailorgramEnums.MessageTypeContact)
                    return "Contacts"; //FIXME: media.firstName + " " + media.lastName + "\n" + media.phoneNumber;

                return sgMessageItem.messageText;
            }
        }
    }
}
