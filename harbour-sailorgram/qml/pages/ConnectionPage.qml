import QtQuick 2.1
import Sailfish.Silica 1.0
//import "../js/Settings.js" as Settings

Page
{
    id: pageconnection
    allowedOrientations: defaultAllowedOrientations

    BusyIndicator
    {
        id: indicator
        anchors.centerIn: parent
        running: true
    }

    Label
    {
        anchors { top: indicator.bottom; topMargin: Theme.paddingMedium }
        width: parent.width
        font.pixelSize: Theme.fontSizeLarge
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("Connecting...")
    }
}
