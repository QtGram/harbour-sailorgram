import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TelegramConstants.js" as TelegramConstants

PullDownMenu
{
    property Settings settings

    id: conversationsmenu

    MenuItem
    {
        text: qsTr("Create Group")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/chat/CreateChatPage.qml"), { "settings": conversationsmenu.settings })
    }

    MenuItem
    {
        text: qsTr("Contacts")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/users/UsersPage.qml"), { "settings": conversationsmenu.settings, "telegram": conversationsmenu.settings.telegram })
    }

}

