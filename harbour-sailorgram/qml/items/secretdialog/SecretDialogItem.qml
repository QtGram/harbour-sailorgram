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
    property Dialog dialog
    property User user
    property EncryptedChat chat
    property Message message
    property bool muted: false

    onDialogChanged: {
        chat = context.telegram.encryptedChat(dialog.peer.userId);

        var userid = (chat.adminId === context.telegram.me) ? chat.participantId : chat.adminId;
        user = context.telegram.user(userid);
        message = context.telegram.message(dialog.topMessage);
        muted = context.telegram.userData.isMuted(dialog.peer.userId);
    }

    id: secretdialogitem

    Connections
    {
        target: dialog

        onTopMessageChanged: {
            message = context.telegram.message(dialog.topMessage);
        }
    }

    Connections
    {
        target: context.telegram.userData

        onMuteChanged: {
            if(id !== dialog.peer.userId)
                return;

            secretdialogitem.muted = context.telegram.userData.isMuted(id);
        }
    }

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: conversationimage
            width: secretdialogitem.height
            height: secretdialogitem.height
            context: secretdialogitem.context
            dialog: secretdialogitem.dialog
            //chat: conversationitem.chat
            user: secretdialogitem.user
        }

        Column
        {
            width: parent.width - conversationimage.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: secretdialogitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }
                spacing: Theme.paddingSmall

                Label {
                    id: lbltitle
                    text: TelegramHelper.completeName(user)
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                    elide: Text.ElideRight

                    width: {
                        var w = parent.width - lbltime.contentWidth;

                        if(imgmute.visible)
                            w -= imgmute.width + Theme.paddingSmall;

                        return w;
                    }
                }

                Image {
                    id: imgmute
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    visible: secretdialogitem.muted
                    source: "image://theme/icon-m-speaker-mute"
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: lbltime
                    height: parent.height
                    font.pixelSize: Theme.fontSizeTiny
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    text: TelegramHelper.printableDate(message.date)
                }
            }

            Row
            {
                height: secretdialogitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                MessageTextContent
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                    emojiPath: context.sailorgram.emojiPath
                    linkColor: Theme.secondaryColor
                    color: TelegramHelper.isServiceMessage(message) ? Theme.highlightColor : Theme.primaryColor

                    font.italic: {
                        if(TelegramHelper.isServiceMessage(message))
                            return true;

                        if(TelegramHelper.isMediaMessage(message) && (message.media.classType === TelegramConstants.typeMessageMediaDocument) && context.telegram.documentIsSticker(message.media.document))
                            return true;

                        return false;
                    }

                    rawText: {
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
