import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Gallery 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../components"
import "../../js/TelegramConstants.js" as TelegramConstants

MediaPage
{
    id: mediaphotopage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
         if(!imageHandler.downloaded)
             imageHandler.download();
    }

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
                    context.sailorgram.moveMediaToGallery(imageHandler.destination, TelegramConstants.typeMessageMediaPhoto); //FIXME: !!!
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
            source: imageHandler.downloaded ? imageHandler.destination : imageHandler.thumbnail

            BusyIndicator
            {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Medium
                running: imageHandler.downloading
            }
        }
    }
}
