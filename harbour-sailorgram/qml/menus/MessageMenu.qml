import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/TelegramConstants.js" as TelegramConstants

ContextMenu
{
    signal cancelRequested()
    signal downloadRequested()
    signal openRequested()

    property Telegram telegram
    property Message message
    property FileHandler fileHandler

    MenuItem
    {
        text: qsTr("Copy")

        onClicked: {
            Clipboard.text = message.message;
            popupmessage.show(qsTr("Message copied to clipboard"));
        }
    }

    MenuItem
    {
        text: qsTr("Delete")

        onClicked: {
            messageitem.remorseAction(qsTr("Deleting Message"), function () {
                telegram.deleteMessages([message.id]);
            });
        }
    }

    MenuItem
    {
        text: qsTr("Cancel")
        visible: false //FIXME: message.out && loader.item && loader.item.transferInProgress
        onClicked: cancelRequested()
    }

    MenuItem
    {
        text: qsTr("Download")
        visible: (!fileHandler || fileHandler.isSticker || message.out) ? false : (fileHandler.filePath.toString().length <= 0) && (message.media && (message.media.classType !== TelegramConstants.typeMessageMediaEmpty))
        onClicked: downloadRequested()
    }

    MenuItem
    {
        text: qsTr("Open");
        visible: !fileHandler ? false : (fileHandler.filePath.toString().length > 0) && (message.media && (message.media.classType !== TelegramConstants.typeMessageMediaEmpty))
        onClicked: openRequested()
    }
}
