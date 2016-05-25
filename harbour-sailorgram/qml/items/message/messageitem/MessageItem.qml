import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
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
    property var dialogModelItem
    property var messageModelItem

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
        modelItem: messageitem.messageModelItem
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
        modelItem: messageitem.messageModelItem
        anchors { topMargin: Theme.paddingSmall; bottomMargin: Theme.paddingSmall }
        anchors.fill: content
    }

    Column
    {
        property bool isSticker: false
        property real maxw: Screen.width * 3 / 4

        id: content

        anchors {
            left: messageModelItem.out ? parent.left : undefined
            right: messageModelItem.out ? undefined : parent.right
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingMedium
        }

        width: {
            if(messageModelItem.messageType === Enums.TypeActionMessage)
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


            color: ColorScheme.colorizeTextItem(messageModelItem, context)
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            textFormat: Text.StyledText

            visible: {
                if(messageModelItem.forwardFromPeer)
                    return true;

                return (messageModelItem.messageType !== Enums.TypeActionMessage) && !messageModelItem.out;
            }

            horizontalAlignment: {
                if(messageModelItem.out)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }

            text: {
                if(!messageModelItem || (messageModelItem.messageType === Enums.TypeActionMessage))
                    return "";

                var completename = "";

                if(dialogModelItem.chat && dialogModelItem.chat.broadcast)
                    completename = dialogModelItem.title;
                else
                    completename = TelegramHelper.completeName(messageModelItem.fromUserItem);

                var fwdpeer = messageModelItem.forwardFromPeer;

                if(fwdpeer)
                    completename += " <i>(" + qsTr("Forwarded from %1").arg(TelegramHelper.completeName(fwdpeer)) + ")</i>";

                return completename;
            }
        }

        MessageContainer
        {
            id: quotedcontainer
            visible: quotedcontainer.item !== null
            anchors { left: parent.left; leftMargin: Theme.paddingMedium }

            Component.onCompleted: {
                var replymsg = messageModelItem.replyMessage;

                if(replymsg !== null) {
                    var params = { "context": messageitem.context,
                                   "message": replymsg,
                                   "messageType": messageModelItem.replyType,
                                   "peer": messageModelItem.replyPeer,
                                   "textColor": ColorScheme.colorizeTextItem(messageModelItem, context),
                                   "linkColor": ColorScheme.colorizeLink(messageModelItem, context),
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
                if(messageModelItem.messageType === Enums.TypeTextMessage)
                    return;

                var params = { "context": messageitem.context, "messageModelItem": messageitem.messageModelItem, "maxWidth": content.maxw - 2 * Theme.paddingMedium };

                if(messageModelItem.messageType === Enums.TypeDocumentMessage)
                    messageTypesPool.documentComponent.createObject(mediacontainer, params);
                else if(messageModelItem.messageType === Enums.TypeStickerMessage)
                    messageTypesPool.stickerComponent.createObject(mediacontainer, params);
                else if(messageModelItem.messageType === Enums.TypePhotoMessage)
                    messageTypesPool.photoComponent.createObject(mediacontainer, params);
                else if(messageModelItem.messageType === Enums.TypeAudioMessage)
                    messageTypesPool.audioComponent.createObject(mediacontainer, params);
                else if(messageModelItem.messageType === Enums.TypeVideoMessage)
                    messageTypesPool.videoComponent.createObject(mediacontainer, params);
                else if((messageModelItem.messageType === Enums.TypeWebPageMessage)) //FIXME: && (media.webpage.url.length > 0))
                    messageTypesPool.webpageComponent.createObject(mediacontainer, params);
                else if((messageModelItem.messageType === Enums.TypeGeoMessage) || (messageModelItem.messageType === Enums.TypeVenueMessage))
                    messageTypesPool.locationComponent.createObject(mediacontainer, params);
                else if(messageModelItem.messageType === Enums.TypeContactMessage)
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
            messageModelItem: messageitem.messageModelItem
        }
    }
}
