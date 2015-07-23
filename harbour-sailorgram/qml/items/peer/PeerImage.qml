import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

Image
{
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    onUserChanged: {
        if(!user)
            return;

        if(!user.photo.photoSmall.download.downloaded)
            context.telegram.getFile(user.photo.photoSmall);
    }

    id: imgpeer
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
        radius: imgpeer.width * 0.5
        visible: TelegramHelper.isChat(dialog) || (user.photo.photoSmall.download.location.length <= 0)

        Label {
            anchors.centerIn: parent
            font.bold: true
            text: TelegramHelper.fallbackText(imgpeer.dialog, TelegramHelper.isChat(imgpeer.dialog) ? imgpeer.chat : imgpeer.user)
        }
    }

    Image
    {
        id: imgpeertype
        anchors { bottom: parent.bottom; right: parent.right }
        fillMode: Image.PreserveAspectFit
        visible: dialog && (dialog.encrypted || TelegramHelper.isChat(dialog))

        source: {
            if(!dialog)
                return "";

            if(dialog.encrypted)
                return "image://theme/icon-s-secure"

            if(TelegramHelper.isChat(dialog))
                return "image://theme/icon-s-chat";

            return "";
        }
    }
}
