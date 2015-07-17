import QtQuick 2.1
import Sailfish.Silica 1.0

Rectangle
{
    property color backColor: "transparent"
    property real minimumValue: 0
    property real maximumValue: 100
    property real value: minimumValue
    property bool hideWhenFinished: false
    property bool canDisplay: true
    property int barHeight: 6

    property alias progressItem: progress
    property alias barColor: progress.color

    height: barHeight
    color: backColor
    z: 11

    onValueChanged: {
        if(hideWhenFinished) {
            if(value === maximumValue) {
                height = 0;
                visible = false;
            }
            else if(canDisplay) {
                height = barHeight;
                visible = true;
            }
        }
    }

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
