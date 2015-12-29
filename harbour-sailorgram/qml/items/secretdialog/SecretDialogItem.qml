import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
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

                Image {
                    id: imgsecure
                    source: "image://theme/icon-s-secure"
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: lbluser
                    text: TelegramHelper.completeName(user)
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
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

                    width: {
                        var w = parent.width - lbluser.contentWidth - imgsecure.width - (Theme.paddingSmall * 2);

                        if(imgmute.visible)
                            w -= imgmute.width + (Theme.paddingSmall * 2);
                        else
                            w -= Theme.paddingSmall;

                        return w;
                    }
}
            }

            Row
            {
                height: secretdialogitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    font.italic: TelegramHelper.isServiceMessage(message)
                    text: TelegramHelper.isServiceMessage(message) ? TelegramAction.actionType(context.telegram, dialog, message) : TelegramHelper.messageContent(message)
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
