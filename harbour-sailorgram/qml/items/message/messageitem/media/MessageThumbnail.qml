import QtQuick 2.1
import QtGraphicalEffects 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../../../models"

Item
{
    readonly property size imageSize: {
        if(useTelegramImage)
            return downloadhandler.imageSize;

        return thumbnailimage.sourceSize;
    }

    function checkDownload() {
        if(!useTelegramImage || !downloadhandler.check())
            return;

        downloadhandler.download();
    }

    function checkAutoDownload() {
        if(!autoDownload) {
            if(useTelegramImage)
                downloadhandler.check();

            return;
        }

        messagethumbnail.checkDownload();
    }

    property Context context
    property var source
    property bool useTelegramImage: false
    property bool autoDownload: false
    property int fillMode: Image.PreserveAspectFit

    id: messagethumbnail
    onSourceChanged: checkAutoDownload()
    onAutoDownloadChanged: checkAutoDownload()

    Telegram.DownloadHandler
    {
        id: downloadhandler
        engine: useTelegramImage ? context.engine : null
        source: useTelegramImage ? messagethumbnail.source: null

        onEngineChanged: checkAutoDownload()
        onSourceChanged: checkAutoDownload()
    }

    Image
    {
        id: thumbnailimage
        anchors.fill: parent
        asynchronous: true
        cache: false
        fillMode: Image.PreserveAspectFit

        source: {
            if(!useTelegramImage && messagethumbnail.source)
                return messagethumbnail.source;

            if(useTelegramImage) {
                if(downloadhandler.destination.length > 0)
                    return downloadhandler.destination;

                return downloadhandler.thumbnail;
            }

            return "";
        }
    }

    FastBlur
    {
        id: thumbnailblur
        anchors.fill: thumbnailimage
        source: thumbnailimage

        radius: {
            if(!useTelegramImage)
                return 0;

            return downloadhandler.downloaded && !downloadhandler.downloading ? 0.0 : 32.0;
        }

        Behavior on radius { NumberAnimation { duration: 250 } }
    }
}
