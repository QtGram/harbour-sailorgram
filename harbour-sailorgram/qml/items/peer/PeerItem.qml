import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property alias context: peerimage.context
    property alias isSecretChat: peerimage.isSecretChat
    property alias peer: peerimage.peer

    property string title
    property string statusText
    property bool isChat
    property bool showUsername: false
    property var typingUsers
    property var user

    id: peeritem

    PeerImage
    {
        id: peerimage
        anchors { right: parent.right; top: parent.top; rightMargin: Theme.horizontalPageMargin }
        width: peeritem.height - Theme.paddingSmall
        height: peeritem.height - Theme.paddingSmall
        peer: peeritem.peer
        isChat: peeritem.isChat
        fallbackTitle: peeritem.title
    }

    Column
    {
        anchors { left: parent.left; top: parent.top; right: peerimage.left; rightMargin: Theme.paddingSmall }

        Label
        {
            id: lbltitle
            width: parent.width
            elide: Text.ElideRight
            text: peeritem.title
            horizontalAlignment: Qt.AlignRight
        }

        Label
        {
            id: lblusername
            width: parent.width
            elide: Text.ElideRight
            visible: !peeritem.isChat && showUsername
            text: peeritem.user ? "@" + peeritem.user.username : ""
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
                if(peeritem.typingUsers.length > 0)
                    return peeritem.typingUsers;

                return peeritem.statusText;
            }
        }
    }
}
