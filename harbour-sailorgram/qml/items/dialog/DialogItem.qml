import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../../models"
import "../../items/peer"
import "../../items/user"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramAction.js" as TelegramAction

ListItem
{
    property Context context
    property SailorgramDialogItem sgDialogItem
    property var topMessage

    id: dialogitem

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: conversationimage
            width: dialogitem.contentHeight
            height: dialogitem.contentHeight
            context: dialogitem.context
            peer: sgDialogItem.peer
            fallbackTitle: sgDialogItem.title
            isChat: sgDialogItem.isChat
            isBroadcast: sgDialogItem.isBroadcast
            isSecretChat: sgDialogItem.isSecretChat
        }

        Column
        {
            width: parent.width - conversationimage.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: dialogitem.contentHeight / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }
                spacing: Theme.paddingSmall

                Label {
                    id: lbltitle
                    font { family: Theme.fontFamilyHeading; pixelSize: Theme.fontSizeSmall }
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                    text: sgDialogItem.title

                    width: {
                        var w = parent.width - msgstatus.contentWidth;

                        if(imgmute.visible)
                            w -= imgmute.width + Theme.paddingSmall;

                        return w;
                    }
                }

                Image {
                    id: imgmute
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    source: "image://theme/icon-m-speaker-mute"
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                    visible: sgDialogItem.isMute
                }

                MessageStatus
                {
                    id: msgstatus
                    height: parent.height
                    horizontalAlignment: Text.AlignRight
                    context: dialogitem.context
                    color: Theme.primaryColor
                    ticksColor: Theme.highlightColor
                    isActionMessage: sgDialogItem.isActionMessage
                    isMessageOut: sgDialogItem.isMessageOut
                    messageDate: sgDialogItem.messageDate
                    isMessageUnread: sgDialogItem.isMessageUnread
                    dateOnly: sgDialogItem.isActionMessage || !sgDialogItem.isMessageOut
                    dateFirst: false
                    visible: true
                }
            }

            Row
            {
                height: dialogitem.contentHeight / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label
                {
                    id: lblfrom
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeExtraSmall

                    visible: {
                        if(sgDialogItem.isActionMessage || (sgDialogItem.typingUsers.length > 0))
                            return false;

                        if(!sgDialogItem.isBroadcast)
                            return true;

                        return sgDialogItem.isMessageOut;
                    }

                    text: {
                        if(!sgDialogItem.isActionMessage && (sgDialogItem.typingUsers.length <= 0)) {
                            if(sgDialogItem.isChat && !sgDialogItem.isBroadcast)
                                return TelegramHelper.userDisplayName(sgDialogItem.messageUser) + ": ";

                            if(sgDialogItem.isMessageOut)
                                return qsTr("You:") + " ";
                        }

                        return "";
                    }
                }

                MessageTextContent
                {
                    id: mtcmessage
                    width: parent.width - rectunread.width - lblfrom.contentWidth
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                    emojiPath: context.sailorgram.emojiPath
                    linkColor: Theme.secondaryColor
                    openUrls: false

                    color: {
                        if((sgDialogItem.typingUsers.length > 0) || !sgDialogItem.isTextMessage)
                            return Theme.highlightColor;

                        return Theme.primaryColor;
                    }

                    font.italic: {
                        if(sgDialogItem.typingUsers.length > 0)
                            return true;

                        return false;
                    }

                    rawText: {
                        if(sgDialogItem.typingUsers.length > 0)
                            return sgDialogItem.typingUsers;
                        else if(sgDialogItem.isActionMessage)
                            return TelegramAction.actionType(sgDialogItem.messageAction, sgDialogItem.isSecretChat);
                        else if(!sgDialogItem.isTextMessage && (sgDialogItem.messageType !== SailorgramEnums.MessageTypeWebPage))
                            return TelegramHelper.mediaType(sgDialogItem.messageType);

                        return TelegramHelper.firstMessageLine(context, sgDialogItem.messageText);
                    }
                }

                Rectangle
                {
                    id: rectunread
                    width: sgDialogItem.unreadCount > 0 ? parent.height : 0
                    height: parent.height
                    color: Theme.secondaryHighlightColor
                    visible: sgDialogItem.unreadCount > 0
                    radius: width * 0.5

                    Label
                    {
                        anchors.centerIn: parent
                        font.pixelSize: Theme.fontSizeTiny
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: sgDialogItem.unreadCount
                    }
                }
            }
        }
    }
}
