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

            FileHandler
            {
                id: bigimagehandler
                telegram: context.telegram
                target: {
                    if(user)
                        return user.photo.photoBig;
                    if(chat)
                        return chat.photo.photoBig;
                    return null;
                }
                onTargetTypeChanged: {
                    if(downloaded)
                        return;
                    download();
                }
            }

            MouseArea
            {
                anchors.fill: parent
                onClicked: {
                    var photoId = 0;
                    if(user)
                        photoId = user.photo.photoId;
                    else if(chat)
                        photoId = chat.photo.photoId;
                    if(photoId !== 0)
                    pageStack.push(
                        Qt.resolvedUrl("../../pages/media/MediaPhotoPage.qml"),
                        {
                            "context":     peerprofile.context,
                            "fileHandler": bigimagehandler
                        });
                }
            }
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

