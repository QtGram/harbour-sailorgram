import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
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
            telegram: mainpage.telegram
            user: chatitem.user
        }

        Column
        {
            width: parent.width - useravatar.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: chatitem.height / 2

                Label {
                    id: lbluser
                    text: ((user.username.length > 0) ? user.username : (user.firstName + " " + user.lastName))
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                }

                Label {
                    id: lbltime
                    width: chatitem.width - lbluser.contentWidth
                    font.pixelSize: Theme.fontSizeTiny
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    elide: Text.ElideRight
                }
            }

            Label
            {
                id: lbllastmessage
                width: chatitem.width
                height: chatitem.height / 2
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                text: telegram.message(dialog.topMessage).message
            }
        }
    }
}
