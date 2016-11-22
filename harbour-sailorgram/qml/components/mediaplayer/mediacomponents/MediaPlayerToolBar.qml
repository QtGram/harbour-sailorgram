import QtQuick 2.1
import QtMultimedia 5.0
import Sailfish.Silica 1.0
import "../../custom"

Rectangle
{
    readonly property bool canClick: opacity > 0.1

    function keepVisible(keep) {
        if(!keep) {
            timerdissolve.restart();
            return;
        }

        timerdissolve.stop()
        toolbar.opacity = 1.0;
    }

    function restoreOpacity() {
        toolbar.opacity = 1.0;
        timerdissolve.restart();
    }

    id: toolbar
    color: Theme.highlightDimmerColor
    visible: Qt.application.state === Qt.ApplicationActive
    z: 10

    Behavior on opacity { NumberAnimation { duration: 800; easing.type: Easing.Linear } }
    BackgroundRectangle { anchors.fill: parent }
    MouseArea { anchors.fill: parent; onClicked: restoreOpacity() }

    Timer
    {
        id: timerdissolve
        interval: 2000

        onTriggered: {
            toolbar.opacity = 0.0;
        }
    }

    MediaPlayerProgressBar
    {
        id: pbbuffer
        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
        bufferMinimum: 0
        bufferMaximum: 1.0
        bufferValue: videoplayer.bufferProgress
        progressMinimum: 0
        progressMaximum: videoplayer.duration
        progressValue: videoplayer.position
        onDragChanged: keepVisible(dragging)

        onSeekRequested: {
            if(internalmediaplayer.state !== "error")
                restoreOpacity();

            if(!canClick)
                return;

            if(videoplayer.seekable)
                videoplayer.seek(seekpos);
        }
    }
}
