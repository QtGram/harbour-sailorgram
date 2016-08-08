import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property alias title: lbltitle.text
    property alias fallbackAvatar: useravatar.fallbackTitle
    property alias isSecretChat: useravatar.isSecretChat
    property Context context
    property var peer
    property Chat chat
    property string statusText

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
            peer: peerprofile.peer
            chat: peerprofile.chat
            showType: false

            MouseArea
            {
                anchors.fill: parent

                onClicked: {
                    if(!useravatar.peer)
                        return;

                    pageStack.push(Qt.resolvedUrl("../../pages/media/MediaPhotoPage.qml"), { "context": peerprofile.context, "imageHandler": useravatar.imageHandler });
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
        }

        Label
        {
            id: lblstatus
            anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.highlightColor
            elide: Text.ElideRight
            visible: statusText.length > 0

            text: {
                if(chat)
                    return qsTr("Group");

                return statusText;
            }
        }
    }
}

