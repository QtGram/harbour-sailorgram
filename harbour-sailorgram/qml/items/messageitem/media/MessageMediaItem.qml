import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0
import "../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property alias fileHandler: filehandler

    property Telegram telegram
    property Message message

    readonly property bool hasMedia: message.media ? (message.media.classType !== messageitem.typeMessageMediaEmpty) : false

    readonly property string mediaPath: {
        if(message.media.classType === TelegramConstants.typeMessageMediaPhoto)
            return message.media.photo.sizes.last.location.download.location;
        else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
            return message.media.video.thumb.location.download.location;
        else if(message.media.classType === TelegramConstants.typeMessageMediaAudio)
            return "image://theme/boh"; // FIXME: Audio Thumb
        else if(message.media.classType === TelegramConstants.typeMessageMediaUnsupported)
            return "image://theme/boh"; // FIXME: Unsupported Thumb
        else if(message.media.classType === TelegramConstants.typeMessageMediaDocument)
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

        if(message.media.classType === TelegramConstants.typeMessageMediaPhoto)
            telegram.getFile(message.media.photo.sizes.last.location);
        else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
            telegram.getFile(message.media.video.thumb.location);
        else if(message.media.classType === TelegramConstants.typeMessageMediaDocument)
            telegram.getFile(message.media.document.thumb.location);
    }
}
