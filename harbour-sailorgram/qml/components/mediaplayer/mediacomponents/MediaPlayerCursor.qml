import QtQuick 2.1
import Sailfish.Silica 1.0

Rectangle
{
    property real size

    width: size
    height: size
    radius: size * 0.5
    color: Theme.primaryColor

    Rectangle
    {
        property real innerSize: size * 0.4

        width: innerSize
        height: innerSize
        radius: innerSize * 0.5
        anchors.centerIn: parent
        color: Theme.highlightColor
    }
}
