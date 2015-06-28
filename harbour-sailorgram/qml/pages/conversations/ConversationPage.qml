import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../components"
import "../../items/user"
import "../../items/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Telegram telegram
    property Dialog dialog
    property int peerId: dialog.peer.chatId !== 0 ? dialog.peer.chatId : dialog.peer.userId
    property bool muted: telegram.userData.isMuted(peerId)

    id: conversationpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        messagemodel.clearNewMessageFlag();
        messagemodel.setReaded();
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("../../pages/users/UserPage.qml"), { "telegram": conversationpage.telegram, "user": telegram.user(dialog.peer.userId), "actionVisible": false });

        messagemodel.telegram = conversationpage.telegram;
        messagemodel.dialog = conversationpage.dialog;
    }

    RemorsePopup { id: remorsepopup }

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
        }

        UserItem
        {
            id: header
            anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: Theme.paddingMedium }
            height: Theme.itemSizeSmall
            telegram: conversationpage.telegram
            user: telegram.user(dialog.peer.userId)
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

                onMessageAdded: { /* We are in this chat, always mark these messages as read */
                    messagemodel.clearNewMessageFlag();
                    messagemodel.setReaded();
                }
            }

            delegate: MessageItem {
                telegram: conversationpage.telegram
                message: item
            }
        }

        MessageBar
        {
            id: messagebar
            anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
        }
    }
}
