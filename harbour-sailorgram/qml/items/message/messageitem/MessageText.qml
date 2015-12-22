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
    property real calculatedWidth: Math.max(mtctextcontent.contentWidth, (msgstatus.width + lbldate.contentWidth))

    id: messagetext
    height: content.height

    Column
    {
        id: content
        anchors.top: parent.top
        width: parent.width
        spacing: Theme.paddingSmall

        MessageTextContent
        {
            id: mtctextcontent

            anchors {
                left: message.out ? parent.left : undefined;
                right: message.out ? undefined : parent.right;
                leftMargin: message.out ? Theme.paddingSmall : undefined
                rightMargin: message.out ? undefined : Theme.paddingSmall
            }

            width: parent.width
            font.pixelSize: TelegramHelper.isServiceMessage(message) ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: TelegramHelper.isServiceMessage(message)
            horizontalAlignment: TelegramHelper.isServiceMessage(message) ? Text.AlignHCenter : (message.out ? Text.AlignLeft : Text.AlignRight)
            emojiPath: context.sailorgram.emojiPath
            rawText: TelegramHelper.isServiceMessage(message) ? TelegramAction.actionType(context.telegram, dialog, message) : messageitem.message.message
            verticalAlignment: Text.AlignTop
            wrapMode: Text.WordWrap
            visible: text.length > 0
            color: ColorScheme.colorize(message, context)
            linkColor: ColorScheme.colorizeLink(message, context)
        }

        Row
        {
            anchors {
                right: message.out ? undefined : parent.right;
                left: message.out ? parent.left : undefined
                leftMargin: message.out ? Theme.paddingSmall : undefined
                rightMargin: message.out ? undefined : Theme.paddingSmall
            }

            Label
            {
                id: lbldate
                font.pixelSize: Theme.fontSizeTiny
                verticalAlignment: Text.AlignBottom
                horizontalAlignment: message.out ? Text.AlignLeft : Text.AlignRight
                text: TelegramHelper.printableDate(message.date)
                visible: !TelegramHelper.isServiceMessage(message)
                width: messagetext.calculatedWidth - msgstatus.paintedWidth
                color: ColorScheme.colorize(message, context)
            }

            MessageStatus
            {
                id: msgstatus
                height: lbldate.paintedHeight
                context: messagetext.context
                message: messagetext.message
            }
        }
    }
}
