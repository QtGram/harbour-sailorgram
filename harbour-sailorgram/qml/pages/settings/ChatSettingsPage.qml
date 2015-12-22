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
        context.chatheaderhidden = tshideheader.checked
        context.bubbleshidden = tshidebubbles.checked
        Settings.set("backgrounddisabled", tsdisablebackground.checked);
        Settings.set("sendwithreturn", tssendwithreturn.checked);
        Settings.set("chatheaderhidden", tshideheader.checked);
        Settings.set("hidebubbles", tshidebubbles.checked);
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

            TextSwitch
            {
                id: tshideheader
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Hide chat header")
                checked: context.chatheaderhidden
            }

            TextSwitch
            {
                id: tshidebubbles
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                text: qsTr("Hide Bubbles")
                checked: context.bubbleshidden
            }
        }
    }
}
