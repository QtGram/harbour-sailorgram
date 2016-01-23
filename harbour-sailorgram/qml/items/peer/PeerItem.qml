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
        anchors { left: parent.left; top: parent.top }
        width: peeritem.height
        height: peeritem.height
        context: peeritem.context
        dialog: peeritem.dialog
        chat: peeritem.chat
        user: peeritem.user
    }

    Column
    {
        anchors { left: peerimage.right; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall }

        Row
        {
            id: rowtitle
            width: parent.width
            height: lbltitle.contentHeight
            spacing: Theme.paddingSmall

            Image
            {
                id: imgsecure
                source: "image://theme/icon-s-secure"
                anchors.verticalCenter: lbltitle.verticalCenter
                fillMode: Image.PreserveAspectFit
                visible: peeritem.dialog.encrypted
            }

            Label
            {
                id: lbltitle
                width: parent.width
                elide: Text.ElideRight
                text: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.completeName(user)
            }
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
        }

        Row
        {
            height: peeritem.height - rowtitle.height

            Label
            {
                id: lblinfo
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor

                text: {
                    if(dialog.typingUsers.length > 0)
                        return TelegramHelper.typingUsers(context, dialog);

                    if(TelegramHelper.isChat(dialog)) {

                        if(chat.participantsCount === 1)
                            return qsTr("%1 member").arg(chat.participantsCount);

                        return qsTr("%1 members").arg(chat.participantsCount);
                    }

                    return TelegramHelper.userStatus(user);
                }
            }
        }
    }
}
