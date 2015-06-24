import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0

ListItem
{
    property Message message
    property bool me: (message.fromId === telegram.me)

    id: messageitem
    contentWidth: parent.width
    contentHeight: content.height

    Column
    {
        id: content
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingLarge; rightMargin: Theme.paddingLarge }

        Rectangle
        {
            anchors { left: me ? parent.left : undefined; right: me ? undefined : parent.right }
            width: Math.max(lbltext.paintedWidth, lbldate.paintedWidth)
            height: lbltext.paintedHeight + lbldate.paintedHeight
            color: Theme.rgba(Theme.highlightDimmerColor, 0.0)

            Label
            {
                id: lbltext
                color: me ? Theme.highlightColor : Theme.primaryColor
                anchors { left: parent.left; top: parent.top }
                width: content.width
                verticalAlignment: Text.AlignTop
                wrapMode: Text.WordWrap
                text: message.message
            }

            Label
            {
                id: lbldate
                anchors { top: lbltext.bottom; right: me ? undefined : parent.right; left: me ? parent.left : undefined }
                width: content.width
                font.pixelSize: Theme.fontSizeTiny
                verticalAlignment: Text.AlignBottom
                horizontalAlignment: me ? Text.AlignLeft : Text.AlignRight
                text: TelegramCalendar.timeToString(message.date)
            }
        }
    }
}
