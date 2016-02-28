import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../models"
import "../../../../js/TelegramMedia.js" as TelegramMedia
import "../../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property alias fileHandler: filehandler

    property Context context
    property Telegram telegram
    property Message message
    property real maxWidth
    property real contentWidth
    property real contentHeight

    readonly property bool isUpload: filehandler.progressType === FileHandler.TypeProgressUpload
    readonly property bool transferInProgress: (fileHandler.progressPercent > 0) && (fileHandler.progressPercent < 100)

    readonly property real mediaSize: { // Size in bytes
        if(isUpload)
            return message.upload.totalSize;

        var media = message.media;

        if(!media)
            return 0;

        var classtype = media.classType;

        if((classtype === TelegramConstants.typeMessageMediaPhoto) && media.photo.sizes.last)
            return media.photo.sizes.last.size;
        else if(classtype === TelegramConstants.typeMessageMediaVideo)
            return media.video.size;
        else if(classtype === TelegramConstants.typeMessageMediaAudio)
            return media.audio.size;
        else if(classtype === TelegramConstants.typeMessageMediaDocument)
            return media.document.size;

        return 0;
    }

    function cancelTransfer() {
        filehandler.cancelProgress();
    }

    function download() {
        if(isUpload)
            return;

        filehandler.download();
    }

    id: messagemediaitem
    visible: message.media ? (message.media.classType !== TelegramConstants.typeMessageMediaEmpty) : false
    width: Math.min(contentWidth, maxWidth)
    height: contentHeight

    FileHandler
    {
        id: filehandler
        telegram: messagemediaitem.context.telegram
        target: {
            var media = messagemediaitem.message.media;
            if(media.classType === TelegramConstants.typeMessageMediaContact)
            {
                var userid = media.userId;
                if(userid > 0)
                    return messagemediaitem.context.telegram.user(userid).photo.photoSmall;
            }
            return messagemediaitem.message;
        }
        defaultThumbnail: TelegramMedia.defaultThumbnail(messagemediaitem.message, messagemediaitem.context)

        onFilePathChanged: {
            var filepathstring = filePath.toString();
            var isdownload = progressType === TelegramConstants.typeProgressDownload;

            if(!isdownload || (targetType !== FileHandler.TypeTargetMediaDocument) || isSticker || (filepathstring.length <= 0) || (progressPercent < 100))
                return;

            var type = message.media.document.mimeType.split("/")[0];

            if((type === "audio") || (type === "video") || (type === "image"))
                return;

            context.sailorgram.moveMediaToDownloads(filepathstring);
        }
    }
}
