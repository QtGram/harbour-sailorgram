import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0

Item
{
    property alias fileHandler: filehandler

    property Telegram telegram
    property Message message

    readonly property bool hasMedia: message.media ? (message.media.classType !== messageitem.typeMessageMediaEmpty) : false

    readonly property string mediaPath: {
        if(message.media.classType === messageitem.typeMessageMediaPhoto)
            return message.media.photo.sizes.last.location.download.location;
        else if(message.media.classType === messageitem.typeMessageMediaVideo)
            return message.media.video.thumb.location.download.location;
        else if(message.media.classType === messageitem.typeMessageMediaAudio)
            return "image://theme/boh"; // FIXME: Audio Thumb
        else if(message.media.classType === messageitem.typeMessageMediaUnsupported)
            return "image://theme/boh"; // FIXME: Unsupported Thumb
        else if(message.media.classType === messageitem.typeMessageMediaDocument)
            return message.media.document.thumb.location.download.location;

        return "";
    }

    id: messagemediaitem
    visible: hasMedia

    FileHandler
    {
        id: filehandler
        telegram: messagemediaitem.telegram
        target: message
        defaultThumbnail: "image://theme/icon-m-document"
    }

    onHasMediaChanged: {
        if(!hasMedia)
            return;

        if(message.media.classType === messageitem.typeMessageMediaPhoto)
            telegram.getFile(message.media.photo.sizes.last.location);
        else if(message.media.classType === messageitem.typeMessageMediaVideo)
            telegram.getFile(message.media.video.thumb.location);
        else if(message.media.classType === messageitem.typeMessageMediaDocument)
            telegram.getFile(message.media.document.thumb.location);
    }
}
