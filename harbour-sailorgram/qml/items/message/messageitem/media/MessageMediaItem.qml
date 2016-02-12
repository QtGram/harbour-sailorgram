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

        if(!message.media)
            return 0;

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) && message.media.photo.sizes.last)
            return message.media.photo.sizes.last.size;
        else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
            return message.media.video.size;
        else if(message.media.classType === TelegramConstants.typeMessageMediaAudio)
            return message.media.audio.size;
        else if(message.media.classType === TelegramConstants.typeMessageMediaDocument)
            return message.media.document.size;

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
        target: messagemediaitem.message
        defaultThumbnail: TelegramMedia.defaultThumbnail(messagemediaitem.message)

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
