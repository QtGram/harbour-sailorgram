import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction
import "../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property Context context
    property Dialog dialog
    property Message message

    property real calculatedWidth: TelegramHelper.isServiceMessage(message) ? mtctextcontent.calculatedWidth :
                                                                              Math.max(mtctextcontent.calculatedWidth, msgstatus.calculatedWidth);
    property real maxWidth

    id: messagetext
    height: content.height + Theme.paddingSmall

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
            height: visible ? undefined : 0
            verticalAlignment: Text.AlignTop
            font.pixelSize: TelegramHelper.isServiceMessage(message) ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: TelegramHelper.isServiceMessage(message)
            emojiPath: context.sailorgram.emojiPath
            wrapMode: Text.Wrap
            visible: rawText.length > 0
            color: ColorScheme.colorizeText(message, context)
            linkColor: ColorScheme.colorizeLink(message, context)

            rawText: {
                if(TelegramHelper.isServiceMessage(message))
                    return TelegramAction.actionType(context.telegram, dialog, message);

                if(TelegramHelper.isMediaMessage(message)) {
                    if((message.media.classType === TelegramConstants.typeMessageMediaWebPage) && (message.media.webpage.url.length > 0))
                        return ""; // There is MessageWebPage for that

                    if(message.media.caption.length > 0)
                        return message.media.caption;
                }

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
