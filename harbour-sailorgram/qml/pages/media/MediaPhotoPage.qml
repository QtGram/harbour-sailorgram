import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../js/TelegramConstants.js" as TelegramConstants

MediaPage
{
    id: mediaphotopage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Save in Gallery")

                onClicked: {
                    context.sailorgram.moveMediaToGallery(fileHandler.filePath.toString(), TelegramConstants.typeMessageMediaPhoto);
                    popupmessage.show(qsTr("Image saved in Gallery"));
                }
            }
        }

        PopupMessage
        {
            id: popupmessage
            anchors { left: parent.left; top: parent.top; right: parent.right }
        }

        ImageViewer
        {
            id: image
            anchors.fill: parent
            enableZoom: !flickable.moving
            source: fileHandler.filePath

            BusyIndicator
            {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Medium
                running: !fileHandler.downloaded && (fileHandler.progressPercent > 0) && (fileHandler.progressPercent < 100)
            }
        }
    }
}
