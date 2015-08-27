import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"
import "../../js/Settings.js" as Settings

Dialog
{
    property Context context

    id: chatsettingspage
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop

    onAccepted: {
        context.sendwithreturn = tssendwithreturn.checked;
        context.backgrounddisabled = tsdisablebackground.checked;
        Settings.set("sendwithreturn", tssendwithreturn.checked);
        Settings.set("backgrounddisabled", tsdisablebackground.checked);
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
                id: tssendwithreturn
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Send Messages by pressing Return Key")
                checked: context.sendwithreturn
            }

            TextSwitch
            {
                id: tsdisablebackground
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Disable background")
                checked: context.backgrounddisabled
            }
        }
    }
}
