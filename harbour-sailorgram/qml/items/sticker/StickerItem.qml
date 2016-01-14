import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"

BackgroundItem
{
    property Context context
    property alias stickerDocument: filehandler.target

    FileHandler
    {
        id: filehandler
        telegram: context.telegram
        Component.onCompleted: download()
    }

    Image
    {
        source: filehandler.filePath
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
        anchors { fill: parent; margins: (Theme.paddingSmall / 2) }
    }
}
