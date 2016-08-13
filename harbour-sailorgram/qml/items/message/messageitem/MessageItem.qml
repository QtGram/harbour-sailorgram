import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
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
    property MessageTypesPool messageTypesPool
    property Context context
    property SailorgramDialogItem sgDialogItem
    property SailorgramMessageItem sgMessageItem
    property bool selected: false

    signal replyRequested()
    signal forwardRequested()
    signal deleteRequested()

    function remorseNeeded() {
        if(context.immediateopen || sgMessageItem.messageMedia.isTransfered)
            return false;

        if((sgMessageItem.messageType === SailorgramEnums.MessageTypeVideo) || (sgMessageItem.messageType === SailorgramEnums.MessageTypeAudio))
            return true;

        var mimetype = sgMessageItem.messageMedia.fileMimeType;

        if((mimetype.indexOf("audio") !== -1) || (mimetype.indexOf("video") !== -1))
            return true;

        if((sgMessageItem.messageType === SailorgramEnums.MessageTypeDocument) && (mimetype.indexOf("image") !== -1))
            return true;

        return false;
    }

    function openOrDownloadMedia() {
        if(!sgMessageItem.messageMedia.isTransfered)
            sgMessageItem.messageMedia.download();

        var mimetype = sgMessageItem.messageMedia.fileMimeType;
        var isvideo = (sgMessageItem.messageType === SailorgramEnums.MessageTypeVideo) || (mimetype.indexOf("video") !== -1);
        var isaudio = (sgMessageItem.messageType === SailorgramEnums.MessageTypeAudio) || (mimetype.indexOf("audio") !== -1);

        if(isvideo || isaudio) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPlayerPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": mediacontainer.item.fileHandler });
            return;
        }

        if(!sgMessageItem.messageMedia.isTransfered)
            return;

        if((sgMessageItem.messageType === SailorgramEnums.MessageTypePhoto) || (mimetype.indexOf("image") !== -1)) {
            pageStack.push(Qt.resolvedUrl("../../../pages/media/MediaPhotoPage.qml"), { "context": messageitem.context, "message": messageitem.message, "fileHandler": mediacontainer.item.fileHandler });
            return;
        }

        popupmessage.show(qsTr("Opening media"));
        Qt.openUrlExternally(sgMessageItem.messageMedia.filePath);
    }

    function displayMedia() {
        if(!sgMessageItem.isMediaMessage || (sgMessageItem.messageType === SailorgramEnums.MessageTypeSticker))
            return;

        var isgeoorvenue = (sgMessageItem.messageType === SailorgramEnums.MessageTypeGeo) || (sgMessageItem.messageType === SailorgramEnums.MessageTypeVenue);

        if(isgeoorvenue) {
            //FIXME: Qt.openUrlExternally("geo:" + media.geo.lat + "," + media.geo.longitude)
            return;
        }

        if(sgMessageItem.messageType === SailorgramEnums.MessageTypeContact) {
            //FIXME: Qt.openUrlExternally("tel:" + media.phoneNumber)
            return;
        }

        if(!remorseNeeded()) {
            openOrDownloadMedia();
            return;
        }

        //FIXME: messageitem.remorseAction((canbeviewed ? qsTr("Opening Media") : qsTr("Downloading Media")), function() {
            //openOrDownloadMedia(canbeviewed, type);
        //});
    }

    id: messageitem
    anchors { topMargin: Theme.paddingSmall; bottomMargin: Theme.paddingSmall }
    contentWidth: parent.width
    contentHeight: content.height
    onClicked: displayMedia()

    menu: MessageMenu {
        id: messagemenu
        context: messageitem.context
        sgMessageItem: messageitem.sgMessageItem

        onReplyRequested: messageitem.replyRequested()
        onForwardRequested: messageitem.forwardRequested()
        onDeleteRequested: messageitem.deleteRequested()
    }

    MessageBubble
    {
        id: messagebubble
        context: messageitem.context
        sgMessageItem: messageitem.sgMessageItem
        anchors { topMargin: Theme.paddingSmall; bottomMargin: Theme.paddingSmall }
        anchors.fill: content
    }

    Column
    {
        property bool isSticker: false
        property real maxw: Screen.width * 3 / 4

        id: content

        anchors {
            left: sgMessageItem.isMessageOut ? parent.left : undefined
            right: sgMessageItem.isMessageOut ? undefined : parent.right
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingMedium
        }

        width: {
            if(sgMessageItem.isActionMessage)
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


            color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            textFormat: Text.StyledText

            visible: {
                if(sgMessageItem.isForward)
                    return true;

                return !sgMessageItem.isActionMessage && !sgMessageItem.isMessageOut
            }

            horizontalAlignment: {
                if(sgMessageItem.isMessageOut)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }

            text: {
                if(sgMessageItem.isActionMessage)
                    return "";

                var completename = "";

                if(sgDialogItem.isBroadcast)
                    completename = sgDialogItem.title;
                else
                    completename = TelegramHelper.completeName(sgMessageItem.fromUser);

                if(sgMessageItem.isForward) {
                    if(sgMessageItem.forwardPeer.isChat)
                        completename += " <i>(" + qsTr("Forwarded from %1").arg(sgMessageItem.forwardPeer.chat.title) + ")</i>";
                    else
                        completename += " <i>(" + qsTr("Forwarded from %1").arg(TelegramHelper.completeName(sgMessageItem.forwardPeer.user)) + ")</i>";
                }

                return completename;
            }
        }

        MessageContainer
        {
            id: quotedcontainer
            visible: quotedcontainer.item !== null
            anchors { left: parent.left; leftMargin: Theme.paddingMedium }

            Component.onCompleted: {
                if(sgMessageItem.hasReply) {
                    var params = { "context": messageitem.context,
                                   "sgMessageItem": sgMessageItem.messageReply,
                                   "sgPeer": sgMessageItem.replyPeer,
                                   "textColor": ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context),
                                   "linkColor": ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context),
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
                if(!sgMessageItem.isMediaMessage)
                    return;

                var params = { "context": messageitem.context,
                               "sgMessageItem": messageitem.sgMessageItem,
                               "maxWidth": content.maxw - 2 * Theme.paddingMedium };

                if(sgMessageItem.messageType === SailorgramEnums.MessageTypeDocument)
                    messageTypesPool.documentComponent.createObject(mediacontainer, params);
                else if(sgMessageItem.messageType === SailorgramEnums.MessageTypeSticker)
                    messageTypesPool.stickerComponent.createObject(mediacontainer, params);
                else if(sgMessageItem.messageType === SailorgramEnums.MessageTypePhoto)
                    messageTypesPool.photoComponent.createObject(mediacontainer, params);
                else if(sgMessageItem.messageType === SailorgramEnums.MessageTypeAudio)
                    messageTypesPool.audioComponent.createObject(mediacontainer, params);
                else if(sgMessageItem.messageType === SailorgramEnums.MessageTypeVideo)
                    messageTypesPool.videoComponent.createObject(mediacontainer, params);
                else if(sgMessageItem.messageType === SailorgramEnums.MessageTypeWebPage)
                    messageTypesPool.webpageComponent.createObject(mediacontainer, params);
                else if((sgMessageItem.messageType === SailorgramEnums.MessageTypeGeo) || (sgMessageItem.messageType === SailorgramEnums.MessageTypeVenue))
                    messageTypesPool.locationComponent.createObject(mediacontainer, params);
                else if(sgMessageItem.messageType === SailorgramEnums.MessageTypeContact)
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
            sgDialogItem: messageitem.sgDialogItem
            sgMessageItem: messageitem.sgMessageItem
        }
    }
}
