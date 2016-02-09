import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property alias showType: useravatar.showType
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    id: peerprofile
    height: content.height

    Column
    {
        id: content
        width: parent.width
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: useravatar
            anchors.horizontalCenter: parent.horizontalCenter
            width: Screen.width * 0.3
            height: width
            context: peerprofile.context
            dialog: peerprofile.dialog
            chat: peerprofile.chat
            user: peerprofile.user
        }

        Label
        {
            id: lbltitle
            anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
            horizontalAlignment: Text.AlignHCenter
            font.family: Theme.fontFamilyHeading
            font.bold: true
            elide: Text.ElideRight

            text: {
                if(chat)
                    return chat.title;

                var name = TelegramHelper.completeName(user);

                if(user.username.length > 0)
                    name += " (@" + user.username + ")";

                return name;
            }
        }

        Label
        {
            id: lblstatus
            anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.highlightColor
            elide: Text.ElideRight

            text: {
                if(peerprofile.chat)
                    return qsTr("Group");

                return peerprofile.user ? TelegramHelper.userStatus(user) : 0;
            }
        }
    }
}

