import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../components/message"

ListItem
{
    id: dialogmodelitem
    contentHeight: Theme.itemSizeSmall

    PeerImage
    {
        id: peerimage
        anchors { left: parent.left; top: parent.top; leftMargin: Theme.paddingSmall }
        size: dialogmodelitem.height
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
            text: model.title
            font { family: Theme.fontFamilyHeading; pixelSize: Theme.fontSizeSmall }
            elide: Text.ElideRight
            color: Theme.highlightColor
            width: parent.width - lblstatus.contentWidth - Theme.paddingSmall
        }

        MessageStatus
        {
            id: lblstatus
            horizontalAlignment: Text.AlignRight
            visible: !model.isTopMessageService
            ticksColor: Theme.highlightColor
            messageDate: model.topMessageDate
            isMessageOut: model.isTopMessageOut
            isMessageUnread: model.isTopMessageUnread
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
            color: model.isTopMessageService ? Theme.secondaryHighlightColor : Theme.primaryColor
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
            width: parent.height
            height: parent.height
            color: Theme.secondaryHighlightColor
            radius: width * 0.5
            visible: model.unreadCount > 0

            Label {
                text: model.unreadCount
                color: Theme.primaryColor
                anchors.centerIn: parent
                font { pixelSize: Theme.fontSizeSmall; bold: true }
            }
        }
    }
}
