import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction

Item
{
    property Context context
    property Message message
    property Message replyToMessage: context.telegram.message(message.replyToMsgId)
    property real calculatedWidth: Math.max(dummytextcontent.contentWidth, lbluser.contentWidth) + quotedindicatorrect.width + row.spacing
    property real maxWidth

    id: messagequoteditem
    height: column.height

    Text
    {
        id: dummytextcontent
        width: maxWidth - quotedIndicatorRect.width - row.spacing
        visible: false
        font.pixelSize: mtctextcontent.font.pixelSize
        font.italic: mtctextcontent.font.italic
        text: mtctextcontent.rawText
        wrapMode: mtctextcontent.wrapMode
    }

    Row
    {
        id: row
        anchors.fill: parent
        spacing: Theme.paddingSmall

        Rectangle
        {
            id: quotedindicatorrect
            color: Theme.secondaryHighlightColor
            width: Theme.paddingSmall
            height: parent.height
        }

        Column
        {
            id: column
            width: parent.width - row.spacing - quotedindicatorrect.width

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

                    if(message.out)
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
                rawText: TelegramHelper.isServiceMessage(replyToMessage) ? TelegramAction.actionType(context.telegram, dialog, replyToMessage) : replyToMessage.message
                verticalAlignment: Text.AlignTop
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                maximumLineCount: 3
                visible: text.length > 0
                color: ColorScheme.colorizeText(message, context)
                linkColor: ColorScheme.colorizeLink(message, context)
            }
        }
    }
}

