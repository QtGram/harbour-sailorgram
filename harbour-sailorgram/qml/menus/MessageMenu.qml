import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../models"
import "../items/message/messageitem/media"
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TelegramConstants.js" as TelegramConstants

ContextMenu
{
    signal replyRequested()
    signal forwardRequested()
    signal deleteRequested()

    property Context context
    property SailorgramMessageItem sgMessageItem

    id: messagemenu

    MenuItem
    {
        text: qsTr("Add to Telegram")
        visible: sgMessageItem.messageType === SailorgramEnums.MessageTypeContact

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
        visible: !sgMessageItem.isActionMessage
        onClicked: replyRequested()
    }

    MenuItem
    {
        text: qsTr("Forward")
        visible: !sgMessageItem.isActionMessage
        onClicked: forwardRequested()
    }

    MenuItem
    {
        text: qsTr("Copy")
        visible: sgMessageItem.isTextMessage

        onClicked: {
            Clipboard.text = sgMessageItem.messageText
            popupmessage.show(qsTr("Message copied to clipboard"));
        }
    }

    MenuItem
    {
        text: qsTr("Delete")
        onClicked: deleteRequested()
    }

    MenuItem
    {
        text: qsTr("Install Sticker set")
        visible: sgMessageItem.messageType === SailorgramEnums.MessageTypeSticker

        onClicked: {
            context.currentSticker = message.media.document;
            context.telegram.getStickerSet(message.media.document);
        }
    }

    MenuItem
    {
        text: {
            if(!sgMessageItem.messageMedia.isTransfering)
                return sgMessageItem.messageMedia.isTransfered ? qsTr("Open") : qsTr("Download");

            return "";
        }

        visible: text.length > 0

        onClicked: {
            messageitem.remorseAction(qsTr("Downloading media"), function() {
                sgMessageItem.messageMedia.download();
            });
        }
    }

    MenuItem
    {
        text: qsTr("Cancel")
        visible: sgMessageItem.messageMedia.isTransfering
        onClicked: sgMessageItem.messageMedia.stop()
    }
}
