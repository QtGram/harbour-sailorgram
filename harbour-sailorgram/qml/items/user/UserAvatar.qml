import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0

Image
{
    property Telegram telegram
    property User user

    Component.onCompleted: {
        if(!user.photo.photoSmall.download.downloaded)
            telegram.getFile(user.photo.photoSmall);
    }

    id: imgcontact
    fillMode: Image.PreserveAspectFit
    asynchronous: true
    source: user.photo.photoSmall.download.location

    Rectangle {
        id: imgfallback
        anchors.fill: parent
        color: Theme.secondaryHighlightColor
        radius: imgcontact.width * 0.5
        visible: user.photo.photoSmall.download.location.length <= 0

        Label {
            anchors.centerIn: parent
            font.bold: true
            text: user.firstName.slice(0, 1).toUpperCase() + ((user.lastName.length > 0) ? user.lastName.slice(0, 1).toUpperCase(): "");
        }
    }
}
