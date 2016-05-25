import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"
import "../../items/peer"
import "../../items/user"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramAction.js" as TelegramAction

ListItem
{
    property Context context
    property var dialogModelItem

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
            peer: dialogModelItem.peer
            fallbackTitle: dialogModelItem.title
            isChat: dialogModelItem.chat !== null
            isBroadcast: dialogModelItem.chat && dialogModelItem.chat.broadcast
            isSecretChat: dialogModelItem.isSecretChat
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
                    text: dialogModelItem.title

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
                    visible: dialogModelItem.mute
                }

                MessageStatus
                {
                    id: msgstatus
                    height: parent.height
                    horizontalAlignment: Text.AlignRight
                    context: dialogitem.context
                    color: Theme.primaryColor
                    ticksColor: Theme.highlightColor
                    messageType: dialogModelItem.messageType
                    messageOut: dialogModelItem.messageOut
                    messageDate: dialogModelItem.messageDate
                    messageUnread: dialogModelItem.messageUnread
                    dateFirst: false
                    visible: true

                    dateOnly: {
                        if(!dialogModelItem.topMessage)
                            return false;

                        return (dialogModelItem.messageType === Telegram.Enums.TypeActionMessage) || !dialogModelItem.messageOut;
                    }
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
                        if((dialogModelItem.messageType === Telegram.Enums.TypeActionMessage) || (dialogModelItem.typing.length > 0))
                            return false;

                        if(dialogModelItem.chat && !dialogModelItem.chat.broadcast)
                            return true;

                        return dialogModelItem.topMessage && dialogModelItem.messageOut;
                    }

                    text: {
                        if(dialogModelItem.topMessage && (dialogModelItem.messageType !== Telegram.Enums.TypeActionMessage) && (dialogModelItem.typing.length <= 0)) {
                            if(dialogModelItem.chat && !dialogModelItem.chat.broadcast && dialogModelItem.messageUser)
                                return TelegramHelper.userDisplayName(dialogModelItem.messageUser) + ": ";

                            if(dialogModelItem.messageOut)
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
                        if((dialogModelItem.typing.length > 0) || (dialogModelItem.messageType !== Telegram.Enums.TypeTextMessage))
                            return Theme.highlightColor;

                        return Theme.primaryColor;
                    }

                    font.italic: {
                        if(dialogModelItem.typing.length > 0)
                            return true;

                        if(dialogModelItem.messageType !== Telegram.Enums.TypeTextMessage)
                            return true;

                        return false;
                    }

                    rawText: {
                        if(dialogModelItem.typing.length > 0) {
                            if(dialogModelItem.chat)
                                return TelegramHelper.typingUsers(context, dialogModelItem.typing);

                            return qsTr("Typing...");
                        }

                        if(dialogModelItem.messageType === Telegram.Enums.TypeActionMessage)
                            return TelegramAction.actionType(dialogModelItem.topMessage.action, dialogModelItem.user, null, dialogModelItem.isSecretChat);

                        return TelegramHelper.firstMessageLine(context, dialogModelItem.topMessage.message);
                    }
                }

                Rectangle
                {
                    id: rectunread
                    width: dialogModelItem.unreadCount > 0 ? parent.height : 0
                    height: parent.height
                    color: Theme.secondaryHighlightColor
                    visible: dialogModelItem.unreadCount > 0
                    radius: width * 0.5

                    Label
                    {
                        anchors.centerIn: parent
                        font.pixelSize: Theme.fontSizeTiny
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: dialogModelItem.unreadCount
                    }
                }
            }
        }
    }
}
