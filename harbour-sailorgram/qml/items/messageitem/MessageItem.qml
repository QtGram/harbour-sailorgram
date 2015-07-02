import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../models"
import "../../menus"
import "media"
import "../../js/TelegramConstants.js" as TelegramConstants

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

    menu: ConversationMenu {
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
