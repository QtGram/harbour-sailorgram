import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction

Item
{
    property Context context
    property SailorgramDialogItem sgDialogItem
    property SailorgramMessageItem sgMessageItem
    property real maxWidth

    property real calculatedWidth: sgMessageItem.isActionMessage ? mtctextcontent.calculatedWidth :
                                                                   Math.max(mtctextcontent.calculatedWidth, msgstatus.calculatedWidth);

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
            font.pixelSize: sgMessageItem.isActionMessage ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: sgMessageItem.isActionMessage
            emojiPath: context.sailorgram.emojiPath
            wrapMode: Text.Wrap
            visible: rawText.length > 0
            color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
            linkColor: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)

            rawText: {
                if(sgMessageItem.isActionMessage)
                    return TelegramAction.actionType(sgMessageItem.messageAction, sgDialogItem.isSecretChat);

                return sgMessageItem.messageText;
            }

            horizontalAlignment: {
                if(sgMessageItem.isActionMessage)
                    return Text.AlignHCenter;

                if(!sgMessageItem.isMessageOut)
                    return Text.AlignRight;

                return Text.AlignLeft;
            }
        }

        MessageStatus
        {
            id: msgstatus
            width: parent.width
            context: messagetext.context
            messageDate: sgMessageItem.messageDate
            isMessageUnread: sgMessageItem.isMessageUnread
            isActionMessage: sgMessageItem.isActionMessage
            isMessageOut: sgMessageItem.isMessageOut
        }
    }
}
