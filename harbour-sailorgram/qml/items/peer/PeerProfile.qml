import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property alias title: lbltitle.text
    property alias fallbackAvatar: peerimage.fallbackTitle
    property alias isSecretChat: peerimage.isSecretChat
    property alias peer: peerimage.peer
    property Context context
    property string statusText
    property bool isChat: false

    id: peerprofile
    height: content.height

    Column
    {
        id: content
        width: parent.width
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: peerimage
            anchors.horizontalCenter: parent.horizontalCenter
            width: Screen.width * 0.3
            height: width
            context: peerprofile.context
            isChat: peerprofile.isChat
            showType: false

            MouseArea
            {
                anchors.fill: parent

                onClicked: {
                    if(!peerimage.peer)
                        return;

                    pageStack.push(Qt.resolvedUrl("../../pages/media/MediaPhotoPage.qml"), { "context": peerprofile.context, "imageHandler": peerimage.imageHandler });
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
                if(isChat)
                    return qsTr("Group");

                return statusText;
            }
        }
    }
}

