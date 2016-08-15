import QtQuick 2.1
import QtGraphicalEffects 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../../../models"

Item
{
    readonly property size imageSize: {
        if(useTelegramImage)
            return tgthumbnailimage.imageSize;

        return thumbnailimage.sourceSize;
    }

    function download() {
        if(!useTelegramImage || tgthumbnailimage.downloaded || tgthumbnailimage.downloading || !tgthumbnailimage.source)
            return;

        tgthumbnailimage.download();
    }

    property Context context
    property var source: null
    property bool useTelegramImage: false
    property bool autoDownload: false
    property int fillMode: Image.PreserveAspectFit

    id: messagethumbnail
    onSourceChanged: messagethumbnail.download()

    onAutoDownloadChanged: {
        if(!autoDownload)
            return;

        messagethumbnail.download();
    }

    Telegram.Image
    {
        id: tgthumbnailimage
        qtQuickVersion: "2.1"
        anchors.fill: parent
        visible: useTelegramImage
        engine: useTelegramImage ? context.engine : null
        source: useTelegramImage ? messagethumbnail.source : null
        fillMode: messagethumbnail.fillMode
    }

    Image
    {
        id: thumbnailimage
        anchors.fill: parent
        asynchronous: true
        fillMode: Image.PreserveAspectFit
        visible: !useTelegramImage
        source: (!useTelegramImage && messagethumbnail.source)? messagethumbnail.source : ""
    }

    FastBlur
    {
        id: thumbnailblur
        anchors.fill: useTelegramImage ? tgthumbnailimage: thumbnailimage
        source: useTelegramImage ? tgthumbnailimage: thumbnailimage

        radius: {
            if(!useTelegramImage)
                return 0;

            return tgthumbnailimage.downloaded && !tgthumbnailimage.downloading ? 0.0 : 32.0;
        }

        Behavior on radius { NumberAnimation { duration: 250 } }
    }
}
