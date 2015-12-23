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

    property real calculatedWidth: {
        if(TelegramHelper.isServiceMessage(message))
            return dummytextcontent.contentWidth;

        return Math.max(dummytextcontent.contentWidth, dummymsgstatus.contentWidth);
    }

    id: messagetext
    height: content.height

    Text
    {
        id: dummytextcontent
        font.pixelSize: mtctextcontent.font.pixelSize
        font.italic: mtctextcontent.font.italic
        text: mtctextcontent.text
        visible: false
    }

    Text
    {
        id: dummymsgstatus
        width: parent.width
        font.pixelSize: msgstatus.font.pixelSize
        font.italic: msgstatus.font.italic
        text: msgstatus.text
        visible: false
    }

    Column
    {
        id: content
        anchors.top: parent.top
        width: parent.width
        spacing: Theme.paddingSmall

        MessageTextContent
        {
            id: mtctextcontent
            width: parent.width
            horizontalAlignment: TelegramHelper.isServiceMessage(message) ? Text.AlignHCenter : Text.AlignLeft
            verticalAlignment: Text.AlignTop
            font.pixelSize: TelegramHelper.isServiceMessage(message) ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: TelegramHelper.isServiceMessage(message)
            emojiPath: context.sailorgram.emojiPath
            rawText: TelegramHelper.isServiceMessage(message) ? TelegramAction.actionType(context.telegram, dialog, message) : messageitem.message.message
            wrapMode: Text.WordWrap
            visible: text.length > 0
            color: ColorScheme.colorize(message, context)
            linkColor: ColorScheme.colorizeLink(message, context)
        }

        MessageStatus
        {
            id: msgstatus
            width: parent.width
            context: messagetext.context
            message: messagetext.message
        }
    }
}
