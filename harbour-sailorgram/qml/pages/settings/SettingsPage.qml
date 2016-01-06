import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../components"
import "../../models"

Page
{
    property Context context

    id: settingspage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(settingspage.status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("../AboutPage.qml"), { "context": settingspage.context });
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

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
                icon: "qrc:///res/daemon.png"
                text: qsTr("Daemon")
                onActionRequested: pageStack.push(Qt.resolvedUrl("DaemonSettingsPage.qml"), { "context": settingspage.context })
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
