import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../menus"

Page
{
    property Telegram telegram
    property Photo photo

    id: mediapage

    FileHandler { id: filehandler }

    SilicaFlickable
    {
        anchors.fill: parent

        MediaMenu { id: mediamenu }

        Image
        {
            id: imgphoto
            asynchronous: true
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: photo.sizes.last.location.download.location
        }
    }
}
