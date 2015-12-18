import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    property alias source: image.source
    property size imageSize: context.sailorgram.imageSize(image.source);
    property real transferProgress

    id: messagethumbnail
    width: image.width
    height: image.height

    Image
    {
        id: image
        smooth: true
        asynchronous: true
        fillMode: Image.PreserveAspectFit
        height: Theme.iconSizeLarge
        width: height ? (height * imageSize.width / imageSize.height) : Theme.iconSizeLarge
        sourceSize: Qt.size(width, height)
        visible: !progresscircle.visible && (image.status === Image.Ready)
    }

    BusyIndicator
    {
        id: busyindicator
        anchors.centerIn: parent
        width: Theme.iconSizeLarge - Theme.paddingSmall
        height: Theme.iconSizeLarge - Theme.paddingSmall
        running: !progresscircle.visible && (image.status !== Image.Ready)
    }

    ProgressCircle
    {
        id: progresscircle
        anchors.centerIn: parent
        width: image.height - Theme.paddingSmall
        height: image.height - Theme.paddingSmall
        value: transferProgress / 100
        visible: (transferProgress > 0) && (transferProgress < 100)
    }
}
