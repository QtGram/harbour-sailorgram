import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"

ListItem
{
    property Context context
    property StickersModel stickersModel
    property string stickerSetId
    property bool isSelected: false
    property StickerSet stickerSet: stickersModel.stickerSetItem(stickerSetId)
    property Document document: stickersModel.stickerSetThumbnailDocument(stickerSetId)

    id: stickersetimage

    FileHandler
    {
        id: filehandler
        telegram: context.telegram
        target: stickersetimage.document
        Component.onCompleted: download()
    }

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Small
        running: !filehandler.downloaded && ((filehandler.progressPercent > 0) && (filehandler.progressPercent < 100))
    }

    Image
    {
        source: filehandler.thumbPath
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
        anchors { left: parent.left; right: parent.right; bottom: selectionrect.top; margins: (Theme.paddingSmall / 2) }
    }

    Rectangle
    {
        id: selectionrect
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: Theme.paddingSmall
        width: parent.width
        color: Theme.secondaryHighlightColor
    }
}
