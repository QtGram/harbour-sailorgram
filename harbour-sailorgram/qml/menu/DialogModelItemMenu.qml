import QtQuick 2.1
import Sailfish.Silica 1.0

ContextMenu
{
    id: dialogmodelitemmenu

    MenuItem
    {
        text: model.isMuted ? qsTr("Enable notifications") : qsTr("Disable notifications")

        onClicked: {
            model.isMuted = !model.isMuted;
        }
    }

    MenuItem
    {
        text: qsTr("Clear history")
        onClicked: remorseAction(qsTr("Clearing history"), function() { context.dialogs.clearHistory(model.index) ; });
    }

    MenuItem
    {
        text: {
            if(model.isBroadcast)
                return qsTr("Leave channel");
            else if(model.isChat)
                return qsTr("Leave group");

            return qsTr("Delete conversation");
        }

        onClicked: {
            var remorsetitle = "";

            if(model.isBroadcast)
                remorsetitle = qsTr("Leaving channel");
            else if(model.isChat)
                remorsetitle = qsTr("Leaving group");
            else
                remorsetitle = qsTr("Deleting conversation");

            remorseAction(remorsetitle, function() { context.dialogs.removeDialog(model.index) ; });
        }
    }
}
