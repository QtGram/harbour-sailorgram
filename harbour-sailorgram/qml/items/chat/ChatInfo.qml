import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../user"
import "../../menus/chat"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Settings settings
    property Telegram telegram
    property Dialog dialog
    property Chat chat
    property ChatFull chatFull: telegram.chatFull(chat.id)
    property bool adminMenu: false

    onChatFullChanged: {
        adminMenu = chatFull.participants.adminId === telegram.me;
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
            telegram: chatinfo.telegram
            dialog: chatinfo.dialog
        }

        delegate: ListItem {
            property ChatParticipant participant: item
            property User user: telegram.user(participant.userId)

            id: liparticipant
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall
            showMenuOnPressAndHold: adminMenu && (user.id !== telegram.me)

            menu: ChatInfoMenu {
                settings: chatinfo.settings
                dialog: chatinfo.dialog
                user: liparticipant.user
            }

            UserItem {
                id: useritem
                anchors { fill: parent; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                telegram: chatinfo.telegram
                user: liparticipant.user
            }
        }
    }
}
