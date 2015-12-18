import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../../models"
import "../../../menus"
import "media"
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper

ListItem
{
    property Context context
    property Message message

    function remorseNeeded(mediatype, type) {
        if(loader.item.fileHandler.downloaded) // No remorse for downloaded medias
            return false;

        if((mediatype === TelegramConstants.typeMessageMediaVideo) || (mediatype === TelegramConstants.typeMessageMediaAudio))
            return true;

        if((type === "audio") || (type === "video"))
            return true;

        if((mediatype === TelegramConstants.typeMessageMediaDocument) && (type !== "image"))
            return true;

        return false;
    }

    function openOrDownloadMedia(canbeviewed, type) {
        if(!loader.item.fileHandler.downloaded)
            loader.item.fileHandler.download();

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) || (type === "image")) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPhotoPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": loader.item.fileHandler });
            return;
        }

        if((message.media.classType === TelegramConstants.typeMessageMediaVideo) || (message.media.classType === TelegramConstants.typeMessageMediaAudio) || (type === "audio") || (type === "video")) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPlayerPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": loader.item.fileHandler });
            return;
        }

        if(!loader.item.fileHandler.downloaded)
            return;

        popupmessage.show(qsTr("Opening media"));
        Qt.openUrlExternally(loader.item.fileHandler.filePath);
    }

    function displayMedia() {
        if(!message.media)
            return;

        var type = "";
        var canbeviewed = (message.media.classType === TelegramConstants.typeMessageMediaPhoto) ||
                          (message.media.classType === TelegramConstants.typeMessageMediaVideo) ||
                          (message.media.classType === TelegramConstants.typeMessageMediaAudio);

        if(message.media.classType === TelegramConstants.typeMessageMediaDocument) {
            var mime = message.media.document.mimeType;
            type = mime.split("/")[0];
            canbeviewed = ((type === "video") || (type === "audio") || (type === "image")) ? true : false;
        }

        if(!remorseNeeded(message.media.classType, type)) {
            openOrDownloadMedia(canbeviewed, type);
            return;
        }

        messageitem.remorseAction((canbeviewed ? qsTr("Opening Media") : qsTr("Downloading Media")), function() {
            openOrDownloadMedia(canbeviewed, type);
        });
    }

    id: messageitem
    contentWidth: parent.width
    contentHeight: content.height

    menu: MessageMenu {
        id: messagemenu
        context: messageitem.context
        message: messageitem.message
        messageMediaItem: loader.item

        onCancelRequested: loader.item.cancelTransfer()
        onDownloadRequested: loader.item.download()
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

    Component {
        id: audiocomponent

        MessageAudio {
            context: messageitem.context
            message: messageitem.message
        }
    }

    Component {
        id: videocomponent

        MessageVideo {
            context: messageitem.context
            message: messageitem.message
        }
    }

    Rectangle
    {
        anchors {
            left: message.out ? parent.left : undefined;
            right: message.out ? undefined : parent.right
            leftMargin: message.out ? Theme.paddingMedium : 0
            rightMargin: message.out ? 0 : Theme.paddingMedium
        }

        radius: 10

        width: {
            if(TelegramHelper.isServiceMessage(message))
                return 0;

            var w = Math.max(lbluser.width, messagetext.calculatedWidth);

            if(TelegramHelper.isMediaMessage(message))
                w = Math.max(w, loader.width);

            return w + Theme.paddingMedium;

        }

        height: {
            if(TelegramHelper.isServiceMessage(message))
                return 0;

            var h = lbluser.height + messagetext.height + Theme.paddingSmall;

            if(TelegramHelper.isMediaMessage(message))
                h += loader.height;

            return h;
        }

        color: {
            if(TelegramHelper.isServiceMessage(message))
                return "transparent"

            if(message.out)
                return Theme.secondaryColor;

            return Theme.rgba(Qt.tint(Theme.secondaryHighlightColor, Theme.rgba(Theme.highlightDimmerColor, 0.3)), 0.7);
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
            visible: !TelegramHelper.isServiceMessage(message)
            font.bold: true
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.NoWrap
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter

            color: {
                if(message.out)
                    return Theme.rgba(Theme.highlightDimmerColor, 1.0);

                return Theme.rgba(Theme.secondaryColor, 1.0);
            }

            text: {

                if(TelegramHelper.isServiceMessage(message))
                    return "";

                if(message.out)
                    return qsTr("You");

                return TelegramHelper.completeName(context.telegram.user(message.fromId))
            }
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
                    else if(message.media.classType === TelegramConstants.typeMessageMediaAudio)
                        return audiocomponent;
                    else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
                        return videocomponent;
                }

                return null;
            }

            onLoaded: {
            }
        }

        MessageText
        {
            id: messagetext
            width: parent.width
            context: messageitem.context
            message: messageitem.message
        }
    }
}
