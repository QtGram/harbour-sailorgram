import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
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
    property int peerId: TelegramHelper.peerId(dialog.peer)
    property bool muted: telegram.userData.isMuted(peerId)

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
            if(id !== conversationpage.peerId)
                return;

            conversationpage.muted = telegram.userData.isMuted(conversationpage.peerId);
        }
    }

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem {
                text: conversationpage.muted ? qsTr("Enable Notifications") : qsTr("Disable Notifications")

                onClicked: {
                    if(conversationpage.muted)
                        telegram.userData.removeMute(conversationpage.peerId)
                    else
                        telegram.userData.addMute(conversationpage.peerId)
                }
            }

            MenuItem {
                text: qsTr("Delete")

                onClicked: {
                    remorsepopup.execute(qsTr("Deleting History"), function() {
                        telegram.messagesDeleteHistory(item.peer.userId);
                        pageStack.pop();
                    });
                }
            }

            MenuItem {
                text: qsTr("Add to Contacts")
                visible: !TelegramHelper.isChat(dialog) && !TelegramHelper.isTelegramUser(user) && (user.classType === conversationpage.typeUserRequest)
                onClicked: telegram.addContact(user.firstName, user.lastName, user.phone)
            }
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
            spacing: Theme.paddingMedium
            clip: true

            BusyIndicator {
                anchors.centerIn: parent
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
