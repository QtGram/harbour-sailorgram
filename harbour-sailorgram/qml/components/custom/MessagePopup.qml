import QtQuick 2.1
import Sailfish.Silica 1.0

MouseArea
{
    function popup(message) {
        lblmessage.text = message;
        opacity = 1.0;
        timhide.start();
    }

    id: messagepopup
    y: -1
    width: parent ? parent.width : Screen.width
    height: Theme.itemSizeSmall + Theme.paddingSmall
    opacity: 0.0
    visible: opacity > 0.0
    z: 10

    Timer
    {
        id: timhide
        interval: 2000

        onTriggered: {
            opacity = 0.0;
        }
    }

    onClicked: {
        opacity = 0.0;
        timhide.stop();
    }

    Behavior on opacity {
        NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
    }

    Rectangle { anchors.fill: parent; color: Theme.rgba(Theme.highlightDimmerColor, 0.9) }

    Label
    {
        id: lblmessage
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Theme.fontSizeSmall
        elide: Text.ElideRight
        wrapMode: Text.NoWrap

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: bottombar.top
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }
    }

    Rectangle
    {
        id: bottombar
        color: Theme.secondaryHighlightColor
        anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
        height: Theme.paddingSmall
    }
}
