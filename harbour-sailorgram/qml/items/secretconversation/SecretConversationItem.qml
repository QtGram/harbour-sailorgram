import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../models"
import "../../items/peer"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramAction.js" as TelegramAction
import "../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property Context context
    property Dialog dialog
    property User user
    property EncryptedChat chat
    property Message message

    onDialogChanged: {
        chat = context.telegram.encryptedChat(dialog.peer.userId);

        if(chat.classType === TelegramConstants.typeEncryptedChatRequested) /* Emulate Android Client: Auto accept encrypted chats */
            context.telegram.messagesAcceptEncryptedChat(dialog.peer.userId);

        var userid = (chat.adminId === context.telegram.me) ? chat.participantId : chat.adminId;
        user = context.telegram.user(userid);
        message = context.telegram.message(dialog.topMessage);
    }

    id: conversationitem

    Connections
    {
        target: dialog

        onTopMessageChanged: {
            message = context.telegram.message(dialog.topMessage);
        }
    }

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: conversationimage
            width: conversationitem.height
            height: conversationitem.height
            context: conversationitem.context
            dialog: conversationitem.dialog
            //chat: conversationitem.chat
            user: conversationitem.user
        }

        Column
        {
            width: parent.width - conversationimage.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: conversationitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label {
                    id: lbluser
                    width: parent.width - lbltime.width
                    text: TelegramHelper.userName(user)
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                }

                Label {
                    id: lbltime
                    height: parent.height
                    font.pixelSize: Theme.fontSizeTiny
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    text: TelegramCalendar.timeToString(message.date)
                }
            }

            Row
            {
                height: conversationitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.italic: TelegramHelper.isActionMessage(message)
                    text: TelegramHelper.isActionMessage(message) ? TelegramAction.actionType(context.telegram, message) : TelegramHelper.messageContent(message)
                }

                Rectangle
                {
                    id: rectunread
                    width: parent.height
                    height: parent.height
                    color: Theme.secondaryHighlightColor
                    visible: dialog.unreadCount > 0
                    radius: width * 0.5

                    Label
                    {
                        anchors.centerIn: parent
                        font.pixelSize: Theme.fontSizeTiny
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: dialog.unreadCount
                    }
                }
            }
        }
    }
}
