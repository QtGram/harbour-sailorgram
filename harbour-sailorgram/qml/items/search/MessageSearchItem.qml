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
    property User user
    property var dialog

    onMessageChanged: {
        user = context.telegram.user(message.fromId);
        dialog = context.telegram.messageDialog(message.id);
    }

    id: searchitem

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        PeerImage
        {
            id: userimage
            width: searchitem.height
            height: searchitem.height
            context: searchitem.context
            user: searchitem.user
        }

        Column
        {
            width: parent.width - userimage.width
            anchors { top: parent.top; bottom: parent.bottom }
            spacing: Theme.paddingSmall

            Label
            {
                id: lbltitle
                text: TelegramHelper.completeName(user)
                verticalAlignment: Text.AlignVCenter
                height: searchitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }
                color: Theme.highlightColor
                elide: Text.ElideRight
            }

            MessageTextContent
            {
                id: lbllastmessage
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }
                height: searchitem.height / 2
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

                    return TelegramHelper.firstMessageLine(message);
                }
            }
        }
    }
}
