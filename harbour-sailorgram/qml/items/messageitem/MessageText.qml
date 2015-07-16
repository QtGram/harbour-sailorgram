import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramAction.js" as TelegramAction

Item
{
    property Message message

    id: messagetext
    height: content.height

    Column
    {
        id: content
        anchors.top: parent.top
        width: parent.width
        spacing: Theme.paddingSmall

        Label
        {
            id: lbltext
            anchors { left: message.out ? parent.left : undefined; right: message.out ? undefined : parent.right }
            width: parent.width
            color: TelegramHelper.isActionMessage(message) ? Theme.primaryColor : (message.out ? Theme.highlightColor : Theme.primaryColor)
            font.pixelSize: TelegramHelper.isActionMessage(message) ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall
            font.italic: TelegramHelper.isActionMessage(message)
            horizontalAlignment: TelegramHelper.isActionMessage(message) ? Text.AlignHCenter : (message.out ? Text.AlignLeft : Text.AlignRight)
            text: TelegramHelper.isActionMessage(message) ? TelegramAction.actionType(context.telegram, dialog, message) : messageitem.message.message
            verticalAlignment: Text.AlignTop
            wrapMode: Text.WordWrap
            visible: text.length > 0
        }

        Row
        {
            anchors { right: message.out ? undefined : parent.right; left: message.out ? parent.left : undefined }
            spacing: Theme.paddingSmall

            Image
            {
                id: imgstatus
                visible: message.out && !TelegramHelper.isActionMessage(message)
                fillMode: Image.PreserveAspectFit
                width: lbldate.contentHeight
                height: lbldate.contentHeight

                source: {
                    if(!message.unread)
                        return "qrc:///res/read.png";

                    if(message.sent)
                        return "qrc:///res/sent.png";

                    return "qrc:///res/out.png";
                }
            }

            Label
            {
                id: lbldate
                font.pixelSize: Theme.fontSizeTiny
                verticalAlignment: Text.AlignBottom
                horizontalAlignment: message.out ? Text.AlignLeft : Text.AlignRight
                text: TelegramCalendar.timeToString(message.date)
                visible: !TelegramHelper.isActionMessage(message)
            }
        }
    }
}
