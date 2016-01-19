import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../../js/TelegramMedia.js" as TelegramMedia

Item
{
    property alias cache: image.cache
    property alias source: image.source
    property alias sourceSize: image.sourceSize
    property real transferProgress: 0

    id: messagethumbnail

    Image
    {
        id: image
        asynchronous: true
        anchors.fill: parent
        visible: !progresscircle.visible && (image.status === Image.Ready)
        fillMode: Image.PreserveAspectFit
    }

    ProgressCircle
    {
        id: progresscircle
        anchors.centerIn: parent
        width: parent.height - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        value: transferProgress / 100
        visible: (transferProgress > 0) && (transferProgress < 100)
    }
}
