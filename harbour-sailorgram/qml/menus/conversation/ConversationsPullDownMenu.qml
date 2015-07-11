import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

PullDownMenu
{
    property Context context

    id: conversationspulldownmenu

    MenuItem
    {
        text: qsTr("Create Group")
        visible: context.heartbeat.connected
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/chat/CreateChatPage.qml"), { "context": conversationspulldownmenu.context })
    }

    MenuItem
    {
        text: qsTr("Contacts")
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/users/UsersPage.qml"), { "context": conversationspulldownmenu.context, "telegram": conversationspulldownmenu.context.telegram })
    }

}

