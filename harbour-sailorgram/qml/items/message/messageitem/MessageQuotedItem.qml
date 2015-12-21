import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction

Item
{
    property Context context
    property Message message
    property Message replyToMessage: context.telegram.message(message.replyToMsgId)
    property real calculatedWidth: Math.min(Math.max(mtctextcontent.contentWidth, lbluser.contentWidth), parent.width)

    id: messagequoteditem
    height: column.height
    width: parent.width

    Row
    {
        id: row
        anchors.fill: parent
        spacing: Theme.paddingSmall

        Rectangle
        {
            color: Theme.secondaryHighlightColor
            width: Theme.paddingSmall
            height: parent.height
        }

        Column
        {
            id: column
            width: parent.width - (Theme.paddingSmall * 2)

            Label
            {
                id: lbluser
                width: parent.width
                visible: !TelegramHelper.isServiceMessage(replyToMessage)
                font.bold: true
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                color: {
                    if(message.out)
                        return Theme.rgba(Theme.highlightDimmerColor, 1.0);

                    return Theme.rgba(Theme.secondaryColor, 1.0);
                }

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
                linkColor: message.out ? Theme.secondaryHighlightColor : Theme.secondaryColor
                rawText: TelegramHelper.isServiceMessage(replyToMessage) ? TelegramAction.actionType(context.telegram, dialog, replyToMessage) : replyToMessage.message
                verticalAlignment: Text.AlignTop
                wrapMode: Text.Wrap
                visible: text.length > 0

                color: {
                    if(TelegramHelper.isServiceMessage(message))
                        return Theme.secondaryHighlightColor;

                    if(message.out)
                        return Theme.highlightDimmerColor;

                    return Theme.primaryColor;
                }
            }
        }
    }
}

