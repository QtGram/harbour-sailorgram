import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0

Item
{
    property Telegram telegram
    property Message message
    property bool hasMedia
    property bool me

    id: messagemediaitem
    anchors { left: me ? parent.left : undefined; right: me ? undefined : parent.right }
    visible: hasMedia

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
