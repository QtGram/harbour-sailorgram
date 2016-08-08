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
    property User fromUser
    property User serviceUser
    property var messageDateTime
    property var messageType
    property var serviceItem
    property string messageText
    property bool messageOut
    property bool messageUnread
    property real maxWidth

    readonly property bool isActionMessage: messageType === Enums.TypeActionMessage

    property real calculatedWidth: isActionMessage ? mtctextcontent.calculatedWidth :
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
            font.pixelSize: isActionMessage ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: isActionMessage
            emojiPath: context.sailorgram.emojiPath
            wrapMode: Text.Wrap
            visible: rawText.length > 0
            color: ColorScheme.colorizeText(messageType, messageOut, context)
            linkColor: ColorScheme.colorizeLink(messageType, messageOut, context)

            rawText: {
                if(isActionMessage) {
                    console.log(messagetext.serviceUser);
                    return TelegramAction.actionType(messagetext.serviceItem, messagetext.fromUser, messagetext.serviceUser);
                }

                /*
                if(TelegramHelper.isMediaMessage(message)) {
                    if((message.media.classType === TelegramConstants.typeMessageMediaWebPage) &&
                       (message.media.webpage.url === message.message))
                        return ""; // Message contains only a link

                    if(message.media.caption.length > 0)
                        return message.media.caption;
                }
                */

                return messageText;
            }

            horizontalAlignment: {
                if(isActionMessage)
                    return Text.AlignHCenter;

                if(!messageOut)
                    return Text.AlignRight;

                return Text.AlignLeft;
            }
        }

        MessageStatus
        {
            id: msgstatus
            width: parent.width
            context: messagetext.context
            messageDateTime: messagetext.messageDateTime
            messageUnread: messagetext.messageUnread
            messageType: messagetext.messageType
            messageOut: messagetext.messageOut
        }
    }
}
