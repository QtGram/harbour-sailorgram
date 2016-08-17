import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"

ListItem
{
    property Context context
    property var document

    id: stickercategory

    Telegram.DownloadHandler
    {
        id: downloadhandler
        engine: context.engine
        source: stickercategory.document
    }

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Small
        running: !downloadhandler.downloaded && downloadhandler.downloading
    }

    Image
    {
        asynchronous: true
        source: downloadhandler.thumbnail
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
