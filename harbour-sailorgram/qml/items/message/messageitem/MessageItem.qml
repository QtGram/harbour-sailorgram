import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../components/message"
import "../../../models"
import "../../../menus"
import "media"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper

ListItem
{
    property Context context
    property MessageTypesPool messageTypesPool
    property Message message

    signal replyRequested()

    function remorseNeeded(mediatype, type) {
        if(context.immediateopen || !mediacontainer.item || mediacontainer.item.fileHandler.downloaded)
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
        if(!mediacontainer.item)
            return;

        if(!mediacontainer.item.fileHandler.downloaded)
            mediacontainer.item.fileHandler.download();

        if((message.media.classType === TelegramConstants.typeMessageMediaVideo) || (message.media.classType === TelegramConstants.typeMessageMediaAudio) || (type === "audio") || (type === "video")) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPlayerPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": mediacontainer.item.fileHandler });
            return;
        }

        if(!mediacontainer.item.fileHandler.downloaded)
            return;

        if((message.media.classType === TelegramConstants.typeMessageMediaPhoto) || (type === "image")) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPhotoPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": mediacontainer.item.fileHandler });
            return;
        }

        popupmessage.show(qsTr("Opening media"));
        Qt.openUrlExternally(mediacontainer.item.fileHandler.filePath);
    }

    function displayMedia() {
        if(!message.media)
            return;

        var type = "";
        var canbeviewed = (message.media.classType === TelegramConstants.typeMessageMediaPhoto) ||
                          (message.media.classType === TelegramConstants.typeMessageMediaVideo) ||
                          (message.media.classType === TelegramConstants.typeMessageMediaAudio);

        if(message.media.classType === TelegramConstants.typeMessageMediaDocument) {
            if(context.telegram.documentIsSticker(message.media.document))
                return;

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
    onClicked: displayMedia()

    menu: MessageMenu {
        id: messagemenu
        context: messageitem.context
        message: messageitem.message
        messageMediaItem: mediacontainer.item

        onCancelRequested: mediacontainer.item.cancelTransfer()
        onDownloadRequested: mediacontainer.item.download()
        onReplyRequested: messageitem.replyRequested()
    }

    MessageBubble
    {
        id: messagebubble
        context: messageitem.context
        message: messageitem.message
        anchors.fill: content
    }

    Column
    {
        property bool isSticker: false
        property real maxw: Screen.width * 3 / 4

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

            if(quotedcontainer.item)
                w = Math.max(w, quotedcontainer.item.width);

            if(mediacontainer.item)
                w = Math.max(w, mediacontainer.item.width);

            w += 2 * Theme.paddingMedium;
            return Math.min(w, content.maxw);
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

            color: ColorScheme.colorizeText(message, context)
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

        MessageContainer
        {
            id: quotedcontainer
            visible: quotedcontainer.item !== null
            anchors { left: parent.left; leftMargin: Theme.paddingMedium }

            Component.onCompleted: {
                if(message.replyToMsgId) {
                    var params = { "context": messageitem.context, "message": messageitem.message, "maxWidth": content.maxw - 2 * Theme.paddingMedium };
                    messageTypesPool.messageQuotedComponent.createObject(quotedcontainer, params);
                }
            }
        }

        MessageContainer
        {
            id: mediacontainer
            anchors.horizontalCenter: parent.horizontalCenter
            visible: mediacontainer.item !== null

            Component.onCompleted: {
                if(!message.media)
                    return;

                var params = { "context": messageitem.context, "message": messageitem.message, "maxWidth": content.maxw - 2 * Theme.paddingMedium };

                if(message.media.classType === TelegramConstants.typeMessageMediaDocument) {
                    if(context.telegram.documentIsSticker(message.media.document))
                        messageTypesPool.stickerComponent.createObject(mediacontainer, params);
                    else
                        messageTypesPool.documentComponent.createObject(mediacontainer, params);

                    return;
                }

                if(message.media.classType === TelegramConstants.typeMessageMediaPhoto)
                    messageTypesPool.photoComponent.createObject(mediacontainer, params);
                else if(message.media.classType === TelegramConstants.typeMessageMediaAudio)
                    messageTypesPool.audioComponent.createObject(mediacontainer, params);
                else if(message.media.classType === TelegramConstants.typeMessageMediaVideo)
                    messageTypesPool.videoComponent.createObject(mediacontainer, params);
                else if(message.media.classType === TelegramConstants.typeMessageMediaGeo)
                    messageTypesPool.locationComponent.createObject(mediacontainer, params);
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

            maxWidth: content.maxw - 2 * Theme.paddingMedium
            context: messageitem.context
            message: messageitem.message
        }
    }
}
