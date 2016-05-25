import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../components"
import "../../models"
import "../../items/peer"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property UserFull userFull: context.engine.our
    property User user: context.engine.our.user

    id: profilepage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            PageHeader { title: qsTr("Profile") }

            PeerProfile
            {
                width: parent.width
                context: profilepage.context
                peer: profilepage.user
                fallbackAvatar: TelegramHelper.completeName(profilepage.user)

                title: {
                    var name = TelegramHelper.completeName(profilepage.user);

                    if(profilepage.user.username.length > 0)
                        name += " (@" + profilepage.user.username + ")";

                    return name;
                }
            }

            SectionHeader { text: qsTr("Phone number") }

            Label
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: Theme.fontSizeSmall
                text: TelegramHelper.completePhoneNumber(user.phone)
            }

            SectionHeader { text: qsTr("Actions"); visible: context.sailorgram.connected }

            ClickableLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                labelText: qsTr("Change Username")
                visible: context.sailorgram.connected
                onActionRequested: pageStack.push(Qt.resolvedUrl("ChangeUsernamePage.qml"), { "context": profilepage.context, "user": profilepage.user } )
            }

            ClickableLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                labelText: qsTr("Change Picture")
                visible: context.sailorgram.connected

                onActionRequested: {
                    var imgselector = pageStack.push(Qt.resolvedUrl("../selector/SelectorImagesPage.qml"), { "context": profilepage.context });

                    imgselector.actionCompleted.connect(function(actiontype, data) {
                        context.telegram.setProfilePhoto(data);
                    });
                }
            }
        }
    }
}
