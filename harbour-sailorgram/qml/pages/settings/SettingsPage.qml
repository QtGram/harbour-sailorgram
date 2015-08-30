import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../components"
import "../../models"

Page
{
    property Context context

    id: settingspage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("../AboutPage.qml"), { "context": settingspage.context })
            }
        }

        Column
        {
            id: content
            width: parent.width

            PageHeader
            {
                title: qsTr("Settings")
            }

            SettingLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                icon: "image://theme/icon-s-message"
                text: qsTr("Chats")
                onActionRequested: pageStack.push(Qt.resolvedUrl("ChatSettingsPage.qml"), { "context": settingspage.context })
            }

            SettingLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                icon: "qrc:///res/debug.png"
                text: qsTr("Debug")
                onActionRequested: pageStack.push(Qt.resolvedUrl("DebugSettingsPage.qml"), { "context": settingspage.context })
            }
        }
    }
}
