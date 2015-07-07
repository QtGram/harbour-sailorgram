import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/chat"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property bool actionVisible: true
    property Settings settings
    property Telegram telegram
    property Dialog dialog
    property Chat chat
    property User user

    id: conversationinfopage
    allowedOrientations: defaultAllowedOrientations

    Component {
        id: userinfocomponent

        UserInfo {
            actionVisible: false
            settings: conversationinfopage.settings
            telegram: conversationinfopage.telegram
            user: conversationinfopage.user
        }
    }

    Component {
        id: chatinfocomponent

        ChatInfo {
            settings: conversationinfopage.settings
            telegram: conversationinfopage.telegram
            dialog: conversationinfopage.dialog
            chat: conversationinfopage.chat
        }
    }

    Column
    {
        id: content
        width: parent.width
        spacing: Theme.paddingMedium

        PageHeader { title: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.userName(user) }

        PeerItem
        {
            x: Theme.paddingMedium
            width: parent.width - (x * 2)
            height: Theme.itemSizeSmall
            telegram: conversationinfopage.telegram
            dialog: conversationinfopage.dialog
            chat: conversationinfopage.chat
            user: conversationinfopage.user
        }
    }

    Loader
    {
        id: loader
        anchors { left: parent.left; top: content.bottom; right: parent.right; bottom: parent.bottom; }

        sourceComponent: {
            if(TelegramHelper.isChat(dialog))
                return chatinfocomponent;

            return userinfocomponent;
        }
    }
}
