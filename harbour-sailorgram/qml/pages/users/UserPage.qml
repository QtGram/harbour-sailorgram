import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property bool actionVisible: true
    property Context context
    property User user

    id: userpage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader { title: TelegramHelper.userName(user) }

            UserItem
            {
                x: Theme.paddingMedium
                width: parent.width - (x * 2)
                height: Theme.itemSizeSmall
                user: userpage.user
            }

            SectionHeader { text: qsTr("Phone Number") }

            Label
            {
                x: Theme.paddingMedium
                width: parent.width - (x * 2)
                text: TelegramHelper.phoneNumber(userpage.user)
            }

            SectionHeader { text: qsTr("Actions"); visible: actionVisible }

            ClickableLabel
            {
                visible: actionVisible
                width: parent.width
                height: Theme.itemSizeSmall
                labelText: qsTr("Send Message")
                onActionRequested: pageStack.replace(Qt.resolvedUrl("../../pages/conversations/ConversationPage.qml"), { "context": context, "dialog": telegram.fakeDialogObject(user.id, false), "user": user } )
            }
        }
    }
}
