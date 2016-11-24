import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"
import "../../components/custom"

Page
{
    property Context context

    id: settingspage

    onStatusChanged: {
        if((status !== PageStatus.Active) || canNavigateForward)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("about/AboutPage.qml"), { context: settingspage.context });
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            PageHeader { title: qsTr("Settings") }

            ClickableLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                icon: "image://theme/icon-m-sms"
                text: qsTr("Chats")
                onActionRequested: pageStack.push(Qt.resolvedUrl("ChatSettingsPage.qml"), { "context": settingspage.context })
            }

            ClickableLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                icon: "qrc:///res/daemon.png"
                text: qsTr("Daemon") + " (Disabled)"
                enabled: false
                //onActionRequested: pageStack.push(Qt.resolvedUrl("DaemonSettingsPage.qml"), { "context": settingspage.context })
            }

            /*
            ClickableLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                icon: "qrc:///res/debug.png"
                text: qsTr("Debug")
                onActionRequested: pageStack.push(Qt.resolvedUrl("DebugSettingsPage.qml"), { "context": settingspage.context })
            }
            */
        }
    }
}
