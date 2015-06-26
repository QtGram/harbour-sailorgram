import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0

Item
{
    property Message message
    property bool me

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
            color: me ? Theme.highlightColor : Theme.primaryColor
            anchors { left: me ? parent.left : undefined; right: me ? undefined : parent.right }
            width: parent.width
            verticalAlignment: Text.AlignTop
            horizontalAlignment: me ? Text.AlignLeft : Text.AlignRight
            wrapMode: Text.WordWrap
            text: message.message
            visible: message.message.length > 0
        }

        Row
        {
            anchors { right: me ? undefined : parent.right; left: me ? parent.left : undefined }
            spacing: Theme.paddingSmall

            Image
            {
                id: imgsent
                visible: message.sent
                fillMode: Image.PreserveAspectFit
                width: lbldate.contentHeight
                height: lbldate.contentHeight
                source: "qrc:///res/sent.png"
            }

            Image
            {
                id: imgread
                visible: !message.unread
                fillMode: Image.PreserveAspectFit
                width: lbldate.contentHeight
                height: lbldate.contentHeight
                source: "qrc:///res/read.png"
            }

            Label
            {
                id: lbldate
                font.pixelSize: Theme.fontSizeTiny
                verticalAlignment: Text.AlignBottom
                horizontalAlignment: me ? Text.AlignLeft : Text.AlignRight
                text: TelegramCalendar.timeToString(message.date)
            }
        }
    }
}
