import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../models"
import "../js/TelegramConstants.js" as TelegramConstants

ContextMenu
{
    signal cancelRequested()

    property Context context
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
                context.telegram.deleteMessages([message.id]);
            });
        }
    }

    MenuItem
    {
        text: qsTr("Cancel")
        visible: false //FIXME: message.out && loader.item && loader.item.transferInProgress
        onClicked: cancelRequested()
    }
}
