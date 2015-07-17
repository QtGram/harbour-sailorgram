import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../js/TelegramHelper.js" as TelegramHelper

Image
{
    property Telegram telegram
    property Dialog dialog
    property Chat chat
    property User user

    Component.onCompleted: {
        if(!TelegramHelper.isChat(dialog) && !user.photo.photoSmall.download.downloaded)
            telegram.getFile(user.photo.photoSmall);
    }

    id: imgconversation
    fillMode: Image.PreserveAspectFit
    asynchronous: true

    source: {
        if(TelegramHelper.isChat(dialog))
            return "";

        return user.photo.photoSmall.download.location;
    }

    Rectangle {
        id: imgfallback
        anchors.fill: parent
        color: Theme.secondaryHighlightColor
        radius: imgconversation.width * 0.5
        visible: TelegramHelper.isChat(dialog) || (user.photo.photoSmall.download.location.length <= 0)

        Label {
            anchors.centerIn: parent
            font.bold: true
            text: TelegramHelper.isChat(dialog) ? chat.title.slice(0, 1) : user.firstName.slice(0, 1).toUpperCase() + ((user.lastName.length > 0) ? user.lastName.slice(0, 1).toUpperCase(): "");
        }
    }
}
