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
    property MessageTypesPool messageTypesPool
    property Context context
    property MessageMedia mediaItem
    property Chat chat
    property User fromUser
    property User serviceUser
    property string dialogTitle
    property string messageText
    property string fileName
    property string fileMimeType
    property int fileSize
    property int fileDuration
    property bool messageOut
    property bool messageUnread
    property var messageDateTime
    property var messageType
    property var forwardFromPeer
    property var forwardFromType
    property var replyMessage
    property var replyPeer
    property var replyType
    property var serviceItem

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
        messageType: messageitem.messageType
        messageText: messageitem.messageText
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
        messageType: messageitem.messageType
        messageOut: messageitem.messageOut
        anchors { topMargin: Theme.paddingSmall; bottomMargin: Theme.paddingSmall }
        anchors.fill: content
    }

    Column
    {
        property bool isSticker: false
        property real maxw: Screen.width * 3 / 4

        id: content

        anchors {
            left: messageOut ? parent.left : undefined
            right: messageOut ? undefined : parent.right
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingMedium
        }

        width: {
            if(messageType === Enums.TypeActionMessage)
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


            color: ColorScheme.colorizeText(messageType, messageOut, context)
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            textFormat: Text.StyledText

            visible: {
                if(forwardFromPeer)
                    return true;

                return (messageType !== Enums.TypeActionMessage) && !messageOut;
            }

            horizontalAlignment: {
                if(messageOut)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }

            text: {
                if(messageType === Enums.TypeActionMessage)
                    return "";

                var completename = "";

                if(chat && chat.broadcast)
                    completename = dialogTitle;
                else
                    completename = TelegramHelper.completeName(fromUser);

                if(forwardFromPeer)
                    completename += " <i>(" + qsTr("Forwarded from %1").arg(TelegramHelper.completeName(forwardFromPeer)) + ")</i>";

                return completename;
            }
        }

        MessageContainer
        {
            id: quotedcontainer
            visible: quotedcontainer.item !== null
            anchors { left: parent.left; leftMargin: Theme.paddingMedium }

            Component.onCompleted: {
                if(messageitem.replyMessage !== null) {
                    var params = { "context": messageitem.context,
                                   "message": messageitem.replyMessage,
                                   "messageType": messageitem.replyType,
                                   "peer": messageitem.replyPeer,
                                   "textColor": ColorScheme.colorizeText(messageType, messageOut, context),
                                   "linkColor": ColorScheme.colorizeLink(messageType, messageOut, context),
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
                if(messageType === Enums.TypeTextMessage)
                    return;

                var params = { "context": messageitem.context,
                               "mediaItem": messageitem.mediaItem,
                               "messageType": messageitem.messageType,
                               "fileSize": messageitem.fileSize,
                               "fileDuration": messageitem.fileDuration,
                               "messageOut": messageitem.messageOut,
                               "fileName": messageitem.fileName,
                               "fileMimeType": messageitem.fileMimeType,
                               "maxWidth": content.maxw - 2 * Theme.paddingMedium };

                if(messageType === Enums.TypeDocumentMessage)
                    messageTypesPool.documentComponent.createObject(mediacontainer, params);
                else if(messageType === Enums.TypeStickerMessage)
                    messageTypesPool.stickerComponent.createObject(mediacontainer, params);
                else if(messageType === Enums.TypePhotoMessage)
                    messageTypesPool.photoComponent.createObject(mediacontainer, params);
                else if(messageType === Enums.TypeAudioMessage)
                    messageTypesPool.audioComponent.createObject(mediacontainer, params);
                else if(messageType === Enums.TypeVideoMessage)
                    messageTypesPool.videoComponent.createObject(mediacontainer, params);
                else if(messageType === Enums.TypeWebPageMessage) //FIXME: && (media.webpage.url.length > 0))
                    messageTypesPool.webpageComponent.createObject(mediacontainer, params);
                else if((messageType === Enums.TypeGeoMessage) || (messageType === Enums.TypeVenueMessage))
                    messageTypesPool.locationComponent.createObject(mediacontainer, params);
                else if(messageType === Enums.TypeContactMessage)
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
            fromUser: messageitem.fromUser
            serviceUser: messageitem.serviceUser
            messageDateTime: messageitem.messageDateTime
            messageType: messageitem.messageType
            serviceItem: messageitem.serviceItem
            messageText: messageitem.messageText
            messageOut: messageitem.messageOut
            messageUnread: messageitem.messageUnread
        }
    }
}
