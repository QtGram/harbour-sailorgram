import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Context context
    property var dialog
    property Chat chat
    property User user
    property bool showUsername: false

    id: peeritem

    PeerImage
    {
        id: peerimage
        anchors { right: parent.right; top: parent.top; rightMargin: Theme.horizontalPageMargin }
        width: peeritem.height - Theme.paddingSmall
        height: peeritem.height - Theme.paddingSmall
        context: peeritem.context
        dialog: peeritem.dialog
        chat: peeritem.chat
        user: peeritem.user
    }

    Column
    {
        anchors { left: parent.left; top: parent.top; right: peerimage.left; rightMargin: Theme.paddingSmall }

        Label
        {
            id: lbltitle
            width: parent.width
            elide: Text.ElideRight
            text: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.completeName(user)
            horizontalAlignment: Qt.AlignRight
        }

        Label
        {
            id: lblusername
            width: parent.width
            elide: Text.ElideRight
            visible: !TelegramHelper.isChat(dialog) && showUsername
            text: user ? "@" + user.username : ""
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryHighlightColor
            horizontalAlignment: Qt.AlignRight
        }

        Label
        {
            id: lblinfo
            width: parent.width
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.highlightColor
            horizontalAlignment: Qt.AlignRight

            text: {
                if(dialog.typingUsers.length > 0)
                    return TelegramHelper.typingUsers(context, dialog);

                if(TelegramHelper.isChat(dialog))
                    return qsTr("%n member(s)", "", chat.participantsCount);

                return TelegramHelper.userStatus(user);
            }
        }
    }
}
