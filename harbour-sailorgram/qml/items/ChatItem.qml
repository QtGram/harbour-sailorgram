import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../items"

Item
{
    property Dialog dialog
    property User user

    id: chatitem

    onDialogChanged: {
        user = telegram.user(dialog.peer.userId);
    }

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        UserAvatar
        {
            id: useravatar
            width: chatitem.height
            height: chatitem.height
            telegram: dialogspage.telegram
            user: chatitem.user
        }

        Column
        {
            width: parent.width - useravatar.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: chatitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label {
                    id: lbluser
                    width: parent.width - lbltime.width
                    text: ((user.username.length > 0) ? user.username : (user.firstName + " " + user.lastName))
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                }

                Label {
                    id: lbltime
                    height: parent.height
                    font.pixelSize: Theme.fontSizeTiny
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    text: TelegramCalendar.timeToString(telegram.message(dialog.topMessage).date)
                }
            }

            Row
            {
                height: chatitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: telegram.message(dialog.topMessage).message
                }

                Rectangle
                {
                    id: rectunread
                    width: parent.height
                    height: parent.height
                    color: Theme.secondaryHighlightColor
                    visible: dialog.unreadCount > 0
                    radius: width * 0.5

                    Label
                    {
                        anchors.centerIn: parent
                        font.pixelSize: Theme.fontSizeTiny
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: dialog.unreadCount
                    }
                }
            }
        }
    }
}
