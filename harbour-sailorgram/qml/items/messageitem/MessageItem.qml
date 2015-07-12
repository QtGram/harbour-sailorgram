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
    property Context context
    property Message message

    function downloadMedia() {
        messageitem.remorseAction(qsTr("Downloading Media"), function() {
            loader.item.fileHandler.target = messageitem.message; // Set Target Object before download
        });
    }

    function displayMedia() {
        if(!message.media)
            return;

        var canbephoto = (message.media.classType === TelegramConstants.typeMessageMediaDocument) || (message.media.classType === TelegramConstants.typeMessageMediaPhoto);

        if(canbephoto)
            loader.item.fileHandler.target = messageitem.message; // Download Photo

        var path = loader.item.mediaPath;

        if(canbephoto && context.sailorgram.fileIsPhoto(path)) {
            pageStack.push(Qt.resolvedUrl("../../pages/media/MediaPhotoPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": loader.item.fileHandler });
            return;
        }

        if(!path.length) {
            downloadMedia();
            return;
        }

        Qt.openUrlExternally(path);
    }

    id: messageitem
    contentWidth: parent.width
    contentHeight: content.height

    menu: MessageMenu {
        id: messagemenu
        context: messageitem.context
        message: messageitem.message

        onCancelRequested: loader.item.cancelTransfer()
        onDownloadRequested: downloadMedia()
        onOpenRequested: displayMedia()
    }

    onClicked: displayMedia()

    Component {
        id: documentcomponent

        MessageDocument {
            context: messageitem.context
            message: messageitem.message
        }
    }

    Component {
        id: photocomponent

        MessagePhoto {
            context: messageitem.context
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
            text: (!TelegramHelper.isServiceMessage(message) && message.out) ? "" : TelegramHelper.userName(context.telegram.user(message.fromId))
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

                if((message.media.classType === TelegramConstants.typeMessageMediaDocument) && context.telegram.documentIsSticker(message.media.document))
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
