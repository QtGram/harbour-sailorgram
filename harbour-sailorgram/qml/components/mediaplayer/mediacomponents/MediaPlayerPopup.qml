import QtQuick 2.1
import Sailfish.Silica 1.0

Image
{
    function animatePlay() {
        mediaplayerpopup.source = "image://theme/icon-l-play";
        mediaplayerpopup.opacity = 1.0;
        timopacity.restart();
    }

    function animatePause() {
        mediaplayerpopup.source = "image://theme/icon-l-pause";
        mediaplayerpopup.opacity = 1.0;
        timopacity.restart();
    }

    Rectangle
    {
        anchors.fill: parent
        radius: width * 0.5
        color: Theme.highlightDimmerColor
        opacity: Math.min(mediaplayerpopup.opacity, 0.4)
        visible: mediaplayerpopup.visible
        z: -1
    }

    Behavior on opacity {
        NumberAnimation { duration: 500; easing.type: Easing.Linear }
    }

    Timer
    {
        id: timopacity
        interval: 500
        repeat: false

        onTriggered: {
            mediaplayerpopup.opacity = 0.0;
        }
    }

    id: mediaplayerpopup
    opacity: 0.0
    visible: opacity > 0.0
    width: Theme.iconSizeLarge
    height: Theme.iconSizeLarge
    z: 10
}

