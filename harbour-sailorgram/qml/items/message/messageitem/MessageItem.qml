import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../components"
import "../../../components/message"
import "../../../models"
import "../../../menus"
import "media"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper

ListItem
{
    property bool selected: false
    property Context context
    property MessageTypesPool messageTypesPool
    property Dialog dialog
    property Message message

    signal replyRequested()
    signal forwardRequested()

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
        var media = message.media;
        if(!media)
            return;

        var classtype = media.classType;

        if((classtype === TelegramConstants.typeMessageMediaGeo) || (classtype === TelegramConstants.typeMessageMediaVenue)) {
            Qt.openUrlExternally("geo:" + media.geo.lat + "," + media.geo.longitude)
            return;
        }

        if(classtype === TelegramConstants.typeMessageMediaContact) {
            Qt.openUrlExternally("tel:" + media.phoneNumber)
            return;
        }

        var type = "";
        var canbeviewed = (classtype === TelegramConstants.typeMessageMediaPhoto) ||
                          (classtype === TelegramConstants.typeMessageMediaVideo) ||
                          (classtype === TelegramConstants.typeMessageMediaAudio);

        if(classtype === TelegramConstants.typeMessageMediaDocument) {
            if(context.telegram.documentIsSticker(message.media.document))
                return;

            var mime = media.document.mimeType;
            type = mime.split("/")[0];
            canbeviewed = ((type === "video") || (type === "audio") || (type === "image")) ? true : false;
        }

        if(!remorseNeeded(classtype, type)) {
            openOrDownloadMedia(canbeviewed, type);
            return;
        }

        messageitem.remorseAction((canbeviewed ? qsTr("Opening Media") : qsTr("Downloading Media")), function() {
            openOrDownloadMedia(canbeviewed, type);
        });
    }

    id: messageitem
    anchors { topMargin: Theme.paddingSmall; bottomMargin: Theme.paddingSmall }
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
        onForwardRequested: messageitem.forwardRequested()
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
                w = Math.max(w, lbluser.calculatedWidth);

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

        ResizableLabel
        {
            id: lbluser

            anchors  {
                left: parent.left
                right: parent.right
                leftMargin: Theme.paddingMedium
                rightMargin: Theme.paddingMedium
            }

            color: ColorScheme.colorizeText(message, context)
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            textFormat: Text.StyledText

            visible: {
                if(TelegramHelper.isForwardedMessage(message))
                    return true;

                return !TelegramHelper.isServiceMessage(message) && !message.out;
            }

            horizontalAlignment: {
                if(message.out)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }

            text: {
                if(TelegramHelper.isServiceMessage(message))
                    return "";

                var completename = message.out ? "" : TelegramHelper.completeName(context.telegram.user(message.fromId));

                if(TelegramHelper.isForwardedMessage(message))
                    completename += " <i>(" + qsTr("Forwarded from %1").arg(TelegramHelper.completeName(context.telegram.user(message.fwdFromId))) + ")</i>";

                return completename;
            }
        }

        MessageContainer
        {
            id: quotedcontainer
            visible: quotedcontainer.item !== null
            anchors { left: parent.left; leftMargin: Theme.paddingMedium }

            Component.onCompleted: {
                if(message.replyToMsgId) {
                    var params = { "context": messageitem.context,
                                   "message": context.telegram.message(message.replyToMsgId),
                                   "textColor": ColorScheme.colorizeText(message, context),
                                   "linkColor": ColorScheme.colorizeLink(message, context),
                                   "maxWidth": content.maxw - 2 * Theme.paddingMedium };

                    messageTypesPool.messagePreview.createObject(quotedcontainer, params);
                }
            }
        }

        MessageContainer
        {
            id: mediacontainer
            anchors.horizontalCenter: parent.horizontalCenter
            visible: mediacontainer.item !== null

            Component.onCompleted: {
                var media = message.media;
                if(!media)
                    return;

                var params = { "context": messageitem.context, "message": messageitem.message, "maxWidth": content.maxw - 2 * Theme.paddingMedium };

                var classtype = media.classType;

                if(classtype === TelegramConstants.typeMessageMediaDocument) {
                    if(context.telegram.documentIsSticker(media.document))
                        messageTypesPool.stickerComponent.createObject(mediacontainer, params);
                    else
                        messageTypesPool.documentComponent.createObject(mediacontainer, params);
                }
                else if(classtype === TelegramConstants.typeMessageMediaPhoto)
                    messageTypesPool.photoComponent.createObject(mediacontainer, params);
                else if(classtype === TelegramConstants.typeMessageMediaAudio)
                    messageTypesPool.audioComponent.createObject(mediacontainer, params);
                else if(classtype === TelegramConstants.typeMessageMediaVideo)
                    messageTypesPool.videoComponent.createObject(mediacontainer, params);
                else if((classtype === TelegramConstants.typeMessageMediaWebPage) && (media.webpage.url.length > 0))
                    messageTypesPool.webpageComponent.createObject(mediacontainer, params);
                else if((classtype === TelegramConstants.typeMessageMediaGeo) || (classtype === TelegramConstants.typeMessageMediaVenue))
                    messageTypesPool.locationComponent.createObject(mediacontainer, params);
                else if(classtype === TelegramConstants.typeMessageMediaContact)
                    messageTypesPool.contactComponent.createObject(mediacontainer, params);
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
            dialog: messageitem.dialog
            message: messageitem.message
        }
    }
}
