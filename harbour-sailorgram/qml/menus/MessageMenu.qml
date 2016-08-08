import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../models"
import "../items/message/messageitem/media"
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TelegramConstants.js" as TelegramConstants

ContextMenu
{
    signal replyRequested()
    signal forwardRequested()
    signal downloadRequested()
    signal cancelRequested()

    property Context context
    property var messageType
    property string messageText
    property MessageMediaItem messageMediaItem

    id: messagemenu

    MenuItem
    {
        text: qsTr("Add to Telegram")
        visible: messageType === Enums.TypeContactMessage

        /* FIXME:
        onClicked: {
            var media = message.media;
            pageStack.push(Qt.resolvedUrl("../pages/contacts/AddContactPage.qml"), {
                               "context": context,
                               "firstname": media.firstName,
                               "lastname": media.lastName,
                               "telephonenumber": media.phoneNumber
                           });
        }
        */
    }

    MenuItem
    {
        text: qsTr("Reply")
        visible: messageType !== Enums.TypeActionMessage
        onClicked: replyRequested();
    }

    MenuItem
    {
        text: qsTr("Forward")
        visible: messageType !== Enums.TypeActionMessage
        onClicked: forwardRequested();
    }

    MenuItem
    {
        text: qsTr("Copy")
        visible: messageType === Enums.TypeTextMessage;

        onClicked: {
            Clipboard.text = messagemenu.messageText;
            popupmessage.show(qsTr("Message copied to clipboard"));
        }
    }

    MenuItem
    {
        text: qsTr("Delete")

        onClicked: {
            messageitem.remorseAction(qsTr("Deleting Message"), function() {
                context.telegram.deleteMessages([message.id]);
            });
        }
    }

    MenuItem
    {
        text: qsTr("Install Sticker set")
        visible: TelegramHelper.isSticker(context, message)

        onClicked: {
            context.currentSticker = message.media.document;
            context.telegram.getStickerSet(message.media.document);
        }
    }

    MenuItem
    {
        text: {
            if(messageMediaItem && !messageMediaItem.downloadHandler.downloading)
                return messageMediaItem.downloadHandler.downloaded ? qsTr("Open") : qsTr("Download");

            return "";
        }

        visible: text.length > 0

        onClicked: {
            messageitem.remorseAction(qsTr("Downloading media"), function() {
                downloadRequested();
            });
        }
    }

    MenuItem
    {
        text: qsTr("Cancel")
        visible: messageMediaItem && messageMediaItem.downloadHandler.downloading
        onClicked: cancelRequested()
    }
}
