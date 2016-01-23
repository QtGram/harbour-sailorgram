import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/peer"
import "../../items/user"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramAction.js" as TelegramAction
import "../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property Context context
    property var dialog
    property User user
    property Chat chat
    property bool muted: context.telegram.userData.isMuted(TelegramHelper.peerId(dialogitem.dialog))
    property Message message: context.telegram.message(dialog.topMessage);

    onDialogChanged: {
        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(dialog.peer.chatId);
        else
            user = context.telegram.user(dialog.peer.userId);
    }

    id: dialogitem

    Connections
    {
        target: context.telegram.userData

        onMuteChanged: {
            var peerid = TelegramHelper.peerId(dialog);

            if(id !== peerid)
                return;

            dialogitem.muted = context.telegram.userData.isMuted(id);
        }
    }

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: conversationimage
            width: dialogitem.height
            height: dialogitem.height
            context: dialogitem.context
            dialog: dialogitem.dialog
            chat: dialogitem.chat
            user: dialogitem.user
        }

        Column
        {
            width: parent.width - conversationimage.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: dialogitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }
                spacing: Theme.paddingSmall

                Label {
                    id: lbltitle
                    text: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.completeName(user)
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                    elide: Text.ElideRight

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
                    visible: dialogitem.muted
                    source: "image://theme/icon-m-speaker-mute"
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                }

                MessageStatus
                {
                    id: msgstatus
                    context: dialogitem.context
                    message: dialogitem.message
                    height: parent.height
                    color: Theme.primaryColor
                    horizontalAlignment: Text.AlignRight
                    dateOnly: TelegramHelper.isServiceMessage(message) || !dialogitem.message.out
                    dateFirst: false
                    ticksColor: Theme.highlightColor
                    visible: true
                }
            }

            Row
            {
                height: dialogitem.height / 2
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
                        if(TelegramHelper.isServiceMessage(message) || (dialog.typingUsers.length > 0))
                            return false;

                        if(TelegramHelper.isChat(dialog))
                            return true;

                        return message && message.out;
                    }

                    text: {
                        if(!TelegramHelper.isServiceMessage(message) && (dialog.typingUsers.length <= 0))
                        {
                            if(TelegramHelper.isChat(dialog))
                            {
                                var user = context.telegram.user(message.fromId);
                                return TelegramHelper.userDisplayName(user) + ": ";
                            }

                            if(message && message.out)
                                return qsTr("You:") + " ";
                        }

                        return "";
                    }
                }

                MessageTextContent
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width - lblfrom.contentWidth
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                    emojiPath: context.sailorgram.emojiPath
                    linkColor: Theme.secondaryColor

                    color: {
                        if(dialog.typingUsers.length > 0)
                            return Theme.highlightColor;

                        if(TelegramHelper.isServiceMessage(message))
                            return Theme.highlightColor;

                        return Theme.primaryColor;
                    }

                    font.italic: {
                        if(TelegramHelper.isServiceMessage(message) || (dialog.typingUsers.length > 0))
                            return true;

                        if(TelegramHelper.isMediaMessage(message) && (message.media.classType === TelegramConstants.typeMessageMediaDocument) && context.telegram.documentIsSticker(message.media.document))
                            return true;

                        return false;
                    }

                    rawText: {
                        if(dialog.typingUsers.length > 0)
                            return TelegramHelper.typingUsers(context, dialog);

                        if(!message)
                            return "";

                        if(TelegramHelper.isServiceMessage(message))
                            return TelegramAction.actionType(context.telegram, dialog, message);

                        return TelegramHelper.firstMessageLine(message);
                    }
                }

                Rectangle
                {
                    id: rectunread
                    width: dialog.unreadCount > 0 ? parent.height : 0
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
