import QtQuick 2.1
import QtMultimedia 5.0

Item
{
    property alias source: mediaplayer.source

    id: animatedmessage

    MediaPlayer {
        id: mediaplayer
        loops: MediaPlayer.Infinite
        autoPlay: true
    }

    VideoOutput {
        anchors.fill: parent
        source: mediaplayer
    }
}
