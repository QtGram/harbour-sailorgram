import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0

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
            color: message.out ? Theme.highlightColor : Theme.primaryColor
            anchors { left: message.out ? parent.left : undefined; right: message.out ? undefined : parent.right }
            width: parent.width
            verticalAlignment: Text.AlignTop
            horizontalAlignment: message.out ? Text.AlignLeft : Text.AlignRight
            wrapMode: Text.WordWrap
            text: message.message
            visible: message.message.length > 0
        }

        Row
        {
            anchors { right: message.out ? undefined : parent.right; left: message.out ? parent.left : undefined }
            spacing: Theme.paddingSmall

            Image
            {
                id: imgstatus
                visible: message.out
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
            }
        }
    }
}
