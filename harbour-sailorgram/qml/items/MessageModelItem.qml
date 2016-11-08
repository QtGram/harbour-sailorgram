import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../components/message/reply"
import "../components/message/media"
import "../components/message"
import "../menu"
import "../js/ColorScheme.js" as ColorScheme

ListItem
{
    property real maxWidth

    signal replyRequested()
    signal editRequested()

    id: messagemodelitem
    contentHeight: content.height + Theme.paddingSmall
    menu: MessageModelItemMenu { }

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
            font { bold: true; pixelSize: Theme.fontSizeSmall }
            color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)
            visible: !model.isMessageOut && !model.isMessageService
            rawText: model.messageFrom

            horizontalAlignment: {
                if(model.isMessageOut)
                    return Text.AlignRight;

                return Text.AlignLeft;
            }
        }

        MessageReplyItem
        {
            id: messagereplyitem
            width: parent.width
            quoteColor: ColorScheme.colorizeLink(model.isMessageService, model.isMessageOut, context)
            color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)
            visible: model.messageHasReply
        }

        MediaMessageItem
        {
            id: mediamessageitem
            message: model.item
            size: parent.width

            imageDelegate: ImageMessage {
                anchors.fill: parent
                needsBlur: !mediamessageitem.downloaded
                source: mediamessageitem.source
            }

            animatedDelegate: AnimatedMessage {
                anchors.fill: parent
                source: mediamessageitem.source
            }

            locationDelegate: LocationMessage {
                title: mediamessageitem.venueTitle
                address: mediamessageitem.venueAddress
                color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)

                source: {
                    return context.locationThumbnail(mediamessageitem.geoPoint.latitude,
                                                     mediamessageitem.geoPoint.longitude,
                                                     maxWidth, maxWidth, 14)
                }
            }

            webPageDelegate: WebPageMessage {
                width: Math.min(contentWidth, maxWidth)
                color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)
                quoteColor: ColorScheme.colorizeLink(model.isMessageService, model.isMessageOut, context)
                title: mediamessageitem.webPageTitle
                description: mediamessageitem.webPageDescription
                url: mediamessageitem.webPageUrl
                source: mediamessageitem.source
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
            rawText: model.messageText
            wrapMode: Text.Wrap
            font { italic: model.isMessageService; pixelSize: model.isMessageService ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall }
            color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)

            horizontalAlignment: {
                if(model.isMessageService)
                    return Text.AlignHCenter;

                if(model.isMessageOut)
                    return Text.AlignRight;

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
            messageDate: model.messageDate
            color: Qt.darker(ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context), 1.5)
            ticksColor: messagestatus.color
        }
    }
}
