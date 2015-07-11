import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property alias fileHandler: filehandler

    property Context context
    property Telegram telegram
    property Message message

    readonly property bool isUpload: message.upload.fileId !== 0
    readonly property bool transferInProgress: (progressPercent > 0) && (progressPercent < 100)
    readonly property bool hasMedia: message.media ? (message.media.classType !== TelegramConstants.typeMessageMediaEmpty) : false
    readonly property real progressPercent: isUpload ? (100 * message.upload.uploaded / message.upload.totalSize) : filehandler.progressPercent

    readonly property real mediaSize: {
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

    readonly property string mediaPath: {
        if(!message.media)
            return "";

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) && message.media.photo.sizes.last)
            return message.media.photo.sizes.last.location.download.location;
        else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
            return message.media.video.thumb.location.download.location;
        else if(message.media.classType === TelegramConstants.typeMessageMediaAudio)
            return "image://theme/icon-m-sounds";
        else if(message.media.classType === TelegramConstants.typeMessageMediaUnsupported)
            return "image://theme/icon-m-other";
        else if(message.media.classType === TelegramConstants.typeMessageMediaDocument) {
            if(telegram.documentIsSticker(message.media.document)) {
                return "image://theme/icon-m-other";
                //FIXME: WebP: Not Supported (return filehandler.thumbPath);
            }

            return message.media.document.thumb.location.download.location;
        }

        return "";
    }

    function cancelTransfer() {
        if(isUpload) {
            telegram.cancelSendGet(message.upload.fileId);
            return;
        }

        filehandler.cancelProgress();
    }

    id: messagemediaitem
    visible: hasMedia

    FileHandler
    {
        id: filehandler
        telegram: messagemediaitem.telegram
        defaultThumbnail: "image://theme/icon-m-document"

        onTargetTypeChanged: {
            switch(targetType) {
                case FileHandler.TypeTargetMediaVideo:
                case FileHandler.TypeTargetMediaPhoto:
                case FileHandler.TypeTargetMediaDocument:
                case FileHandler.TypeTargetMediaAudio:
                    fileHandler.download();
                    break;

                default:
                    break;
            }
        }

        onFilePathChanged: {
            if((filePath.toString().length <= 0) || (progressPercent < 100))
                return;

            context.sailorgram.moveMediaToDownloads(message.media);
        }
    }

    onHasMediaChanged: {
        if(!hasMedia)
            return;

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) && message.media.photo.sizes.last)
            telegram.getFile(message.media.photo.sizes.last.location);
        else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
            telegram.getFile(message.media.video.thumb.location);
        else if(message.media.classType === TelegramConstants.typeMessageMediaDocument)
            telegram.getFile(message.media.document.thumb.location);
    }
}
