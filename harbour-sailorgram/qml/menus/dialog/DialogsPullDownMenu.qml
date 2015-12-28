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

    id: dialogspulldownmenu

    MenuItem
    {
        text: qsTr("Settings")
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/settings/SettingsPage.qml"), { "context": dialogspulldownmenu.context })
    }

    MenuItem
    {
        text: qsTr("Profile")
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/profile/ProfilePage.qml"), { "context": dialogspulldownmenu.context })
    }

    TelegramMenuItem
    {
        text: qsTr("New Secret Chat")
        context: dialogspulldownmenu.context
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/secretdialogs/CreateSecretDialogPage.qml"), { "context": dialogspulldownmenu.context })
    }

    TelegramMenuItem
    {
        text: qsTr("New Group")
        context: dialogspulldownmenu.context
        onClicked: pageStack.push(Qt.resolvedUrl("../../pages/chat/CreateChatPage.qml"), { "context": dialogspulldownmenu.context })
    }
}

