import QtQuick 2.1
import Sailfish.Silica 1.0
import QtMultimedia 5.0
import "../../custom"

Item
{
    property alias source: mediaplayer.source

    id: animatedmessage

    MediaPlayer { id: mediaplayer; autoPlay: false; autoLoad: false; loops: MediaPlayer.Infinite }
    VideoOutput { anchors.fill: parent; source: mediaplayer }
    BusyIndicator { size: BusyIndicatorSize.Small; anchors.centerIn: parent; running: mediamessageitem.downloading }

    BlurredImage
    {
        anchors.fill: parent
        source: mediamessageitem.thumbnail
        showActions: false
    }

    Image {
        source: "image://theme/icon-m-play"
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        width: parent.width * 0.4
        height: parent.height * 0.4
        visible: (mediaplayer.playbackState !== MediaPlayer.PlayingState) && mediamessageitem.downloaded
    }

    MouseArea
    {
        anchors.fill: parent
        enabled: !messageslist.selectionMode

        onClicked: {
            if(mediamessageitem.downloaded) {
                if(mediaplayer.playbackState !== MediaPlayer.PlayingState)
                    mediaplayer.play();
                else
                    mediaplayer.stop();

                return;
            }

            mediamessageitem.download();
        }
    }
}
