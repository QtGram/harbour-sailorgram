import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../components"
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

RoundedImage
{
    property bool showType: true
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    FileHandler
    {
        id: filehandler
        telegram: context.telegram

        target: {
            if(user)
                return user.photo.photoSmall;

            if(chat)
                return chat.photo.photoSmall;

            return null;
        }

        onTargetTypeChanged: {
            if(downloaded)
                return;

            download();
        }
    }

    id: imgpeer
    asynchronous: true
    fillMode: Image.PreserveAspectFit
    source: filehandler.filePath

    Rectangle
    {
        id: imgfallback
        anchors.fill: parent
        color: Theme.secondaryHighlightColor
        radius: imgpeer.width * 0.5
        visible: !filehandler.downloaded || (imgpeer.status === Image.Error)

        Label {
            anchors.centerIn: parent
            font.bold: true
            font.pixelSize: parent.height * 0.6
            text: TelegramHelper.fallbackText(imgpeer.dialog, TelegramHelper.isChat(imgpeer.dialog) ? imgpeer.chat : imgpeer.user)
        }
    }

    Image
    {
        id: imgpeertype
        asynchronous: true
        anchors { bottom: parent.bottom; right: parent.right }
        fillMode: Image.PreserveAspectFit
        visible: showType && dialog && (dialog.encrypted || TelegramHelper.isChat(dialog))

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
