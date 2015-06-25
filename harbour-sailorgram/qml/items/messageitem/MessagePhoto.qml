import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0

Image
{
    property Telegram telegram
    property Message message
    property bool hasMedia
    property bool me

    onHasMediaChanged: {
        switch(message.media.classType) {
            case messageitem.typeMessageMediaPhoto:
                telegram.getFile(message.media.photo.sizes.last.location);
                break;

            default:
                break;
        }
    }

    id: messagephoto
    anchors { left: me ? parent.left : undefined; right: me ? undefined : parent.right }
    height: 48 * Theme.pixelSize
    width: height * sourceSize.width / sourceSize.height
    fillMode: Image.PreserveAspectFit
    smooth: true
    asynchronous: true
    visible: (message.media.classType === messageitem.typeMessageMediaPhoto)

    source: {
        switch(message.media.classType) {
            case messageitem.typeMessageMediaPhoto:
                return message.media.photo.sizes.last.location.download.location;

            default:
                break;
        }

        return "";
    }
}
