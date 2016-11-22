import QtQuick 2.1
import Sailfish.Silica 1.0

ProgressCircle
{
    property real size

    id: progressindicator
    width: size
    height: size

    Rectangle
    {
        anchors.fill: parent
        color: Theme.rgba(backgroundColor, 0.2)
        radius: size * 0.5
    }

    Label
    {
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: Math.ceil(value * 100) + "%"
        font { bold: true; pixelSize: Theme.fontSizeMedium }
        color: progressColor
    }
}
