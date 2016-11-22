import QtQuick 2.1
import Sailfish.Silica 1.0

Rectangle
{
    property alias showError: lblmessage.visible
    property alias errorMessage: lblmessage.text

    id: mediaplayererror
    color: "black"
    z: -1

    Label
    {
        id: lblmessage
        anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
        width: parent.width
        font.pixelSize: Theme.fontSizeLarge
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        color: "white"
    }
}
