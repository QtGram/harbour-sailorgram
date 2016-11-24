import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../components/message"
import "../menu"

ListItem
{
    id: dialogmodelitem
    contentHeight: Theme.itemSizeSmall
    menu: DialogModelItemMenu { }

    PeerImage
    {
        id: peerimage
        anchors { left: parent.left; top: parent.top; leftMargin: Theme.paddingSmall }
        size: dialogmodelitem.contentHeight
        backgroundColor: Theme.secondaryHighlightColor
        foregroundColor: Theme.primaryColor
        fontPixelSize: Theme.fontSizeLarge
        peer: model.item
    }

    Row
    {
        id: headerrow
        anchors { left: peerimage.right; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        height: lbltitle.contentHeight

        Label
        {
            id: lbltitle
            textFormat: Text.StyledText
            font { family: Theme.fontFamilyHeading; pixelSize: Theme.fontSizeExtraSmall }
            elide: Text.ElideRight
            color: Theme.highlightColor
            verticalAlignment: Text.AlignTop

            width: {
                var w = parent.width - Theme.paddingSmall;

                if(lblstatus.visible)
                    w -= lblstatus.contentWidth - Theme.paddingSmall;

                return w;
            }

            text: {
                if(model.isCloud)
                    return "\u2601 " + model.title;

                if(model.isBroadcast)
                    return "\uD83D\uDCE3 " + model.title;

                if(model.isChat || model.isMegaGroup)
                    return "\uD83D\uDC65 " + model.title;

                return model.title;
            }
        }

        MessageStatus
        {
            id: lblstatus
            horizontalAlignment: Text.AlignRight
            ticksColor: Theme.highlightColor
            messageDate: model.topMessageDate
            isMessageOut: model.isTopMessageOut
            isMessageUnread: model.isTopMessageUnread
            isMute: model.isMuted
            dateFirst: false
        }
    }

    Row
    {
        anchors {
            left: peerimage.right
            top: headerrow.bottom
            bottom: parent.bottom
            right: parent.right
            leftMargin: Theme.paddingSmall
            rightMargin: Theme.paddingMedium
        }

        spacing: Theme.paddingSmall

        Label
        {
            id: lblfrom
            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeExtraSmall
            verticalAlignment: Text.AlignVCenter

            text: {
                if(model.topMessage)
                    return (model.topMessage.isOut ? qsTr("You") : model.topMessageFrom) + ":";

                return "";
            }

            visible: {
                if(model.draftMessage.length > 0 || model.isBroadcast || model.isTopMessageService)
                    return false;

                if(model.isChat || model.isMegaGroup)
                    return true;

                if(model.topMessage)
                    return model.isTopMessageOut;

                return false;
            }
        }

        MessageText
        {
            id: lbllastmessage
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            color: model.isTopMessageService ? Theme.rgba(Theme.secondaryHighlightColor, 1.0) : Theme.primaryColor
            linkColor: Theme.secondaryColor
            font { pixelSize: Theme.fontSizeExtraSmall; italic: model.isTopMessageService }
            emojiPath: context.sailorgram.emojiPath
            openUrls: false

            width: {
                var w = parent.width;

                if(lblfrom.visible)
                    w -= lblfrom.contentWidth + Theme.paddingSmall;

                if(rectunreadcount.visible)
                    w -= rectunreadcount.width + Theme.paddingSmall;

                return w;
            }

            rawText: {
                var msg = "";

                if(model.draftMessage.length > 0)
                    msg = qsTr("Draft: %1").arg(model.draftMessage);
                else
                    msg = model.topMessageText;

                return msg;
            }
        }

        Rectangle
        {
            id: rectunreadcount
            width: parent.height - Theme.paddingSmall
            height: parent.height - Theme.paddingSmall
            color: Theme.secondaryHighlightColor
            radius: width * 0.5
            visible: model.unreadCount > 0

            Label {
                text: model.unreadCount
                color: Theme.primaryColor
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font { pixelSize: Theme.fontSizeExtraSmall; bold: true }
            }
        }
    }
}
