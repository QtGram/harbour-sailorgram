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
}
