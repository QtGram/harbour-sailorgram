import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0

ContextMenu
{
    property Telegram telegram
    property Message message

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
        text: qsTr("Download")
        visible: message.media && (message.media.classType !== typeMessageMediaEmpty)
    }
}
