import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/TelegramAction.js" as TelegramAction
import "../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property Context context
    property var messageModelItem

    readonly property bool isActionMessage: messageModelItem.messageType === Enums.TypeActionMessage

    property real calculatedWidth: isActionMessage ? mtctextcontent.calculatedWidth :
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
            font.pixelSize: isActionMessage ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: isActionMessage
            emojiPath: context.sailorgram.emojiPath
            wrapMode: Text.Wrap
            visible: rawText.length > 0
            color: ColorScheme.colorizeTextItem(messageModelItem, context)
            linkColor: ColorScheme.colorizeLink(messageModelItem, context)

            rawText: {
                if(isActionMessage)
                    return TelegramAction.actionType(messageModelItem.serviceItem, messageModelItem.fromUserItem, messageModelItem.toUserItem);

                /*
                if(TelegramHelper.isMediaMessage(message)) {
                    if((message.media.classType === TelegramConstants.typeMessageMediaWebPage) &&
                       (message.media.webpage.url === message.message))
                        return ""; // Message contains only a link

                    if(message.media.caption.length > 0)
                        return message.media.caption;
                }
                */

                return messageModelItem.message;
            }

            horizontalAlignment: {
                if(isActionMessage)
                    return Text.AlignHCenter;

                if(!messageModelItem.out)
                    return Text.AlignRight;

                return Text.AlignLeft;
            }
        }

        MessageStatus
        {
            id: msgstatus
            width: parent.width
            context: messagetext.context
            messageDate: messageModelItem.dateTime
            messageUnread: messageModelItem.unread
            messageType: messageModelItem.messageType
            messageOut: messageModelItem.out
        }
    }
}
