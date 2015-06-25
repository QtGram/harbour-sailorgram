import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0

Item
{
    property FileLocation fileLocation
    property Telegram telegram
    property Message message
    property bool hasMedia
    property bool me

    property alias fileHandler: filehandler

    id: messagemediaitem
    anchors { left: me ? parent.left : undefined; right: me ? undefined : parent.right }
    visible: hasMedia

    FileHandler
    {
        id: filehandler
        target: message
    }

    onHasMediaChanged: {
        switch(message.media.classType) {
            case messageitem.typeMessageMediaPhoto:
                telegram.getFile(message.media.photo.sizes.last.location);
                break;

            case messageitem.typeMessageMediaVideo:
                telegram.getFile(message.media.video.thumb.location);
                break;

            case messageitem.typeMessageMediaDocument:
                telegram.getFile(message.media.document.thumb.location);
                break;

            default:
                break;
        }
    }
}
