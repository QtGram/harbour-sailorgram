import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Context context
    property var dialogModelItem
    property bool showUsername: false

    id: peeritem

    PeerImage
    {
        id: peerimage
        anchors { right: parent.right; top: parent.top; rightMargin: Theme.horizontalPageMargin }
        width: peeritem.height - Theme.paddingSmall
        height: peeritem.height - Theme.paddingSmall
        context: peeritem.context
        peer: dialogModelItem.peer
        fallbackTitle: dialogModelItem.title
        isChat: dialogModelItem.chat !== null
        isSecretChat: dialogModelItem.isSecretChat
    }

    Column
    {
        anchors { left: parent.left; top: parent.top; right: peerimage.left; rightMargin: Theme.paddingSmall }

        Label
        {
            id: lbltitle
            width: parent.width
            elide: Text.ElideRight
            text: dialogModelItem.title
            horizontalAlignment: Qt.AlignRight
        }

        Label
        {
            id: lblusername
            width: parent.width
            elide: Text.ElideRight
            visible: !dialogModelItem.chat && showUsername
            text: dialogModelItem.user ? "@" + dialogModelItem.user.username : ""
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
                if(dialogModelItem.typing.length > 0) {
                    if(!dialogModelItem.chat)
                        return qsTr("Typing...");

                    return TelegramHelper.typingUsers(context, dialogModelItem.typing);
                }

                return dialogModelItem.statusText;
            }
        }
    }
}
