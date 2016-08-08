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
    property string title
    property int unreadCount
    property bool mute
    property bool isSecretChat
    property bool messageOut
    property var messageType
    property var messageDate
    property var messageUnread
    property var messageUser
    property var peer
    property var chat
    property var topMessage
    property var typing

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
            peer: dialogitem.peer
            fallbackTitle: dialogitem.title
            chat: dialogitem.chat
            isSecretChat: dialogitem.isSecretChat
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
                    text: dialogitem.title

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
                    visible: dialogitem.mute
                }

                MessageStatus
                {
                    id: msgstatus
                    height: parent.height
                    horizontalAlignment: Text.AlignRight
                    context: dialogitem.context
                    color: Theme.primaryColor
                    ticksColor: Theme.highlightColor
                    messageType: dialogitem.messageType
                    messageOut: dialogitem.messageOut
                    messageDateTime: dialogitem.messageDate
                    messageUnread: dialogitem.messageUnread
                    dateFirst: false
                    visible: true
                    dateOnly: (dialogitem.messageType === Telegram.Enums.TypeActionMessage) || !dialogitem.messageOut;
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
                        if((dialogitem.messageType === Telegram.Enums.TypeActionMessage) || (dialogitem.typing.length > 0))
                            return false;

                        if(dialogitem.chat && !dialogitem.chat.broadcast)
                            return true;

                        return dialogitem.messageOut;
                    }

                    text: {
                        if((dialogitem.messageType !== Telegram.Enums.TypeActionMessage) && (dialogitem.typing.length <= 0)) {
                            if(dialogitem.chat && !dialogitem.chat.broadcast && dialogitem.messageUser)
                                return TelegramHelper.userDisplayName(dialogitem.messageUser) + ": ";

                            if(dialogitem.messageOut)
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
                        if((dialogitem.typing.length > 0) || (dialogitem.messageType !== Telegram.Enums.TypeTextMessage))
                            return Theme.highlightColor;

                        return Theme.primaryColor;
                    }

                    font.italic: {
                        if(dialogitem.typing.length > 0)
                            return true;

                        if(dialogitem.messageType !== Telegram.Enums.TypeTextMessage)
                            return true;

                        return false;
                    }

                    rawText: {
                        if(dialogitem.typing.length > 0) {
                            if(dialogitem.chat)
                                return TelegramHelper.typingUsers(context, dialogitem.typing);

                            return qsTr("Typing...");
                        }

                        if(dialogitem.messageType === Telegram.Enums.TypeActionMessage)
                            return TelegramAction.actionType(dialogitem.topMessage.action, dialogitem.user, null, dialogitem.isSecretChat);
                        else if((dialogitem.messageType !== Telegram.Enums.TypeTextMessage) && (dialogitem.messageType !== Telegram.Enums.TypeWebPageMessage))
                            return TelegramHelper.mediaType(dialogitem.messageType);

                        return TelegramHelper.firstMessageLine(context, dialogitem.topMessage.message);
                    }
                }

                Rectangle
                {
                    id: rectunread
                    width: dialogitem.unreadCount > 0 ? parent.height : 0
                    height: parent.height
                    color: Theme.secondaryHighlightColor
                    visible: dialogitem.unreadCount > 0
                    radius: width * 0.5

                    Label
                    {
                        anchors.centerIn: parent
                        font.pixelSize: Theme.fontSizeTiny
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: dialogitem.unreadCount
                    }
                }
            }
        }
    }
}
