import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"

BackgroundItem
{
    property Context context
    property alias sticker: downloadhandler.source

    Telegram.DownloadHandler
    {
        id: downloadhandler
        engine: context.engine
    }

    Image
    {
        asynchronous: true
        source: downloadhandler.thumbnail
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
        anchors { fill: parent; margins: (Theme.paddingSmall / 2) }
    }
}
