import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../conversation"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Telegram telegram
    property Dialog dialog
    property Chat chat
    property User user

    id: peeritem

    PeerImage
    {
        id: peerimage
        anchors { left: parent.left; top: parent.top }
        width: peeritem.height
        height: peeritem.height
        telegram: peeritem.telegram
        dialog: peeritem.dialog
        chat: peeritem.chat
        user: peeritem.user
    }

    Column
    {
        anchors { left: peerimage.right; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall }

        Label
        {
            id: lbltitle
            width: parent.width
            elide: Text.ElideRight
            text: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.userName(user)
        }

        Row
        {
            height: peeritem.height - lbltitle.contentHeight

            Label
            {
                id: lblinfo
                text: TelegramHelper.isChat(dialog) ? qsTr("%1 members", chat.participantsCount.toString()) : TelegramHelper.userStatus(user)
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
            }
        }
    }
}
