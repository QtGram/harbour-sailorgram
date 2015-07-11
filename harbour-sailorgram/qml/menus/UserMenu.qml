import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../models"
import "../items/user"
import "../js/TelegramHelper.js" as TelegramHelper

ContextMenu
{
    property Context context
    property User user

    id: usermenu

    MenuItem
    {
        text: qsTr("Send Message")
        onClicked: pageStack.replace(Qt.resolvedUrl("../pages/conversations/ConversationPage.qml"), { "context": context, "dialog": usermenu.context.fakeDialogObject(usermenu.user.id, false) } )
    }

    MenuItem
    {
        text: qsTr("Profile")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/users/UserPage.qml"), {"context": context, "user": usermenu.user })
    }
}
