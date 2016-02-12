import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../items/message/messageitem/media"
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction
import "../../../js/TelegramMedia.js" as TelegramMedia
import "../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property Context context
    property Message message
    property Message replyToMessage: context.telegram.message(message.replyToMsgId)
    property real maxWidth

    id: messagequoteditem
    height: row.height

    width: {
        var w = Math.max(dummytextcontent.contentWidth, dummyuser.contentWidth) + quotedindicatorrect.width + row.spacing
        return Math.min(w, maxWidth);
    }

    FileHandler
    {
        id: filehandler
        telegram: context.telegram
        target: replyToMessage
        defaultThumbnail: TelegramMedia.defaultThumbnail(replyToMessage)
    }

    Text
    {
        id: dummytextcontent
        visible: false
        font.pixelSize: mtctextcontent.font.pixelSize
        font.italic: mtctextcontent.font.italic
        text: mtctextcontent.rawText
        wrapMode: mtctextcontent.wrapMode
    }

    Text
    {
        id: dummyuser
        visible: false
        font.bold: true
        font.pixelSize: Theme.fontSizeTiny
        text: lbluser.text
        wrapMode: lbluser.wrapMode
    }

    Row
    {
        id: row
        width: parent.width
        height: column.height
        spacing: Theme.paddingSmall

        Rectangle
        {
            id: quotedindicatorrect
            color: Theme.secondaryHighlightColor
            width: Theme.paddingSmall
            height: column.height
        }

        MessageThumbnail
        {
            id: imgmedia
            width: parent.height
            height: parent.height
            source: filehandler.thumbPath
            visible: TelegramHelper.isMediaMessage(replyToMessage)
        }

        Column
        {
            id: column
            width: parent.width
            height: lbluser.contentHeight + mtctextcontent.contentHeight

            Label
            {
                id: lbluser
                width: parent.width
                visible: !TelegramHelper.isServiceMessage(replyToMessage)
                color: ColorScheme.colorizeText(message, context)
                font.bold: true
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter

                text: {
                    if(TelegramHelper.isServiceMessage(replyToMessage))
                        return "";

                    if(replyToMessage.out)
                        return qsTr("You");

                    return TelegramHelper.completeName(context.telegram.user(replyToMessage.fromId))
                }
            }

            MessageTextContent
            {
                id: mtctextcontent
                font.pixelSize: Theme.fontSizeExtraSmall
                font.italic: true
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                emojiPath: context.sailorgram.emojiPath
                verticalAlignment: Text.AlignTop
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                maximumLineCount: 3
                visible: text.length > 0
                color: ColorScheme.colorizeText(message, context)
                linkColor: ColorScheme.colorizeLink(message, context)

                rawText: {
                    if(TelegramHelper.isServiceMessage(replyToMessage))
                        return TelegramAction.actionType(context.telegram, dialog, replyToMessage);

                    if(TelegramHelper.isSticker(context, replyToMessage))
                        return "Sticker";

                    if(TelegramHelper.isMediaMessage(replyToMessage) && (replyToMessage.media.classType === TelegramConstants.typeMessageMediaDocument))
                        return filehandler.fileName;

                    return TelegramHelper.messageContent(replyToMessage);
                }
            }
        }
    }
}

