import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"

ListItem
{
    property Context context
    property StickersModel stickersModel
    property string stickerSetId
    property StickerSet stickerSet: stickersModel.stickerSetItem(stickerSetId)
    property Document document: stickersModel.stickerSetThumbnailDocument(stickerSetId)

    id: stickersetimage

    FileHandler
    {
        id: filehandler
        telegram: context.telegram
        target: stickersetimage.document

        onTargetTypeChanged: {
            if(downloaded)
                return;

            download();
        }
    }

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Small
        running: !filehandler.downloaded && ((filehandler.progressPercent > 0) && (filehandler.progressPercent < 100))
    }

    Image
    {
        asynchronous: true
        source: filehandler.thumbPath
        fillMode: Image.PreserveAspectFit
        anchors { left: parent.left; top: parent.top; right: parent.right; bottom: selectionrect.top; bottomMargin: Theme.paddingSmall; topMargin: Theme.paddingSmall }
    }

    Rectangle
    {
        id: selectionrect
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: Theme.paddingSmall
        color: Theme.secondaryHighlightColor
    }
}
