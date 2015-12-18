import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../../models"
import "../../items/peer"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramAction.js" as TelegramAction

Item
{
    property Context context
    property var dialog
    property User user
    property Chat chat
    property Message message
    property bool muted: false

    onDialogChanged: {
        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(dialog.peer.chatId);
        else
            user = context.telegram.user(dialog.peer.userId);

        message = context.telegram.message(dialog.topMessage);
        muted = context.telegram.userData.isMuted(TelegramHelper.peerId(dialog));
    }

    id: dialogitem

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
            if(user && (id !== user.id))
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

                Image {
                    id: imgchat
                    width: lbltitle.contentHeight
                    height: lbltitle.contentHeight
                    visible: TelegramHelper.isChat(dialog)
                    source: TelegramHelper.isChat(dialog) ? "image://theme/icon-s-chat" : ""
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: lbltitle
                    text: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.completeName(user)
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor

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

                Label {
                    id: lbltime
                    height: parent.height
                    font.pixelSize: Theme.fontSizeTiny
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    text: TelegramCalendar.timeToString(message.date)

                    width: {
                        var w = parent.width - lbltitle.contentWidth;

                        if(imgchat.visible)
                            w -= imgchat.width + (Theme.paddingSmall * 2);
                        else
                            w -= Theme.paddingSmall;

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
                        if(TelegramHelper.isServiceMessage(message))
                            return false;

                        if(TelegramHelper.isChat(dialog))
                            return true;

                        return message && message.out;
                    }

                    text: {
                        if(!TelegramHelper.isServiceMessage(message))
                        {
                            if(TelegramHelper.isChat(dialog))
                            {
                                var user = context.telegram.user(message.fromId);
                                return TelegramHelper.completeName(user) + ": ";
                            }

                            if(message && message.out)
                                return qsTr("You:") + " ";
                        }

                        return "";
                    }
                }

                Label
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width - lblfrom.width
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
