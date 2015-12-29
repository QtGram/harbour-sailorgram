import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0
import "../../../../models"
import "../../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property alias fileHandler: filehandler

    property Context context
    property Telegram telegram
    property Message message

    readonly property bool isUpload: message.upload.fileId !== 0
    readonly property bool transferInProgress: (progressPercent > 0) && (progressPercent < 100)
    readonly property bool hasMedia: message.media ? (message.media.classType !== TelegramConstants.typeMessageMediaEmpty) : false

    readonly property real progressPercent: {
        if(filehandler.downloaded)
            return 0;

        if(isUpload)
            return 100 * message.upload.uploaded / message.upload.totalSize;

        return filehandler.progressPercent;
    }

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

    readonly property string mediaThumbnail: {
        if(!message.media)
            return "";

        if(context.telegram.documentIsSticker(message.media.document))
            return "image://theme/icon-m-other"; //FIXME: WebP: Not Supported

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) && filehandler.downloaded)
            return filehandler.filePath;

        return fileHandler.thumbPath;
    }

    function cancelTransfer() {
        if(isUpload) {
            telegram.cancelSendGet(message.upload.fileId);
            return;
        }

        filehandler.cancelProgress();
    }

    function download() {
        if(isUpload)
            return;

        filehandler.download();
    }

    id: messagemediaitem
    visible: hasMedia

    FileHandler
    {
        id: filehandler
        telegram: messagemediaitem.context.telegram
        target: messagemediaitem.message
        defaultThumbnail: "image://theme/icon-m-other"

        onFilePathChanged: {
            var filepathstring = filePath.toString();

            if((progressType !== TelegramConstants.typeProgressDownload) || (filepathstring.length <= 0) || (progressPercent < 100) || isSticker)
                return;

            if(targetType !== FileHandler.TypeTargetMediaDocument)
                return;

            var type = message.media.document.mimeType.split("/")[0];

            if((type === "audio") || (type === "video") || (type === "image"))
                return;

            context.sailorgram.moveMediaToDownloads(message.media);
        }
    }
}
