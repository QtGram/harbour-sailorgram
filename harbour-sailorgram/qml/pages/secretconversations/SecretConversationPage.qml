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
    property EncryptedChat chat
    property User user
    property bool muted: context.telegram.userData.isMuted(TelegramHelper.peerId(dialog.peer))

    id: secretconversationpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        chat = context.telegram.encryptedChat(dialog.peer.userId);

        var userid = (chat.adminId === context.telegram.me) ? chat.participantId : chat.adminId;
        user = context.telegram.user(userid);
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        //pageStack.pushAttached(Qt.resolvedUrl("ConversationInfoPage.qml"), { "context": secretconversationpage.context, "dialog": secretconversationpage.dialog, "chat": secretconversationpage.chat, "user": secretconversationpage.user });
        context.foregroundDialog = secretconversationpage.dialog;

        messagemodel.setReaded();
        messagemodel.telegram = secretconversationpage.context.telegram;
        messagemodel.dialog = secretconversationpage.dialog;
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
            var peerid = TelegramHelper.peerId(dialog.peer);

            if(id !== peerid)
                return;

            secretconversationpage.muted = context.telegram.userData.isMuted(peerid);
        }
    }

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent

        ConversationMenu
        {
            id: conversationmenu
            context: secretconversationpage.context
            dialog: secretconversationpage.dialog
        }

        PeerItem
        {
            id: header
            anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: Theme.paddingMedium }
            height: Theme.itemSizeSmall
            context: secretconversationpage.context
            dialog: secretconversationpage.dialog
            user: secretconversationpage.user
        }

        SilicaListView
        {
            id: lvconversation
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: messagebar.top; topMargin: Theme.paddingSmall }
            verticalLayoutDirection: ListView.BottomToTop
            spacing: Theme.paddingLarge
            clip: true

            TelegramBackground { id: telegrambackground; z: -1 }

            BusyIndicator {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
                running: messagemodel.count <= 0
            }

            model: MessagesModel {
                id: messagemodel

                onCountChanged: {
                    if(count > 1)
                        messagemodel.setReaded(); /* We are in this chat, always mark these messages as read */
                }
            }

            delegate: MessageItem {
                context: secretconversationpage.context
                message: item
            }
        }

        MessageBar
        {
            id: messagebar
            anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
            context: secretconversationpage.context
            dialog: secretconversationpage.dialog
        }
    }
}
