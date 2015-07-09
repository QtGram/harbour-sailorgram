import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../menus"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Settings settings
    property Telegram telegram
    property Dialog dialog
    property Chat chat
    property User user
    property bool muted: telegram.userData.isMuted(TelegramHelper.peerId(dialog.peer))

    id: conversationpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        if(TelegramHelper.isChat(dialog))
            chat = telegram.chat(dialog.peer.chatId);
        else
            user = telegram.user(dialog.peer.userId);
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("ConversationInfoPage.qml"), { "settings": conversationpage.settings, "telegram": conversationpage.telegram, "dialog": conversationpage.dialog, "chat": conversationpage.chat, "user": conversationpage.user });
        settings.foregroundDialog = conversationpage.dialog;

        messagemodel.setReaded();
        messagemodel.telegram = conversationpage.telegram;
        messagemodel.dialog = conversationpage.dialog;
    }

    RemorsePopup { id: remorsepopup }

    Timer
    {
        id: refreshtimer
        repeat: true
        interval: 10000
        onTriggered: messagemodel.refresh()
        Component.onCompleted: start()
    }

    PopupMessage
    {
        id: popupmessage
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }

    Connections
    {
        target: telegram.userData

        onMuteChanged: {
            var peerid = TelegramHelper.peerId(dialog.peer);

            if(id !== peerid)
                return;

            conversationpage.muted = telegram.userData.isMuted(peerid);
        }
    }

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent

        ConversationMenu
        {
            id: conversationmenu
            settings: conversationpage.settings
            dialog: conversationpage.dialog
        }

        PeerItem
        {
            id: header
            anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: Theme.paddingMedium }
            height: Theme.itemSizeSmall
            telegram: conversationpage.telegram
            dialog: conversationpage.dialog
            chat: conversationpage.chat
            user: conversationpage.user
        }

        SilicaListView
        {
            id: lvconversation
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: messagebar.top; topMargin: Theme.paddingSmall }
            verticalLayoutDirection: ListView.BottomToTop
            spacing: Theme.paddingLarge
            clip: true

            BusyIndicator {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
                running: (messagemodel.count <= 0) || messagemodel.refreshing
            }

            model: MessagesModel {
                id: messagemodel

                onCountChanged: {
                    if(count > 1)
                        messagemodel.setReaded(); /* We are in this chat, always mark these messages as read */
                }
            }

            delegate: MessageItem {
                settings: conversationpage.settings
                telegram: conversationpage.telegram
                message: item
            }
        }

        MessageBar
        {
            id: messagebar
            anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
            settings: conversationpage.settings
            telegram: conversationpage.telegram
            dialog: conversationpage.dialog
        }
    }
}
