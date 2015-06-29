import QtQuick 2.1
import Sailfish.Silica 1.0

Image
{
    property real downloadProgress

    id: image
    smooth: true
    asynchronous: true
    height: 48 * Theme.pixelSize
    width: height * sourceSize.width / sourceSize.height
    fillMode: Image.PreserveAspectFit

    ProgressCircle
    {
        id: processcircle
        anchors.centerIn: parent
        width: image.width
        height: image.width
        value: downloadProgress / 100
        visible: (downloadProgress > 0) && (downloadProgress < 100)
    }
}
