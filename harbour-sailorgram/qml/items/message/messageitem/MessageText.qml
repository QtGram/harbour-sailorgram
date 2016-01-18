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

    property real calculatedWidth: TelegramHelper.isServiceMessage(message) ?
                                       dummytextcontent.contentWidth :
                                       Math.max(dummytextcontent.contentWidth, dummymsgstatus.contentWidth);
    property real maxWidth

    id: messagetext
    height: content.height

    Text
    {
        id: dummytextcontent
        width: maxWidth
        wrapMode: mtctextcontent.wrapMode
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
            verticalAlignment: Text.AlignTop
            font.pixelSize: TelegramHelper.isServiceMessage(message) ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: TelegramHelper.isServiceMessage(message)
            emojiPath: context.sailorgram.emojiPath
            wrapMode: Text.Wrap
            visible: text.length > 0
            color: ColorScheme.colorizeText(message, context)
            linkColor: ColorScheme.colorizeLink(message, context)

            rawText: {
                if(TelegramHelper.isServiceMessage(message))
                    return TelegramAction.actionType(context.telegram, dialog, message);

                if(TelegramHelper.isMediaMessage(message) && (message.media.caption.length > 0))
                    return message.media.caption;

                return message.message;
            }

            horizontalAlignment: {
                if(TelegramHelper.isServiceMessage(message))
                    return Text.AlignHCenter;

                if(!message.out)
                    return Text.AlignRight;

                return Text.AlignLeft;
            }
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
