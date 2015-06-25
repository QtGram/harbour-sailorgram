import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

MessageMediaItem
{
    id: messagephoto
    width: image.width
    height: image.height
    hasMedia: message.media ? (message.media.classType === messageitem.typeMessageMediaPhoto) : false

    Image
    {
        id: image
        smooth: true
        anchors { left: parent.left; top: parent.top }
        asynchronous: true
        height: 48 * Theme.pixelSize
        width: height * sourceSize.width / sourceSize.height
        fillMode: Image.PreserveAspectFit
        source: hasMedia ? message.media.photo.sizes.last.location.download.location : ""
    }
}
