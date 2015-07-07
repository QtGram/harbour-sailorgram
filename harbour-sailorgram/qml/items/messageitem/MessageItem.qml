import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../models"
import "../../menus"
import "media"
import "../../js/TelegramConstants.js" as TelegramConstants
import "../../js/TelegramHelper.js" as TelegramHelper

ListItem
{
    property Settings settings
    property Telegram telegram
    property Message message

    function downloadMedia() {
        messageitem.remorseAction(qsTr("Downloading Media"), function() {
            loader.item.fileHandler.target = messageitem.message; // Set Target Object before download
        });
    }

    id: messageitem
    contentWidth: parent.width
    contentHeight: content.height

    menu: MessageMenu {
        id: messagemenu
        telegram: messageitem.telegram
        message: messageitem.message

        onCancelRequested: loader.item.cancelTransfer()
        onDownloadRequested: downloadMedia()
        onOpenRequested: Qt.openUrlExternally(loader.item.fileHandler.filePath)
    }

    onClicked: {
        if(!message.media)
            return;

        var path = loader.item.fileHandler.filePath.toString();

        if(path.length > 0) {
            Qt.openUrlExternally(loader.item.fileHandler.filePath);
            return;
        }

        downloadMedia();
    }

    Component {
        id: documentcomponent

        MessageDocument {
            settings: messageitem.settings
            telegram: messageitem.telegram
            message: messageitem.message
        }
    }

    Component {
        id: photocomponent

        MessagePhoto {
            settings: messageitem.settings
            telegram: messageitem.telegram
            message: messageitem.message
        }
    }

    Column
    {
        id: content
        anchors { top: parent.top; left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }

        Label
        {
            id: lbluser
            anchors { left: message.out ? parent.left : undefined; right: message.out ? undefined : parent.right }
            visible: !TelegramHelper.isServiceMessage(message) && !message.out
            text: (!TelegramHelper.isServiceMessage(message) && message.out) ? "" : TelegramHelper.userName(telegram.user(message.fromId))
            font.bold: true
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.NoWrap
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            color: Theme.secondaryHighlightColor
        }

        Loader
        {
            id: loader
            anchors { left: message.out ? parent.left : undefined; right: message.out ? undefined : parent.right }

            sourceComponent: {
                if(message.media) {
                    if(message.media.classType === TelegramConstants.typeMessageMediaPhoto)
                        return photocomponent;
                    else if(message.media.classType === TelegramConstants.typeMessageMediaDocument)
                        return documentcomponent;
                }

                return null;
            }

            onLoaded: {
                messagemenu.fileHandler = loader.item.fileHandler;

                if((message.media.classType === TelegramConstants.typeMessageMediaDocument) && telegram.documentIsSticker(message.media.document))
                    loader.item.fileHandler.target = messageitem.message;
            }
        }

        MessageText
        {
            id: messagetext
            width: parent.width
            message: messageitem.message
        }
    }
}
