import QtQuick 2.1
import Sailfish.Silica 1.0

Rectangle
{
    property color backColor: "transparent"
    property real minimumValue: 0
    property real maximumValue: 100
    property real value: minimumValue
    property bool canDisplay: true
    property int barHeight: Theme.paddingSmall

    property alias progressItem: progress
    property alias barColor: progress.color

    id: mediaplayerloadingbar
    height: barHeight
    color: backColor
    visible: (value > 0 && value < 100)

    Rectangle
    {
        id: progress
        width: (value / (maximumValue - minimumValue)) * parent.width
        color: Theme.highlightColor
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
}
