import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../../js/TelegramMedia.js" as TelegramMedia

Item
{
    property alias cache: image.cache
    property alias source: image.source
    property real transferProgress

    id: messagethumbnail

    Image
    {
        id: image
        asynchronous: true
        anchors.fill: parent
        visible: !progresscircle.visible && (image.status === Image.Ready)
        fillMode: Image.PreserveAspectFit
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
