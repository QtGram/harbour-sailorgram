import QtQuick 2.1
import QtMultimedia 5.0
import Sailfish.Silica 1.0

Rectangle
{
    function keepVisible(keep)
    {
        if(!keep)
        {
            timerdissolve.restart();
            return;
        }

        timerdissolve.stop()
        toolbar.opacity = 1.0;
    }

    function restoreOpacity()
    {
        toolbar.opacity = 1.0;
        timerdissolve.restart();
    }

    id: toolbar
    color: Theme.highlightDimmerColor
    visible: Qt.application.state === Qt.ApplicationActive
    z: 10

    Behavior on opacity { NumberAnimation { duration: 800; easing.type: Easing.Linear } }

    MouseArea
    {
        anchors.fill: parent
        onClicked: restoreOpacity()
    }

    Timer
    {
        id: timerdissolve
        interval: 2000

        onTriggered: {
            toolbar.opacity = 0.0;
        }
    }

    IconButton
    {
        id: btnplaystop
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
        anchors { left: parent.left; verticalCenter: parent.verticalCenter }
        icon.source: videoplayer.playbackState === MediaPlayer.PlayingState ? "image://theme/icon-m-pause" : "image://theme/icon-m-play"
        z: 10

        onClicked: {
            restoreOpacity();
            videoplayer.playbackState === MediaPlayer.PlayingState ? videoplayer.pause() : videoplayer.play();
        }
    }

    MediaPlayerProgressBar
    {
        id: pbbuffer
        anchors { left: btnplaystop.right; right: btndownloaad.left; verticalCenter: parent.verticalCenter }
        bufferMinimum: 0
        bufferMaximum: 1.0
        bufferValue: videoplayer.bufferProgress
        progressMinimum: 0
        progressMaximum: videoplayer.duration
        progressValue: videoplayer.position
        onDragChanged: keepVisible(dragging)

        onSeekRequested: {
            restoreOpacity();

            if(videoplayer.seekable)
                videoplayer.seek(seekpos);
        }
    }


    IconButton
    {
        id: btndownloaad
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
        enabled: fileHandler.filePath.toString().length > 0
        anchors { right: parent.right; rightMargin: Theme.paddingMedium; verticalCenter: parent.verticalCenter }
        icon.source: "qrc:///res/download.png"
        z: 10

        onClicked: {
            restoreOpacity();

            remorsepopup.execute(qsTr("Downloading media"), function() {
                context.sailorgram.moveMediaToGallery(message.media);
                popupmessage.show(qsTr("Image saved in Gallery"));
            });
        }
    }
}
