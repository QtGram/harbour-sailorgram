import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../components"
import "../../components/telegram"
import "../../models"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property User user: context.telegram.user(context.telegram.me)

    id: profilepage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        TelegramPullDownMenu
        {
            context: profilepage.context

            MenuItem
            {
                text: qsTr("Change Username")
                onClicked: pageStack.push(Qt.resolvedUrl("ChangeUsernamePage.qml"), { "context": profilepage.context, "user": profilepage.user } )
            }

            MenuItem
            {
                text: qsTr("Change Picture")

                onClicked: {
                    var picker = pageStack.push(Qt.resolvedUrl("../picker/FilePickerPage.qml"), { "rootPage": profilepage, "mime": "image" })

                    picker.filePicked.connect(function(file) {
                        context.telegram.setProfilePhoto(file);
                    });
                }
            }
        }

        Column
        {
            id: content
            width: parent.width

            PageHeader
            {
                title: qsTr("Profile")
            }

            UserItem
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                height: Theme.itemSizeSmall
                context: profilepage.context
                user: profilepage.user
            }

            SectionHeader
            {
                text: qsTr("User")
            }

            Label
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                text: user.firstName + " " + user.lastName
            }

            SectionHeader
            {
                text: qsTr("Username")
                visible: user.username.length > 0
            }

            Label
            {
                visible: user.username.length > 0
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                text: user.username
            }

            SectionHeader
            {
                text: qsTr("Phone number")
            }

            Label
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                text: user.phone
            }
        }
    }
}
