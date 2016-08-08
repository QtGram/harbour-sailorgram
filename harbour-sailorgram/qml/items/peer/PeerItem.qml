import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property alias isSecretChat: peerimage.isSecretChat

    property Context context
    property string title
    property string statusText
    property var peer
    property var chat
    property var user
    property var typing
    property bool showUsername: false

    id: peeritem

    PeerImage
    {
        id: peerimage
        anchors { right: parent.right; top: parent.top; rightMargin: Theme.horizontalPageMargin }
        width: peeritem.height - Theme.paddingSmall
        height: peeritem.height - Theme.paddingSmall
        context: peeritem.context
        peer: peeritem.peer
        chat: peeritem.chat
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
            visible: !peeritem.chat && showUsername
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
                if(peeritem.typing.length > 0) {
                    if(!peeritem.chat)
                        return qsTr("Typing...");

                    return TelegramHelper.typingUsers(context, peeritem.typing);
                }

                return peeritem.statusText;
            }
        }
    }
}
