import QtQuick 2.1
import Sailfish.Silica 1.0
import QtMultimedia 5.0
import harbour.sailorgram.LibQTelegram 1.0

Item
{
    readonly property real contentWidth: waveform.contentWidth
    property alias color: lblduration.color
    property alias barColor: waveform.barColor
    property alias message: waveform.message
    property alias source: mediaplayer.source
    property string duration

    id: audiomessage
    height: waveform.height + lblduration.contentHeight

    MediaPlayer { id: mediaplayer; autoPlay: false; autoLoad: false }

    Image
    {
        id: imgplay
        width: Theme.iconSizeMedium
        height: Theme.iconSizeMedium
        anchors { verticalCenter: parent.verticalCenter }
        fillMode: Image.PreserveAspectFit

        source: {
            if(mediaplayer.playbackState !== MediaPlayer.PlayingState)
                return "image://theme/icon-m-play?" + waveform.barColor;

            return "image://theme/icon-m-pause?" + waveform.barColor;
        }

        BusyIndicator { z: 2; anchors.centerIn: parent; running: mediamessageitem.downloading }

        MouseArea
        {
            anchors.fill: parent

            onClicked: {
                if(mediamessageitem.downloaded) {
                    if(mediaplayer.playbackState !== MediaPlayer.PlayingState)
                        mediaplayer.play();
                    else
                        mediaplayer.pause();

                    return;
                }

                mediamessageitem.download();
            }
        }
    }

    Waveform
    {
        id: waveform
        anchors { left: imgplay.right; top: parent.top; topMargin: Theme.paddingSmall; right: parent.right; leftMargin: Theme.paddingSmall }
        height: Theme.iconSizeSmall
    }

    Label
    {
        id: lblduration
        anchors { left: imgplay.right; top: waveform.bottom; right: parent.right; leftMargin: Theme.paddingSmall; topMargin: Theme.paddingMedium }
        font.pixelSize: Theme.fontSizeSmall
        text: audiomessage.duration + " <font color='" + waveform.barColor + "'>⚫</font>";
    }
}
