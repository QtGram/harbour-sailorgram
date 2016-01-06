import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../menus"
import "media"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper

ListItem
{
    property Context context
    property Message message

    function remorseNeeded(mediatype, type) {
        if(medialoader.item.fileHandler.downloaded) // No remorse for downloaded medias
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
        if(!medialoader.item.fileHandler.downloaded)
            medialoader.item.fileHandler.download();

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) || (type === "image")) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPhotoPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": medialoader.item.fileHandler });
            return;
        }

        if((message.media.classType === TelegramConstants.typeMessageMediaVideo) || (message.media.classType === TelegramConstants.typeMessageMediaAudio) || (type === "audio") || (type === "video")) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPlayerPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": medialoader.item.fileHandler });
            return;
        }

        if(!medialoader.item.fileHandler.downloaded)
            return;

        popupmessage.show(qsTr("Opening media"));
        Qt.openUrlExternally(medialoader.item.fileHandler.filePath);
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
        messageMediaItem: medialoader.item

        onCancelRequested: medialoader.item.cancelTransfer()
        onDownloadRequested: medialoader.item.download()
    }

    onClicked: displayMedia()

    Component {
        id: messagequoteditem

        MessageQuotedItem {
            context: messageitem.context
            message: messageitem.message
        }
    }

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
        id: bubble
        radius: 4
        visible: !context.bubbleshidden
        color: ColorScheme.colorizeBubble(message, context, false)
        width: content.width
        height: content.height

        anchors {
            left: message.out ? parent.left : undefined
            right: message.out ? undefined : parent.right
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingMedium
        }
    }

    Column
    {
        id: content

        anchors {
            left: message.out ? parent.left : undefined
            right: message.out ? undefined : parent.right
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingMedium
        }

        width: {
            if(TelegramHelper.isServiceMessage(message))
                return messageitem.width - Theme.paddingMedium;

            var w = messagetext.calculatedWidth;

            if(lbluser.visible)
                w = Math.max(w, lbluser.contentWidth);

            if(quotedloader.item)
                w = Math.max(w, quotedloader.item.calculatedWidth);

            if(medialoader.item)
                w = Math.max(w, medialoader.width);

            var maxw = messageitem.width - (Theme.paddingMedium * 2);

            if(w >= maxw)
                return maxw;

            return w + (Theme.paddingMedium * 2);
        }

        height: {
            var h = padding.height + messagetext.height + Theme.paddingSmall;

            if(lbluser.visible)
                h += lbluser.contentHeight;

            if(quotedloader.item)
                h += quotedloader.height;

            if(medialoader.item)
                h += medialoader.height;

            return h;
        }

        Item
        {
            id: padding
            width: parent.width
            height: Theme.paddingMedium
        }

        Label
        {
            id: lbluser

            anchors  {
                left: parent.left
                right: parent.right
                leftMargin: Theme.paddingMedium
                rightMargin: Theme.paddingMedium
            }

            color: ColorScheme.colorize(message, context)
            visible: !TelegramHelper.isServiceMessage(message) && !message.out
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            wrapMode: Text.NoWrap

            horizontalAlignment: {
                if(message.out)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }

            text: {
                if(TelegramHelper.isServiceMessage(message) || message.out)
                    return "";

                return TelegramHelper.completeName(context.telegram.user(message.fromId))
            }
        }

        Loader
        {
            id: quotedloader
            asynchronous: true

            anchors  {
                left: parent.left
                right: parent.right
                leftMargin: Theme.paddingMedium
                rightMargin: Theme.paddingMedium
            }

            sourceComponent: {
                if(message.replyToMsgId)
                     return messagequoteditem;

                return null;
            }
        }

        Loader
        {
            id: medialoader
            asynchronous: true
            anchors.horizontalCenter: parent.horizontalCenter

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
        }

        MessageText
        {
            id: messagetext

            anchors  {
                left: parent.left
                right: parent.right
                leftMargin: Theme.paddingMedium
                rightMargin: Theme.paddingMedium
            }

            context: messageitem.context
            message: messageitem.message
        }
    }
}
