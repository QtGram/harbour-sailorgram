import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "./../../components/telegram"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

PullDownMenu
{
    property Context context

    id: conversationspulldownmenu

    MenuItem
    {
        text: qsTr("Profile")
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/profile/ProfilePage.qml"), { "context": conversationspulldownmenu.context })
    }

    TelegramMenuItem
    {
        text: qsTr("New Secret Chat")
        context: conversationspulldownmenu.context
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/secretchat/CreateSecretChatPage.qml"), { "context": conversationspulldownmenu.context })
    }

    TelegramMenuItem
    {
        text: qsTr("New Group")
        context: conversationspulldownmenu.context
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/chat/CreateChatPage.qml"), { "context": conversationspulldownmenu.context })
    }

    MenuItem
    {
        text: qsTr("Contacts")
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/users/UsersPage.qml"), { "context": conversationspulldownmenu.context })
    }

}

