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

ListItem
{
    property Context context
    property Message message
    property Chat chat
    property User user
    property var dialog

    onMessageChanged: {
        dialog = context.telegram.messageDialog(message.id);
        peerimage.dialog = dialog;

        var peerid = TelegramHelper.peerId(dialog);

        if(TelegramHelper.isChat(dialog))
        {
            chat = context.telegram.chat(peerid);
            user = context.telegram.user(message.fromId);
            peerimage.chat = chat;
        }
        else
        {
            user = context.telegram.user(peerid);
            peerimage.user = user;
        }
    }

    id: searchitem

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: peerimage
            width: searchitem.height
            height: searchitem.height
            context: searchitem.context
        }

        Column
        {
            width: parent.width - peerimage.width
            anchors { top: parent.top; bottom: parent.bottom }
            spacing: Theme.paddingSmall

            Label
            {
                id: lbltitle
                text: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.completeName(user)
                verticalAlignment: Text.AlignVCenter
                height: searchitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }
                color: Theme.highlightColor
                elide: Text.ElideRight
            }

            Row
            {

                height: searchitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label
                {
                    id: lblfrom
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeExtraSmall

                    text: {
                        if(!TelegramHelper.isServiceMessage(message))
                        {
                            if(TelegramHelper.isChat(dialog))
                                return TelegramHelper.userDisplayName(user) + ": ";

                            if(message && message.out)
                                return qsTr("You:") + " ";
                        }

                        return "";
                    }
                }

                MessageTextContent
                {
                    id: lbllastmessage
                    width: parent.width - lblfrom.contentWidth
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                    emojiPath: context.sailorgram.emojiPath
                    linkColor: Theme.secondaryColor

                    color: {
                        if(TelegramHelper.isServiceMessage(message))
                            return Theme.highlightColor;

                        return Theme.primaryColor;
                    }

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

                        return TelegramHelper.firstMessageLine(context, message);
                    }
                }
            }
        }
    }
}
