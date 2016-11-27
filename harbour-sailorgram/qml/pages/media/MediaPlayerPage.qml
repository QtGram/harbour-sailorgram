import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../components/mediaplayer"

Page
{
    readonly property bool isMediaPage: true
    property alias source: mediaplayer.videoSource
    property alias thumbnail: mediaplayer.videoThumbnail
    property alias title: mediaplayer.videoTitle

    id: mediaplayerpage
    allowedOrientations: defaultAllowedOrientations

    InternalMediaPlayer
    {
        id: mediaplayer
        anchors.fill: parent
    }
}
