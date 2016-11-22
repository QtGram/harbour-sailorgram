import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../js/MediaPlayerHelper.js" as MediaPlayerHelper

Item
{
    property alias progressMinimum: videoprogress.minimumValue
    property alias progressMaximum: videoprogress.maximumValue
    property alias progressValue: videoprogress.value
    property alias bufferMinimum: bufferprogress.minimumValue
    property alias bufferMaximum: bufferprogress.maximumValue
    property alias bufferValue: bufferprogress.value

    signal dragChanged(bool dragging)
    signal seekRequested(int seekpos)

    function reverseRgb(rgba) {
        return Qt.rgba(1.0 - rgba.r, 1.0 - rgba.g, 1.0 - rgba.b, 1.0);
    }

    function seekTo(seekpx) {
        var seekpos = Math.floor(((videoprogress.maximumValue - videoprogress.minimumValue) / videoprogress.width) * seekpx);
        seekRequested(seekpos);
    }

    id: mpprogressbar
    height: parent.height / 4
    opacity: enabled ? 1.0 : 0.4

    Label
    {
        id: lblcurrentprogress
        anchors { left: parent.left; verticalCenter: progresscontainer.verticalCenter; leftMargin: Theme.paddingSmall }
        text: MediaPlayerHelper.displayDuration(Math.floor(videoprogress.value / 1000.0))
        font.pixelSize: Theme.fontSizeSmall
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle
    {
        id: progresscontainer
        anchors { left: lblcurrentprogress.right; right: lblduration.left; top: parent.top; bottom: parent.bottom; leftMargin: Theme.paddingLarge; rightMargin: Theme.paddingLarge }
        color: "white"

        MediaPlayerLoadingBar
        {
            id: bufferprogress
            anchors.fill: parent
            barColor: reverseRgb(Theme.highlightColor)
            barHeight: mpprogressbar.height
            visible: true
        }

        MediaPlayerLoadingBar
        {
            id: videoprogress
            anchors.fill: parent
            barHeight: mpprogressbar.height
            visible: true

            MediaPlayerCursor
            {
                id: mediacursor
                size: videoprogress.barHeight + Theme.paddingMedium
                anchors.verticalCenter: videoprogress.progressItem.verticalCenter
                z: 15

                x: {
                    if(mousearea.drag.active)
                        return mousearea.mouseX;

                    return videoprogress.progressItem.width - (size / 2);
                }
            }
        }

        MouseArea
        {
            id: mousearea
            z: 20
            anchors.fill: parent
            drag { target: mpprogressbar.enabled ? mediacursor : null; axis: Drag.XAxis; minimumX: 0; maximumX: progresscontainer.width }
            drag.onActiveChanged: dragChanged(drag.active)

            onClicked: {
                if(!mpprogressbar.enabled)
                    return;

                seekTo(mouse.x);
            }

            onReleased: {
                if(drag.active)
                    seekTo(mediacursor.x + (mediacursor.size / 2));
            }
        }
    }

    Label
    {
        id: lblduration
        anchors { right: parent.right; verticalCenter: progresscontainer.verticalCenter; rightMargin: Theme.paddingSmall }
        text: MediaPlayerHelper.displayDuration(Math.floor(videoprogress.maximumValue / 1000.0))
        font.pixelSize: Theme.fontSizeSmall
        verticalAlignment: Text.AlignVCenter
    }
}
