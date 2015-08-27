import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../menus/conversation"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property Dialog dialog
    property Chat chat
    property User user
    property bool muted: context.telegram.userData.isMuted(TelegramHelper.conversationId(dialog, context))

    id: conversationpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(dialog.peer.chatId);
        else
            user = context.telegram.user(dialog.peer.userId);
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("ConversationInfoPage.qml"), { "context": conversationpage.context, "dialog": conversationpage.dialog, "chat": conversationpage.chat, "user": conversationpage.user });
        context.foregroundDialog = conversationpage.dialog;

        messagemodel.telegram = conversationpage.context.telegram;
        messagemodel.dialog = conversationpage.dialog;
        messagemodel.setReaded();
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
        target: context.telegram.userData

        onMuteChanged: {
            conversationpage.muted = context.telegram.userData.isMuted(TelegramHelper.conversationId(dialog, context));
        }
    }

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent

        ConversationMenu
        {
            id: conversationmenu
            page: conversationpage
            context: conversationpage.context
            dialog: conversationpage.dialog
        }

        PeerItem
        {
            id: header
            visible: !context.chatheaderhidden
            anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: Theme.paddingMedium }
            height: context.chatheaderhidden ? 0 : (conversationpage.isPortrait ? Theme.itemSizeSmall : Theme.itemSizeExtraSmall)
            context: conversationpage.context
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

            TelegramBackground { id: telegrambackground; visible: !context.backgrounddisabled; z: -1 }

            BusyIndicator {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
                running: messagemodel.refreshing
            }

            model: MessagesModel {
                id: messagemodel

                onCountChanged: {
                    if(!count)
                        return;

                    messagemodel.setReaded(); /* We are in this chat, always mark these messages as read */
                }
            }

            delegate: MessageItem {
                context: conversationpage.context
                message: item
            }
        }

        MessageBar
        {
            id: messagebar
            anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
            context: conversationpage.context
            dialog: conversationpage.dialog
        }
    }
}
