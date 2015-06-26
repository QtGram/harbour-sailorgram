import QtQuick 2.1
import Sailfish.Silica 1.0

Rectangle
{
    height: Theme.itemSizeExtraSmall
    visible: opacity > 0.0 ? true : false
    color: Theme.highlightColor
    opacity: 0.0
    z: 10

    function show(message)
    {
        lblmessage.text = message;
        opacity = 1.0;
        timer.start();
    }

    function hide()
    {
        opacity = 0.0;
        lblmessage.text = "";
    }

    Behavior on opacity {
        NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
    }

    Timer
    {
        id: timer
        interval: 1500
        onTriggered: hide()
    }

    MouseArea
    {
        anchors.fill: parent

        Label
        {
            id: lblmessage
            anchors.fill: parent
            color: Theme.highlightDimmerColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            clip: true
        }

        onClicked: {
            timer.stop();
            hide();
        }
    }
}
