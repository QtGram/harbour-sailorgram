import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../peer"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Telegram telegram
    property User user

    id: contactitem

    PeerImage
    {
        id: useravatar
        anchors { left: parent.left; top: parent.top }
        width: contactitem.height
        height: contactitem.height
        telegram: contactitem.telegram
        user: contactitem.user
    }

    Column
    {
        anchors { left: useravatar.right; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall }

        Label
        {
            id: lblfullname
            width: parent.width
            elide: Text.ElideRight
            text: TelegramHelper.userName(user)
        }

        Row
        {
            height: contactitem.height - lblfullname.contentHeight

            Label {
                id: lblstaticstatus
                text: TelegramHelper.userStatus(user)
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
            }

            Label {
                id: lblstatus
                width: contactitem.width - lblstaticstatus.contentWidth
                font.pixelSize: Theme.fontSizeExtraSmall
                elide: Text.ElideRight
            }
        }
    }
}
