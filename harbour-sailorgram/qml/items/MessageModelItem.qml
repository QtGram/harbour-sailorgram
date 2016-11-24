import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../components/message/reply"
import "../components/message/media"
import "../components/message"
import "../components/custom"
import "../menu"
import "../js/ColorScheme.js" as ColorScheme

ListItem
{
    readonly property color bubbleColor: {
        var c = ColorScheme.colorizeBubble(model.isMessageService, model.isMessageOut, context)

        if(down)
            return Qt.darker(c, 1.5);

        return c;
    }

    readonly property color textColor: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)
    readonly property color linkColor: ColorScheme.colorizeLink(model.isMessageService, model.isMessageOut, context)
    readonly property color quoteColor: linkColor

    property bool selected: false
    property real maxWidth

    signal replyRequested()
    signal editRequested()

    id: messagemodelitem
    contentHeight: content.height + Theme.paddingLarge

    _backgroundColor: {
        if(context.bubbleshidden)
            return _showPress ? highlightedColor : "transparent";

        return "transparent";
    }

    onClicked: {
        if(!messageslist.selectionMode)
            return;

        if(messageslist.selectedMessages[model.index] === true) {
            delete messageslist.selectedMessages[model.index];
            messagemodelitem.selected = false;
            return;
        }

        messageslist.selectedMessages[model.index] = true;
        messagemodelitem.selected = true;
    }

    menu: MessageModelItemMenu {
        enabled: !messageslist.selectionMode
    }

    MessageBubble
    {
        anchors { fill: content; margins: -Theme.paddingSmall }

        visible: {
            if(context.bubbleshidden)
                return false;

            return !model.isMessageService;
        }
    }

    Column
    {
        id: content

        width: {
            if(model.isMessageService)
                return parent.width;

            var w = Math.max(lblfrom.calculatedWidth,
                             lblmessage.calculatedWidth,
                             messagereplyitem.calculatedWidth,
                             mediamessageitem.contentWidth,
                             messagestatus.contentWidth);

            return Math.min(w, maxWidth) + Theme.paddingSmall;
        }

        anchors {
            top: parent.top
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingMedium

            left: {
                if(model.isMessageService)
                    return parent.left;

                return model.isMessageOut ? undefined : parent.left;
            }

            right: {
                if(model.isMessageService)
                    return parent.right;

                return !model.isMessageOut ? undefined : parent.right;
            }
        }

        MessageText
        {
            id: lblfrom
            width: parent.width
            emojiPath: context.sailorgram.emojiPath
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            font { bold: true; pixelSize: Theme.fontSizeSmall }
            color: messagemodelitem.textColor
            linkColor: messagemodelitem.linkColor
            rawText: model.isMessageForwarded ? qsTr("Forwarded from %1").arg(model.forwardedFromName) : model.messageFrom
            visible: !model.isMessageService
            openUrls: !messageslist.selectionMode
        }

        MessageReplyItem
        {
            id: messagereplyitem
            width: parent.width
            quoteColor: messagemodelitem.quoteColor
            color: messagemodelitem.textColor
            visible: model.messageHasReply
        }

        MediaMessageItem
        {
            id: mediamessageitem
            message: model.item
            size: parent.width

            imageDelegate: ImageMessage {
                anchors.fill: parent
                source: mediamessageitem.isVideo ? mediamessageitem.videoThumbnail : mediamessageitem.source
            }

            animatedDelegate: AnimatedMessage {
                anchors.fill: parent
                source: mediamessageitem.source
            }

            locationDelegate: LocationMessage {
                title: mediamessageitem.venueTitle
                address: mediamessageitem.venueAddress
                color: messagemodelitem.textColor

                source: {
                    return context.locationThumbnail(mediamessageitem.geoPoint.latitude,
                                                     mediamessageitem.geoPoint.longitude,
                                                     maxWidth, maxWidth, 14)
                }
            }

            webPageDelegate: WebPageMessage {
                width: Math.min(contentWidth, maxWidth)
                color: messagemodelitem.textColor
                quoteColor: messagemodelitem.linkColor
                title: mediamessageitem.webPageTitle
                description: mediamessageitem.webPageDescription
                destinationUrl: mediamessageitem.webPageUrl
                source: mediamessageitem.source
                messageText: model.messageText
            }

            audioDelegate: AudioMessage {
                width: Math.min(contentWidth, maxWidth)
                duration: mediamessageitem.duration
                color: messagemodelitem.textColor
                barColor: messagemodelitem.linkColor
                source: mediamessageitem.source
                message: model.item
            }

            fileDelegate: FileMessage {
                width: Math.min(contentWidth, maxWidth)
                fileName: mediamessageitem.fileName
                fileSize: mediamessageitem.fileSize
            }
        }

        MessageText
        {
            id: lblmessage
            width: parent.width
            emojiPath: context.sailorgram.emojiPath
            wrapMode: Text.Wrap
            font { italic: model.isMessageService; pixelSize: model.isMessageService ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall }
            color: messagemodelitem.textColor
            linkColor: messagemodelitem.linkColor
            openUrls: !messageslist.selectionMode
            visible: rawText.length > 0

            rawText: {
                if(model.isMessageMedia)
                    return model.messageCaption;

                return model.messageText;
            }

            horizontalAlignment: {
                if(model.isMessageService)
                    return Text.AlignHCenter;

                return Text.AlignLeft;
            }
        }

        MessageStatus
        {
            id: messagestatus
            font.pixelSize: Theme.fontSizeTiny
            width: parent.width
            visible: !model.isMessageService
            horizontalAlignment: model.isMessageOut ? Text.AlignRight : Text.AlignLeft
            isMessageOut: model.isMessageOut
            isMessageUnread: model.isMessageUnread
            isMessageEdited: model.isMessageEdited
            isMessagePending: model.isMessagePending
            messageDate: model.messageDate
            color: messagemodelitem.textColor
            ticksColor: messagestatus.color
        }
    }
}
