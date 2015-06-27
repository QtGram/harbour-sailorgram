import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../menus"

Page
{
    property Telegram telegram
    property Document document

    id: mediapage

    FileHandler {

        id: filehandler
        target: document

        onTargetChanged: {
            filehandler.download();
        }
    }

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
            source: filehandler.filePath
        }
    }
}
