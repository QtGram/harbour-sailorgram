import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../user"
import "../../menus/chat"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Context context
    property Dialog dialog
    property Chat chat
    property ChatFull chatFull: context.telegram.chatFull(chat.id)
    property bool adminMenu: false

    onChatFullChanged: {
        adminMenu = (chatFull.participants.adminId === context.telegram.me);
    }

    id: chatinfo

    SectionHeader {
        id: hdrsection
        text: qsTr("Members")
    }

    SilicaListView
    {
        id: lvpartecipants
        spacing: Theme.paddingMedium
        anchors { left: parent.left; top: hdrsection.bottom; right: parent.right; bottom: parent.bottom }

        model: ChatParticipantsModel {
            id: chatparticipantsmodel
            telegram: context.telegram
            dialog: chatinfo.dialog
        }

        delegate: ListItem {
            property ChatParticipant participant: item
            property User user: context.telegram.user(participant.userId)

            id: liparticipant
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall
            showMenuOnPressAndHold: adminMenu && (user.id !== context.telegram.me)

            menu: ChatInfoMenu {
                context: chatinfo.context
                dialog: chatinfo.dialog
                user: liparticipant.user
            }

            UserItem {
                id: useritem
                anchors { fill: parent; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                context: chatinfo.context
                user: liparticipant.user
            }
        }
    }
}
