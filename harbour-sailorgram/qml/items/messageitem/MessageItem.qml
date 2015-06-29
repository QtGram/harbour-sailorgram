import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../menus"
import "media"

ListItem
{
    readonly property real typeMessageMediaDocument: 0x2fda2204
    readonly property real typeMessageMediaContact: 0x5e7d2f39
    readonly property real typeMessageMediaEmpty: 0x3ded6320
    readonly property real typeMessageMediaVideo: 0xa2d24290
    readonly property real typeMessageMediaUnsupported: 0x9f84f49e
    readonly property real typeMessageMediaAudio: 0xc6b68300
    readonly property real typeMessageMediaPhoto: 0xc8c45a2a
    readonly property real typeMessageMediaGeo: 0x56e0d474

    property Telegram telegram
    property Message message

    function downloadMedia() {
        messageitem.remorseAction(qsTr("Downloading Media"), function() {
            switch(loader.item.fileHandler.targetType) {
                case FileHandler.TypeTargetMediaVideo:
                case FileHandler.TypeTargetMediaPhoto:
                case FileHandler.TypeTargetMediaDocument:
                case FileHandler.TypeTargetMediaAudio:
                    loader.item.fileHandler.download();
                    break;

                default:
                    break;
            }

        });
    }

    id: messageitem
    contentWidth: parent.width
    contentHeight: content.height

    menu: ConversationMenu {
        id: messagemenu
        telegram: messageitem.telegram
        message: messageitem.message

        onDownloadRequested: downloadMedia()
        onOpenRequested: Qt.openUrlExternally(loader.item.fileHandler.filePath)
    }

    onClicked: {
        if(!message.media)
            return;

        var path = loader.item.fileHandler.filePath.toString();

        if(path.length > 0)
        {
            Qt.openUrlExternally(loader.item.fileHandler.filePath)
            return;
        }

        downloadMedia();
    }

    Component {
        id: documentcomponent

        MessageDocument {
            telegram: messageitem.telegram
            message: messageitem.message
        }
    }

    Component {
        id: photocomponent

        MessagePhoto {
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
                    if(message.media.classType === messageitem.typeMessageMediaPhoto)
                        return photocomponent;
                    else if(message.media.classType === messageitem.typeMessageMediaDocument)
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
