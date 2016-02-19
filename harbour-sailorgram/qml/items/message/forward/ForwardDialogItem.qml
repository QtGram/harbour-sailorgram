import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../items/peer"
import "../../../js/TelegramHelper.js" as TelegramHelper

BackgroundItem
{
    property Context context
    property var dialog
    property User user
    property Chat chat
    property EncryptedChat encryptedChat

    id: forwarddialogitem

    onDialogChanged: {
        var peerid = TelegramHelper.peerId(forwarddialogitem.dialog);

        if(dialog.encrypted) {
            encryptedChat = context.telegram.encryptedChat(peerid);
            user = context.telegram.user(TelegramHelper.encryptedUserId(context, encryptedChat));
            return;
        }

        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(peerid);
        else
            user = context.telegram.user(peerid);
    }

    PeerImage
    {
        id: peerimage
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.height - lbltitle.contentHeight
        height: width
        context: forwarddialogitem.context
        dialog: forwarddialogitem.dialog
        chat: forwarddialogitem.chat
        user: forwarddialogitem.user
    }

    Label
    {
        id: lbltitle
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        font.pixelSize: Theme.fontSizeTiny
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight

        text: {
            if(TelegramHelper.isChat(dialog))
                return chat.title;
            else
                return TelegramHelper.completeName(user);
        }
    }
}
