import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../menus/conversation"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/secretconversation"
import "../../items/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

Page
{
    property Context context
    property Dialog dialog
    property EncryptedChat chat: context.telegram.encryptedChat(dialog.peer.userId)
    property User user: context.telegram.user((chat.adminId === context.telegram.me) ? chat.participantId : chat.adminId)
    property bool muted: context.telegram.userData.isMuted(TelegramHelper.peerId(dialog))

    id: secretconversationpage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("../conversations/ConversationInfoPage.qml"), { "context": secretconversationpage.context, "dialog": secretconversationpage.dialog, "user": secretconversationpage.user });
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
            var peerid = TelegramHelper.peerId(dialog);

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
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: bottomcontainer.top; topMargin: Theme.paddingSmall }
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
                context: secretconversationpage.context
                message: item
            }
        }

        Item
        {
            id: bottomcontainer
            anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
            height: messagebar.height

            MessageBar
            {
                id: messagebar
                anchors.fill: parent
                context: secretconversationpage.context
                dialog: secretconversationpage.dialog
                visible: chat && (chat.classType !== TelegramConstants.typeEncryptedChatDiscarded) && (chat.classType !== TelegramConstants.typeEncryptedChatWaiting)
            }

            SecretChatDiscarded
            {
                anchors { fill: parent; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                chat: secretconversationpage.chat
            }

            SecretChatWaiting
            {
                anchors { fill: parent; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                chat: secretconversationpage.chat
                user: secretconversationpage.user
            }
        }
    }
}
