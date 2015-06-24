import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../components"
import "../items"
import "../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Telegram telegram
    property Dialog dialog

    id: dialogpage

    Component.onCompleted: {
        dialog.unreadCount = 0; /* Mark all messages as readed */
    }

    SilicaListView
    {
        PullDownMenu {
            id: menu

            MenuItem {
                text: qsTr("Delete History")
            }

            MenuItem {
                text: qsTr("Remove Chat")
            }

            MenuItem {
                text: qsTr("Silence Notifications")
            }
        }

        id: lvdialog
        anchors { left: parent.left; top: parent.top; right: parent.right; bottom: messagebar.top }
        verticalLayoutDirection: ListView.BottomToTop
        spacing: Theme.paddingMedium
        clip: true

        footer: PageHeader { /* ListView is reversed, header = footer */
            title: qsTr("Chatting with: ") + TelegramHelper.userName(telegram.user(dialog.peer.userId))
        }

        model: MessagesModel {
            id: messagemodel
            telegram: dialogpage.telegram
            dialog: dialogpage.dialog

            onMessageAdded: { /* We are in this chat, always mark these messages as unread */
                var message = telegram.message(msgId);
                message.unread = false;
            }
        }

        delegate: MessageItem {
            message: item
        }
    }

    MessageBar
    {
        id: messagebar
        anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
    }
}
