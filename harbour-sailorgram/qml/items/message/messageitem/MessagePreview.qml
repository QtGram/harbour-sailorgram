import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
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
    property var messageType
    property var messageOut
    property string messageText
    property MessageMedia mediaItem
    property var peer
    property real maxWidth
    property string titlePrefix: ""
    property bool showUser: true
    property color textColor: Theme.primaryColor
    property color linkColor: Theme.highlightColor

    id: messagepreview
    spacing: Theme.paddingSmall
    width: Math.min(contentWidth, maxWidth)
    height: column.height

    DownloadHandler
    {
        id: downloadhandler
        engine: context.engine
        source: messagepreview.mediaItem
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
        source: downloadhandler.downloaded ? downloadhandler.destination : downloadhandler.thumbnail

        visible: {
            //FIXME: if(TelegramHelper.isWebPage(message) && !TelegramHelper.webPageHasThumb(message))
            if(messageType === Enums.TypeWebPageMessage)
                return false;

            return messagepreview.messageType !== Enums.TypeTextMessage;
        }

        fillMode: {
            //FIXME: if(TelegramHelper.isWebPage(message) && TelegramHelper.webPageHasThumb(message))
            if(messageType === Enums.TypeWebPageMessage)
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
            visible: messageType !== Enums.TypeActionMessage
            color: messagepreview.textColor
            font.bold: true
            font.pixelSize: Theme.fontSizeTiny
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter

            text: {
                if(messageType === Enums.TypeActionMessage)
                    return "";

                if(!showUser)
                    return titlePrefix;

                if(titlePrefix.length > 0)
                    titlePrefix += " ";

                if(messageOut)
                    return titlePrefix + qsTr("You");

                return titlePrefix + TelegramHelper.completeName(peer);
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
                if(messageType === Enums.TypeActionMessage)
                    return "!!!"; //FIXME: TelegramAction.actionType(context.telegram, dialog, message);

                if(messageType === Enums.TypeStickerMessage)
                    return "Sticker";

                /*
                if(messageType === Enums.TypeDocumentMessage)
                    return "";

                if(messageType !== Enums.TypeTextMessage)
                {
                    switch(media.classType)
                    {
                        case TelegramConstants.typeMessageMediaDocument:
                            return downloadhandler.fileName;
                        case TelegramConstants.typeMessageMediaContact:
                            return media.firstName + " " + media.lastName + "\n" + media.phoneNumber;
                    }
                }
                */

                return messageText;
            }
        }
    }
}
