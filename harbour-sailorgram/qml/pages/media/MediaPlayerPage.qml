import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../components"
import "../../components/mediaplayer"

MediaPage
{
    id: mediaplayerpage
    allowedOrientations: defaultAllowedOrientations

    RemorsePopup { id: remorsepopup }

    PopupMessage
    {
        id: popupmessage
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }

    MediaPlayer
    {
        id: mediaplayer
        anchors.fill: parent
        videoThumbnail: imageHandler.thumbPath
        videoSource: imageHandler.filePath
    }

    ProgressCircle
    {
        anchors.centerIn: parent
        width: Theme.iconSizeLarge
        height: Theme.iconSizeLarge
        visible: (imageHandler.progressPercent > 0) && (imageHandler.progressPercent < 100)
        progressValue: imageHandler.progressPercent / 100
    }
}
