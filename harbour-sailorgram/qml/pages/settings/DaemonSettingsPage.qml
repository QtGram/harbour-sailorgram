import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"
import "../../js/Settings.js" as Settings

Dialog
{
    property Context context

    id: daemonsettingspage
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop

    onAccepted: {
        context.sailorgram.keepRunning = tskeeprunning.checked;
        context.sailorgram.autostart = tsdaemonenabled.checked;
        Settings.set("keeprunning", tskeeprunning.checked);
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            DialogHeader
            {
                acceptText: qsTr("Save")
            }

            TextSwitch
            {
                id: tskeeprunning
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Keep running")
                description: qsTr("SailorGram keeps running in background even when closed")
                checked: context.sailorgram.keepRunning
            }

            TextSwitch
            {
                id: tsdaemonenabled
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Autostart")
                description: qsTr("SailorGram starts automatically")
                checked: context.sailorgram.autostart
            }
        }
    }
}
