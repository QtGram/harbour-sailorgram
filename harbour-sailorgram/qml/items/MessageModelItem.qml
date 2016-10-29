import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../components/message"
import "../components/message/media"
import "../js/ColorScheme.js" as ColorScheme

MouseArea
{
    property real maxWidth

    id: messagemodelitem

    width: {
        if(model.isMessageService)
            return maxWidth;

        var w = Math.max(lblhiddenfrom.contentWidth, lblhiddenmessage.contentWidth, mediamessageitem.contentWidth, messagestatus.contentWidth);
        return Math.min(w, maxWidth);
    }

    height: {
        var h = Theme.paddingSmall;

        if(lblfrom.visible)
            h += lblfrom.contentHeight + content.spacing;

        if(mediamessageitem.visible)
            h += mediamessageitem.height + content.spacing;

        if(lblmessage.visible)
            h += lblmessage.contentHeight + content.spacing;

        if(messagestatus.visible)
            h += messagestatus.contentHeight + content.spacing;

        return h;
    }

    anchors {
        right: !model.isMessageOut ? undefined : parent.right
        left: model.isMessageOut ? undefined : parent.left
        rightMargin: Theme.paddingMedium
        leftMargin: Theme.paddingMedium
    }

    MessageBubble
    {
        anchors.fill: parent

        visible: {
            if(context.bubbleshidden || mediamessageitem.isSticker || mediamessageitem.isAnimated)
                return false;

            return !model.isMessageService;
        }
    }

    Column
    {
        id: content
        spacing: Theme.paddingSmall

        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            leftMargin: Theme.paddingSmall
            topMargin: Theme.paddingSmall
            rightMargin: Theme.paddingSmall
        }

        MessageText { id: lblhiddenfrom; visible: false; emojiPath: context.sailorgram.emojiPath; rawText: model.messageFrom; font { bold: true; pixelSize: Theme.fontSizeSmall } }
        MessageText { id: lblhiddenmessage; visible: false; emojiPath: context.sailorgram.emojiPath; rawText: model.messageText; font.pixelSize: Theme.fontSizeSmall }

        Label
        {
            id: lblfrom
            width: parent.width
            font { bold: true; pixelSize: Theme.fontSizeSmall }
            color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)
            visible: messagesModel.isChat && !model.isMessageOut && !model.isMessageService
            text: lblhiddenfrom.text

            horizontalAlignment: {
                if(model.isMessageOut)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }
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
                width: parent.width
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

        Label
        {
            id: lblmessage
            width: parent.width
            text: lblhiddenmessage.text
            wrapMode: Text.Wrap
            font { italic: model.isMessageService; pixelSize: Theme.fontSizeSmall }
            color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)

            horizontalAlignment: {
                if(model.isMessageService)
                    return Text.AlignHCenter;

                if(model.isMessageOut)
                    return Text.AlignLeft;

                return Text.AlignRight;
            }
        }

        MessageStatus
        {
            id: messagestatus
            width: parent.width
            visible: !model.isMessageService
            horizontalAlignment: Text.AlignRight
            isMessageOut: model.isMessageOut
            isMessageUnread: model.isMessageUnread
            isMessageEdited: model.isMessageEdited
            messageDate: model.messageDate
            color: ColorScheme.colorizeText(model.isMessageService, model.isMessageOut, context)
            ticksColor: messagestatus.color
        }
    }
}
