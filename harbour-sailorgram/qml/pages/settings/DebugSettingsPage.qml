import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"

Page
{
    property Context context

    id: debugsettingspage
    allowedOrientations: defaultAllowedOrientations

    function refreshData() {
        context.contacts.refresh();
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Error log")
                onClicked: pageStack.push(Qt.resolvedUrl("debug/ErrorsPage.qml"), { "context": debugsettingspage.context })
            }

            MenuItem
            {
                text: qsTr("Refresh")
                onClicked: refreshData()
            }
        }

        Column
        {
            id: content
            width: parent.width

            PageHeader { title: qsTr("Debug") }

            DebugLabel
            {
                title: qsTr("Heartbeat:")
                text: context.heartbeat.connected ? qsTr("Connected") : qsTr("Not Connected")
                errorState: !context.heartbeat.connected
            }

            DebugLabel
            {
                title: qsTr("Contacts:")
                text: context.contacts.count
            }
        }
    }
}
